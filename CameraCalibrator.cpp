/*------------------------------------------------------------------------------------------*\
   This file contains material supporting chapter 9 of the cookbook:  
   Computer Vision Programming using the OpenCV Library. 
   by Robert Laganiere, Packt Publishing, 2011.

   This program is free software; permission is hereby granted to use, copy, modify, 
   and distribute this source code, or portions thereof, for any purpose, without fee, 
   subject to the restriction that the copyright notice may not be removed 
   or altered from any source or altered source distribution. 
   The software is released on an as-is basis and without any warranties of any kind. 
   In particular, the software is not guaranteed to be fault-tolerant or free from failure. 
   The author disclaims all warranties with regard to this software, any use, 
   and any consequent failure, is purely the responsibility of the user.
 
   Copyright (C) 2010-2011 Robert Laganiere, www.laganiere.name
\*------------------------------------------------------------------------------------------*/

#include "CameraCalibrator.h"

// 导入标定图片提取角点
int CameraCalibrator::addChessboardPoints(
         const std::vector<std::string>& filelist, 
         cv::Size & boardSize) {

	// the points on the chessboard
    std::vector<cv::Point2f> imageCorners;//像素坐标系下的点
    std::vector<cv::Point3f> objectCorners;//世界坐标系下的点

    // 3D Scene Points:
    // Initialize the chessboard corners 
    // in the chessboard reference frame
	// The corners are at 3D location (X,Y,Z)= (i,j,0)
	for (int i=0; i<boardSize.height; i++) {
		for (int j=0; j<boardSize.width; j++) {

			objectCorners.push_back(cv::Point3f(i, j, 0.0f));
		}
    }

    // 2D Image points:
    cv::Mat image; // to contain chessboard image
    int successes = 0;

    // for all viewpoints
    for (int i=0; i<filelist.size(); i++) {

        // Open the image
        image = cv::imread(filelist[i],0);

        // Get the chessboard corners
        bool found = cv::findChessboardCorners(
                        image, boardSize, imageCorners);//棋盘角点检测

        // Get subpixel accuracy on the corners,对检测到的角点作进一步的优化计算，可使角点的精度达到亚像素级别。
        cv::cornerSubPix(image, imageCorners, 
                  cv::Size(5,5), //计算亚像素角点时考虑的区域的大小
                  cv::Size(-1,-1), 
			cv::TermCriteria(cv::TermCriteria::MAX_ITER +
                          cv::TermCriteria::EPS, 
             30,		// max number of iterations 
             0.1));     // min accuracy

          // If we have a good board, add it to our data
		  if (imageCorners.size() == boardSize.area()) {

			// Add image and scene points from one view
            addPoints(imageCorners, objectCorners);
            successes++;
          }

        //Draw the corners,棋盘角点的绘制
        cv::drawChessboardCorners(image, boardSize, imageCorners, found);
        cv::imshow("Corners on Chessboard", image);
        cv::waitKey(100);
    }

	return successes;
}



// Add scene points and corresponding image points,添加场景点和相应的图像点
void CameraCalibrator::addPoints(const std::vector<cv::Point2f>& imageCorners, const std::vector<cv::Point3f>& objectCorners) {

	// 2D image points from one view
	imagePoints.push_back(imageCorners);          
	// corresponding 3D scene points
	objectPoints.push_back(objectCorners);
}


// Calibrate the camera
// returns the re-projection error
double CameraCalibrator::calibrate(cv::Size &imageSize)
{
	// undistorter must be reinitialized
	mustInitUndistort= true;

	//Output rotations and translations
    std::vector<cv::Mat> rvecs, tvecs;

	// start calibration,开始标定
	return 
     calibrateCamera(objectPoints, // the 3D points
		            imagePoints,  // the image points
					imageSize,    // image size
					cameraMatrix, // output camera matrix
					distCoeffs,   // output distortion matrix
					rvecs, tvecs, // Rs, Ts 
					flag);        // set options
//					,CV_CALIB_USE_INTRINSIC_GUESS);

}

// remove distortion in an image (after calibration)，去除畸变
cv::Mat CameraCalibrator::remap(const cv::Mat &image) {

	cv::Mat undistorted;

	if (mustInitUndistort) { // called once per calibration
    
		cv::initUndistortRectifyMap(
			cameraMatrix,  // computed camera matrix
            distCoeffs,    // computed distortion matrix
            cv::Mat(),     // optional rectification (none) 
			cv::Mat(),     // camera matrix to generate undistorted
			cv::Size(640,480),
//            image.size(),  // size of undistorted
            CV_32FC1,      // type of output map
            map1, map2);   // the x and y mapping functions

		mustInitUndistort= false;
	}

	// Apply mapping functions
    cv::remap(image, undistorted, map1, map2, 
		cv::INTER_LINEAR); // interpolation type

	return undistorted;
}


// Set the calibration options
// 8radialCoeffEnabled should be true if 8 radial coefficients are required (5 is default)
// tangentialParamEnabled should be true if tangeantial distortion is present
void CameraCalibrator::setCalibrationFlag(bool radial8CoeffEnabled, bool tangentialParamEnabled) {

    // Set the flag used in cv::calibrateCamera()
    flag = 0;
    if (!tangentialParamEnabled) flag += CV_CALIB_ZERO_TANGENT_DIST;
	if (radial8CoeffEnabled) flag += CV_CALIB_RATIONAL_MODEL;
}

