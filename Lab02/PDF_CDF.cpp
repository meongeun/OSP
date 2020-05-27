#include "hist_func.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

int main() {
	
	Mat input = imread("input.jpg", IMREAD_COLOR);
	Mat input_gray;
	//Mat histImg;
	Mat histImage(255, input.rows*input.cols, CV_32FC3, Scalar(0, 0, 0));
	cvtColor(input, input_gray, COLOR_RGB2GRAY);	// convert RGB to Grayscale
	std::cout << input.rows << '\t' << input.cols << std::endl;
	// PDF, CDF txt files
	FILE *f_PDF, *f_CDF;
	
	fopen_s(&f_PDF, "PDF.txt", "w+");
	fopen_s(&f_CDF, "CDF.txt", "w+");

	// each histogram
	float *PDF = cal_PDF(input_gray);		// PDF of Input image(Grayscale) : [L]
	float *CDF = cal_CDF(input_gray);		// CDF of Input image(Grayscale) : [L]

	for (int i = 0; i < L; i++) {
		// write PDF, CDF
		fprintf(f_PDF, "%d\t%f\n", i, PDF[i]);
		fprintf(f_CDF, "%d\t%f\n", i, CDF[i]);
		//std::cout << i << '\t' << i + 1 << std::endl;
		//std::cout << PDF[i] << '\t' << PDF[i + 1] << std::endl;
		line(histImage, Point(0,i), Point((PDF[i]/100)*(input.rows*input.cols), i), Scalar(255, 0, 0), 2, 8, 0);
	}
	
	//line(histImage, Point(10, 0), Point(10, 0.5), Scalar(255, 0, 0), 2, 8, 0);
	//line(histImage, Point(20, 0), Point(20, 50), Scalar(255, 0, 0), 2, 8, 0);

	// memory release
	free(PDF);
	free(CDF);
	fclose(f_PDF);
	fclose(f_CDF);
	
	////////////////////// Show each image ///////////////////////
	
	imshow("histImg", histImage);
	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);
	

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
}