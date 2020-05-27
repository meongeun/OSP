#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	Mat srcImage = imread("lena.jpg");
	//	Mat srcImage = imread("flower.jpg");
	if (srcImage.empty())
		return -1;
	int K = 10; // # of clusters , 2, 3, 
	// make color tables for displaying objects 
	Mat colorTable(K, 1, CV_8UC3);
	Vec3b color;
	for (int k = 0; k < K; k++)
	{
		color[0] = rand() & 180 + 50;
		color[1] = rand() & 180 + 50;
		color[2] = rand() & 180 + 50;
		colorTable.at<Vec3b>(k, 0) = color;
	}
	Mat dstImage(srcImage.size(), srcImage.type());

	int attempts = 1;
	int flags = KMEANS_RANDOM_CENTERS;
	TermCriteria criteria(TermCriteria::COUNT + TermCriteria::EPS, 10, 1.0);
	//kmeans 함수 매개변수 data는 1행의 입력값이 들어가야 된다. 


	//그래서 reshape 함수로 1행의 데이터 값으로 바꿔준다.
	//Mat samples = srcImage.reshape(3, srcImage.rows * srcImage.cols);
	//samples.convertTo(samples, CV_32FC3);
	Mat samples(srcImage.rows * srcImage.cols, 3, CV_32F);
	for (int y = 0; y < srcImage.rows; y++)
		for (int x = 0; x < srcImage.cols; x++)
			for (int z = 0; z < 3; z++) {
				samples.at<float>(y + x * srcImage.rows, z) = srcImage.at<Vec3b>(y, x)[z];
			}

	Mat labels;
	kmeans(samples, K, labels, criteria, attempts, flags);

	// display clusters using labels
	for (int y = 0, k = 0; y < srcImage.rows; y++)
		for (int x = 0; x < srcImage.cols; x++, k++)
		{
			//만약 srcImage 이미지를 1행의 행렬로 바꿔주지 않았을 시 k = y*srcImage.cols + x
			int idx = labels.at<int>(y + x * srcImage.rows, 0);
			//std::cout << labels.at<int>(y + x * srcImage.rows, 0)<< '\t';
			//int idx = labels.at<int>(k, 0);
			color = colorTable.at<Vec3b>(idx, 0);
			dstImage.at<Vec3b>(y, x) = color;
		}
	imshow("dstImage", dstImage);
	waitKey();

	return 0;
}