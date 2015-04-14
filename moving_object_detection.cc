#include "stdafx.h"
#include "moving_object_detection.h"
#include "../system/system_manager.h"
#include <cv.h>
#include <vector>
using namespace std;
MovingObjectDetection::MovingObjectDetection(){
	temp = NULL;
	element=cvCreateStructuringElementEx( 4, 4, 1, 1, CV_SHAPE_ELLIPSE, 0);
	storage = cvCreateMemStorage(0);
	contour = 0;
	rects=new vector<CvRect>;
}
void MovingObjectDetection::movingObjectDetect(IplImage* frontFrame){
	if(!temp){
		temp = cvCreateImage(cvSize(frontFrame->width,frontFrame->height),frontFrame->depth,frontFrame->nChannels);
	}
	if(!element){
		element=cvCreateStructuringElementEx( 4, 4, 1, 1, CV_SHAPE_ELLIPSE, 0);
	}
	cvSmooth(frontFrame, frontFrame, CV_MEDIAN, 3, 0, 0, 0); 
	cvDilate( frontFrame, temp, element, 1);
	cvErode( temp, frontFrame, element, 1);
		
	cvThreshold( frontFrame, frontFrame, 1, 255, CV_THRESH_BINARY );
	cvFindContours( frontFrame, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );

	rects->clear();
	for( ; contour != 0; contour = contour->h_next )
	{
		//	CvScalar color = CV_RGB( rand()&255, rand()&255, rand()&255 );
		/* 用1替代 CV_FILLED  所指示的轮廓外形 */
		float area = fabs(cvContourArea( contour, CV_WHOLE_SEQ )); 
		if(area>100){
			//		cvDrawContours( pFront, contour, color, color,1,1,8 );
			CvRect rect=cvBoundingRect(contour);
			rects->push_back(rect);

		}
	}
	if(!rects->empty()){
		vector<CvRect> v[10];
		CvPoint p0; //第一个矩形中心点
		int num_objects=0;
		//cout<<"v1: "<<v1.size()<<endl;
		while(num_objects<10 && !rects->empty()){
			p0=CvtRectToPoint((*rects)[0]);
			for(vector<CvRect>::iterator i = rects->begin();i != rects->end(); ){
				if(dist(CvtRectToPoint(*i),p0) < 80.0){
					v[num_objects].push_back(*i);
					i=rects->erase(i);
				}
				else{
					i++;
				}
			}
			num_objects++;
		}

		for(int i=0;i<num_objects;i++){
			int x1=10000,y1=10000,x2=0,y2=0;
			for(int j=0;j<v[i].size();j++){
				if(v[i][j].x<x1){
					x1=v[i][j].x;
				}
				if(v[i][j].y<y1){
					y1=v[i][j].y;
				}
				if(v[i][j].x+v[i][j].width>x2){
					x2=v[i][j].x+v[i][j].width;
				}
				if(v[i][j].y+v[i][j].height>y2){
					y2=v[i][j].y+v[i][j].height;
				}

			}
			int x=x1;
			int y=y1;
			int width=x2-x1;
			int height=y2-y1;
			SystemManager::Instance()->addMovingObject(cvRect(x,y,width,height));
		}

	}
}
double MovingObjectDetection::dist(CvPoint  p1, CvPoint p2){
	return sqrt((double)(p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p2.y)*(p2.y-p2.y));
}

CvPoint MovingObjectDetection::CvtRectToPoint(CvRect rect){
	int x=rect.x+rect.width/2;
	int y=rect.y+rect.height/2;
	return cvPoint(x,y);
}