Compile Program: Visual Studio 2019
OpenCV version 4.2
# How To Run
1. put hist_func.h in header folder
2. add cpp file in source folder 
3. Change imread files path to your image path
4. Change saved file name to what you want  
5. Compile by Ctrl+F5

Code&output txt files& used image will be given in readme.zip file
# 1. hist_eq.cpp	
Purpose
To make gray image histogram equalized and get a better contrast image.
Output
equalized_PDF_gray.txt trans_func_eq.txt
# 2. hist_eq_RGB.cpp	
Purpose
To make color image histogram each RGB equalized and get a better contrast image.
but it would have distortion in color.
Output
equalized_PDF_RGB.txt trans_func_eq_RGB.txt
# 3. hist_eq_YUV.cpp	
Purpose
To make color image histogram equalized in Y color space and get a better contrast image with 
no color distortion.
Output
equalized_PDF_YUV.txt trans_func_eq_YUV.txt
# 4. hist_matching_gray.cpp	(Homework)	
Purpose
input gray image follows histogram distribution of reference gray image. 
Output
mat_trans_func_eq.txt mat_equalized_PDF_gray.txt 
# 5. hist_matching_YUV.cpp	(Homework)	
Purpose
input Y color space image follows histogram distribution of reference Y color space image. 
by merging YUV each color space shows fully colored image. 
Output
f_matched_func_YUV.txt f_matched_PDF_YUV.txt 
# 6. PDF_CDF.cpp 	
Purpose
To generate PDF, CDF file. 
Output
PDF.txt CDF.txt
# 7. hist_stretching.cpp 	
Purpose
To stretch image histogram to what i have wrote. 
Output
stretched_PDF.txt