#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>

#define RATIO_THR 0.4

using namespace std;
using namespace cv;

double euclidDistance(Mat& vec1, Mat& vec2);
int nearestNeighbor(Mat& vec, vector<KeyPoint>& keypoints, Mat& descriptors);
double nearestNeighborDistance(Mat& vec, vector<KeyPoint>& keypoints, Mat& descriptors);
void findPairs(vector<KeyPoint>& keypoints1, Mat& descriptors1,
	vector<KeyPoint>& keypoints2, Mat& descriptors2,
	vector<Point2f>& srcPoints, vector<Point2f>& dstPoints, bool crossCheck, bool ratio_threshold);
template <typename T>
Mat cal_affine(vector<Point2f> srcPoints, vector<Point2f> dstPoints, int number_of_points);
template <typename T>
Mat cal_random_affine(vector<Point2f> srcPoints, vector<Point2f> dstPoints, int number_of_points, vector<int> ki);
void blend_stitching(const Mat I1, const Mat I2, Mat& I_f, int diff_x, int diff_y, float alpha);

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

	
	findPairs(keypoints2, descriptors2, keypoints1, descriptors1, srcPoints2, dstPoints2, crossCheck, ratio_threshold);
	printf("%zd keypoints are matched.\n", srcPoints2.size());

	int n = 3;
	vector<int> ki;
	srand((unsigned int)time(NULL));
	for (int i = 0; i < n; i++) {
		ki.push_back(rand() % (int)srcPoints2.size());
		//ki[i] = rand() % (int)srcPoint{s1.size();
		//cout << ki[i] << endl;
	}
	
	input1.convertTo(input1, CV_32FC3, 1.0 / 255);
	input2.convertTo(input2, CV_32FC3, 1.0 / 255);

	const float I1_row = input1.rows;
	const float I2_row = input2.rows;
	const float I1_col = input1.cols;
	const float I2_col = input2.cols;
	int s = 7;
	int maxscore = 0;
	int sigma = 400;
	vector<int> ll;
	for (int l = 0; l < s; l++) {
		int score = 0;
		int u = 0;
		vector<int> li;
		cout << "srcPoints1 size: " << srcPoints2.size() << endl;
		Mat A12 = cal_random_affine<float>(dstPoints2, srcPoints2, n, ki);

		for (int j = 0; j < dstPoints2.size(); j++) {
			Point2f p12(A12.at<float>(0) * dstPoints2[j].y + A12.at<float>(1) * dstPoints2[j].x + A12.at<float>(2),
				A12.at<float>(3) * dstPoints2[j].y + A12.at<float>(4) * dstPoints2[j].x + A12.at<float>(5));
			cout << (p12.x - srcPoints2[j].x) * (p12.x - srcPoints2[j].x) + (p12.y - srcPoints2[j].y) * (p12.y - srcPoints2[j].y) << '\t' << sigma * sigma << endl;
			if ((p12.x - srcPoints2[j].x) * (p12.x - srcPoints2[j].x) + (p12.y - srcPoints2[j].y) * (p12.y - srcPoints2[j].y) < sigma * sigma) {
				score = score + 1;
				li.push_back(j);
				u++;

			}
		}
		if (score > maxscore) {
			maxscore = score;
			cout << "maxscore" << maxscore << endl;
			for (int j = u - 1; j > 0; j--) {
				ll.push_back(li.at(j));
			}

			cout << "maxscore index " << l << endl;
		}
	}
	Mat A12 = cal_random_affine<float>(dstPoints2, srcPoints2, ll.size(), ll);
	Mat A21 = cal_random_affine<float>(srcPoints2, dstPoints2, ll.size(), ll);

	// compute corners (p1, p2, p3, p4)
	Point2f p1(A21.at<float>(0) * 0 + A21.at<float>(1) * 0 + A21.at<float>(2),
		A21.at<float>(3) * 0 + A21.at<float>(4) * 0 + A21.at<float>(5));

	Point2f p2(A21.at<float>(0) * 0 + A21.at<float>(1) * I2_col + A21.at<float>(2),
		A21.at<float>(3) * 0 + A21.at<float>(4) * I2_col + A21.at<float>(5));

	Point2f p3(A21.at<float>(0) * I2_row + A21.at<float>(1) * 0 + A21.at<float>(2),
		A21.at<float>(3) * I2_row + A21.at<float>(4) * 0 + A21.at<float>(5));

	Point2f p4(A21.at<float>(0) * I2_row + A21.at<float>(1) * I2_col + A21.at<float>(2),
		A21.at<float>(3) * I2_row + A21.at<float>(4) * I2_col + A21.at<float>(5));

	// for inverse warping
	Point2f p1_(A12.at<float>(0) * 0 + A12.at<float>(1) * 0 + A12.at<float>(2),
		A12.at<float>(3) * 0 + A12.at<float>(4) * 0 + A12.at<float>(5));

	Point2f p2_(A12.at<float>(0) * 0 + A12.at<float>(1) * I1_col + A12.at<float>(2),
		A12.at<float>(3) * 0 + A12.at<float>(4) * I1_col + A12.at<float>(5));

	Point2f p3_(A12.at<float>(0) * I1_row + A12.at<float>(1) * 0 + A12.at<float>(2),
		A12.at<float>(3) * I1_row + A12.at<float>(4) * 0 + A12.at<float>(5));

	Point2f p4_(A12.at<float>(0) * I1_row + A12.at<float>(1) * I1_col + A12.at<float>(2),
		A12.at<float>(3) * I1_row + A12.at<float>(4) * I1_col + A12.at<float>(5));

	cout << "compute points" << endl;
	// compute boundary for merged image(I_f)
	int bound_u = (int)round(min(0.0f, min(p1.x, p2.x)));
	int bound_b = (int)round(max(I1_row, max(p3.x, p4.x)));
	int bound_l = (int)round(min(0.0f, min(p1.y, p3.y)));
	int bound_r = (int)round(max(I1_col, max(p2.y, p4.y)));

	// compute boundary for inverse warping
	int bound_u_ = (int)round(min(0.0f, min(p1_.x, p2_.x)));
	int bound_b_ = (int)round(max(I2_row, std::max(p3_.x, p4_.x)));
	int bound_l_ = (int)round(min(0.0f, min(p1_.y, p3_.y)));
	int bound_r_ = (int)round(max(I2_col, std::max(p2_.y, p4_.y)));

	int diff_x = abs(bound_u);
	int diff_y = abs(bound_l);

	int diff_x_ = abs(bound_u_);
	int diff_y_ = abs(bound_l_);

	// initialize merged image
	Mat I_f(bound_b + -bound_u + 1, bound_r - bound_l + 1, CV_32FC3, Scalar(0));
	// inverse warping with bilinear interplolation
	for (int i = -diff_x_; i < I_f.rows - diff_x_; i++) {
		for (int j = -diff_y_; j < I_f.cols - diff_y_; j++) {

			float x = A12.at<float>(0) * i + A12.at<float>(1) * j + A12.at<float>(2) + diff_x_;
			float y = A12.at<float>(3) * i + A12.at<float>(4) * j + A12.at<float>(5) + diff_y_;

			float y1 = floor(y);
			float y2 = floor(y) + 1;
			float x1 = floor(x);
			float x2 = floor(x) + 1;

			float mu = y - y1;
			float lambda = x - x1;

			if (x1 >= 0 && x2 < I2_row && y1 >= 0 && y2 < I2_col)
			{
				//Move by diff_x_, diff_y_
				I_f.at<Vec3f>(i + diff_x_, j + diff_y_) = (1 - mu) * ((1 - lambda) * input2.at<Vec3f>(x1, y1) + (lambda)*input2.at<Vec3f>(x2, y1)) + (mu) * ((1 - lambda) * input2.at<Vec3f>(x1, y2) + (lambda)*input2.at<Vec3f>(x2, y2));
				//cout << I_f.at<Vec3f>(i + diff_x_, j + diff_y_) << '\t' << input2.at<Vec3f>(x1, y1) << '\t' << input2.at<Vec3f>(x2, y1) << '\t' << input2.at<Vec3f>(x1, y2) << '\t' << input2.at<Vec3f>(x2, y2) << endl;
			}

		}
	}
	cout << "inverse matched" << endl;
	imshow("l_f", I_f);

	// image stitching with blend
	blend_stitching(input1, input2, I_f, diff_x, diff_y, 0.5);
	cout << "blend_stitching end" << endl;
	namedWindow("Left Image");
	imshow("Left Image", input1);

	namedWindow("Right Image");
	imshow("Right Image", input2);


	namedWindow("result");
	imshow("result", I_f);
	
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

template <typename T>
Mat cal_affine(vector<Point2f> srcPoints, vector<Point2f> dstPoints, int number_of_points) {
	/*
	Mat M(2 * number_of_points, 6, CV_32F, Scalar(0));
	Mat b(2 * number_of_points, 1, CV_32F, Scalar(0));
	*/
	Mat M(2 * (int)srcPoints.size(), 6, CV_32F, Scalar(0));
	Mat b(2 * (int)srcPoints.size(), 1, CV_32F, Scalar(0));

	Mat M_trans, temp, affineM;
	int n = 0;
	// initialize matrix
	cout << "cal_affine" << endl;
	for (int i = 0; i < number_of_points; i++) {
		//for (int i = 0; i < (int)srcPoints.size(); ++i) {
		Point2f pt1 = srcPoints[i];
		Point2f pt2 = dstPoints[i];
		M.at<float>(2 * i, 0) = pt1.y;
		M.at<float>(2 * i, 1) = pt1.x;
		M.at<float>(2 * i, 2) = 1;
		M.at<float>(2 * i + 1, 3) = pt1.y;
		M.at<float>(2 * i + 1, 4) = pt1.x;
		M.at<float>(2 * i + 1, 5) = 1;

		b.at<float>(2 * i) = pt2.y;
		b.at<float>(2 * i + 1) = pt2.x;

	}

	// (M^T * M)^(?1) * M^T * b ( * : Matrix multiplication)
	M_trans = M.t();
	temp = (M_trans * M).inv();
	affineM = temp * M_trans * b;

	cout << "return affineM" << endl;
	return affineM;
}

void blend_stitching(const Mat I1, const Mat I2, Mat& I_f, int diff_x, int diff_y, float alpha) {

	int bound_x = I1.rows + diff_x;
	int bound_y = I1.cols + diff_y;

	int col = I_f.cols;
	int row = I_f.rows;
	cout << "blend_stitching" << endl;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			// for check validation of I1 & I2
			bool cond1 = (i < bound_x&& i > diff_x) && (j < bound_y&& j > diff_y) ? true : false;
			bool cond2 = I_f.at<Vec3f>(i, j) != Vec3f(0, 0, 0) ? true : false;

			// I2 is already in I_f by inverse warping
			// So, It is not necessary to check that only I2 is valid
			// if both are valid

			if (cond1 && cond2) {
				I_f.at<Vec3f>(i, j) = alpha * I1.at<Vec3f>(i - diff_x, j - diff_y) + (1 - alpha) * I_f.at<Vec3f>(i, j);
			}
			// only I1 is valid
			else if (cond1) {
				I_f.at<Vec3f>(i, j) = I1.at<Vec3f>(i - diff_x, j - diff_y);
			}

		}
	}
}

template <typename T>
Mat cal_random_affine(vector<Point2f> srcPoints, vector<Point2f> dstPoints, int number_of_points, vector<int> ki) {
	//vector<int> ki

	Mat M(2 * number_of_points, 6, CV_32F, Scalar(0));
	Mat b(2 * number_of_points, 1, CV_32F, Scalar(0));

	/*
	Mat M(2 * (int)srcPoints.size(), 6, CV_32F, Scalar(0));
	Mat b(2 * (int)srcPoints.size(), 1, CV_32F, Scalar(0));
	*/
	Mat M_trans, temp, affineM;

	// initialize matrix
	cout << "cal_random_affine" << endl;
	for (int i = 0; i < number_of_points; i++) {

		Point2f pt1 = srcPoints[ki.at(i)];
		Point2f pt2 = dstPoints[ki.at(i)];
		M.at<float>(2 * i, 0) = pt1.y;
		M.at<float>(2 * i, 1) = pt1.x;
		M.at<float>(2 * i, 2) = 1;
		M.at<float>(2 * i + 1, 3) = pt1.y;
		M.at<float>(2 * i + 1, 4) = pt1.x;
		M.at<float>(2 * i + 1, 5) = 1;

		b.at<float>(2 * i) = pt2.y;
		b.at<float>(2 * i + 1) = pt2.x;

	}

	// (M^T * M)^(?1) * M^T * b ( * : Matrix multiplication)
	M_trans = M.t();
	temp = (M_trans * M).inv();
	affineM = temp * M_trans * b;

	cout << "return RandomAffineM" << endl;
	return affineM;
}

