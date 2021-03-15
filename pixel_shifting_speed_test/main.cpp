#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include<vector>
#include <ctime>
#include<cmath>


void PixelShifting_1(const cv::Mat temp, const cv::Mat left, cv::Mat right,
                   int deviation) {
  float *temp_pixel = (float *)temp.data;
  uchar *left_pixel = left.data;
  uchar *right_pixel = right.data;
  uchar *right_pixel_d = right.data;
  int height = left.rows;
  int width = left.cols;
  for (int row = 0; row < height; row++) {
    left_pixel = left.data + row * left.step;
    right_pixel = right.data + row * right.step;
    temp_pixel = (float *)(temp.data + row * temp.step);
    for (int col = 0; col < width; col++) {
      int dis = (int)((1.0 + temp_pixel[0]) * deviation);
      if (col > dis) {
        right_pixel_d = right_pixel - dis * 3;
        right_pixel_d[0] = left_pixel[0];
        right_pixel_d[1] = left_pixel[1];
        right_pixel_d[2] = left_pixel[2];
      }
      right_pixel += 3;
      left_pixel += 3;
      temp_pixel++;
    }
  }
}


int main(int argc, const char *argv[]) {
  cv::Mat left_image = cv::imread("../test.jpg");
  cv::Mat right_image_1(left_image.rows, left_image.cols, CV_8UC3);
  cv::Mat right_image_2(left_image.rows, left_image.cols, CV_8UC3);
  cv::Mat right_image_3(left_image.rows, left_image.cols, CV_8UC3);

  cv::Mat depth(left_image.rows, left_image.cols, CV_32FC1, cv::Scalar(10));
  int const DEVIATION = 7;
  // maximize floating window
  namedWindow("Right image", cv::WINDOW_NORMAL);
  setWindowProperty("Right image", cv::WND_PROP_FULLSCREEN,
                    cv::WINDOW_FULLSCREEN);
  // timer
  double method_1;
  double method_2;
  double method_3;

  clock_t method_1_start, method_1_end;
  clock_t method_2_start, method_2_end;
  clock_t method_3_start, method_3_end;

  method_1_start = clock();
  PixelShifting_1(depth, left_image, right_image_1, DEVIATION);
  method_1_end = clock();


  method_2_start = clock();
  float *ptr_temp_rows;
  cv::Vec3b *ptr_right_rows;
  cv::Vec3b *ptr_left_rows;

  for (int row = 0; row < left_image.rows; row++) {
    ptr_temp_rows = depth.ptr<float>(row);
    ptr_right_rows = right_image_2.ptr<cv::Vec3b>(row);
    ptr_left_rows = left_image.ptr<cv::Vec3b>(row);
    for (int col = 0; col < left_image.cols; col++) {
      int dis = (int)((1.0 + ptr_temp_rows[col]) * DEVIATION);
      int col_r = col - dis;
      if (col_r > 0) {
        ptr_right_rows[col_r] = ptr_left_rows[col];
      }
    }
  }
  method_2_end = clock();
  
  method_3_start = clock();
  for (int row = 0; row < left_image.rows; row++) {
    for (int col = 0; col < left_image.cols; col++) {
      int dis = (int)((1.0 + depth.ptr<float>(row)[col]) * DEVIATION);
      int col_r = col - dis;
      if (col_r > 0) {
        right_image_3.ptr<cv::Vec3b>(row)[col_r] =
            left_image.ptr<cv::Vec3b>(row)[col];
      }
    }
  }
  method_3_end = clock();

  method_1 = (double)(method_1_end - method_1_start); // / CLOCKS_PER_SEC;
  std::cout << "Method : " << method_1 << "(ms)" << std::endl;
  method_2 = (double)(method_2_end - method_2_start);  // / CLOCKS_PER_SEC;
  std::cout << "Method : " << method_2 << "(ms)" << std::endl;
  method_3 = (double)(method_3_end - method_3_start);  // / CLOCKS_PER_SEC;
  std::cout << "Method : " << method_3 << "(ms)" << std::endl;

  cv::imshow("Right image", right_image_1);
  cv::waitKey(0);
  cv::imshow("Right image", right_image_2);
  cv::waitKey(0);
  cv::imshow("Right image", right_image_3);
  cv::waitKey(0);
}