// opencv_test.cpp : Defines the entry point for the console application.
//

#include "windows.h"
//#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include <iostream>
#include <string>


using namespace cv;
using namespace std;

int main() {

	Mat src = imread("building1.png", CV_LOAD_IMAGE_COLOR);
	Mat dst, color_dst;

	// check for validation
	if (!src.data) {
		printf("Could not open\n");
		return -1;
	}

	Canny(src, dst, 50, 200, 3);
	cvtColor(dst, color_dst, COLOR_GRAY2BGR);
	
	//Standard Hough transform (using 'HoughLines')
	//imshow("Canny", color_dst);
#if 1
	std::cout << "1" << endl;
	vector<Vec2f> lines;
	//Fill this line
	cv::HoughLines(dst, lines, 1, CV_PI / 180, 100, 0, 0);
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
		Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
		line(color_dst, pt1, pt2, Scalar(0, 0, 255), 0.5, 8);
	}

	//Probabilistic Hough transform (using 'HoughLinesP')
#else
	std::cout << "else" << endl;
	vector<Vec4i> lines;
	//Fill this line
	HoughLinesP(dst, lines, 1, CV_PI/180, 30, 30, 3);
	for (size_t i = 0; i < lines.size(); i++)
	{
		line(color_dst, Point(lines[i][0], lines[i][1]),
			Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
	}
#endif

	namedWindow("Source", 1);
	imshow("Source", src);
	namedWindow("Detected Lines", 1);
	imshow("Detected Lines", color_dst);
	
	waitKey(0);

	return 0;
}

