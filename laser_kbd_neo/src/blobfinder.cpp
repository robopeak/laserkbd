/* 
 *  Blob Finder 
 *
 *  By Shikai Chen (csk@live.com)
 *  
 *  Copyright 2009 - 2013 RoboPeak Team
 *  http://www.robopeak.net
 */


#include "common.h"
#include "cv_common.h"

using namespace cv;
#define CVCONTOUR_APPROX_LEVEL  1   // Approx.threshold - the bigger it is, the simpler is the boundary


void FindBlobs(IplImage *src, std::vector<Blob_t>& blobs, int minArea, int maxArea)
{
	static CvMemStorage*	mem_storage	= NULL;
	static CvMoments myMoments;

	if( mem_storage==NULL ) mem_storage = cvCreateMemStorage(0);
	else cvClearMemStorage(mem_storage);

	blobs.clear();

	CvSeq* contour_list = 0;
	cvFindContours(src,mem_storage,&contour_list, sizeof(CvContour),
		CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE);

	for (CvSeq* d = contour_list; d != NULL; d=d->h_next)
	{
		bool isHole = false;
		CvSeq* c = d;
		while (c != NULL)
		{
			double area = fabs(cvContourArea( c ));
			if( area >= minArea && area <= maxArea)
			{
				CvSeq* approx;

				//Polygonal approximation of the segmentation
			    approx = cvApproxPoly(c,sizeof(CvContour),mem_storage,CV_POLY_APPROX_DP, CVCONTOUR_APPROX_LEVEL,0);

				float area = (float)cvContourArea( approx, CV_WHOLE_SEQ );
				CvRect box = cvBoundingRect(approx);
				cvMoments( approx, &myMoments );

                Blob_t obj( box, cvPoint(0,0), fabs(area));

				if (myMoments.m10 > -DBL_EPSILON && myMoments.m10 < DBL_EPSILON)
				{
					obj._center.x = obj._box.x + obj._box.width/2;
					obj._center.y = obj._box.y + obj._box.height/2;
				}
				else
				{
					obj._center.x = myMoments.m10 / myMoments.m00;
					obj._center.y = myMoments.m01 / myMoments.m00;
				}
                blobs.push_back(obj);
			}//END if( area >= minArea)

			if (isHole)
				c = c->h_next;//one_hole->h_next is another_hole
			else
				c = c->v_next;//one_contour->h_next is one_hole
			isHole = true;
		}//END while (c != NULL)
	} 
}