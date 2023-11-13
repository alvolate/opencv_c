#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


void get_default_frame(VideoCapture cap, cv::Mat &default_frame){
   // get a frame from the camera and set it as the default frame 
   // skip the first 10 frames
   Mat temp_frame;
   for(int i=0; i<10; i++){
      if(!cap.read(temp_frame)){
         cout << "Error reading frame" << endl;
         return;
      }
   }
   if(cap.read(temp_frame)){
      cvtColor(temp_frame, temp_frame, COLOR_BGR2GRAY);
      default_frame = temp_frame.clone();
   }
   return;
}

int main(){
   int thresh = 60;
   Mat frame;
   Mat default_frame;
   Mat sub_frame;
   Mat raw_frame;

   VideoCapture cap(0);

   if(!cap.isOpened()){
      cout << "Error opening video stream or file" << endl;
      return -1;
   }

   get_default_frame(cap, default_frame);
   while(1){
      if(!(cap.read(frame)) || default_frame.empty()){
         // get a new frame from camera and check if default frame is empty
         cout << "No frame" << endl;
         break;
      }
      raw_frame=frame.clone();

      cvtColor(frame, frame, COLOR_BGR2GRAY);
      addWeighted(frame, 0.5, default_frame, 0.5, 0, default_frame);

      absdiff(default_frame,frame , sub_frame);
      threshold(sub_frame, sub_frame, thresh, 255, THRESH_BINARY);
      morphologyEx(sub_frame, sub_frame, MORPH_OPEN, Mat(), Point(-1,-1), 2);

      vector<vector<Point> > contours;
      vector< Vec4i> hierarchy;
      findContours(sub_frame, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0,0));

      vector<vector<Point> > save_contour;
      for(auto contour : contours){
         double area = contourArea(contour);
         if(area > 200){
            save_contour.push_back(contour);
            drawContours(frame, contours, -1, Scalar(0,255,0), 2);
         }


      }

      imshow("sub_frame", sub_frame);
      imshow("frame", frame);

      if(save_contour.size() >0){
         //get current time
         time_t now = time(0);
         tm *ltm = localtime(&now);

         //generate random number
         int rand_num = rand() % 1000;

         //save image
         char filename[100];
         sprintf(filename, "img_%d_%d_%d_%d.jpg", ltm->tm_hour, ltm->tm_min, ltm->tm_sec, rand_num);
         cout << filename << endl;

         imwrite(filename, raw_frame);
      }

      if( waitKey(10) == 27 ){
         // stop the video
         cout << "Esc key is pressed by user. Stoppig the video" << endl;
         break;
      }
   }


   // }
   return 0;
   
}