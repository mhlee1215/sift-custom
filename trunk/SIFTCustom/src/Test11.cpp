/*
 * Test11.cpp
 *
 *  Created on: 2010. 5. 26.
 *      Author: hp
 */

#include <iostream>
using namespace std;
#include <windows.h>

#include "cv.h"
#include "highgui.h"

#include "CvUtils.h"
#include "CvPointCustom.h"

int main(void){
	CvPointCustom* a = new CvPointCustom(1, 1);

	CvPointCustom* b = new CvPointCustom(1, 2);

	if(*a == *b) cout << "ok" << endl;
	else cout << "no" << endl;
}
