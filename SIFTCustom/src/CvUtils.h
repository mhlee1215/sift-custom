/*
 * CvUtils.h
 *
 *  Created on: 2010. 5. 25.
 *      Author: hp
 */

#ifndef CVUTILS_H_
#define CVUTILS_H_

#include <iostream>
using namespace std;

#include "cv.h"
#include "highgui.h"

class CvUtils {
public:
	CvUtils();
	virtual ~CvUtils();

	uchar get8BitPixcel(IplImage* img, int x, int y);
	uchar get8BitPixcel(IplImage* img, int x, int y, int channel, int rgb);
};

#endif /* CVUTILS_H_ */
