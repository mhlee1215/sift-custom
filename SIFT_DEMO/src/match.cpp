/*
Detects SIFT features in two images and finds matches between them.

Copyright (C) 2006  Rob Hess <hess@eecs.oregonstate.edu>

@version 1.1.1-20070913
 */

#include "sift.h"
#include "imgfeatures.h"
#include "kdtree.h"
#include "utils.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <iostream>

using namespace std;


/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49

/******************************** Globals ************************************/

char img1_file[] = "C:/workspace/SIFT_DEMO/src/beaver.png";
char img2_file[] = "C:/workspace/SIFT_DEMO/src/2010-06-03 11-39-56.484.jpg";

/********************************** Main *************************************/


int main( int argc, char** argv )
{
	IplImage* img1, * img2, * stacked;
	struct feature* feat1, * feat2, * feat;
	struct feature** nbrs;
	struct kd_node* kd_root;
	CvPoint pt1, pt2;
	double d0, d1;
	int n1, n2, k, i, m = 0;

	img1 = cvLoadImage( img1_file, 1 );
	if( ! img1 )
		cout << "unable to load image from " << img1_file << endl;
	img2 = cvLoadImage( img2_file, 1 );
	if( ! img2 )
		cout << "unable to load image from " << img2_file << endl;
	//stacked = stack_imgs( img1, img2 ); // 두 이미지를 세로로 나열하여 하나의 이미지로 만듬
	cout << "Finding features in " << img2_file << "..." << endl;
	n2 = sift_features( img2, &feat2 );

	CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
	if( !capture ) {
		fprintf( stderr, "ERROR: capture is NULL \n" );
		getchar();
		return -1;
	}

	// Create a window in which the captured images will be presented
	cvNamedWindow( "mywindow", CV_WINDOW_AUTOSIZE );

	// Show the image captured from the camera in the window and repeat
	while( 1 ) {
		// Get one frame
		IplImage* frame = cvQueryFrame( capture );
		if( !frame ) {
			fprintf( stderr, "ERROR: frame is null...\n" );
			getchar();
			break;
		}
		m = 0;
		/////////////SIFT start////////////////////////////
		img1 = frame;
		stacked = stack_imgs( img1, img2 ); // 두 이미지를 세로로 나열하여 하나의 이미지로 만듬
		//cout << "Finding features in " << img1_file << "..." << endl;
		n1 = sift_features( img1, &feat1 );
		//import_features("test.feat", FEATURE_LOWE, &feat1);
		//export_features("test.feat", feat1, n1);

		kd_root = kdtree_build( feat2, n2 );
		for( i = 0; i < n1; i++ )
		{
			feat = feat1 + i;
			k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
			if( k == 2 )
			{
				d0 = descr_dist_sq( feat, nbrs[0] );
				d1 = descr_dist_sq( feat, nbrs[1] );
				if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
				{
					pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
					pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
					pt2.y += img1->height; // stacked 이미지 변수는 세로로 합쳐진 이미지 이므로 높이에 img1-height를 더함
					cvLine( stacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
					m++;
					feat1[i].fwd_match = nbrs[0];
				}
			}
			free( nbrs );
		}

		cout << "Found " << m << " total matches" << endl;
		cvNamedWindow( "mywindow", 1 );
		//cvShowImage( "Matches", stacked );


		///////SIFT END/////////////////////////////////////////

		cvShowImage( "mywindow", stacked );
		// Do not release the frame!

		//If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
		//remove higher bits using AND operator
		if( (cvWaitKey(10) & 255) == 27 ) break;
	}

	// Release the capture device housekeeping
	cvReleaseCapture( &capture );
	cvDestroyWindow( "mywindow" );



	//cvWaitKey( 0 );


	/* 
	UNCOMMENT BELOW TO SEE HOW RANSAC FUNCTION WORKS

	Note that this line above:

	feat1[i].fwd_match = nbrs[0];

	is important for the RANSAC function to work.
	 */
	/*
	{
		CvMat* H;
		H = ransac_xform( feat1, n1, FEATURE_FWD_MATCH, lsq_homog, 4, 0.01,
			homog_xfer_err, 3.0, NULL, NULL );
		if( H )
		{
			IplImage* xformed;
			xformed = cvCreateImage( cvGetSize( img2 ), IPL_DEPTH_8U, 3 );
			cvWarpPerspective( img1, xformed, H, 
				CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS,
				cvScalarAll( 0 ) );
			cvNamedWindow( "Xformed", 1 );
			cvShowImage( "Xformed", xformed );
			cvWaitKey( 0 );
			cvReleaseImage( &xformed );
			cvReleaseMat( &H );
		}
	}
	 */

	cvReleaseImage( &stacked );
	cvReleaseImage( &img1 );
	cvReleaseImage( &img2 );
	kdtree_release( kd_root );
	free( feat1 );
	free( feat2 );
	return 0;
}
