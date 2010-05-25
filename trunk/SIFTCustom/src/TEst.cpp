//============================================================================
// Name        : TEst.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include <windows.h>

#include "cv.h"
#include "highgui.h"

#include "CvUtils.h"

int WinMain(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPTSTR    lpCmdLine,
		int       nCmdShow)
{
	// MessageBox(NULL, "11Hello in EclipseCDT", "EclipseCDT", MB_OK);
	//return 0;

	CvImage img("C:/workspace/TEst/src/lena.jpg", 0, CV_LOAD_IMAGE_COLOR),
			img_yuv, y, noise;
	CvRNG rng = cvRNG(-1);

	if (!img.data()){ // check if the image has been loaded properly
		cout << "이미지가 요기 없넹" << endl;
		return -1;
	}

	img_yuv = img.clone(); // clone the image
	// (although, later the content will be replaced with cvCvtColor,
	// clone() is used for simplicity and for the illustration)
	cvCvtColor(img, img_yuv, CV_BGR2YCrCb); // simply call OpenCV functions and pass the class instances there

	y.create(img.size(), IPL_DEPTH_8U, 1); // another method to create an image - from scratch
	noise.create(img.size(), IPL_DEPTH_32F, 1);

	cvSplit(img_yuv, y, 0, 0, 0);
	cvRandArr(&rng, noise, CV_RAND_NORMAL, cvScalarAll(0), cvScalarAll(20));
	cvSmooth(noise, noise, CV_GAUSSIAN, 5, 5, 1, 1);
	cvAcc(y, noise);
	cvConvert(noise, y);
	cvMerge(y, 0, 0, 0, img_yuv);
	//cvCvtColor(img_yuv, img, CV_YCrCb2BGR);

	CvSize size = img.size();

	IplImage* resizedImg = cvCreateImage(cvSize(size.width/2, size.height/2), IPL_DEPTH_8U, 3);
	cvPyrDown(img, resizedImg, CV_GAUSSIAN_5x5);


	IplImage* hw = cvCreateImage(cvSize(size.width/2, size.height/2), IPL_DEPTH_8U, 1);

	cout << img.size().height << ", " << img.size().width << endl;
	cout << hw->height << ", " << hw->width << endl;



	//
	cvCvtColor(resizedImg, hw, CV_RGB2GRAY);

	CvUtils* util = new CvUtils();



	for(int i = 0 ; i < hw->width ; i ++){
		for(int j = 0 ; j < hw->height ; j++){
			cout << (int)util->get8BitPixcel(hw, i, j) << " ";
		}
		cout << endl;
	}
	//cout << sizeof(hw->imageData) << ".." << endl;
	cvNamedWindow("image with grain", CV_WINDOW_AUTOSIZE);
	//hw.show("image with grain"); // .show method is the conveninient form of cvShowImage
	cvShowImage("Hello World", hw);
	cvWaitKey();

	/*
	//declare for the height and width of the image
	int height = 620;
	int width = 440;
	//specify the point to place the text
	CvPoint pt = cvPoint( height/4, width/2 );
	//Create an 8 bit, 3 plane image
	IplImage* hw = cvCreateImage(cvSize(height, width), 8, 3);
	//Clearing the Image
	cvSet(hw,cvScalar(0,0,0));
	//initialize the font
	CvFont font;cvInitFont( &font, CV_FONT_HERSHEY_COMPLEX,1.0, 1.0, 0, 1, CV_AA);
	//place the text on the image using the font
	cvPutText(hw, "Welcome To OpenCV", pt, &font, CV_RGB(150, 0, 150) );
	//create the window container
	cvNamedWindow("Hello World", 0);
	//display the image in the container
	cvShowImage("Hello World", hw);
	//hold the output windows
	cvWaitKey(0);
	 */

	return 0;

}
