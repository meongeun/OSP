#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#define RATIO_THR 0.4

using namespace std;
using namespace cv;

double euclidDistance(Mat& vec1, Mat& vec2);
int nearestNeighbor(Mat& vec, vector<KeyPoint>& keypoints, Mat& descriptors);
double nearestNeighborDistance(Mat& vec, vector<KeyPoint>& keypoints, Mat& descriptors);
void findPairs(vector<KeyPoint>& keypoints1, Mat& descriptors1,
	vector<KeyPoint>& keypoints2, Mat& descriptors2,
	vector<Point2f>& srcPoints, vector<Point2f>& dstPoints, bool crossCheck, bool ratio_threshold);

int main() {

	Mat input1 = imread("input1.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input2 = imread("input2.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input1_gray, input2_gray;

	if (!input1.data || !input2.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	//resize(input1, input1, Size(input1.cols / 2, input1.rows / 2));
	//resize(input2, input2, Size(input2.cols / 2, input2.rows / 2));

	cvtColor(input1, input1_gray, CV_RGB2GRAY);
	cvtColor(input2, input2_gray, CV_RGB2GRAY);

	FeatureDetector* detector = new SiftFeatureDetector(
		0,		// nFeatures
		4,		// nOctaveLayers
		0.04,	// contrastThreshold
		10,		// edgeThreshold
		1.6		// sigma
	);

	DescriptorExtractor* extractor = new SiftDescriptorExtractor();

	// Create a image for displaying mathing keypoints
	Size size2 = input2.size();
	std::cout << input2.size() << '\t' << input1.size()<< std::endl;
	Size sz2 = Size(size2.width + input1_gray.size().width, max(size2.height, input1_gray.size().height));
	Mat matchingImage2 = Mat::zeros(sz2, CV_8UC3);
	

	input1.copyTo(matchingImage2(Rect(size2.width, 0, input1_gray.size().width, input1_gray.size().height)));
	input2.copyTo(matchingImage2(Rect(0, 0, size2.width, size2.height)));
	
	// Compute keypoints and descriptor from the source image in advance
	vector<KeyPoint> keypoints1;
	Mat descriptors1;

	detector->detect(input1_gray, keypoints1);
	extractor->compute(input1_gray, keypoints1, descriptors1);
	printf("input1 : %d keypoints are found.\n", (int)keypoints1.size());

	vector<KeyPoint> keypoints2;
	Mat descriptors2;

	// Detect keypoints
	detector->detect(input2_gray, keypoints2);
	extractor->compute(input2_gray, keypoints2, descriptors2);

	printf("input2 : %zd keypoints are found.\n", keypoints2.size());
	
	for (int i = 0; i < keypoints1.size(); i++) {
		KeyPoint kp = keypoints1[i];
		kp.pt.x += size2.width;
		circle(matchingImage2, kp.pt, cvRound(kp.size*0.25), Scalar(255, 255, 0), 1, 8, 0);
	}

	for (int i = 0; i < keypoints2.size(); i++) {
		KeyPoint kp = keypoints2[i];
		circle(matchingImage2, kp.pt, cvRound(kp.size*0.25), Scalar(255, 255, 0), 1, 8, 0);
	}

	// Find nearest neighbor pairs
	vector<Point2f> srcPoints2;
	vector<Point2f> dstPoints2;
	bool crossCheck = true;
	bool ratio_threshold = true;
	//cout << "2 " << descriptors2.rows << " 1 " << descriptors1.rows;


	
	findPairs(keypoints2, descriptors2, keypoints1, descriptors1, srcPoints2, dstPoints2, crossCheck, ratio_threshold);
	printf("%zd keypoints are matched.\n", srcPoints2.size());

	// Draw line between nearest neighbor pairs
	for (int i = 0; i < (int)srcPoints2.size(); ++i) {
		Point2f pt2 = srcPoints2[i];
		Point2f pt1 = dstPoints2[i];
		Point2f from = pt2;
		Point2f to = Point(size2.width + pt1.x, pt1.y);
		line(matchingImage2, from, to, Scalar(0, 0, 255));
	}

	// Display mathing image
	namedWindow("Matching2");
	imshow("Matching2", matchingImage2);
	
	
	
	waitKey(0);

	return 0;
}

/**
* Calculate euclid distance
*/
double euclidDistance(Mat& vec1, Mat& vec2) {
	double sum = 0.0;
	int dim = vec1.cols;

	for (int i = 0; i < dim; i++) {
		sum += (vec1.at<float>(0,i) - vec2.at<float>(0,i)) * (vec1.at<float>(0,i) - vec2.at<float>(0,i));
	}
	return sqrt(sum);
}

/**
* Find the index of nearest neighbor point from keypoints.
*/
double nearestNeighborDistance(Mat& vec, vector<KeyPoint>& keypoints, Mat& descriptors) {
	int neighbor = -1;
	int secondneighbor = -1;
	double minDist = 1e6;
	double secondMinDist = 1e6;
	for (int i = 0; i < descriptors.rows; i++) {
		Mat v = descriptors.row(i);
		double distance = euclidDistance(vec, v);
		if (minDist > distance) {
			minDist = distance;
			neighbor = i;
		}
	}
	for (int i = 0; i < descriptors.rows; i++) {
		if (i==neighbor) {
			continue;
		}
		Mat v = descriptors.row(i);
		double distance = euclidDistance(vec, v);
		if (secondMinDist > distance) {
			secondMinDist = distance;
			secondneighbor = i;
		}
	}
	return minDist/secondMinDist;
}

int nearestNeighbor(Mat& vec, vector<KeyPoint>& keypoints, Mat& descriptors) {
	int neighbor = -1;
	double minDist = 1e6;
	for (int i = 0; i < descriptors.rows; i++) {
		Mat v = descriptors.row(i);
		if (minDist > euclidDistance(vec, v)) {
			minDist = euclidDistance(vec, v);
			neighbor = i;
		}
	}
	return neighbor;
}

/**
* Find pairs of points with the smallest distace between them
*/
void findPairs(vector<KeyPoint>& keypoints1, Mat& descriptors1,
	vector<KeyPoint>& keypoints2, Mat& descriptors2,
	vector<Point2f>& srcPoints, vector<Point2f>& dstPoints, bool crossCheck, bool ratio_threshold) {
	int count = 0;
	for (int i = 0; i < descriptors1.rows; i++) {
		KeyPoint pt1 = keypoints1[i];
		Mat desc1 = descriptors1.row(i);

		int nn1 = nearestNeighbor(desc1, keypoints2, descriptors2);
		double ratio = nearestNeighborDistance(desc1, keypoints2, descriptors2);
		//std::cout << ratio << std::endl;
		// Refine matching points using ratio_based thresholding
		if (ratio_threshold && ratio >= RATIO_THR) {
			continue;
		}

		// Refine matching points using cross-checking
		if (crossCheck) {
			Mat desc2 = descriptors2.row(nn1);
			int nn2 = nearestNeighbor(desc2, keypoints1, descriptors1);
			cout << i << '\t' << nn1 << '\t' << nn2 << endl;
			if (nn2 != i) {
				continue;
			}

		}
		KeyPoint pt2 = keypoints2[nn1];
		srcPoints.push_back(pt1.pt);
		dstPoints.push_back(pt2.pt);
		count += 1;
		//cout<<count<< " : (" << i << ", " << nn1 << ")" << endl;
	}
}