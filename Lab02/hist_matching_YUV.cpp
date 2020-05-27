#include "hist_func.h"

void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF);
void hist_mat(Mat& input, Mat& ref, Mat& z_equalized, Mat& matched, Mat& s_equalized, G* z_trans_func_eq, G* match_func);

int main() {

	Mat input = imread("input.jpg", IMREAD_COLOR);
	Mat ref= imread("ref_image4_eq.jpg", IMREAD_COLOR);
	Mat s_equalized_YUV;
	Mat s_YUV;
	Mat z_equalized_YUV;
	Mat matched_YUV;
	Mat s,z;

	cvtColor(input, s_equalized_YUV, COLOR_RGB2YUV);	// RGB -> YUV
	cvtColor(input, s, COLOR_RGB2YUV);
	cvtColor(ref, z, COLOR_RGB2YUV);
	cvtColor(ref, z_equalized_YUV, COLOR_RGB2YUV);	// RGB -> YUV
	
	// split each channel(Y, U, V)
	Mat s_channels[3];
	split(s_equalized_YUV, s_channels);
	Mat s_Y = s_channels[0];						// U = channels[1], V = channels[2]
	
	Mat s_i[3];
	split(s, s_i);
	Mat s_input = s_i[0];

	// split each channel(Y, U, V)
	Mat z_channels[3];
	split(z_equalized_YUV, z_channels);
	Mat z_Y = z_channels[0];						// U = channels[1], V = channels[2]

	Mat z_i[3];
	split(z, z_i);
	Mat z_input = z_i[0];

	// split each channel(Y, U, V)
	Mat m_channels[3];
	split(s_equalized_YUV, m_channels);
	Mat m_Y = m_channels[0];						// U = channels[1], V = channels[2]

	// PDF or transfer function txt files
	FILE *s_f_equalized_PDF_YUV, *s_f_PDF_RGB, * s_Y_f_PDF_RGB;
	FILE *s_f_trans_func_eq_YUV;

	FILE* z_f_equalized_PDF_YUV, * z_Y_f_PDF_RGB;
	FILE* f_matched_func_YUV;

	FILE* f_matched_PDF_YUV, * z_f_PDF_RGB, * m_Y_f_PDF_RGB;
	FILE* z_f_trans_func_eq_YUV;

	float **s_PDF_RGB = cal_PDF_RGB(input);		// PDF of Input image(RGB) : [L][3]
	float *s_CDF_YUV = cal_CDF(s_Y);				// CDF of Y channel image
	float** z_PDF_RGB = cal_PDF_RGB(input);		// PDF of Input image(RGB) : [L][3]
	float* z_CDF_YUV = cal_CDF(z_Y);				// CDF of Y channel image

	fopen_s(&s_f_PDF_RGB, "s_PDF_RGB.txt", "w+");
	fopen_s(&s_f_equalized_PDF_YUV, "s_equalized_PDF_YUV.txt", "w+");
	fopen_s(&s_f_trans_func_eq_YUV, "s_trans_func_eq_YUV.txt", "w+");

	fopen_s(&s_Y_f_PDF_RGB, "s_Y_PDF_RGB.txt", "w+");
	fopen_s(&z_Y_f_PDF_RGB, "z_Y_PDF_RGB.txt", "w+");
	fopen_s(&m_Y_f_PDF_RGB, "m_Y_PDF_RGB.txt", "w+");

	fopen_s(&z_f_PDF_RGB, "z_PDF_RGB.txt", "w+");
	fopen_s(&z_f_equalized_PDF_YUV, "z_equalized_PDF_YUV.txt", "w+");
	fopen_s(&z_f_trans_func_eq_YUV, "z_trans_func_eq_YUV.txt", "w+");

	fopen_s(&f_matched_PDF_YUV, "f_matched_PDF_YUV.txt", "w+");
	fopen_s(&f_matched_func_YUV, "f_matched_func_YUV.txt", "w+");

	G s_trans_func_eq_YUV[L] = { 0 };			// transfer function
	G z_trans_func_eq_YUV[L] = { 0 };			// transfer function
	G match_func[L] = {0};
	// histogram equalization on Y channel
	// ...

	namedWindow("Y1", WINDOW_AUTOSIZE);
	imshow("Y1", s_Y);
	namedWindow("Y2", WINDOW_AUTOSIZE);
	imshow("Y2", z_Y);
	imwrite("Y2.jpg", z_Y);
	//namedWindow("Y2", WINDOW_AUTOSIZE);
	//imshow("Y3", m_Y);
	
	hist_eq(s_Y, s_channels[0], s_trans_func_eq_YUV, s_CDF_YUV);
	// merge Y, U, V channels
	namedWindow("s_Y", WINDOW_AUTOSIZE);
	imshow("s_Y", s_Y);
	merge(s_channels, 3, s_equalized_YUV);
	namedWindow("s_equalized", WINDOW_AUTOSIZE);
	imshow("s_equalized", s_channels[0]);
	namedWindow("z_i", WINDOW_AUTOSIZE);
	imshow("z_i", z_i[0]);
	

	hist_eq(z_Y, z_channels[0], z_trans_func_eq_YUV, z_CDF_YUV);
	// merge Y, U, V channels
	namedWindow("z_Y", WINDOW_AUTOSIZE);
	imshow("z_Y", z_Y);
	imwrite("z_Y.jpg", z_Y);
	merge(z_channels, 3, z_equalized_YUV);
	//void hist_mat(Mat& input,Mat& ref,Mat& z_equalized, Mat& matched, Mat& s_equalized, G* z_trans_func_eq, G* match_func)
	hist_mat(s_input, z_input, z_channels[0], m_channels[0], s_channels[0], z_trans_func_eq_YUV, match_func);				// histogram equalization on grayscale image
	
	merge(m_channels, 3, matched_YUV);
	//float* matched_PDF_YUV = cal_PDF(m_channels[0]);									// equalized PDF (grayscale)

	Mat m_i[3];
	split(matched_YUV, m_i);
	Mat m_input = m_i[0];

	// YUV -> RGB (use "CV_YUV2RGB" flag)
	// ...
	cvtColor(s_equalized_YUV, s_equalized_YUV, COLOR_YUV2RGB);
	cvtColor(z_equalized_YUV, z_equalized_YUV, COLOR_YUV2RGB);
	cvtColor(matched_YUV, matched_YUV, COLOR_YUV2RGB);

	// equalized PDF (YUV)
	// ...
	float** s_equalized_PDF_YUV = cal_PDF_RGB(s_equalized_YUV);
	float** z_equalized_PDF_YUV = cal_PDF_RGB(z_equalized_YUV);
	float** matched_PDF_YUV = cal_PDF_RGB(matched_YUV);
	float* s_Y_PDF_RGB = cal_PDF(s_input);
	float* z_Y_PDF_RGB = cal_PDF(z_input);
	float* m_Y_PDF_RGB = cal_PDF(m_input);
	namedWindow("m_Y", WINDOW_AUTOSIZE);
	imshow("m_Y", m_Y);

	for (int i = 0; i < L; i++) {
		// write PDF
		// ...

		// write transfer functions
		// ...
		fprintf(s_f_PDF_RGB, "%d\t%f\t%f\t%f\n", i, s_PDF_RGB[i][0], s_PDF_RGB[i][1], s_PDF_RGB[i][2]);
		fprintf(s_f_equalized_PDF_YUV, "%d\t%f\n", i, s_equalized_PDF_YUV);

		fprintf(s_Y_f_PDF_RGB, "%d\t%f\n", i, s_Y_PDF_RGB[i]);
		fprintf(z_Y_f_PDF_RGB, "%d\t%f\n", i, z_Y_PDF_RGB[i]);
		fprintf(m_Y_f_PDF_RGB, "%d\t%f\n", i, m_Y_PDF_RGB[i]);

		fprintf(z_f_PDF_RGB, "%d\t%f\t%f\t%f\n", i, z_PDF_RGB[i][0], z_PDF_RGB[i][1], z_PDF_RGB[i][2]);
		fprintf(z_f_equalized_PDF_YUV, "%d\t%f\n", i, z_equalized_PDF_YUV);
		fprintf(f_matched_PDF_YUV, "%d\t%f\t%f\t%f\n", i, matched_PDF_YUV[i][0], matched_PDF_YUV[i][1], matched_PDF_YUV[i][2]);

		fprintf(s_f_trans_func_eq_YUV, "%d\t%d\n", i, s_trans_func_eq_YUV[i]);
		fprintf(z_f_trans_func_eq_YUV, "%d\t%d\n", i, z_trans_func_eq_YUV[i]);
		fprintf(f_matched_func_YUV, "%d\t%d\n", i, match_func[i]);
	}

	// memory release
	free(s_PDF_RGB);
	free(s_CDF_YUV);
	fclose(s_f_PDF_RGB);
	fclose(s_f_equalized_PDF_YUV);
	fclose(s_f_trans_func_eq_YUV);

	fclose(s_Y_f_PDF_RGB);
	fclose(z_Y_f_PDF_RGB);

	free(z_PDF_RGB);
	free(z_CDF_YUV);
	fclose(z_f_PDF_RGB);
	fclose(z_f_equalized_PDF_YUV);
	fclose(z_f_trans_func_eq_YUV);

	fclose(f_matched_PDF_YUV);
	fclose(f_matched_func_YUV);
	////////////////////// Show each image ///////////////////////

	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Equalized_S_YUV", WINDOW_AUTOSIZE);
	imshow("Equalized_S_YUV", s_equalized_YUV);

	namedWindow("Equalized_Z_YUV", WINDOW_AUTOSIZE);
	imshow("Equalized_Z_YUV", z_equalized_YUV);
	imwrite("Equalized_Z_yuv.jpg", z_equalized_YUV);

	namedWindow("Matched_YUV", WINDOW_AUTOSIZE);
	imshow("Matched_YUV", matched_YUV);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
	
}

// histogram equalization
void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF) {

	// compute transfer function
	for (int i = 0; i < L; i++)
		trans_func[i] = (G)((L - 1) * CDF[i]);

	// perform the transfer function
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			equalized.at<G>(i, j) = trans_func[input.at<G>(i, j)];
			
}

// histogram matching
void hist_mat(Mat& input,Mat& ref,Mat& z_equalized, Mat& matched, Mat& s_equalized, G* z_trans_func_eq, G* match_func) {

	G inverse_z_trans_func_eq[L] = { 0 };

	for (int i = 0; i < ref.rows; i++) {
		for (int j = 0; j < ref.cols; j++) {
			inverse_z_trans_func_eq[z_equalized.at<G>(i, j)] = ref.at<G>(i, j);
		}
	}

	for (int i = 0; i < L; i++) {
		int j = i;
		if (!inverse_z_trans_func_eq[i]) {
			j--;
			inverse_z_trans_func_eq[i] = inverse_z_trans_func_eq[j];

		}
	}

	// perform the transfer function
	for (int i = 0; i < input.rows; i++) {
		for (int j = 0; j < input.cols; j++) {
			match_func[input.at<G>(i, j)] = inverse_z_trans_func_eq[s_equalized.at<G>(i, j)];
			matched.at<G>(i, j) = match_func[input.at<G>(i, j)];
			//std::cout << matched.at<G>(i, j);
			//std::cout << match_func[s_equalized.at<G>(i, j)];
		}
	}
}
