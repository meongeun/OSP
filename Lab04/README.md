Compile Program: Visual Studio 2019
OpenCV version 2.4.13
# How To Run
1. put your image in the same code folder 
2. add image file to your compile program 
3. Change imread file path to your image path
4. Compile by Ctrl+F5

Code& used image will be given in code.zip file
1. adaptivethresholdSkeleton.cpp	
Purpose
To implement adaptive threshold using moving averages in gray image
Output
adaptive thresholded image
2. Bilateral.cpp	
Purpose
To denoise Gaussian noise with bilateral filtering with 3 kind of bounding options.
Output
gaussian noise gray, color image
filtered gray, color image
3. Gaussian1.cpp	
Purpose
To denoise Gaussian noise with gaussian filtering with 3 kind of bounding options.
Output
gaussian noise gray, color image
filtered gray, color image
4. kmeansSkeleton.cpp	
Purpose
To perform image segmentation using K-means clustering function provided by opencv in color image.
color values use clustered center value.
Input has only intensity.
Output
segment image
5. kmeansSkeletonGray.cpp	
Purpose
To perform image segmentation using K-means clustering function provided by opencv in gray image.
color values use clustered center value.
Input has only r,g,b intensity.
Output
segment image
6. kmeansSkeletonPosition.cpp	
Purpose
To perform image segmentation using K-means clustering function provided by opencv in color image.
color values use clustered center value. 
Input has r,g,b intensity and x/sigma, y/sigma. 
Output
segment image
7. kmeansSkeletonGrayPosition.cpp	
Purpose
To perform image segmentation using K-means clustering function provided by opencv in gray image.
color values use clustered center value.
Input has intensity and x/sigma, y/sigma. 
Output
segment image
8. salt_and_pepper.cpp	
Purpose
generate salt and pepper noise and denoise by median filter
Output
generated gray, color image 
denoised gray, color image
9. kmeansclusteringGray.cpp	
Purpose
To perform image segmentation using K-means clustering function provided by opencv in gray image.
color will be used in a randomly selected way. 
Input has intensity and x/sigma, y/sigma. 
Output
segment image
10. kmeansclustering.cpp		
Purpose
To perform image segmentation using K-means clustering function provided by opencv in color image.
color will be used in a randomly selected way. 
Input has intensity and x/sigma, y/sigma. 
Output
generated gray, color image 
denoised gray, color image
