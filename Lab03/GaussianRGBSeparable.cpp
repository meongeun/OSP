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
	
	output = gaussianfilter(input, 1, 1, 1, "mirroring"); //Boundary process: zero-paddle, mirroring, adjustkernel

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
	//Mat r = Mat::zeros(row, col, CV_32FC3);
	
	//CV_32F
	//std::cout << input.type();
	//std::cout << output.type();
	
	std::cout << row << '\t' << col <<  std::endl;
	clock_t start = clock();
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			//input.at<int>(i, j) = 255;
			float sum1 = 0;
			float sum2 = 0;
			float sum3 = 0;
			if (!strcmp(opt, "zero-paddle")) {
				for (int a = -n; a <= n; a++) {
					float r = 0;
					float g = 0;
					float b = 0;
					//rgb = 0;
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							//r += skernel.at<float>(b + n) * float(input.at<C>(i + a, j + b)[0]);
							//g += skernel.at<float>(b + n) * float(input.at<C>(i + a, j + b)[1]);
							//b += skernel.at<float>(b + n) * float(input.at<C>(i + a, j + b)[2]);
		
							
							
							output.at<C>(i, j)[0] += wkernel.at<float>(a + n) * skernel.at<float>(b + n) * input.at<C>(i + a, j + b)[0];
							output.at<C>(i, j)[1] += wkernel.at<float>(a + n) * skernel.at<float>(b + n) * input.at<C>(i + a, j + b)[1];
							output.at<C>(i, j)[2] += wkernel.at<float>(a + n) * skernel.at<float>(b + n) * input.at<C>(i + a, j + b)[2];
							
							
							}
					}
					//sum1 += wkernel.at<float>(a + n) * r;
					//sum2 += wkernel.at<float>(a + n) * g;
					//sum3 += wkernel.at<float>(a + n) * b;

				}
				//output.at<C>(i, j)[0] = sum1;
				//output.at<C>(i, j)[1] = sum2;
				//output.at<C>(i, j)[2] = sum3;
			}
			
			else if (!strcmp(opt, "mirroring")) {
				float sum1 = 0;
				float sum2 = 0;
				float sum3 = 0;
				for (int a = -n; a <= n; a++) {
					float r = 0.0;
					float g = 0.0;
					float b = 0.0;
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
						
						output.at<C>(i, j)[0] += wkernel.at<float>(a + n) * skernel.at<float>(b + n) * input.at<C>(tempa, tempb)[0];
						output.at<C>(i, j)[1] += wkernel.at<float>(a + n) * skernel.at<float>(b + n) * input.at<C>(tempa, tempb)[1];
						output.at<C>(i, j)[2] += wkernel.at<float>(a + n) * skernel.at<float>(b + n) * input.at<C>(tempa, tempb)[2];
						
						//r += skernel.at<float>(b + n) * (float)input.at<C>(tempa, tempb)[2];
						//g += skernel.at<float>(b + n) * (float)input.at<C>(tempa, tempb)[1];
						//b += skernel.at<float>(b + n) * (float)input.at<C>(tempa, tempb)[0];
						
					}
					//sum1 += wkernel.at<float>(a + n) * r;
					//sum2+= wkernel.at<float>(a + n) * g;
					//sum3 += wkernel.at<float>(a + n) * b;
				}
				//output.at<C>(i, j)[2] = sum1;
				//output.at<C>(i, j)[1] = sum2;
				//output.at<C>(i, j)[0] = sum3;
			}


			else if (!strcmp(opt, "adjustkernel")) {
				wdenom = 0.0;
				sdenom = 0.0;
				/**/
				for (int a = -n; a <= n; a++) {

					if (i + a >= 0 && i + a < row) {
						float value1 = exp(-(pow(a, 2) / (2 * pow(sigmaS, 2))));
						wkernel.at<float>(a + n) = value1;
						wdenom += value1;
					}
					else {
						wkernel.at<float>(a + n) = 0.0;
					}


				}
				for (int a = -n; a <= n; a++) {

					wkernel.at<float>(a + n) /= wdenom;

				}
				for (int b = -n; b <= n; b++) {
					if (j + b >= 0 && j + b < col) {
						float value1 = exp(-(pow(b, 2) / (2 * pow(sigmaT, 2))));
						skernel.at<float>(b + n) = value1;
						sdenom += value1;
					}
					else {
						skernel.at<float>(b + n) = 0.0;
					}

				}


				for (int b = -n; b <= n; b++) {
					skernel.at<float>(b + n) /= sdenom;
				}


				for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
					for (int b = -n; b <= n; b++) {
						if (i + a >= 0 && j + b >= 0 && i + a < row && j + b < col) {
							output.at<C>(i, j)[0] += wkernel.at<float>(a + n) * skernel.at<float>(b + n) * input.at<C>(i + a, j + b)[0];
							output.at<C>(i, j)[1] += wkernel.at<float>(a + n) * skernel.at<float>(b + n) * input.at<C>(i + a, j + b)[1];
							output.at<C>(i, j)[2] += wkernel.at<float>(a + n) * skernel.at<float>(b + n) * input.at<C>(i + a, j + b)[2];
							
						}
					}
				}
				
			}
		}
	}
	clock_t end = clock();
	printf("Time: %lf\n", (double)(end - start) / CLOCKS_PER_SEC);
	return output;
}