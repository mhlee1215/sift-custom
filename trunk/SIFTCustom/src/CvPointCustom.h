/*
 * CvPointCustom.h
 *
 *  Created on: 2010. 5. 26.
 *      Author: hp
 */

#ifndef CVPOINTCUSTOM_H_
#define CVPOINTCUSTOM_H_

class CvPointCustom {
private:
	int x, y;

public:
	CvPointCustom(int x, int y);
	virtual ~CvPointCustom();
	bool operator == (CvPointCustom);
};

#endif /* CVPOINTCUSTOM_H_ */
