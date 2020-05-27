Compile Program: Visual Studio 2019
OpenCV version 2.4.13
# How To Run
1. put your image in the same code folder 
2. add image file to your compile program 
input1은 왼쪽 이미지, input2는 오른쪽 이미지에 위치 
대응의 시작이미지와 도착이미지를 선택 
3. Change imread file path to your image path
4. Compile by Ctrl+F5

Code& used image will be given in code.zip file
1. SIFT-skeleton1.cpp	
Purpose
To feature match by NN I1 image to I2 image. you can cross check the matched points if you set crossCheck to true.
you can filter the matched points using ratio of first and second min distance by setting ratio_threshold true.   
Output
I1->I2 matched pairs connected in a line image
I1 is in the left I2 is in the right
2. SIFT-skeleton2.cpp	
Purpose
To feature match by NN I2 image to I1 image. you can cross check the matched points if you set crossCheck to true.
you can filter the matched points using ratio of first and second min distance by setting ratio_threshold true.  
Output
I2->I2 matched pairs connected in a line image
I2 is in the left I1 is in the right
3.featureHomography.cpp
Purpose
To find known object by using SURF and flan based matching in scene image.  
Output
object matched to scene image connected in a line
