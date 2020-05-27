Compile Program: Visual Studio 2019
OpenCV version 2.4.13
# How To Run
1. put your image name in the same code folder 
2. add image file to your compile program 
3. Change imread files path to your image path
4. Compile by Ctrl+F5

Code& used image will be given in code.zip file
1. MeanFilterGray.cpp	
Purpose
To make gray image blurred by uniform mean filter.
Output
blurred image
2. MeanFilterRGB.cpp	
Purpose
To make color image blurred by uniform mean filter.
Output
blurred color image
3. GaussianGraySkeleton.cpp	
Purpose
To make gray image blurred by gaussian filter.
Output
bluured gray image
4. GaussianRGBSkeleton.cpp	
Purpose
To make color image blurred by gaussian filter.
Output
blurred color image
5. GaussianGraySeparable2.cpp	
Purpose
To make gray image blurred by gaussian filter. 
as computing x coordinate, y coordinate seperately.
Output
blurred gray image
6. GaussianRGBSeparable.cpp	
Purpose
To make color image blurred by gaussian filter. 
as computing x coordinate, y coordinate seperately.
Output
blurred color image
7. SobelGraySkeleton.cpp	
Purpose
To detect edge by Sobel filter(first order gradient) & Laplacian(2nd derivative) Filter.
Output
gray image of detected edge.
8. SobelRGBSkeleton.cpp	
Purpose
To detect edge by Sobel filter(first order gradient) and Laplacian(2nd derivative) Filter in color image. 
by averaging 3channel(rgb) values. 
Output
gray image of detected edge
9. GaussianGrayUnmasked.cpp	
Purpose
To make gray image more sharpened by boosting high-frequency components.
Output
sharpened image
10. GaussianRGBUnmasked.cpp	
Purpose
To make color image more sharpened by boosting high-frequency components.
Output
sharpened color image
