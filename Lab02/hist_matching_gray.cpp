#include "hist_func.h"

void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF);
void hist_mat(Mat& input, Mat& ref, Mat& z_equalized, Mat& matched, Mat& s_equalized, G* s_trans_func_eq, G* z_trans_func_eq, G* match_func);

int main() {

	Mat input = imread("input.jpg", IMREAD_COLOR);
	Mat ref = imread("ref_image2.jpg", IMREAD_COLOR);
	Mat input_gray;
	Mat ref_gray;
	

	cvtColor(input, input_gray, COLOR_RGB2GRAY);	// convert RGB to Grayscale
	cvtColor(ref, ref_gray, COLOR_RGB2GRAY);

	Mat s_equalized = input_gray.clone();
	Mat z_equalized = ref_gray.clone();
	Mat matched = input_gray.clone();

	// PDF or transfer function txt files
	FILE *f_PDF;
	FILE *f_equalized_PDF_gray;
	FILE *f_trans_func_eq;

	FILE* ref_f_PDF;
	FILE* ref_f_equalized_PDF_gray;
	FILE* ref_f_trans_func_eq;

	//FILE* mat_f_PDF;
	FILE* mat_f_equalized_PDF_gray;
	FILE* mat_f_trans_func_eq;
	
	fopen_s(&f_PDF, "PDF.txt", "w+");
	fopen_s(&f_equalized_PDF_gray, "equalized_PDF_gray.txt", "w+");
	fopen_s(&f_trans_func_eq, "trans_func_eq.txt", "w+");

	fopen_s(&ref_f_PDF, "ref_PDF.txt", "w+");
	fopen_s(&ref_f_equalized_PDF_gray, "ref_equalized_PDF_gray.txt", "w+");
	fopen_s(&ref_f_trans_func_eq, "ref_trans_func_eq.txt", "w+");

	//fopen_s(&mat_f_PDF, "mat_PDF.txt", "w+");
	fopen_s(&mat_f_equalized_PDF_gray, "mat_equalized_PDF_gray.txt", "w+");
	fopen_s(&mat_f_trans_func_eq, "mat_trans_func_eq.txt", "w+");

	float *PDF = cal_PDF(input_gray);	// PDF of Input image(Grayscale) : [L]
	float *CDF_s = cal_CDF(input_gray);	// CDF of Input image(Grayscale) : [L]

	float* ref_PDF = cal_PDF(ref_gray);	// PDF of Ref image(Grayscale) : [L]
	float* CDF_z = cal_CDF(ref_gray);	// CDF of Ref image(Grayscale) : [L]

	G s_trans_func_eq[L] = { 0 };			// transfer function
	G z_trans_func_eq[L] = { 0 };
	G match_func[L] = { 0 };
	G inverse_z_trans_func_eq[L] = { 0 };
	std::cout << "before eq";
	hist_eq(input_gray, s_equalized, s_trans_func_eq, CDF_s);								// histogram equalization on grayscale image
	float *equalized_PDF_gray = cal_PDF(s_equalized);									// equalized PDF (grayscale)
	std::cout << "before z eq";
	hist_eq(ref_gray, z_equalized, z_trans_func_eq, CDF_z);		
	//hist_eq(z_equalized, ref_gray , inverse_z_trans_func_eq, CDF_z);		// histogram equalization on grayscale image
	float* ref_equalized_PDF_gray = cal_PDF(z_equalized);									// equalized PDF (grayscale)
	std::cout << "before mat";
	hist_mat(input_gray, ref_gray, z_equalized, matched, s_equalized, s_trans_func_eq, z_trans_func_eq, match_func);				// histogram equalization on grayscale image
	float* matched_PDF_gray = cal_PDF(matched);									// equalized PDF (grayscale)
	//float* matched_CDF_gray = cal_CDF(matched);

	for (int i = 0; i < L; i++) {
		// write PDF
		fprintf(f_PDF, "%d\t%f\n", i, PDF[i]);
		fprintf(f_equalized_PDF_gray, "%d\t%f\n", i, equalized_PDF_gray[i]);

		fprintf(ref_f_PDF, "%d\t%f\n", i, ref_PDF[i]);
		fprintf(ref_f_equalized_PDF_gray, "%d\t%f\n", i, ref_equalized_PDF_gray[i]);

		fprintf(mat_f_equalized_PDF_gray, "%d\t%f\n", i, matched_PDF_gray[i]);


		// write transfer functions
		fprintf(f_trans_func_eq, "%d\t%d\n", i, s_trans_func_eq[i]);
		fprintf(ref_f_trans_func_eq, "%d\t%d\n", i, z_trans_func_eq[i]);
		fprintf(mat_f_trans_func_eq, "%d\t%d\n", i, match_func[i]);
	}

	// memory release
	free(PDF);
	free(CDF_s);
	fclose(f_PDF);
	fclose(f_equalized_PDF_gray);
	fclose(f_trans_func_eq);

	free(ref_PDF);
	free(CDF_z);
	fclose(ref_f_PDF);
	fclose(ref_f_equalized_PDF_gray);
	fclose(ref_f_trans_func_eq);

	//free(matched_PDF_gray);
	//free(matched_CDF_gray);
	//fclose(mat_f_PDF);
	fclose(mat_f_equalized_PDF_gray);
	fclose(mat_f_trans_func_eq);

	////////////////////// Show each image ///////////////////////

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("Equalized", WINDOW_AUTOSIZE);
	imshow("Equalized", s_equalized);

	namedWindow("Matched", WINDOW_AUTOSIZE);
	imshow("Matched", matched);

	namedWindow("RefGrayscale", WINDOW_AUTOSIZE);
	imshow("RefGrayscale", ref_gray);

	namedWindow("RefEqualized", WINDOW_AUTOSIZE);
	imshow("RefEqualized", z_equalized);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
}

// histogram equalization
void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF) {

	// compute transfer function
	for (int i = 0; i < L; i++) {
		trans_func[i] = (G)((L - 1) * CDF[i]);
	}

	// perform the transfer function
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++) {
			equalized.at<G>(i, j) = trans_func[input.at<G>(i, j)];
		}

}

// histogram matching
void hist_mat(Mat& input, Mat& ref, Mat& z_equalized, Mat& matched, Mat& s_equalized, G* s_trans_func_eq, G* z_trans_func_eq, G* match_func) {
	G inverse_z_trans_func_eq[L] = { 0 };
	
	
	for (int i = 0; i < ref.rows; i++) {
		for (int j = 0; j < ref.cols; j++) {
			inverse_z_trans_func_eq[z_equalized.at<G>(i, j)] = ref.at<G>(i, j);
			
		}
	}
	for (int i = 0; i < L; i++){
		int j = i;
		if (!inverse_z_trans_func_eq[i]) {
			j--;
			inverse_z_trans_func_eq[i] = inverse_z_trans_func_eq[j];
			
		}
	}
	//std::cout << "before";
	// perform the transfer function
	for (int i = 0; i < input.rows; i++) {
		for (int j = 0; j < input.cols; j++) {
			match_func[input.at<G>(i, j)] = inverse_z_trans_func_eq[s_equalized.at<G>(i, j)];
				//ref.at<G>(i, j);
				//z_trans_func_eq[s_equalized.at<G>(i, j)];
			matched.at<G>(i, j) = match_func[input.at<G>(i, j)];
			//std::cout << matched.at<G>(i, j);
			//std::cout << match_func[s_equalized.at<G>(i, j)];
		}
	}

	std::cout << "hi";


}