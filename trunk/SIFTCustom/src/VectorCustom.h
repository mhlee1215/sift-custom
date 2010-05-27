/*
 * VectorCustom.h
 *
 *  Created on: 2010. 5. 26.
 *      Author: hp
 */

#ifndef VECTORCUSTOM_H_
#define VECTORCUSTOM_H_

using namespace std;

#include "CvPointCustom.h"
#include <vector>

class VectorCustom
{
private:
	vector<CvPointCustom> points;
public:
	VectorCustom();
	virtual ~VectorCustom();
};

#endif /* VECTORCUSTOM_H_ */
