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

#define CV_MAT_ELEM_CN( mat, elemtype, row, col ) \
    (*(elemtype*)((mat).data.ptr + (size_t)(mat).step*(row) + sizeof(elemtype)*(col)))

int WinMain(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPTSTR    lpCmdLine,
		int       nCmdShow)
{
	// MessageBox(NULL, "11Hello in EclipseCDT", "EclipseCDT", MB_OK);
	//return 0;

	CvImage img("C:/workspace/SIFTCustom/src/lena.jpg", 0, CV_LOAD_IMAGE_COLOR),
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


	IplImage* srcImg = cvCreateImage(cvSize(size.width/2, size.height/2), IPL_DEPTH_8U, 1);

	//cout << img.size().height << ", " << img.size().width << endl;
	//cout << srcImg->height << ", " << srcImg->width << endl;



	//
	cvCvtColor(resizedImg, srcImg, CV_RGB2GRAY);

	CvUtils* util = new CvUtils();




	//cout << sizeof(hw->imageData) << ".." << endl;
	//cvNamedWindow("image with grain", CV_WINDOW_AUTOSIZE);
	//hw.show("image with grain"); // .show method is the conveninient form of cvShowImage


	//	for(int i = 0 ; i < hw->width ; i ++){
	//		for(int j = 0 ; j < hw->height ; j++){
	//			cout << (int)util->get8BitPixcel(hw, i, j) << " ";
	//		}
	//		cout << endl;
	//	}

	CvMat* srcMat = cvCreateMat(srcImg->height, srcImg->width, CV_32FC1);
	//cout << srcImg->width << ", " << srcImg->height << ", " << srcImg->nChannels << endl;
	//cout << srcMat->width << ", " << srcMat->height << ", " << srcMat->type << endl;
	cvConvert(srcImg, srcMat);

	int maxOctave = 3;
	int maxScale = 5;
	float initSigma = sqrt(2);

	//모든 옥타브에 대해 스케일별 이미지 저장
	vector<IplImage**> octaveScaleVector;
	//모든 옥타브에 대해 DOG 이미지 저장
	vector<IplImage**> octaveDOGVector;

	//처음에는 원본과 동일함
	IplImage* currentSrc = cvCloneImage(srcImg);

	IplImage* nextSrc = NULL;
	//1단계. 옥타브별로 D이미지 구함
	for(int octave = 0 ; octave < maxOctave ; octave++){

		IplImage** oneOctaveImages = new IplImage*[maxScale];
		float sigma = initSigma;
		for(int scale = 0 ; scale < maxScale ; scale++, sigma*=20){
			//현재 스케일의 일반 이미지
			IplImage* smoothImage = cvCreateImage(cvSize(currentSrc->width, currentSrc->height), IPL_DEPTH_8U, 1);
			cvSmooth(currentSrc, smoothImage, CV_GAUSSIAN, 3+scale*2, 3+scale*2);
			oneOctaveImages[scale] = smoothImage;
		}
		octaveScaleVector.push_back(oneOctaveImages);
		//IplImage* test = cvCreateImage(cvSize(srcImg->width, srcImg->height), IPL_DEPTH_8U, 1);

		//스케일 올라감 (이미지의 사이즈 =/2)
		nextSrc = cvCreateImage(cvSize(currentSrc->width/2, currentSrc->height/2), IPL_DEPTH_8U, 1);
		cvPyrDown(currentSrc, nextSrc);
		currentSrc = cvCloneImage(nextSrc);
		cvReleaseImage(&nextSrc);
	}

	//2단계. 1단계에서 얻은 D이미지를 이용, 옥타브 별로 DOG 이미지 구함
	vector<IplImage**>::iterator it;

	//모든 옥타브에 대해서 수행함
	for(it = octaveScaleVector.begin(); it != octaveScaleVector.end() ; it++){
		IplImage** oneOctaveImages = *it;

		//한 옥타브에 대해서 DOG를 구함
		IplImage** oneOctaveDOGImages = new IplImage*[maxScale-1];
		//한 옥타브의 모든 스케일에 대해서
		//DOG이므로 스케일의 수보다 하나가 준다.
		for(int scale = 0; scale < maxScale-1 ; scale++){
			IplImage* currentScaleImage =  oneOctaveImages[scale];
			IplImage* nextScaleImage =  oneOctaveImages[scale+1];

			IplImage* dogImage =  cvCreateImage(cvSize(currentScaleImage->width, currentScaleImage->height), IPL_DEPTH_8U, 1);
			cvSub(nextScaleImage, currentScaleImage, dogImage);
			oneOctaveDOGImages[scale] = dogImage;
		}
		octaveDOGVector.push_back(oneOctaveDOGImages);
	}

	//3단계 DOG이미지를 통한 keypoint 추출
	int initThreshold = 100;
	vector<vector<CvPoint> > octaveKeypoint;

	vector<IplImage**>::iterator itDog;
	for(itDog = octaveScaleVector.begin(); itDog != octaveScaleVector.end() ; itDog++){
		IplImage** oneOctaveImages = *itDog;
		vector<CvPoint> oneOctaveKeypoint;


		CvMat** oneOctaveMat = new CvMat*[maxScale];

		//DOG이므로 스케일보다 하나가 준다.
		for(int scale = 0 ; scale < maxScale-1 ; scale++){
			//cout << oneOctaveImages[scale] << endl;
			//CvMat mathdr, *mat = cvGetMat( oneOctaveImages[scale], &mathdr );
			CvMat *mat = cvCreateMat( oneOctaveImages[scale]->height, oneOctaveImages[scale]->width, CV_32S );
			cvConvert( oneOctaveImages[scale], mat );
			oneOctaveMat[scale] = mat;
		}

		//DOG이미지간의 아래위 비교를 해야 하므로, 시작부 1개, 종료부 추가 1개 도합 3개가 줄어든다.
		//ex) 1, 2, 3, 4, 5 -> (DOG) -> D1, D2, D3, D4 -> 비교 -> (D1, D2, D3), (D2, D3, D4) => index (2, 3) 만 고려됨

		for(int scale = 1 ; scale < maxScale-2 ; scale++){
			CvMat *mat = oneOctaveMat[scale];
			//인덱스가 0부터 가므로, 1, 2 만 고려됨
			//if(itDog == octaveScaleVector.begin() && scale == 1){

			for(int y = 0 ; y < mat->height ; y++){
				for(int x = 0 ; x < mat->width ; x++){
					int center = CV_MAT_ELEM_CN( *mat, int, y, x * CV_MAT_CN(mat->type) + CV_32S );

					if(center > initThreshold){
						//cout <<  val << " ";
						CvPoint cvPoint;
						cvPoint.x = x;
						cvPoint.y = y;
						oneOctaveKeypoint.push_back(cvPoint);
					}
				}
				//cout << endl;
			}
			//}


		}


		cout << "size: " << oneOctaveKeypoint.size() << endl;
		octaveKeypoint.push_back(oneOctaveKeypoint);
	}







	cout << octaveScaleVector.size() << endl;
	cout << octaveDOGVector.size() << endl;
	//IplImage**
	IplImage* test = cvCreateImage(cvSize(srcImg->width, srcImg->height), IPL_DEPTH_8U, 1);
	cvSmooth(srcImg, test, CV_GAUSSIAN, 3, 3);
	IplImage* test1 = cvCreateImage(cvSize(srcImg->width, srcImg->height), IPL_DEPTH_8U, 1);
	cvSmooth(srcImg, test1, CV_GAUSSIAN, 5, 5);

	CvMat* dstMat = cvCreateMat(srcImg->height, srcImg->width, CV_32FC1);
	cvConvert(test, dstMat);

	CvMat* resultMat = cvCreateMat(srcImg->height, srcImg->width, CV_32FC1);

	cvmSub(dstMat, srcMat, resultMat);

	//	for(int i = 0 ; i < srcImg->width && i < 100 ; i ++){
	//		for(int j = 0 ; j < srcImg->height && j < 5; j++){
	//			cout << srcMat->data.fl[j + srcMat->cols + i] << " ";
	//		}
	//		cout << endl;
	//	}
	//	cout << endl;
	//	for(int i = 0 ; i < srcImg->width && i < 100 ; i ++){
	//		for(int j = 0 ; j < srcImg->height && j < 5; j++){
	//			cout << dstMat->data.fl[j + dstMat->cols + i] << " ";
	//		}
	//		cout << endl;
	//	}
	//	cout << endl;
	//	for(int i = 0 ; i < srcImg->width && i < 100 ; i ++){
	//		for(int j = 0 ; j < srcImg->height && j < 5; j++){
	//			cout << resultMat->data.fl[j + resultMat->cols + i] << " ";
	//		}
	//		cout << endl;
	//	}
	//IplConvKernel* kernel = cvCreateStructuringElementEx(3, 3, 0, 0, CV_SHAPE_RECT, NULL);

	IplImage* diffImg = cvCreateImage(cvSize(srcImg->width, srcImg->height), IPL_DEPTH_8U, 1);

	cvSub(test, test1, diffImg);

	/*int xloc = 10;
	int yloc = 10;
	cvShowImage("Original Image", srcImg);
	cvMoveWindow("Original Image", xloc, yloc);
	xloc += srcImg->width;
	cvShowImage("Test Image", test);
	cvMoveWindow("Test Image", xloc, yloc);
	xloc += test->width;
	cvShowImage("Diff Image", diffImg);
	cvMoveWindow("Diff Image", xloc, yloc);*/

	int cnt = 0;
	for(it = octaveDOGVector.begin(); it != octaveDOGVector.end() ; it++){
		if( it == octaveDOGVector.begin()){
			for(int scale = 0 ; scale < maxScale-1 ; scale++){
				cvShowImage("DOG Image: "+scale, (*it)[scale]);
			}

		}

	}

	//	IplImage* img1 = hw;
	//	IplImage* img2 = test;
	//
	//	CvMat *dist  = cvCreateMat( img1->height, img1->width, CV_64FC1 );
	//	int D = img1->nChannels; // D: Number of colors (dimension)
	//	int N = img1->width * img1->height; // N: number of pixels
	//	CvMat mat1hdr, *mat1 = cvReshape( img1, &mat1hdr, 1, N ); // N x D(colors)
	//	CvMat mat2hdr, *mat2 = cvReshape( img2, &mat2hdr, 1, N ); // N x D(colors)
	//	CvMat diffhdr, *diff  = cvCreateMat( N, D, CV_64FC1 ); // N x D, temporal buff
	//	cvSub( mat1, mat2, diff );
	//	cvMul( diff, diff, diff );
	//	dist = cvReshape( dist, &diffhdr, 1, N ); // nRow x nCol to N x 1
	//	cvReduce( diff, dist, 1, CV_REDUCE_SUM ); // N x D to N x 1
	//	dist = cvReshape( dist, &diffhdr, 1, img1->height ); // Restore N x 1 to nRow x nCol

	//cvShowImage("Diff2 Image", diffImg);
	cvWaitKey();

	//cvReleaseMat( &diff );




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
