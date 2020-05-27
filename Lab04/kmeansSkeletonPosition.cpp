#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#define IM_TYPE	CV_8UC3
#include <vector>

using namespace cv;


// Note that this code is for the case when an input data is a color value.
int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat output;


	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Original", WINDOW_AUTOSIZE);
	imshow("Original", input);
	float sigma = 20.0;
	float maxVal = -1;
	float maxValx = -1;
	float maxValy = -1;
	Mat rgb(input.rows , input.cols, CV_32FC3);
	Mat samples(input.rows * input.cols, 5, CV_32F);

	std::cout << input.type();
	for (int y = 0; y < input.rows; y++)
		for (int x = 0; x < input.cols; x++)
			{
				
				samples.at<float>(y + x * input.rows, 0) = input.at<Vec3b>(y, x)[0];
				samples.at<float>(y + x * input.rows, 1) = input.at<Vec3b>(y, x)[1];
				samples.at<float>(y + x * input.rows, 2) = input.at<Vec3b>(y, x)[2];
				
				samples.at<float>(y + x * input.rows, 3) = x/sigma;
				samples.at<float>(y + x * input.rows, 4) = y / sigma;
				
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
	//Mat new_image(input.rows, input.cols, CV_8UC(5));
	Mat new_image(input.size(), input.type());
	for (int y = 0; y < input.rows; y++)
		for (int x = 0; x < input.cols; x++)
		{
			int cluster_idx = labels.at<int>(y + x*input.rows, 0);

			new_image.at<Vec3b>(y, x)[2] = centers.at<float>(cluster_idx, 2);
			new_image.at<Vec3b>(y, x)[1] = centers.at<float>(cluster_idx, 1);
			new_image.at<Vec3b>(y, x)[0] = centers.at<float>(cluster_idx, 0);

		}
	imshow("clustered image", new_image);

	waitKey(0);

	return 0;
}

