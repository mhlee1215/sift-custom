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

template <class _Ty, class _A = std::allocator<_Ty> >
class VectorCustom : public std::vector<_Ty, _A>
{
public:
	VectorCustom();
	virtual ~VectorCustom();
};

#endif /* VECTORCUSTOM_H_ */
