// opencv_test.cpp : Defines the entry point for the console application.
//
#include "windows.h"
//#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;


Mat get_Gaussian_Kernel(int n, double sigma_t, double sigma_s, bool normalize);
Mat get_Laplacian_Kernel();
Mat Gaussianfilter(const Mat input, int n, double sigma_t, double sigma_s);
Mat Laplacianfilter(const Mat input);
Mat Mirroring(const Mat input, int n);


int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input_gray;
	Mat bgr[3];
	Mat h_f[3];
	Mat Laplacian[3];
	Mat h_ff;
	Mat Laplacianf;

	split(input, bgr);

	// check for validation
	if (!input.data) {
		printf("Could not open\n");
		return -1;
	}

	//Gaussian smoothing parameters
	int window_radius = 2;
	double sigma_t = 2.0;
	double sigma_s = 2.0;

	cvtColor(input, input_gray, CV_RGB2GRAY);	// convert RGB to Grayscale
	bgr[0].convertTo(bgr[0], CV_64F, 1.0 / 255);	// 8-bit unsigned char -> 64-bit floating point
	bgr[1].convertTo(bgr[1], CV_64F, 1.0 / 255);
	bgr[2].convertTo(bgr[2], CV_64F, 1.0 / 255);

	h_f[0] = Gaussianfilter(bgr[0], window_radius, sigma_t, sigma_s);	// h(x,y) * f(x,y)
	h_f[1] = Gaussianfilter(bgr[1], window_radius, sigma_t, sigma_s);
	h_f[2] = Gaussianfilter(bgr[2], window_radius, sigma_t, sigma_s);

	
	Laplacian[0] = Laplacianfilter(h_f[0]);
	Laplacian[1] = Laplacianfilter(h_f[1]);
	Laplacian[2] = Laplacianfilter(h_f[2]);

	normalize(Laplacian[0], Laplacian[0], 0, 1, CV_MINMAX);
	normalize(Laplacian[1], Laplacian[1], 0, 1, CV_MINMAX);
	normalize(Laplacian[2], Laplacian[2], 0, 1, CV_MINMAX);
	
	namedWindow("Input", WINDOW_AUTOSIZE);
	imshow("Input", input);

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	merge(h_f, 3, h_ff);
	namedWindow("Gaussian blur", WINDOW_AUTOSIZE);
	imshow("Gaussian blur", h_ff);

	merge(Laplacian, 3, Laplacianf);
	namedWindow("Laplacian filter", WINDOW_AUTOSIZE);
	imshow("Laplacian filter", Laplacianf);
	
	waitKey(0);

	return 0;
}



Mat Gaussianfilter(const Mat input, int n, double sigma_t, double sigma_s) {

	int row = input.rows;
	int col = input.cols;
	
	// generate gaussian kernel
	Mat kernel = get_Gaussian_Kernel(n, sigma_t, sigma_s, true);
	Mat output = Mat::zeros(row, col, input.type());

	//Intermediate data generation for mirroring
	Mat input_mirror = Mirroring(input, n);

	//std::cout << input.type() << '\t' << output.type() << '\t'<< kernel.type();
	for (int i = n; i < row+n; i++) {
		for (int j = n; j < col+n; j++) {
			for (int a = -n; a <= n; a++) {
				for (int b = -n; b <= n; b++) {
						output.at<double>(i-n, j-n) += kernel.at<double>(a + n, b + n) * input_mirror.at<double>(i + a, j + b);
				}
			}
		}
	}
	
	return output;
}

Mat Laplacianfilter(const Mat input) {

	int row = input.rows;
	int col = input.cols;	
	int tempa, tempb;
	Mat kernel = get_Laplacian_Kernel();
	Mat output = Mat::zeros(row, col, input.type());
	Mat goutput = Mat::zeros(row, col, input.type());
	double minVal = 0.0;
	double maxVal = 0.0;
	int n = 1;
	Mat input_mirror = Mirroring(input, n);

	for (int i = n; i < row + n; i++) {
		for (int j = n; j < col + n; j++) {
			for (int a = -n; a <= n; a++) {
				for (int b = -n; b <= n; b++) {
					if (i -n + a > row - 1) {  //mirroring for the border pixels
						tempa = i - n - a;
					}
					else if (i -n + a < 0) {
						tempa = -(i-n + a);
					}
					else {
						tempa = i-n + a;
					}
					if (j-n + b > col - 1) {
						tempb = j-n - b;
					}
					else if (j-n + b < 0) {
						tempb = -(j-n + b);
					}
					else {
						tempb = j-n + b;
					}
					output.at<double>(i-n, j-n) += kernel.at<double>(a + n, b + n) * input_mirror.at<double>(tempa, tempb);
				}

			}
			
			maxVal = max(maxVal, abs(output.at<double>(i - n, j - n)));
			minVal = min(minVal, abs(output.at<double>(i - n, j - n)));
			
		}
	}
	std::cout << minVal << '\t' << maxVal << std::endl;
	double minVal2 = 0.0;
	double maxVal2 = 0.0;

	for (int i = n; i < row + n; i++) {
		for (int j = n; j < col + n; j++) {
			goutput.at<double>(i - n, j - n) = 255*(abs(output.at<double>(i - n, j - n))- minVal)/maxVal;
			maxVal2 = max(maxVal2, goutput.at<double>(i - n, j - n) );
			minVal2 = min(minVal2, goutput.at<double>(i - n, j - n) );
		}
	}
	
	std::cout << minVal2 << '\t' << maxVal2 << std::endl;

	return 200*goutput;
}

Mat Mirroring(const Mat input, int n)
{
	int row = input.rows;
	int col = input.cols;

	Mat input2 = Mat::zeros(row + 2 * n, col + 2 * n, input.type());
	int row2 = input2.rows;
	int col2 = input2.cols;

	for (int i = n; i < row + n; i++) {
		for (int j = n; j < col + n; j++) {
			input2.at<double>(i, j) = input.at<double>(i - n, j - n);
		}
	}
	for (int i = n; i < row + n; i++) {
		for (int j = 0; j < n; j++) {
			input2.at<double>(i, j) = input2.at<double>(i, 2 * n - j);
		}
		for (int j = col + n; j < col2; j++) {
			input2.at<double>(i, j) = input2.at<double>(i, 2 * col - 2 + 2 * n - j);
		}
	}
	for (int j = 0; j < col2; j++) {
		for (int i = 0; i < n; i++) {
			input2.at<double>(i, j) = input2.at<double>(2 * n - i, j);
		}
		for (int i = row + n; i < row2; i++) {
			input2.at<double>(i, j) = input2.at<double>(2 * row - 2 + 2 * n - i, j);
		}
	}

	return input2;
}


Mat get_Gaussian_Kernel(int n, double sigma_t, double sigma_s, bool normalize) {

	int kernel_size = (2 * n + 1);
	Mat kernel = Mat::zeros(kernel_size, kernel_size, CV_64F);
	double kernel_sum = 0.0;

	for (int i = -n; i <= n; i++) {
		for (int j = -n; j <= n; j++) {
			kernel.at<double>(i + n, j + n) = exp(-((i * i) / (2.0 * sigma_t * sigma_t) + (j * j) / (2.0 * sigma_s * sigma_s)));
			kernel_sum += kernel.at<double>(i + n, j + n);
		}
	}

	if (normalize) {
		for (int i = 0; i < kernel_size; i++)
			for (int j = 0; j < kernel_size; j++)
				kernel.at<double>(i, j) /= kernel_sum;		// normalize
	}

	return kernel;
}

Mat get_Laplacian_Kernel() {
	
	Mat kernel = Mat::zeros(3, 3, CV_64F);

	kernel.at<double>(0, 1) = 1.0;
	kernel.at<double>(2, 1) = 1.0;
	kernel.at<double>(1, 0) = 1.0;
	kernel.at<double>(1, 2) = 1.0;	
	kernel.at<double>(1, 1) = -4.0;
	
	return kernel;
}