#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>       /* exp */
#define IM_TYPE	CV_8UC3

using namespace cv;

// Image Type
// "G" for GrayScale Image, "C" for Color Image
#if (IM_TYPE == CV_8UC3)
typedef uchar G;
typedef cv::Vec3b C;
#elif (IM_TYPE == CV_16SC3)
typedef short G;
typedef Vec3s C;
#elif (IM_TYPE == CV_32SC3)
typedef int G;
typedef Vec3i C;
#elif (IM_TYPE == CV_32FC3)
typedef float G;
typedef Vec3f C;
#elif (IM_TYPE == CV_64FC3)
typedef double G;
typedef Vec3d C;
#endif

Mat sobelfilter(const Mat input);
Mat laplacianfilter(const Mat input);

int main() {

	Mat input = imread("lenna.png", IMREAD_COLOR);
	Mat output1, output2;





	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Input", WINDOW_AUTOSIZE);
	imshow("Input", input);
	output1 = sobelfilter(input); //Boundary process: zero-paddle, mirroring, adjustkernel
	output2 = laplacianfilter(input);
	
	namedWindow("Sobel Filter", WINDOW_AUTOSIZE);
	imshow("Sobel Filter", output1);

	namedWindow("Laplacian Filter", WINDOW_AUTOSIZE);
	imshow("Laplacian Filter", output2);


	waitKey(0);

	return 0;
}


Mat sobelfilter(const Mat input) {

	Mat kernel,Sx,Sy, lxr, lyr, lxg, lyg, lxb, lyb;

	int row = input.rows;
	int col = input.cols;
	int n = 1; // Sobel Filter Kernel N

	// Initialiazing 2 Kernel Matrix with 3x3 size for Sx and Sy
	//Fill code to initialize Sobel filter kernel matrix for Sx and Sy (Given in the lecture notes)
	
	kernel = Mat::zeros(3, 3, CV_32F);
	Sx = Mat::zeros(3, 3, CV_32F);
	Sy = Mat::zeros(3, 3, CV_32F);
	lxr = Mat::zeros(row, col, CV_32F);
	lyr = Mat::zeros(row, col, CV_32F);
	lxg = Mat::zeros(row, col, CV_32F);
	lyg = Mat::zeros(row, col, CV_32F);
	lxb = Mat::zeros(row, col, CV_32F);
	lyb = Mat::zeros(row, col, CV_32F);
	

	Sx.at<float>(0, 0) = -1;
	Sx.at<float>(0, 2) = 1;
	Sx.at<float>(1, 0) = -2;
	Sx.at<float>(1, 2) = 2;
	Sx.at<float>(2, 0) = -1;
	Sx.at<float>(2, 2) = 1;

	Sy.at<float>(0, 0) = -1;
	Sy.at<float>(0, 1) = -2;
	Sy.at<float>(0, 2) = -1;
	Sy.at<float>(2, 0) = 1;
	Sy.at<float>(2, 1) = 2;
	Sy.at<float>(2, 2) = 1;

	Mat output = Mat::zeros(row, col, CV_8U);
	Mat Mr = Mat::zeros(row, col, CV_8U);
	Mat Mg = Mat::zeros(row, col, CV_8U);
	Mat Mb = Mat::zeros(row, col, CV_8U);
	float c = 1;
		//255 / 230;
	float minVal = 0.0;
	float maxVal = 0.0;
	int tempa;
	int tempb;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			//std::cout << input.at<G>(i, j);
			for (int a = -n; a <= n; a++) {
				for (int b = -n; b <= n; b++) {
					//bgr
					
					if (i + a > row - 1) {  //mirroring for the border pixels
						tempa = i - a;
					}
					else if (i + a < 0) {
						tempa = -(i + a);
					}
					else {
						tempa = i + a;
					}
					if (j + b > col - 1) {
						tempb = j - b;
					}
					else if (j + b < 0) {
						tempb = -(j + b);
					}
					else {
						tempb = j + b;
					}
					lyr.at<float>(i, j) += Sy.at<float>(a + n, b + n) * input.at<C>(tempa, tempb)[2];
					lxr.at<float>(i, j) += Sx.at<float>(a + n, b + n) * input.at<C>(tempa, tempb)[2];
					lyg.at<float>(i, j) += Sy.at<float>(a + n, b + n) * input.at<C>(tempa, tempb)[1];
					lxg.at<float>(i, j) += Sx.at<float>(a + n, b + n) * input.at<C>(tempa, tempb)[1];
					lyb.at<float>(i, j) += Sy.at<float>(a + n, b + n) * input.at<C>(tempa, tempb)[0];
					lxb.at<float>(i, j) += Sx.at<float>(a + n, b + n) * input.at<C>(tempa, tempb)[0];

				}
			  
			}

			Mr.at<uchar>(i, j) = sqrt(pow(lxr.at<float>(i,j),2)+pow(lyr.at<float>(i, j),2));
			Mg.at<uchar>(i, j)= sqrt(pow(lxg.at<float>(i, j), 2) + pow(lyg.at<float>(i, j), 2));
			Mb.at<uchar>(i, j)= sqrt(pow(lxb.at<float>(i, j), 2) + pow(lyb.at<float>(i, j), 2));

			output.at<uchar>(i, j) = (Mr.at<uchar>(i, j)+ Mg.at<uchar>(i, j) + Mb.at<uchar>(i, j)) / 3;
			maxVal = max(maxVal, (float)output.at<uchar>(i, j));
			minVal = min(minVal, (float)output.at<uchar>(i, j));

		}
	}
	std::cout << maxVal << '\t' << minVal << std::endl;
	return output;
}


Mat laplacianfilter(const Mat input) {

	Mat kernel, Mr, Mg, Mb;

	int tempa;
	int tempb;
	int row = input.rows;
	int col = input.cols;
	int n = 1; // Sobel Filter Kernel N

	// Initialiazing 2 Kernel Matrix with 3x3 size for Sx and Sy
	//Fill code to initialize Sobel filter kernel matrix for Sx and Sy (Given in the lecture notes)

	kernel = Mat::zeros(3, 3, CV_32F);


	kernel.at<float>(0, 1) = 1;
	kernel.at<float>(1, 0) = 1;
	kernel.at<float>(1, 1) = -4;
	kernel.at<float>(1, 2) = 1;
	kernel.at<float>(2, 1) = 1;



	Mat output = Mat::zeros(row, col, CV_8U);
	Mr = Mat::zeros(row, col, CV_32F);
	Mg = Mat::zeros(row, col, CV_32F);
	Mb = Mat::zeros(row, col, CV_32F);

	//255 / 230;
	float minVal = 0.0;
	float maxVal = 0.0;
	float c = 15;
	uchar iminVal = 18340192;
	uchar imaxVal = -13479117;
	
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			for (int a = -n; a <= n; a++) {
				for (int b = -n; b <= n; b++) {
					if (i + a > row - 1) {  //mirroring for the border pixels
						tempa = i - a;
					}
					else if (i + a < 0) {
						tempa = -(i + a);
					}
					else {
						tempa = i + a;
					}
					if (j + b > col - 1) {
						tempb = j - b;
					}
					else if (j + b < 0) {
						tempb = -(j + b);
					}
					else {
						tempb = j + b;
					}
					Mr.at<float>(i, j) += kernel.at<float>(a + n, b + n) * input.at<C>(tempa, tempb)[0];
					Mg.at<float>(i, j) += kernel.at<float>(a + n, b + n) * input.at<C>(tempa, tempb)[1];
					Mb.at<float>(i, j) += kernel.at<float>(a + n, b + n) * input.at<C>(tempa, tempb)[2];
					
				}

			}

			output.at<uchar>(i, j) = (abs(Mr.at<float>(i, j))+ abs(Mg.at<float>(i, j))+ abs(Mb.at<float>(i, j)) ) / 3;

		}
	}
	
	return 3*output;
}
/*
Mat laplacianfilter(const Mat input) {

	Mat kernel, Sx, Sy, Ix, Iy;

	int row = input.rows;
	int col = input.cols;
	int n = 1; // Sobel Filter Kernel N

	// Initialiazing 2 Kernel Matrix with 3x3 size for Sx and Sy
	//Fill code to initialize Sobel filter kernel matrix for Sx and Sy (Given in the lecture notes)

	kernel = Mat::zeros(3, 3, CV_32F);
	Sx = Mat::zeros(3, 1, CV_32F);
	Sy = Mat::zeros(1, 3, CV_32F);
	Ix = Mat::zeros(row, col, CV_32F);
	Iy = Mat::zeros(row, col, CV_32F);

	Sx.at<float>(0) = 1;
	Sx.at<float>(1) = 1;
	Sx.at<float>(2) = -2;


	Sy.at<float>(0) = 1;
	Sy.at<float>(1) = -2;
	Sy.at<float>(2) = 1;


	Mat output = Mat::zeros(row, col, input.type());
	float c = 1;
	//255 / 230;
	float minVal = 0.0;
	float maxVal = 0.0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			//std::cout << input.at<G>(i, j);
			for (int a = -n; a <= n; a++) {
				for (int b = -n; b <= n; b++) {

					if (i + a < 0 && j + b < 0) {
						Iy.at<float>(i, j) += Sy.at<float>(b + n) * input.at<uchar>(0, 0);
						Ix.at<float>(i, j) += Sx.at<float>(a + n) * input.at<uchar>(0, 0);
					}

					else if (i + a < 0 && j + b < col) {
						Iy.at<float>(i, j) += Sy.at<float>(b + n) * input.at<uchar>(0, j + b);
						Ix.at<float>(i, j) += Sx.at<float>(a + n) * input.at<uchar>(0, j + b);
					}
					else if (j + b < 0 && i + a < row) {
						Iy.at<float>(i, j) += Sy.at<float>(b + n) * input.at<uchar>(i + a, 0);
						Ix.at<float>(i, j) += Sx.at<float>(a + n) * input.at<uchar>(i + a, 0);
					}
					else if (j + b >= col && i + a >= row) {
						Iy.at<float>(i, j) += Sy.at<float>(b + n) * input.at<uchar>(row - 1, col - 1);
						Ix.at<float>(i, j) += Sx.at<float>(a + n) * input.at<uchar>(row - 1, col - 1);
					}
					else if (j + b >= col && i + a >= 0) {
						Iy.at<float>(i, j) += Sy.at<float>(b + n) * input.at<uchar>(i + a, col - 1);
						Ix.at<float>(i, j) += Sx.at<float>(a + n) * input.at<uchar>(i + a, col - 1);
					}
					else if (j + b >= 0 && i + a >= row) {
						Iy.at<float>(i, j) += Sy.at<float>(b + n) * input.at<uchar>(row - 1, j + b);
						Ix.at<float>(i, j) += Sx.at<float>(a + n) * input.at<uchar>(row - 1, j + b);
					}
					else if (j + b >= col && i + a < 0) {
						Iy.at<float>(i, j) += Sy.at<float>(b + n) * input.at<uchar>(0, col - 1);
						Ix.at<float>(i, j) += Sx.at<float>(a + n) * input.at<uchar>(0, col - 1);
					}
					else if (j + b <= 0 && i + a >= row) {
						Iy.at<float>(i, j) += Sy.at<float>(b + n) * input.at<uchar>(row - 1, 0);
						Ix.at<float>(i, j) += Sx.at<float>(a + n) * input.at<uchar>(row - 1, 0);
					}
					else {
						Iy.at<float>(i, j) += Sy.at<float>(b + n) * input.at<uchar>(i + a, j + b);
						Ix.at<float>(i, j) += Sx.at<float>(a + n) * input.at<uchar>(i + a, j + b);
					}


					// Use mirroring boundary process
					// Find output M(x,y) = sqrt( input.at<G>(x, y)*Sx + input.at<G>(x, y)*Sy ) 

					//output.at<uchar>(i, j) = sqrt(input.at<G>(i, j) * Sx + input.at<G>(i, j) * Sy);
				}

			}

			output.at<uchar>(i, j) = abs(Ix.at<float>(i, j)+ Iy.at<float>(i, j));
			maxVal = max(maxVal, (float)output.at<uchar>(i, j));
			minVal = min(minVal, (float)output.at<uchar>(i, j));
			//std::cout << maxVal << std::endl;

			//std::cout << maxVal << '\t' << minVal << std::endl;
		}
	}
	std::cout << maxVal << '\t' << minVal << std::endl;
	return output;
}
*/
