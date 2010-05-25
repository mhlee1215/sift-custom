/*
 * CvUtils.cpp
 *
 *  Created on: 2010. 5. 25.
 *      Author: hp
 */

#include "CvUtils.h"

CvUtils::CvUtils() {
	// TODO Auto-generated constructor stub

}

CvUtils::~CvUtils() {
	// TODO Auto-generated destructor stub
}

uchar CvUtils::get8BitPixcel(IplImage* img, int x, int y){
	return get8BitPixcel(img, x, y, 1, 0);
}
uchar CvUtils::get8BitPixcel(IplImage* img, int x, int y, int channel, int rgb){
	if(channel == 1)
			return ((uchar*)(img->imageData + img->widthStep*y))[x];
		else if(channel == 3){
			if(rgb == 1)
				return ((uchar*)(img->imageData + img->widthStep*y))[x*3];
			else if(rgb == 1)
						return ((uchar*)(img->imageData + img->widthStep*y))[x*3+1];
			else if(rgb == 1)
						return ((uchar*)(img->imageData + img->widthStep*y))[x*3+2];
		}
}
