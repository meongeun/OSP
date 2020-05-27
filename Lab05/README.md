Compile Program: Visual Studio 2019
OpenCV version 2.4.13
# How To Run
1. put your image in the same code folder 
2. add image file to your compile program 
3. Change imread file path to your image path
4. Compile by Ctrl+F5

Code& used image will be given in code.zip file
1. Canny-skeleton.cpp	
Purpose
To Detect edge by Canny edge detector
Output
edge detected image
2. Harris_corner-skeleton.cpp	
Purpose
To detect corners by harris corner detector
Output
corner detected image
3. LoG-skeleton.cpp	
Purpose
1. apply gaussian filter to remove noise in gary image
2. apply laplacian filter to detect contours in gray gaussianed image
Output
gray gussianed image, gray LoG image
4. LoG-skeletonRGB2.cpp	
Purpose
1. apply gaussian filter to remove noise in color image
2. apply laplacian filter to detect contours in color gaussianed image
Output
color gussianed image, color LoG image