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

Mat gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt, float k);

int main() {

	Mat input = imread("lenna.png", CV_LOAD_IMAGE_COLOR);
	
	Mat output;


	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Input", WINDOW_AUTOSIZE);
	imshow("Input", input);

	output = gaussianfilter(input, 1, 1, 1, "zero-paddle", 0.5); //Boundary process: zero-paddle, mirroring, adjustkernel

	namedWindow("Unmasked Filter", WINDOW_AUTOSIZE);
	imshow("Unmasked Filter", output);


	waitKey(0);

	return 0;
}


Mat gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt, float k) {

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

	Mat output = Mat::zeros(row, col, input.type());
	Mat Mr = Mat::zeros(row, col, CV_32F);
	Mat Mg= Mat::zeros(row, col, CV_32F);
	Mat Mb= Mat::zeros(row, col, CV_32F);

	//CV_32F
	//std::cout << input.type();
	//std::cout << output.type();
	float maxValr = 0;
	float maxValg = 0;
	float maxValb = 0;
	float minValr = 0;
	float minValg = 0;
	float minValb = 0;

	std::cout << row << '\t' << col <<  std::endl;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			//input.at<int>(i, j) = 255;

			if (!strcmp(opt, "zero-paddle")) {
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							Mr.at<float>(i, j)+= (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, j + b)[2];
							Mg.at<float>(i, j)+= (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, j + b)[1];
							Mb.at<float>(i, j)+= (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, j + b)[0];
						
							}
												
					}
				}

				maxValr = max(maxValr, Mr.at<float>(i, j));
				maxValg = max(maxValg, Mg.at<float>(i, j));
				maxValb = max(maxValb, Mb.at<float>(i, j));
				minValr = min(minValr, Mr.at<float>(i, j));
				minValg = min(minValg, Mg.at<float>(i, j));
				minValb = min(minValb, Mb.at<float>(i, j));
				/*
				maxValr = max(maxValr, Mr.at<float>(i, j));
				maxValg = max(maxValg, Mg.at<float>(i, j));
				maxValb = max(maxValb, Mb.at<float>(i, j));
				minValr = min(minValr, Mr.at<float>(i, j));
				minValg = min(minValg, Mg.at<float>(i, j));
				minValb = min(minValb, Mb.at<float>(i, j));
				*/
			}
			
			else if (!strcmp(opt, "mirroring")) {
				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						if (i + a < 0 && j+b <0) {
							Mb.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(0, 0)[0];
							Mg.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(0, 0)[1];
							Mr.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(0, 0)[2];
						
						}
						else if (i + a < 0 && j+b<col) {
							Mb.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(0, j + b)[0];
							Mg.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(0, j + b)[1];
							Mr.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(0, j + b)[2];
							
						}
						else if (j +b < 0 && i + a < row) {
							Mb.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, 0)[0];
							Mg.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, 0)[1];
							Mr.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, 0)[2];
							
						}
						else if (j + b >= col && i + a >= row) {
							Mb.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(row - 1, col - 1)[0];
							Mg.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(row - 1, col - 1)[1];
							Mr.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(row - 1, col - 1)[2];
							
						}
						else if (j + b >= col && i + a >= 0) {
							Mb.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, col - 1)[0];
							Mg.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, col - 1)[1];
							Mr.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, col - 1)[2];
							
						}
						else if (j + b >= 0 && i + a >= row) {
							Mb.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(row - 1, j + b)[0];
							Mg.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(row - 1, j + b)[1];
							Mr.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(row - 1, j + b)[2];
							
						}
						else if (j + b >= col && i + a <0) {
							Mb.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(0, col - 1)[0];
							Mg.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(0, col - 1)[1];
							Mr.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(0, col - 1)[2];
						
						}
						else if (j + b <=0  && i + a >= row) {
							Mb.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(row - 1, 0)[0];
							Mg.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(row - 1, 0)[1];
							Mr.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(row - 1, 0)[2];
							
							
						}
						else {
							Mb.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, j + b)[0];
							Mg.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, j + b)[1];
							Mr.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, j + b)[2];
							
							
						}

					}
				}
				maxValr = max(maxValr, Mr.at<float>(i, j));
				maxValg = max(maxValg, Mg.at<float>(i, j));
				maxValb = max(maxValb, Mb.at<float>(i, j));
				minValr = min(minValr, Mr.at<float>(i, j));
				minValg = min(minValg, Mg.at<float>(i, j));
				minValb = min(minValb, Mb.at<float>(i, j));
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
							Mb.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, j + b)[0];
							Mg.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, j + b)[1];
							Mr.at<float>(i, j) += (ukernel.at<float>(a + n, b + n) / (1 - k) - k * kernel.at<float>(a + n, b + n) / (1 - k)) * input.at<C>(i + a, j + b)[2];
							
						}
					}
				}
				maxValr = max(maxValr, Mr.at<float>(i, j));
				maxValg = max(maxValg, Mg.at<float>(i, j));
				maxValb = max(maxValb, Mb.at<float>(i, j));
				minValr = min(minValr, Mr.at<float>(i, j));
				minValg = min(minValg, Mg.at<float>(i, j));
				minValb = min(minValb, Mb.at<float>(i, j));
				
			}
		}
	}

	std::cout << maxValr << '\t' << maxValg << '\t' << maxValb << std::endl;
	std::cout << minValr << '\t' << minValg << '\t' << minValb << std::endl;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			
			if (Mr.at<float>(i, j) > 255 || Mg.at<float>(i, j) > 255 || Mb.at<float>(i, j) > 255) {
				//std::cout<<output.at<C>(i, j)[2] << std::endl;
				output.at<C>(i, j)[2] = input.at<C>(i, j)[2];
				output.at<C>(i, j)[1] = input.at<C>(i, j)[1];
				output.at<C>(i, j)[0] = input.at<C>(i, j)[0];
				//std::cout << (float)input.at<C>(i, j)[2] << std::endl;
					//255 * (Mr.at<float>(i, j) + minValr) / (maxValr - minValr);
			}else if(Mr.at<float>(i, j) < 0) {
				//std::cout<<output.at<C>(i, j)[2] << std::endl;
				output.at<C>(i, j)[2] = 0;
			}else if(Mg.at<float>(i, j) < 0) {
				//std::cout<<output.at<C>(i, j)[1] << std::endl;
				output.at<C>(i, j)[1] = 0;
			}else if(Mb.at<float>(i, j) < 0) {
				//std::cout<<output.at<C>(i, j)[0] << std::endl;
				output.at<C>(i, j)[0] = 0;
			}
			
			else {
				output.at<C>(i, j)[2] = Mr.at<float>(i, j);
				output.at<C>(i, j)[1] = Mg.at<float>(i, j);
				output.at<C>(i, j)[0] = Mb.at<float>(i, j);
			}
		}
	}
	return output;
}