#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <vector>
#include <algorithm>
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

Mat Add_salt_pepper_Noise(const Mat input, float ps, float pp);
Mat Salt_pepper_noise_removal_Gray(const Mat input, int n, const char *opt);
Mat Salt_pepper_noise_removal_RGB(const Mat input, int n, const char *opt);

int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input_gray;

	// check for validation
	if (!input.data) {
		printf("Could not open\n");
		return -1;
	}

	cvtColor(input, input_gray, CV_RGB2GRAY);	// convert RGB to Grayscale
	imwrite("lena_gray.jpg", input_gray);
												// Add noise to original image
	Mat noise_Gray = Add_salt_pepper_Noise(input_gray, 0.1f, 0.1f);
	Mat noise_RGB = Add_salt_pepper_Noise(input, 0.1f, 0.1f);

	// Denoise, using median filter
	int window_radius = 2;
	Mat Denoised_Gray = Salt_pepper_noise_removal_Gray(noise_Gray, window_radius, "zero-padding");
	Mat Denoised_RGB = Salt_pepper_noise_removal_RGB(noise_RGB, window_radius, "zero-padding");

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Impulse Noise (Grayscale)", WINDOW_AUTOSIZE);
	imshow("Impulse Noise (Grayscale)", noise_Gray);

	namedWindow("Impulse Noise (RGB)", WINDOW_AUTOSIZE);
	imshow("Impulse Noise (RGB)", noise_RGB);

	namedWindow("Denoised (Grayscale)", WINDOW_AUTOSIZE);
	imshow("Denoised (Grayscale)", Denoised_Gray);

	namedWindow("Denoised (RGB)", WINDOW_AUTOSIZE);
	imshow("Denoised (RGB)", Denoised_RGB);

	waitKey(0);

	return 0;
}

Mat Add_salt_pepper_Noise(const Mat input, float ps, float pp)
{
	Mat output = input.clone();
	RNG rng;

	int amount1 = (int)(output.rows * output.cols * pp);
	int amount2 = (int)(output.rows * output.cols * ps);

	int x, y;

	// Grayscale image
	if (output.channels() == 1) {
		for (int counter = 0; counter < amount1; ++counter)
			output.at<G>(rng.uniform(0, output.rows), rng.uniform(0, output.cols)) = 0;

		for (int counter = 0; counter < amount2; ++counter)
			output.at<G>(rng.uniform(0, output.rows), rng.uniform(0, output.cols)) = 255;
	}
	// Color image	
	else if (output.channels() == 3) {
		for (int counter = 0; counter < amount1; ++counter) {
			x = rng.uniform(0, output.rows);
			y = rng.uniform(0, output.cols);
			output.at<C>(x, y)[0] = 0;

			x = rng.uniform(0, output.rows);
			y = rng.uniform(0, output.cols);
			output.at<C>(x, y)[1] = 0;

			x = rng.uniform(0, output.rows);
			y = rng.uniform(0, output.cols);
			output.at<C>(x, y)[2] = 0;
		}

		for (int counter = 0; counter < amount2; ++counter) {
			x = rng.uniform(0, output.rows);
			y = rng.uniform(0, output.cols);
			output.at<C>(x, y)[0] = 255;

			x = rng.uniform(0, output.rows);
			y = rng.uniform(0, output.cols);
			output.at<C>(x, y)[1] = 255;

			x = rng.uniform(0, output.rows);
			y = rng.uniform(0, output.cols);
			output.at<C>(x, y)[2] = 255;
		}
	}

	return output;
}

Mat Salt_pepper_noise_removal_Gray(const Mat input, int n, const char *opt) {

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int xs = 0;
	int median = kernel_size*kernel_size/2;		// index of median value

	// initialize median filter kernel
	Mat kernel = Mat::zeros(kernel_size * kernel_size, 1, input.type());
	
	Mat output = Mat::zeros(row, col, input.type());

	
	// convolution
	
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			std::vector<float> local;
			if (!strcmp(opt, "zero-padding")) {
				
				int k = 0;
				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							kernel.at<G>((kernel_size) * (x + n) + y + n) = input.at<G>(i + x, j + y);
						}
						else {
							kernel.at<G>((kernel_size) * (x + n) + y + n) = 0;
						}
						
					}
				}
				sort(kernel, kernel, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);

			output.at<G>(i, j) = kernel.at<G>(median, 0);
			}

			else if (!strcmp(opt, "mirroring")) {

				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						if (i + x > row - 1 && j+y > col-1) {  //mirroring for the border pixels
							kernel.at<G>((kernel_size) * (x + n) + y + n) = input.at<G>(row-1, col-1);
						}
						else if (i + x < 0 && j+y <0) {
							kernel.at<G>((kernel_size) * (x + n) + y + n) = input.at<G>(0, 0);
						}
						else if(i + x < 0 && j + y > col-1) {
							kernel.at<G>((kernel_size) * (x + n) + y + n) = input.at<G>(0, col-1);
						}
						else if (i + x > row-1 && j + y <0) {
							kernel.at<G>((kernel_size) * (x + n) + y + n) = input.at<G>(row-1, 0);
						}
						else if (j + y < 0) {
							kernel.at<G>((kernel_size) * (x + n) + y + n) = input.at<G>(i + x, 0);
						}
						else if(i+x<0){
							kernel.at<G>((kernel_size) * (x + n) + y + n) = input.at<G>(0, j + y);
						}
						else if (j + y > col-1) {
							kernel.at<G>((kernel_size) * (x + n) + y + n) = input.at<G>(i + x, col-1);
						}
						else if (i + x > row-1) {
							kernel.at<G>((kernel_size) * (x + n) + y + n) = input.at<G>(row-1, j + y);
						}
						else {
							kernel.at<G>((kernel_size) * (x + n) + y + n) = input.at<G>(i+x, j + y);
						}
					}
				}
				sort(kernel, kernel, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);

				output.at<G>(i, j) = kernel.at<G>(median, 0);
			}

			else if (!strcmp(opt, "adjustkernel")) {
				Mat new_kernel;
				int median;
				if (i < n && j < n) {
					kernel_size = (1+n + i) * (1+n + j);
					xs = n + i+1;
					new_kernel = Mat::zeros(kernel_size, 1, CV_32F);
					median = kernel_size / 2;
					//std::cout << new_kernel << std::endl;
					for (int x = -i; x <= n; x++) { // for each kernel window
						for (int y = -j; y <= n; y++) {
							if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
								//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
								new_kernel.at<float>((xs) * (y + j) + x + i) = input.at<G>(i + x, j + y);
							}
						}
					}
				}
				
				else if (i < n && j >= n && j<= col-1-n) {
					kernel_size = (1 + n + i) * (2*n+1);
					xs = n + i + 1;
					new_kernel = Mat::zeros(kernel_size, 1, CV_32F);
					median = kernel_size / 2;
					//std::cout << new_kernel << std::endl;
					for (int x = -i; x <= n; x++) { // for each kernel window
						for (int y = -n; y <= n; y++) {
							if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
								//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
								new_kernel.at<float>((xs) * (y + n) + x + i) = input.at<G>(i + x, j + y);
							}
						}
					}
				}
				
				else if (i < n  && j > col - 1 - n) {
					kernel_size = (1 + n + i) * (col-j+n);
					xs = n + i + 1;
					new_kernel = Mat::zeros(kernel_size, 1, CV_32F);
					median = kernel_size / 2;
					//std::cout << new_kernel << std::endl;
					for (int x = -i; x <= n; x++) { // for each kernel window
						for (int y = -n; y <= j; y++) {
							if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
								//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
								new_kernel.at<float>((xs) * (y + n) + x + i) = input.at<G>(i + x, j + y);
							}
						}
					}
				}
				else if (i<= row-1-n &&i >= n && j < n) {
					kernel_size = (2*n+1) * (j+1+n);
					xs = 2*n+1;
					new_kernel = Mat::zeros(kernel_size, 1, CV_32F);
					median = kernel_size / 2;
					//std::cout << new_kernel << std::endl;
					for (int x = -n; x <= n; x++) { // for each kernel window
						for (int y = -j; y <= n; y++) {
							if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
								//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
								new_kernel.at<float>((xs) * (y + j) + x + n) = input.at<G>(i + x, j + y);
							}
						}
					}
				}
				else if (i <= row - 1 - n && i >= n && j >= n && col-1-n >= j) {
					kernel_size = (2 * n + 1) * (2*n+1);
					xs = 2 * n + 1;
					new_kernel = Mat::zeros(kernel_size, 1, CV_32F);
					median = kernel_size / 2;
					//std::cout << new_kernel << std::endl;
					for (int x = -n; x <= n; x++) { // for each kernel window
						for (int y = -n; y <= n; y++) {
							if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
								//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
								new_kernel.at<float>((xs) * (y + n) + x + n) = input.at<G>(i + x, j + y);
							}
						}
					}
				}
				else if (i <= row - 1 - n && i >= n && col - 1 - n < j) {
					kernel_size = (col-j+n) * (2 * n + 1);
					xs = 2 * n + 1;
					new_kernel = Mat::zeros(kernel_size, 1, CV_32F);
					median = kernel_size / 2;
					//std::cout << new_kernel << std::endl;
					for (int x = -n; x <= n; x++) { // for each kernel window
						for (int y = -n; y <= j; y++) {
							if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
								//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
								new_kernel.at<float>((xs) * (y + n) + x + n) = input.at<G>(i + x, j + y);
							}
						}
					}
				}
				else if (i > row - 1 - n && j < n ) {
					kernel_size = (row-i+n) * (j+1+n);
					xs = row-i+n;
					new_kernel = Mat::zeros(kernel_size, 1, CV_32F);
					median = kernel_size / 2;
					//std::cout << new_kernel << std::endl;
					for (int x = n; x <= i; x++) { // for each kernel window
						for (int y = -j; y <= n; y++) {
							if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
								//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
								new_kernel.at<float>((xs) * (y + j) + x + n) = input.at<G>(i + x, j + y);
							}
						}
					}
				}
				else if (i > row - 1 - n && j >= n && j<=col-1-n) {
					kernel_size = (2*n+1) * (row-i+n);
					xs = row - i + n;
					new_kernel = Mat::zeros(kernel_size, 1, CV_32F);
					median = kernel_size / 2;
					//std::cout << new_kernel << std::endl;
					for (int x = -n; x <= i; x++) { // for each kernel window
						for (int y = -n; y <= n; y++) {
							if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
								//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
								new_kernel.at<float>((xs) * (y + n) + x + n) = input.at<G>(i + x, j + y);
							}
						}
					}
				}
				else if (i > row - 1 - n &&  j > col - 1 - n) {
					kernel_size = (col-j+n) * (row - i + n);
					xs = row - i + n;
					new_kernel = Mat::zeros(kernel_size, 1, CV_32F);
					median = kernel_size / 2;
					//std::cout << new_kernel << std::endl;
					for (int x = -n; x <= i; x++) { // for each kernel window
						for (int y = -n; y <= j; y++) {
							if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
								//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
								new_kernel.at<float>((xs) * (y + n) + x + n) = input.at<G>(i + x, j + y);
							}
						}
					}
				}
				
				sort(new_kernel, new_kernel, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
				//std::cout << median << std::endl;
				output.at<G>(i, j) = new_kernel.at<float>(median);
				
				
			}
		}
	}

	return output;
}

Mat Salt_pepper_noise_removal_RGB(const Mat input, int n, const char *opt) {

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int xs = 0;
	int median = (kernel_size)*(kernel_size)/2;		// index of median value
	int channel = input.channels();

	// initialize ( (TypeX with 3 channel) - (TypeX with 1 channel) = 16 )
	// ex) CV_8UC3 - CV_8U = 16
	Mat kernelr = Mat::zeros(kernel_size * kernel_size, channel, input.type() - 16);
	Mat kernelg = Mat::zeros(kernel_size * kernel_size, channel, input.type() - 16);
	Mat kernelb = Mat::zeros(kernel_size * kernel_size, channel, input.type() - 16);
	
	Mat output = Mat::zeros(row, col, input.type());
	//std::cout << kernel.type();	
	// convolution
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
					
			if (!strcmp(opt, "zero-padding")) {

				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						
							if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
								kernelr.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(i + x, j + y)[2];
								kernelg.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(i + x, j + y)[1];
								kernelb.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(i + x, j + y)[0];
								
							}
						
						
						
					}
				}
				// Sort the kernels in ascending order
				sort(kernelr, kernelr, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
				sort(kernelg, kernelg, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
				sort(kernelb, kernelb, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);

				//(kernel_size)*(kernel_size)*z+(kernel_size)*(x + n) + y + n
				output.at<C>(i, j)[0] = kernelb.at<G>(median);
				output.at<C>(i, j)[1] = kernelg.at<G>(median);
				output.at<C>(i, j)[2] = kernelr.at<G>(median);
			}

			else if (!strcmp(opt, "mirroring")) {
				
				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						if (i + x > row - 1 && j + y > col - 1) {  //mirroring for the border pixels
							kernelr.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(row - 1, col - 1)[2];
							kernelg.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(row - 1, col - 1)[1];
							kernelb.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(row - 1, col - 1)[0];
							
						}
						else if (i + x < 0 && j + y < 0) {
							kernelr.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(0, 0)[2];
							kernelg.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(0, 0)[1];
							kernelb.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(0, 0)[0];
							
						}
						else if (i + x < 0 && j + y > col - 1) {
							kernelr.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(0, col - 1)[2];
							kernelg.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(0, col - 1)[1];
							kernelb.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(0, col - 1)[0];
							

						}
						else if (i + x > row - 1 && j + y < 0) {
							kernelr.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(row - 1, 0)[2];
							kernelg.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(row - 1, 0)[1];
							kernelb.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(row - 1, 0)[0];
							
						}
						else if (j + y < 0) {
							kernelr.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(i + x, 0)[2];
							kernelg.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(i + x, 0)[1];
							kernelb.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(i + x, 0)[0];
							
						}
						else if (i + x < 0) {
							kernelr.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(0, j + y)[2];
							kernelg.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(0, j + y)[1];
							kernelb.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(0, j + y)[0];
							
						}
						else if (j + y > col - 1) {
							kernelr.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(i + x, col - 1)[2];
							kernelg.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(i + x, col - 1)[1];
							kernelb.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(i + x, col - 1)[0];
						
						}
						else if (i + x > row - 1) {
							kernelr.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(row - 1, j + y)[2];
							kernelg.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(row - 1, j + y)[1];
							kernelb.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(row - 1, j + y)[0];
						
						}
						else {
							kernelr.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(i + x, j + y)[2];
							kernelg.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(i + x, j + y)[1];
							kernelb.at<G>((kernel_size) * (x + n) + y + n) = input.at<C>(i + x, j + y)[0];
							
						}

					}
				}
				// Sort the kernels in ascending order
				sort(kernelr, kernelr, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
				sort(kernelg, kernelg, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
				sort(kernelb, kernelb, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);

				//(kernel_size)*(kernel_size)*z+(kernel_size)*(x + n) + y + n
				output.at<C>(i, j)[0] = kernelb.at<G>(median);
				output.at<C>(i, j)[1] = kernelg.at<G>(median);
				output.at<C>(i, j)[2] = kernelr.at<G>(median);
			}

			else if (!strcmp(opt, "adjustkernel")) {

			
			int median;
			if (i < n && j < n) {
				kernel_size = (1 + n + i) * (1 + n + j);
				xs = n + i + 1;
				kernelr = Mat::zeros(kernel_size, 1, CV_32F);
				kernelg = Mat::zeros(kernel_size, 1, CV_32F);
				kernelb = Mat::zeros(kernel_size, 1, CV_32F);
				median = kernel_size / 2;
				//std::cout << new_kernel << std::endl;
				
				for (int x = -i; x <= n; x++) { // for each kernel window
					for (int y = -j; y <= n; y++) {
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							
							kernelr.at<float>((xs) * (y + j) + x + i) = input.at<C>(i + x, j + y)[2];
							kernelg.at<float>((xs)* (y + j) + x + i) = input.at<C>(i + x, j + y)[1];
							kernelb.at<float>((xs)* (y + j) + x + i) = input.at<C>(i + x, j + y)[0];
						}
					}
				}
			}
			
			else if (i < n && j >= n && j <= col - 1 - n) {
				kernel_size = (1 + n + i) * (2 * n + 1);
				xs = n + i + 1;
				kernelr = Mat::zeros(kernel_size, 1, CV_32F);
				kernelg = Mat::zeros(kernel_size, 1, CV_32F);
				kernelb = Mat::zeros(kernel_size, 1, CV_32F);
				median = kernel_size / 2;
				//std::cout << new_kernel << std::endl;
				for (int x = -i; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
							kernelr.at<float>((xs) * (y + n) + x + i) = input.at<C>(i + x, j + y)[2];
							kernelg.at<float>((xs)* (y + n) + x + i) = input.at<C>(i + x, j + y)[1];
							kernelb.at<float>((xs)* (y + n) + x + i) = input.at<C>(i + x, j + y)[0];
						}
					}
				}
			}

			else if (i < n && j > col - 1 - n) {
				kernel_size = (1 + n + i) * (col - j + n);
				xs = n + i + 1;
				kernelr = Mat::zeros(kernel_size, 1, CV_32F);
				kernelg = Mat::zeros(kernel_size, 1, CV_32F);
				kernelb = Mat::zeros(kernel_size, 1, CV_32F);
				median = kernel_size / 2;
				//std::cout << new_kernel << std::endl;
				for (int x = -i; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= j; y++) {
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
							kernelr.at<float>((xs)* (y + n) + x + i) = input.at<C>(i + x, j + y)[2];
							kernelg.at<float>((xs)* (y + n) + x + i) = input.at<C>(i + x, j + y)[1];
							kernelb.at<float>((xs)* (y + n) + x + i) = input.at<C>(i + x, j + y)[0];
							
						}
					}
				}
			}
			else if (i <= row - 1 - n && i >= n && j < n) {
				kernel_size = (2 * n + 1) * (j + 1 + n);
				xs = 2 * n + 1;
				kernelr = Mat::zeros(kernel_size, 1, CV_32F);
				kernelg = Mat::zeros(kernel_size, 1, CV_32F);
				kernelb = Mat::zeros(kernel_size, 1, CV_32F);
				median = kernel_size / 2;
				//std::cout << new_kernel << std::endl;
				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -j; y <= n; y++) {
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
							kernelr.at<float>((xs)* (y + j) + x + n) = input.at<C>(i + x, j + y)[2];
							kernelg.at<float>((xs)* (y + j) + x + n) = input.at<C>(i + x, j + y)[1];
							kernelb.at<float>((xs)* (y + j) + x + n) = input.at<C>(i + x, j + y)[0];
							
						}
					}
				}
			}
			else if (i <= row - 1 - n && i >= n && j >= n && col - 1 - n >= j) {
				kernel_size = (2 * n + 1) * (2 * n + 1);
				xs = 2 * n + 1;
				kernelr = Mat::zeros(kernel_size, 1, CV_32F);
				kernelg = Mat::zeros(kernel_size, 1, CV_32F);
				kernelb = Mat::zeros(kernel_size, 1, CV_32F);
				median = kernel_size / 2;
				//std::cout << new_kernel << std::endl;
				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
							kernelr.at<float>((xs)* (y + n) + x + n) = input.at<C>(i + x, j + y)[2];
							kernelg.at<float>((xs)* (y + n) + x + n) = input.at<C>(i + x, j + y)[1];
							kernelb.at<float>((xs)* (y + n) + x + n) = input.at<C>(i + x, j + y)[0];
							
						}
					}
				}
			}
			else if (i <= row - 1 - n && i >= n && col - 1 - n < j) {
				kernel_size = (col - j + n) * (2 * n + 1);
				xs = 2 * n + 1;
				kernelr = Mat::zeros(kernel_size, 1, CV_32F);
				kernelg = Mat::zeros(kernel_size, 1, CV_32F);
				kernelb = Mat::zeros(kernel_size, 1, CV_32F);
				median = kernel_size / 2;
				//std::cout << new_kernel << std::endl;
				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= j; y++) {
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
							kernelr.at<float>((xs)* (y + n) + x + n) = input.at<C>(i + x, j + y)[2];
							kernelg.at<float>((xs)* (y + n) + x + n) = input.at<C>(i + x, j + y)[1];
							kernelb.at<float>((xs)* (y + n) + x + n) = input.at<C>(i + x, j + y)[0];
							
						}
					}
				}
			}
			else if (i > row - 1 - n && j < n) {
				kernel_size = (row - i + n) * (j + 1 + n);
				xs = row - i + n;
				kernelr = Mat::zeros(kernel_size, 1, CV_32F);
				kernelg = Mat::zeros(kernel_size, 1, CV_32F);
				kernelb = Mat::zeros(kernel_size, 1, CV_32F);
				median = kernel_size / 2;
				//std::cout << new_kernel << std::endl;
				for (int x = n; x <= i; x++) { // for each kernel window
					for (int y = -j; y <= n; y++) {
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
							kernelr.at<float>((xs)* (y + j) + x + n) = input.at<C>(i + x, j + y)[2];
							kernelg.at<float>((xs)* (y + j) + x + n) = input.at<C>(i + x, j + y)[1];
							kernelb.at<float>((xs)* (y + j) + x + n) = input.at<C>(i + x, j + y)[0];
							
						}
					}
				}
			}
			else if (i > row - 1 - n && j >= n && j <= col - 1 - n) {
				kernel_size = (2 * n + 1) * (row - i + n);
				xs = row - i + n;
				kernelr = Mat::zeros(kernel_size, 1, CV_32F);
				kernelg = Mat::zeros(kernel_size, 1, CV_32F);
				kernelb = Mat::zeros(kernel_size, 1, CV_32F);
				median = kernel_size / 2;
				//std::cout << new_kernel << std::endl;
				for (int x = -n; x <= i; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
							kernelr.at<float>((xs)* (y + n) + x + n) = input.at<C>(i + x, j + y)[2];
							kernelg.at<float>((xs)* (y + n) + x + n) = input.at<C>(i + x, j + y)[1];
							kernelb.at<float>((xs)* (y + n) + x + n) = input.at<C>(i + x, j + y)[0];
							
						}
					}
				}
			}
			else if (i > row - 1 - n && j > col - 1 - n) {
				kernel_size = (col - j + n) * (row - i + n);
				xs = row - i + n;
				kernelr = Mat::zeros(kernel_size, 1, CV_32F);
				kernelg = Mat::zeros(kernel_size, 1, CV_32F);
				kernelb = Mat::zeros(kernel_size, 1, CV_32F);
				median = kernel_size / 2;
				//std::cout << new_kernel << std::endl;
				for (int x = -n; x <= i; x++) { // for each kernel window
					for (int y = -n; y <= j; y++) {
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							//std::cout << xs <<'\t' << x+i << '\t'<< y+j << '\t' << std::endl;
							kernelr.at<float>((xs)* (y + n) + x + n) = input.at<C>(i + x, j + y)[2];
							kernelg.at<float>((xs)* (y + n) + x + n) = input.at<C>(i + x, j + y)[1];
							kernelb.at<float>((xs)* (y + n) + x + n) = input.at<C>(i + x, j + y)[0];
							
						}
					}
				}
			}
			
				// Sort the kernels in ascending order
				sort(kernelr, kernelr, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
				sort(kernelg, kernelg, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
				sort(kernelb, kernelb, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
				
				
				//(kernel_size)*(kernel_size)*z+(kernel_size)*(x + n) + y + n
				output.at<C>(i, j)[0] = kernelb.at<float>(median);
				output.at<C>(i, j)[1] = kernelg.at<float>(median);
				output.at<C>(i, j)[2] = kernelr.at<float>(median);
				
			}
	
			
		}
	}

	return output;
}