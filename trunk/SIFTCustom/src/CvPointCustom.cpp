/*
 * CvPointCustom.cpp
 *
 *  Created on: 2010. 5. 26.
 *      Author: hp
 */

#include "CvPointCustom.h"

CvPointCustom::CvPointCustom(int inX, int inY) {
	// TODO Auto-generated constructor stub
	x = inX;
	y = inY;
}

CvPointCustom::~CvPointCustom() {
	// TODO Auto-generated destructor stub
}

bool CvPointCustom::operator == (CvPointCustom another){
	if(x == another.x && y == another.y)
		return true;
	return false;
}
