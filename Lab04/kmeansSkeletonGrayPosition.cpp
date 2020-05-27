#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#define IM_TYPE	CV_8UC3

using namespace cv;


// Note that this code is for the case when an input data is a color value.
int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat output;
	Mat input_gray;

	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}
	cvtColor(input, input_gray, CV_RGB2GRAY);

	namedWindow("Original", WINDOW_AUTOSIZE);
	imshow("Original", input_gray);
	std::cout << input_gray.type() << std::endl;
	float sigma = 5.0;
	float maxVal = -1;
	float maxValx = -1;
	float maxValy = -1;
	Mat samples(input_gray.rows * input_gray.cols, 3, CV_32F);
	
	for (int y = 0; y < input_gray.rows; y++)
		for (int x = 0; x < input_gray.cols; x++) {

			samples.at<float>(y + x * input_gray.rows, 0) = input_gray.at<uchar>(y, x);
			samples.at<float>(y + x * input_gray.rows, 1) = x / sigma;
			samples.at<float>(y + x * input_gray.rows, 2) = y / sigma;

		}
	//std::cout << maxVal << '\t' << maxValx << '\t' << maxValy << std::endl;
	// Clustering is performed for each channel (RGB)
	//std::cout << samples.type() << std::endl;
	// Note that the intensity value is not normalized here (0~1). You should normalize both intensity and position when using them simultaneously.
	int clusterCount = 10;
	Mat labels;
	int attempts = 5;
	Mat centers;
	
	kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);
	
	//std::cout << centers << std::endl;
	std::cout << centers.type() << std::endl;
	std::cout << labels.type() << std::endl;
	Mat new_image(input_gray.size(), input_gray.type());
	//Mat new_image(input_gray.rows, input_gray.cols, CV_8UC3);
	for (int y = 0; y < input_gray.rows; y++)
		for (int x = 0; x < input_gray.cols; x++)
		{
			int cluster_idx = labels.at<int>(y + x*input_gray.rows);
			/*
			new_image.at<Vec3b>(y, x)[0] = centers.at<float>(cluster_idx, 0);
			new_image.at<Vec3b>(y, x)[1] = centers.at<float>(cluster_idx, 1);
			new_image.at<Vec3b>(y, x)[2] = centers.at<float>(cluster_idx, 2);
			*/
			new_image.at<uchar>(y, x) = centers.at<float>(cluster_idx, 0);
			//Fill code that finds for each pixel of each channel of the output image the intensity of the cluster center.
		}
	imshow("clustered image", new_image);

	waitKey(0);

	return 0;
}

