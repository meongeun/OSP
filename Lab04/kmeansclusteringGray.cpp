#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	Mat srcImage = imread("lena.jpg");
	Mat input_gray;

	cvtColor(srcImage, input_gray, CV_RGB2GRAY);
	//std::cout << input_gray.type() << std::endl;
	if (srcImage.empty())
		return -1;
	int K = 10; // # of clusters , 2, 3, 
	// make color tables for displaying objects 
	
	Mat colorTable(K, 1, CV_8U);
	float color;
	for (int k = 0; k < K; k++)
	{
		color = rand() & 180 + 50;
		colorTable.at<uchar>(k) = color;
	}
	Mat dstImage(input_gray.size(), input_gray.type());
	
	int attempts = 1;
	int flags = KMEANS_RANDOM_CENTERS;
	TermCriteria criteria(TermCriteria::COUNT + TermCriteria::EPS, 10, 1.0);
	//kmeans �Լ� �Ű����� data�� 1���� �Է°��� ���� �ȴ�. 


	//�׷��� reshape �Լ��� 1���� ������ ������ �ٲ��ش�.
	//Mat samples = srcImage.reshape(3, srcImage.rows * srcImage.cols);
	//samples.convertTo(samples, CV_32FC3);
	Mat samples(input_gray.rows * input_gray.cols, 1, CV_32F);
	for (int y = 0; y < input_gray.rows; y++)
		for (int x = 0; x < input_gray.cols; x++)
			for (int z = 0; z < 3; z++) {
				samples.at<float>(y + x * input_gray.rows) = input_gray.at<uchar>(y, x);
			}

	Mat labels;
	kmeans(samples, K, labels, criteria, attempts, flags);

	// display clusters using labels
	for (int y = 0, k = 0; y < input_gray.rows; y++)
		for (int x = 0; x < input_gray.cols; x++, k++)
		{
			//���� srcImage �̹����� 1���� ��ķ� �ٲ����� �ʾ��� �� k = y*srcImage.cols + x
			int idx = labels.at<int>(y + x * input_gray.rows, 0);
			
			//std::cout << labels.at<int>(y + x * srcImage.rows, 0)<< '\t';
			//int idx = labels.at<int>(k, 0);
			
			
			color = colorTable.at<uchar>(idx);
			dstImage.at<uchar>(y, x) = color;
		
		}
	imshow("dstImage", dstImage);
	waitKey();

	return 0;
}