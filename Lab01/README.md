## Compile Program: Visual Studio 2019
# Rotation
OpenCV version 4.2
Purpose 
To rotate picture by bilinear interpolation or nearest interpolation 
How To Run
1. Change imread files path to your image path
2. Change myrotate function angle argument to angle you want to rotate 
3. Change myrotate function opt argument to bilinear or nearest (choose one you want) 
4. To save result image change imwrite name by what you want 
5. Compile by Ctrl+F5

# Sitiching 
OpenCV version 2.4.13 
Purpose 
To sitich two pictures by Affine Transformation 
How To Run
1. Change imread files path to your image path
2. Change ptr_x,ptr_y,ptl_x,ptl_y to your image corresponding points 
(it should be more than 6)
ptr_x: right image mapping point x coordinate value
ptr_y: right image mapping point y coordinate value
ptl_x: left image mapping point x coordinate value
ptl_y: left image mapping point y coordinate value
3. Change blend_stitching function alpha  argument to what you want(choose between in zero and one)
4. Compile by Ctrl+F5
