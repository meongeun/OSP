#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

template <typename T>
Mat myrotate(const Mat input, float angle, const char* opt);

int main()
{
	Mat input, rotated;

	// Read each image
	input = imread("C:\\Users\\user\\Desktop\\KME\\OpenSW\\Lab\\Lab02\\stitchingL.jpg");
	//stitchingL

	// Check for invalid input
	if (!input.data) {
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	// original image
	namedWindow("image");
	imshow("image", input);
	
	rotated = myrotate<Vec3b>(input, 90, "nearest");
	//nearest bilinear

	// rotated image
	namedWindow("rotated");
	imshow("rotated", rotated);
	imwrite("bilinear_rotated.jpg", rotated);
	//imwrite("nearest_rotated.jpg", rotated);

	waitKey(0);

	return 0;
}

template <typename T>
Mat myrotate(const Mat input, float angle, const char* opt) {
	int row = input.rows;	//세로
	int col = input.cols;	//가로

	float radian = angle * CV_PI / 180;
	cout << row * sin(radian) << '\t' << col * cos(radian) << endl;
	cout << col * sin(radian) << '\t' << row * cos(radian) << endl;
	float sq_col = ceil(abs(row * sin(radian)) + abs(col * cos(radian)));	//가로
	float sq_row = ceil(abs(col * sin(radian)) + abs(row * cos(radian))); //세로
	cout << col << '\t' << row << endl;
	cout << sq_col << '\t' << sq_row << endl;
 
	Mat output = Mat::zeros(sq_row, sq_col, input.type());

	for (int i = 0; i < sq_row; i++) {	//세로
		for (int j = 0; j < sq_col; j++) {	//가로
			float x = (i - sq_row/ 2) * cos(radian) - (j - sq_col/ 2) * sin(radian) + row / 2;	//세로
			float y = (i - sq_row/ 2) * sin(radian) + (j - sq_col/ 2) * cos(radian) + col / 2;	//가로
 
중심으로 이동 후 회전 후 다시 원래만큼 이동 
 

			if ((y >= 0) && (y <= (col - 1)) && (x >= 0) && (x <= (row - 1))) {
				if (!strcmp(opt, "nearest")) {
					output.at<Vec3b>(i, j) = input.at<Vec3b>(round(x), round(y));
//가장 가까운 정수는 반올림한 것과 동일하므로 round함수 이용

				}
				else if (!strcmp(opt, "bilinear")) {
					int x1 = floor(x);
					int x2 = floor(x) + 1;
					int y1 = floor(y);
					int y2 = floor(y) + 1;
					output.at<Vec3b>(i, j) = (y2 - y) * ((x2 - x) * input.at<Vec3b>(x1, y1) + (x - x1) * input.at<Vec3b>(x2, y1)) + (y - y1) * ((x2 - x) * input.at<Vec3b>(x1, y2) + (x - x1) * input.at<Vec3b>(x2, y2));
//Bilinear Interpolation 공식
 


				}
			}
		}
	}

	return output;
}
