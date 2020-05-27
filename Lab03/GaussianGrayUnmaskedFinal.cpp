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

Mat unmaskedfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt, float k);

int main() {

	Mat input = imread("lenna.png", CV_LOAD_IMAGE_COLOR);
	Mat input_gray;
	Mat output;


	cvtColor(input, input_gray, CV_RGB2GRAY);



	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);
	output = unmaskedfilter(input_gray, 1, 1, 1, "zero-paddle", 0.8); //Boundary process: zero-paddle, mirroring, adjustkernel

	namedWindow("Unmasked Filter", WINDOW_AUTOSIZE);
	imshow("Unmasked Filter", output);


	waitKey(0);

	return 0;
}


Mat unmaskedfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt, float k) {

	Mat kernel;

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;
	float denom;
	float kernelvalue;

	Mat ukernel;
	ukernel = Mat::zeros(3, 3, CV_32F);
	ukernel.at<float>(1, 1) = 1;

	// Initialiazing Kernel Matrix 
	kernel = Mat::zeros(kernel_size, kernel_size, CV_32F);


	denom = 0.0;
	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
		for (int b = -n; b <= n; b++) {
			float value1 = exp(-(pow(a, 2) / (2 * pow(sigmaS, 2))) - (pow(b, 2) / (2 * pow(sigmaT, 2))));
			kernel.at<float>(a + n, b + n) = value1;
			denom += value1;
		}
	}

	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
		for (int b = -n; b <= n; b++) {
			kernel.at<float>(a + n, b + n) /= denom;
		}
	}

	Mat output = Mat::zeros(row, col, CV_32F);
	Mat goutput = Mat::zeros(row, col, input.type());

	//CV_32F
	//std::cout << input.type();
	//std::cout << output.type();
	
	std::cout << row << '\t' << col <<  std::endl;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			//input.at<int>(i, j) = 255;

			if (!strcmp(opt, "zero-paddle")) {
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							output.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<uchar>(i + a, j + b);
							
							}

					}
				}
			}
			
			else if (!strcmp(opt, "mirroring")) {
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
						output.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<uchar>(tempa, tempb);
					}
				}
			}


			else if (!strcmp(opt, "adjustkernel")) {
				denom = 0.0;
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						if (i + a >= 0 && j + b >= 0 && i + a < row && j + b < col) {
							float value1 = exp(-(pow(a, 2) / (2 * pow(sigmaS, 2))) - (pow(b, 2) / (2 * pow(sigmaT, 2))));
							kernel.at<float>(a + n, b + n) = value1;
							denom += value1;
						}
						else {
							kernel.at<float>(a + n, b + n) = 0.0;
						}
						
					}
				}
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						kernel.at<float>(a + n, b + n) /= denom;
					}
				}

				for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
					for (int b = -n; b <= n; b++) {
						if (i + a >= 0 && j + b >= 0 && i + a < row && j + b < col) {
							output.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<uchar>(i + a, j + b);
							
						}
					}
				}
				
			}
		}
	}

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {

			if (output.at<float>(i, j) > 255 ) {
				goutput.at<uchar>(i, j) = input.at<uchar>(i, j);					
			}
			else if (output.at<float>(i, j) < 0) {
				//std::cout<<output.at<C>(i, j)[2] << std::endl;
				goutput.at<uchar>(i, j) = 0;
			}
			else {
				goutput.at<uchar>(i, j) = output.at<float>(i, j);
			}
			
		}
	}
	return goutput;
}