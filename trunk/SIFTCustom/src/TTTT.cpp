
#include <iostream>
using namespace std;

#include <windows.h>

#include "cv.h"
#include "highgui.h"
#include "cxtypes.h"

#include "CvUtils.h"
#define CV_MAT_ELEM_CN( mat, elemtype, row, col ) \
    (*(elemtype*)((mat).data.ptr + (size_t)(mat).step*(row) + sizeof(elemtype)*(col)))

/*
int main(void){
	CvMat *mat = cvCreateMat(10,10, CV_32S);
	CV_MAT_ELEM( *mat, int, 1, 1 * CV_MAT_CN(mat->type) + CV_32S  ) = 1;
	CV_MAT_ELEM( *mat, int, 1, 2 * CV_MAT_CN(mat->type) + CV_32S  ) = 1;

	for(int i = 0 ; i < mat->height ; i++){
		for(int j = 0 ; j < mat->width ; j++){
			cout << CV_MAT_ELEM_CN( *mat, int, i, j * CV_MAT_CN(mat->type) + CV_32S ) << " ";
		}
		cout << endl;
	}

	return 0;
}
*/
