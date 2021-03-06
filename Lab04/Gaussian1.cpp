#include <opencv2/opencv.hpp>
#include <stdio.h>

#define IM_TYPE	CV_64FC3

using namespace cv;

Mat Add_Gaussian_noise(const Mat input, double mean, double sigma);
Mat Gaussianfilter_Gray(const Mat input, int n, double sigmaT, double sigmaS, const char *opt);
Mat Gaussianfilter_RGB(const Mat input, int n, double sigmaT, double sigmaS, const char* opt);

int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input_gray;

	// check for validation
	if (!input.data) {
		printf("Could not open\n");
		return -1;
	}

	cvtColor(input, input_gray, CV_RGB2GRAY);	// convert RGB to Grayscale
	
	// 8-bit unsigned char -> 64-bit floating point
	input.convertTo(input, CV_64FC3, 1.0 / 255);
	input_gray.convertTo(input_gray, CV_64F, 1.0 / 255);

	// Add noise to original image
	Mat noise_Gray = Add_Gaussian_noise(input_gray, 0, 0.1);
	Mat noise_RGB = Add_Gaussian_noise(input, 0, 0.1);

	// Denoise, using gaussian filter
	Mat Denoised_Gray = Gaussianfilter_Gray(noise_Gray, 3, 10, 10, "adjustkernel");
	Mat Denoised_RGB = Gaussianfilter_RGB(noise_RGB, 3, 10, 10, "zero-padding");//zero-padding , mirroring, adjustkernel

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Gaussian Noise (Grayscale)", WINDOW_AUTOSIZE);
	imshow("Gaussian Noise (Grayscale)", noise_Gray);

	namedWindow("Gaussian Noise (RGB)", WINDOW_AUTOSIZE);
	imshow("Gaussian Noise (RGB)", noise_RGB);

	namedWindow("Denoised (Grayscale)", WINDOW_AUTOSIZE);
	imshow("Denoised (Grayscale)", Denoised_Gray);

	namedWindow("Denoised (RGB)", WINDOW_AUTOSIZE);
	imshow("Denoised (RGB)", Denoised_RGB);

	waitKey(0);

	return 0;
}

Mat Add_Gaussian_noise(const Mat input, double mean, double sigma) {

	Mat NoiseArr = Mat::zeros(input.rows, input.cols, input.type());
	RNG rng;
	rng.fill(NoiseArr, RNG::NORMAL, mean, sigma);

	add(input, NoiseArr, NoiseArr);

	return NoiseArr;
}

Mat Gaussianfilter_Gray(const Mat input, int n, double sigmaT, double sigmaS, const char *opt) {

	Mat kernel;
	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;
	float denom;
	float kernelvalue;

	// Initialiazing Gaussian Kernel Matrix
	// Fill code to initialize Gaussian filter kernel matrix


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
	std::cout << input.type();
	// convolution
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {

			if (!strcmp(opt, "zero-padding")) {

				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							//std::cout << input.at<double>(i + x, j + y) << std::endl;
							output.at<double>(i, j) += kernel.at<float>(x + n, y + n) * input.at<double>(i + x, j + y);
						}
						//std::cout<< kernel.at<float>(n + x, n + y) << std::endl;
						//input.at<float>(i + x, j + y) 
						//output.at<uchar>(i, j) += kernel.at<float>(x + n, y + n) * input.at<uchar>(i + x, j + y);
						/* Gaussian filter with "zero-padding" boundary process:

						Fill the code:
						*/

					}
				}

			}

			else if (!strcmp(opt, "mirroring")) {

				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {

						/* Gaussian filter with "mirroring" boundary process:

						Fill the code:
						*/
						if (i + x > row - 1) {  //mirroring for the border pixels
							tempa = i - x;
						}
						else if (i + x < 0) {
							tempa = -(i + x);
						}
						else {
							tempa = i + x;
						}
						if (j + y > col - 1) {
							tempb = j - y;
						}
						else if (j + y < 0) {
							tempb = -(j + y);
						}
						else {
							tempb = j + y;
						}
						output.at<double>(i, j) += kernel.at<float>(x + n, y + n) * input.at<double>(tempa, tempb);
					}
				}

			}

			else if (!strcmp(opt, "adjustkernel")) {
				double sum1 = 0.0;
				double sum2 = 0.0;

				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {

						/* Gaussian filter with "adjustkernel" boundary process:

						Fill the code:
						*/
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							sum1 += exp(-(pow(x, 2) / (2 * pow(sigmaS, 2))) - (pow(y, 2) / (2 * pow(sigmaT, 2)))) * (input.at<double>(i + x, j + y));
							sum2 += exp(-(pow(x, 2) / (2 * pow(sigmaS, 2))) - (pow(y, 2) / (2 * pow(sigmaT, 2))));
						}
					}
				}
				output.at<double>(i, j) += (sum1 / sum2);

			}

		}
	}

	return output;
}

Mat Gaussianfilter_RGB(const Mat input, int n, double sigmaT, double sigmaS, const char *opt) {
	
	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;
	float denom;
	float kernelvalue;

	// Initialiazing Gaussian Kernel Matrix
	// Fill code to initialize Gaussian filter kernel matrix
	Mat kernel = Mat::zeros(kernel_size, kernel_size, CV_32F);


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

	// convolution
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {

			if (!strcmp(opt, "zero-padding")) {
				
				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {

							output.at<Vec3d>(i, j)[0] += kernel.at<float>(x + n, y + n) * input.at<Vec3d>(i + x, j + y)[0];
							output.at<Vec3d>(i, j)[1] += kernel.at<float>(x + n, y + n) * input.at<Vec3d>(i + x, j + y)[1];
							output.at<Vec3d>(i, j)[2] += kernel.at<float>(x + n, y + n) * input.at<Vec3d>(i + x, j + y)[2];
						}
						/* Gaussian filter with "zero-padding" boundary process (3-channel input):		

						Fill the code:
						*/

					}
				}

			}

			else if (!strcmp(opt, "mirroring")) {
				
				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {

						/* Gaussian filter with "mirroring" boundary process (3-channel input):		

						Fill the code:
						*/
						if (i + x > row - 1) {  //mirroring for the border pixels
							tempa = i - x;
						}
						else if (i + x < 0) {
							tempa = -(i + x);
						}
						else {
							tempa = i + x;
						}
						if (j + y > col - 1) {
							tempb = j - y;
						}
						else if (j + y < 0) {
							tempb = -(j + y);
						}
						else {
							tempb = j + y;
						}

						output.at<Vec3d>(i, j)[0] += kernel.at<float>(x + n, y + n) * input.at<Vec3d>(tempa, tempb)[0];
						output.at<Vec3d>(i, j)[1] += kernel.at<float>(x + n, y + n) * input.at<Vec3d>(tempa, tempb)[1];
						output.at<Vec3d>(i, j)[2] += kernel.at<float>(x + n, y + n) * input.at<Vec3d>(tempa, tempb)[2];

					}
				}

			}

			else if (!strcmp(opt, "adjustkernel")) {

				double sumr = 0.0;
				double sumg = 0.0;
				double sumb = 0.0;
				double sum = 0.0;
				for (int x = -n; x <= n; x++) { // for each kernel window
					for (int y = -n; y <= n; y++) {

						/* Gaussian filter with "adjustkernel" boundary process (3-channel input):		

						Fill the code:
						*/
						
						if ((i + x <= row - 1) && (i + x >= 0) && (j + y <= col - 1) && (j + y >= 0)) {
							sumb += exp(-(pow(x, 2) / (2 * pow(sigmaS, 2))) - (pow(y, 2) / (2 * pow(sigmaT, 2)))) * (input.at<Vec3d>(i + x, j + y)[0]);
							sumg += exp(-(pow(x, 2) / (2 * pow(sigmaS, 2))) - (pow(y, 2) / (2 * pow(sigmaT, 2)))) * (input.at<Vec3d>(i + x, j + y)[1]);
							sumr += exp(-(pow(x, 2) / (2 * pow(sigmaS, 2))) - (pow(y, 2) / (2 * pow(sigmaT, 2)))) * (input.at<Vec3d>(i + x, j + y)[2]);
							sum += exp(-(pow(x, 2) / (2 * pow(sigmaS, 2))) - (pow(y, 2) / (2 * pow(sigmaT, 2))));
						}
					}
				}
				output.at<Vec3d>(i, j)[2] += (sumr / sum);
				output.at<Vec3d>(i, j)[1] += (sumg / sum);
				output.at<Vec3d>(i, j)[0] += (sumb / sum);

			}

		}
	}

	return output;
}