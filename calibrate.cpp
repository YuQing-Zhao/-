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
#include <iostream>
#include <iomanip>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

//#include "stdafx.h"
#include "CameraCalibrator.h"

int main()
{

	cv::namedWindow("Image");//创建一个可以放置图像和trackbar的窗口。 被创建的窗口可以通过它们的名字被引用
	cv::Mat image;
	std::vector<std::string> filelist;

	//生成棋盘图像文件名列表,棋盘图片存在当前目录下的left文件夹中，棋盘图片文件名保存为：left01.jpg……
	for (int i=1; i<=14; i++)
	{

		std::stringstream str;
		str << "../left/left" << std::setw(2) << std::setfill('0') << i << ".jpg";
		std::cout << str.str() << std::endl;//输出图片流

		filelist.push_back(str.str());//为在vector尾部加入一个数据
		image= cv::imread(str.str(),0);//imread函数从文件中加载图像并返回该图像
		cv::imshow("Image",image);//在指定的窗口中显示图像
	
		 cv::waitKey(100);
	}

	// 创建校准器对象
    CameraCalibrator cameraCalibrator;
	// 从棋盘添加角点
	cv::Size boardSize(9,6);//棋盘图片中角点为9*6
	cameraCalibrator.addChessboardPoints(
		filelist,	// filenames of chessboard image
		boardSize);	// size of chessboard
		// calibrate the camera
    //	cameraCalibrator.setCalibrationFlag(true,true);
	cameraCalibrator.calibrate(image.size());

    // 对某一张图片进行畸变的恢复
    image = cv::imread(filelist[6]);
	cv::Mat uImage= cameraCalibrator.remap(image);

	// 输出相机矩阵
	cv::Mat cameraMatrix= cameraCalibrator.getCameraMatrix();
	std::cout << " Camera intrinsic: " << cameraMatrix.rows << "x" << cameraMatrix.cols << std::endl;
	std::cout << cameraMatrix.at<double>(0,0) << " " << cameraMatrix.at<double>(0,1) << " " << cameraMatrix.at<double>(0,2) << std::endl;
	std::cout << cameraMatrix.at<double>(1,0) << " " << cameraMatrix.at<double>(1,1) << " " << cameraMatrix.at<double>(1,2) << std::endl;
	std::cout << cameraMatrix.at<double>(2,0) << " " << cameraMatrix.at<double>(2,1) << " " << cameraMatrix.at<double>(2,2) << std::endl;

    imshow("Original Image", image);
    imshow("Undistorted Image", uImage);

	cv::waitKey();
	return 0;
}