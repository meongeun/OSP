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



	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Input", WINDOW_AUTOSIZE);
	imshow("Input", input);
	
	output = gaussianfilter(input, 1, 1, 1, "adjustkernel"); //Boundary process: zero-paddle, mirroring, adjustkernel

	namedWindow("Gaussian Filter", WINDOW_AUTOSIZE);
	imshow("Gaussian Filter", output);


	waitKey(0);

	return 0;
}


Mat gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt) {

	Mat kernel;
	clock_t start, end;
	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;
	float denom;
	float kernelvalue;

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
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							
							output.at<C>(i, j)[0] += kernel.at<float>(a + n, b + n) * input.at<C>(i + a, j + b)[0];
							output.at<C>(i, j)[1] += kernel.at<float>(a + n, b + n) * input.at<C>(i + a, j + b)[1];
							output.at<C>(i, j)[2] += kernel.at<float>(a + n, b + n) * input.at<C>(i + a, j + b)[2];
							//std::cout << i + a <<'\t'<< j + b << std::endl;
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

						output.at<C>(i, j)[0] += kernel.at<float>(a + n, b + n) * input.at<C>(tempa, tempb)[0];
						output.at<C>(i, j)[1] += kernel.at<float>(a + n, b + n) * input.at<C>(tempa, tempb)[1];
						output.at<C>(i, j)[2] += kernel.at<float>(a + n, b + n) * input.at<C>(tempa, tempb)[2];

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
				float sumr = 0.0;
				float sumg = 0.0;
				float sumb = 0.0;
				float sum = 0.0;

				for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							sumr += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b)[2]);
							sumg += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b)[1]);
							sumb += kernel.at<float>(a + n, b + n) * (float)(input.at<C>(i + a, j + b)[0]);
							sum += kernel.at<float>(a + n, b + n);
						}
					}
				}
				output.at<C>(i, j)[0] += (G)(sumb / sum);
				output.at<C>(i, j)[1] += (G)(sumg / sum);
				output.at<C>(i, j)[2] += (G)(sumr / sum);
				
			}
		}
	}
	end = clock();
	printf("Time: %lf\n", (double)(end - start) / CLOCKS_PER_SEC);
	return output;
}