#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>       /* exp */
#include <time.h>
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

Mat gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt);

int main() {

	Mat input = imread("lenna.png", CV_LOAD_IMAGE_COLOR);
	Mat input_gray;
	Mat output;
	clock_t start, end;

	cvtColor(input, input_gray, CV_RGB2GRAY);



	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);
	
	output = gaussianfilter(input_gray, 1, 1, 1, "zero-paddle"); //Boundary process: zero-paddle, mirroring, adjustkernel


	namedWindow("Gaussian Filter", WINDOW_AUTOSIZE);
	imshow("Gaussian Filter", output);


	waitKey(0);

	return 0;
}


Mat gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt) {
	
	Mat wkernel, skernel;

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;
	float wdenom, sdenom;
	float kernelvalue;

	// Initialiazing Kernel Matrix 
	wkernel = Mat::zeros(kernel_size, 1, CV_32F);
	skernel = Mat::zeros(1, kernel_size, CV_32F);

	wdenom = 0.0;
	sdenom = 0.0;
	clock_t start, end;

	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
		float value1 = exp(-(pow(a, 2) / (2 * pow(sigmaS, 2))));
		wkernel.at<float>(a + n) = value1;
		wdenom += value1;
		//std::cout << wkernel.at<float>(a + n);
	}
	for (int b = -n; b <= n; b++) {
		float value2 = exp(-(pow(b, 2) / (2 * pow(sigmaT, 2))));
		skernel.at<float>(b + n) = value2;
		sdenom += value2;
		//std::cout << skernel.at<float>(b + n) << std::endl;
	}



	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)

		wkernel.at<float>(a + n) /= wdenom;


	}

	for (int b = -n; b <= n; b++) {

		skernel.at<float>(b + n) /= sdenom;
	}


	Mat output = Mat::zeros(row, col, input.type());

	//CV_32F
	//std::cout << input.type();
	//std::cout << output.type();
	
	std::cout << row << '\t' << col <<  std::endl;
	start = clock();
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			//input.at<int>(i, j) = 255;

			if (!strcmp(opt, "zero-paddle")) {
				for (int a = -n; a <= n; a++) {
					float u = 0;
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							u += skernel.at<float>(b + n)* input.at<uchar>(i + a, j + b);
							}
					}
					output.at<uchar>(i, j) += wkernel.at<float>(a + n) * u;
				}
			}
			
			else if (!strcmp(opt, "mirroring")) {
				for (int a = -n; a <= n; a++) {
					float u = 0;
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
						//output.at<uchar>(i, j) += wkernel.at<float>(a + n, b + n) * input.at<uchar>(tempa, tempb);
						u += skernel.at<float>(b + n)*input.at<uchar>(tempa, tempb);
			
					}
					output.at<uchar>(i, j) += wkernel.at<float>(a + n) * u;
				}
			}


			else if (!strcmp(opt, "adjustkernel")) {
				
				float sum1 = 0.0;
				float sum2 = 0.0;
				for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
					float u = 0;
					float h = 0;
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							u += exp(-(pow(b, 2) / (2 * pow(sigmaT, 2)))) * (float)(input.at<G>(i + a, j + b));
							h += exp(-(pow(b, 2) / (2 * pow(sigmaT, 2))));

						}
					}
					sum1 += exp(-(pow(a, 2) / (2 * pow(sigmaS, 2)))) * u;
					sum2 += exp(-(pow(a, 2) / (2 * pow(sigmaS, 2)))) * h;
				}
				output.at<uchar>(i, j) += (G)(sum1 / sum2);
				
			}
		}
	}
	end = clock();
	printf("Time: %lf\n", (double)(end - start) / CLOCKS_PER_SEC);
	return output;
}