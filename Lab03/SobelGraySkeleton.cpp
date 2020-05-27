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
Mat laplacianfilter1(const Mat input);
Mat laplacianfilter2(const Mat input);

int main() {

	Mat input = imread("lenna.png", IMREAD_COLOR);
	Mat input_gray;
	Mat output1, output2, output3;


	cvtColor(input, input_gray, COLOR_RGB2GRAY);



	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);
	output1 = sobelfilter(input_gray); //Boundary process: zero-paddle, mirroring, adjustkernel
	output2 = laplacianfilter1(input_gray);
	output3 = laplacianfilter2(input_gray);
	
	namedWindow("Sobel Filter", WINDOW_AUTOSIZE);
	imshow("Sobel Filter", output1);

	namedWindow("Laplacian Filter1", WINDOW_AUTOSIZE);
	imshow("Laplacian Filter1", output2);

	namedWindow("Laplacian Filter2", WINDOW_AUTOSIZE);
	imshow("Laplacian Filter2", output3);
	
	waitKey(0);

	return 0;
}


Mat sobelfilter(const Mat input) {

	Mat kernel,Sx,Sy, Ix, Iy;

	int row = input.rows;
	int col = input.cols;
	int n = 1; // Sobel Filter Kernel N
	
	// Initialiazing 2 Kernel Matrix with 3x3 size for Sx and Sy
	//Fill code to initialize Sobel filter kernel matrix for Sx and Sy (Given in the lecture notes)
	
	kernel = Mat::zeros(3, 3, CV_32F);
	Sx = Mat::zeros(3, 3, CV_32F);
	Sy = Mat::zeros(3, 3, CV_32F);
	Ix = Mat::zeros(row, col, CV_32F);
	Iy = Mat::zeros(row, col, CV_32F);

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

	Mat output = Mat::zeros(row, col, input.type());
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
					Iy.at<float>(i, j) += Sy.at<float>(a + n, b + n) * input.at<uchar>(tempa, tempb);
					Ix.at<float>(i, j) += Sx.at<float>(a + n, b + n) * input.at<uchar>(tempa, tempb);
				}
			  
			}

			output.at<uchar>(i, j) = sqrt(pow(Ix.at<float>(i,j),2)+pow(Iy.at<float>(i, j),2));
			maxVal = max(maxVal, (float)output.at<uchar>(i, j));
			minVal = min(minVal, (float)output.at<uchar>(i, j));

		}
	}

	std::cout << maxVal << '\t' << minVal << std::endl;
	return output;
}


Mat laplacianfilter1(const Mat input) {

	Mat kernel;

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

	Mat output = Mat::zeros(row, col, CV_32F);
	Mat goutput = Mat::zeros(row, col, input.type());
	int tempa;
	int tempb;
	//255 / 230;
	float minVal = 0.0;
	float maxVal = 0.0;
	float c = 15;
	uchar iminVal = 18340192;
	uchar imaxVal = -13479117;
	float maxValo = -51384;
	float minValo = 137491;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			//std::cout << (float)input.at<uchar>(i, j)<<'\t';
			if (iminVal > input.at<uchar>(i, j)) {
				iminVal = input.at<uchar>(i, j);
			}
			else if (imaxVal < input.at<uchar>(i, j)) {
				imaxVal = input.at<uchar>(i, j);
			}
			
			//std::cout << input.at<G>(i, j);
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

					output.at<float>(i, j) += kernel.at<float>(a + n, b + n) * (float)input.at<uchar>(tempa, tempb);
					maxVal = max(maxVal, output.at<float>(i, j));
					minVal = min(minVal, output.at<float>(i, j));
					
				}

			}
			goutput.at<uchar>(i, j) = abs(output.at<float>(i, j));
			maxValo = max(maxValo, abs(output.at<float>(i, j)) );
			minValo = min(minValo, abs(output.at<float>(i, j)) );

			
		}
	}


	std::cout << maxValo << '\t' << minValo << '\t' << std::endl;

	return 2.5*goutput;
}

Mat laplacianfilter2(const Mat input) {

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

	int tempa;
	int tempb;
	Mat output = Mat::zeros(row, col, input.type());
	float c = 1;
	//255 / 230;
	float minValx = 0.0;
	float minValy = 0.0;
	float maxValx = 0.0;
	float maxValy = 0.0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			//std::cout << input.at<G>(i, j);
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
				
					Iy.at<float>(i, j) += Sy.at<float>(b + n) * input.at<uchar>(tempa, tempb);
					Ix.at<float>(i, j) += Sx.at<float>(a + n) * input.at<uchar>(tempa, tempb);
					maxValx = max(maxValx, Sx.at<float>(a + n) * input.at<uchar>(tempa, tempb));
					maxValy = max(maxValx, Sy.at<float>(b + n) * input.at<uchar>(tempa, tempb));
					minValx = min(minValx, Sx.at<float>(a + n) * input.at<uchar>(tempa, tempb));
					minValy = min(minValy, Sy.at<float>(b + n) * input.at<uchar>(tempa, tempb));

				}

			}

			output.at<uchar>(i, j) = abs(Ix.at<float>(i, j)+ Iy.at<float>(i, j));

		}
	}


	return output;
}

