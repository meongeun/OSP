Compile Program: Visual Studio 2019
OpenCV version 2.4.13
# How To Run
1. put your image in the same code folder 
2. add image file to your compile program 
input1은 왼쪽 이미지, input2는 오른쪽 이미지를 의미 
3. Change imread file path to your image path
4. Compile by Ctrl+F5
5. Hough-skeleton.cpp에서 if 다음을 1로 하면 HoughTransform 
0으로 하면  Probablilistic Hough Transform  방식 이용
Code& used image will be given in code.zip file
6. you can change n,s, sigma in SIFT-skeleton2RANSAC.cpp & SIFT-skeleton2RANSAC.cpp
n is number of sampled points, s is number of trials, sigma is distance threshold  
1. SIFT-skeleton1.cpp	
Purpose
first, feature match by NN I1 image to I2 image. you can cross check the matched points if you set crossCheck to true.
you can filter the matched points using ratio of first and second min distance by setting ratio_threshold true.
With those matched points we sitich two images.     
Output
sitiched image
2. SIFT-skeleton2.cpp	
Purpose
first, feature match by NN I2 image to I1 image. you can cross check the matched points if you set crossCheck to true.
you can filter the matched points using ratio of first and second min distance by setting ratio_threshold true.  
With those matched points we sitich two images.  
Output
sitiched image
3. SIFT-skeleton1RANSAC.cpp	
Purpose
first, feature match by NN I1 image to I2 image. you can cross check the matched points if you set crossCheck to true.
you can filter the matched points using ratio of first and second min distance by setting ratio_threshold true.
We filter those matched points with RANSAC to remove the outliers. Then, sitich the two images.   
Output
sitiched image more effectively
4. SIFT-skeleton2RANSAC.cpp	
Purpose
first, feature match by NN I2 image to I1 image. you can cross check the matched points if you set crossCheck to true.
you can filter the matched points using ratio of first and second min distance by setting ratio_threshold true.  
We filter those matched points with RANSAC to remove the outliers. Then, sitich the two images.  
Output
sitiched image more effectively
5. Hough-skeleton.cpp
Purpose
To dectect line using HoughTransform & Probablilistic Hough Transform 
Output
detected line image
