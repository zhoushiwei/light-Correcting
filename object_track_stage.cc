#include "stdafx.h"
#include "../system/stage.h"
#include "../system/system_manager.h"
#include "object_track_stage.h"
#include "moving_object_detection.h"
#include "face_detection.h"
#include "bezier.h"
#include <iostream>
#include <vector>
using namespace std;
ObjectTrackStage* ObjectTrackStage::_instance = NULL;
ObjectTrackStage::ObjectTrackStage(){
	moving_object_detection=new MovingObjectDetection();
	face_detection=new FaceDetection;
}
ObjectTrackStage* ObjectTrackStage::Instance(){
	if(!_instance){
		_instance = new ObjectTrackStage;
	}
	return _instance;
}
//Point mousePosition = Point(640>>1, 480>>1);
//void mouseEvent(int event, int x, int y, int flags, void *param)
//{
//	if(event==CV_EVENT_MOUSEMOVE)
//	{
//		mousePosition=Point(x,y);
//	}
//}
void ObjectTrackStage::Execute(){
	//cout<<"object track stage executing"<<endl;
	moving_object_detection->movingObjectDetect(SystemManager::Instance()->getFront());
	//	cout<<SystemManager::Instance()->getMovingObjects()->size()<<endl;
	SystemManager* sm=SystemManager::Instance();
	face_detection->CompensateLight(sm->getOutFrame());
	CvPoint pathPoint;
	for(vector<CvRect>::iterator i=sm->getMovingObjects()->begin();i!=sm->getMovingObjects()->end();i++){
		//	cout<<i<<": "<<box[i].x<<" "<<box[i].y<<endl;
		/*double ratio=(double)(*i).width/(*i).height;
		if (ratio > 0.8 && ratio < 0.3 )
		{
		continue;
		}*/

		int y=((*i).y-10)>0 ? (*i).y-10 : 0;
		CvRect scaleRect = cvRect((*i).x, y, (*i).width, (*i).height+(*i).y-y);
	//	cvRectangle(sm->getOutFrame(),cvPoint(scaleRect.x,scaleRect.y),cvPoint(scaleRect.x+scaleRect.width,scaleRect.y+scaleRect.height),cvScalar(255,0,0),2);
	//	pathPoint=cvPoint(scaleRect.x+scaleRect.width/2,scaleRect.y+scaleRect.height);
	//	sm->addPath(pathPoint);
		cvSetImageROI(sm->getOutFrame(),scaleRect);
		IplImage* scaleImg=cvCreateImage(cvSize(scaleRect.width,scaleRect.height),sm->getOutFrame()->depth,sm->getOutFrame()->nChannels);
		cvCopy(sm->getOutFrame(),scaleImg);
		// detect by adaboost
		//	vector<CvRect>* faces=face_detection->DetectFaces(scaleImg);

		//   detect by skin
		vector<CvRect>* faces=face_detection->DetectSkinCrCb(scaleImg);
		int offset=4;
		int index=0;
		for(vector<CvRect>::iterator j=faces->begin();j!=faces->end();j++){
			index++;
			CvRect scaleFace=cvRect(
				(*j).x-offset > 0 ?(*j).x-offset : 0 ,
				(*j).y -offset > 0 ? (*j).y -offset : 0,
				//(*j).width+7,
				//(*j).height
				(*j).x - offset + (*j).width  + offset*2 < sm->getPFrame()->width  ? (*j).width  + offset*2 : (sm->getPFrame()->width) - ((*j).x-offset) ,
				(*j).y - offset + (*j).height + offset*2 < sm->getPFrame()->height ? (*j).height + offset*2 : (sm->getPFrame()->height)- ((*j).y-offset) 
				);


			IplImage* saveFace=cvCreateImage(cvSize(scaleFace.width,scaleFace.height),8,3);
			/*IplImage* realFace=cvCreateImage(cvSize((*j).width,(*j).height),8,3);
			cvSetImageROI(sm->getPFrame(),cvRect(scaleRect.x+(*j).x,scaleRect.y+(*j).y,(*j).width,().height));
			cvCopy(sm->getPFrame(),realFace);*/
		//	if(face_detection->VerifyFace(saveFace)>0){
		//		cvSetImageROI(sm->getPFrame(),cvRect(scaleRect.x+scaleFace.x,scaleRect.y+scaleFace.y,scaleFace.width,scaleFace.height));
		//		cvCopy(sm->getPFrame(),saveFace);
		//		char c[30];
		//		sprintf_s(c,"saveFace%d+%d.jpg",sm->getFre(),index);
		//		cvSaveImage(c,saveFace);
			//	cvShowImage("face",saveFace);
		//		cvReleaseImage(&saveFace);
			cvSetImageROI(sm->getOutFrame(),scaleRect);
			cvRectangle(sm->getOutFrame(),cvPoint(scaleFace.x,scaleFace.y),cvPoint(scaleFace.x+scaleFace.width,scaleFace.y+scaleFace.height),cvScalar(0,0,255),2);
			}
		//}


		////	cout<<sm->getPFrame()->roi->width<<endl;
		cvResetImageROI(sm->getPFrame());
		cvResetImageROI(sm->getOutFrame());
		cvReleaseImage(&scaleImg);

		/*setMouseCallback("Kalman", mouseEvent);
		 
		const CvMat prediction=sm->getKalmanFilter()->predict();  
		CvPoint predict_pt=cvPoint((int)prediction.data.fl[0],(int)prediction.data.fl[1]);
		cvCircle(sm->getOutFrame(),predict_pt,3,CV_RGB(0,255,0),3);
		cvCircle(sm->getOutFrame(), mousePosition, 5, CV_RGB(0,0,255),1);
		CvMat* measurement = cvCreateMat( 2, 1, CV_32FC1 );
		measurement->data.fl[0]=(float)mousePosition.x;  
		measurement->data.fl[1]=(float)mousePosition.y;  
		sm->getKalmanFilter()->correct(measurement); */
	//	cvCircle(sm->getOutFrame(), cvPoint(pathPoint.x+3,pathPoint.y+3), 3, CV_RGB(0,0,255),2);

	}

	//»­Æ½»¬¹ì¼£
	//CvPoint* points=sm->getPath()->data();
	////cout<<sm->getPath()->size()<<endl;
	//vector<CvPoint>* subPoints=new vector<CvPoint>;
	//int i;
	//for(i=0;i<sm->getPath()->size()-1;i++){
	//	//cvDrawLine(sm->getOutFrame(),points[i],points[i+1],cvScalar(0,0,255));
	//	if( i % 10 == 0 ){
	//		subPoints->push_back(points[i]);

	//	}
	//}
	//subPoints->push_back(points[i]);
	//cout<<"sub: "<<subPoints->size()<<endl;
	//if(subPoints->size()>3){
	//	Bezier* bezier=new Bezier;
	//	CvPoint* p=subPoints->data();
	//	vector<CvPoint> curveList;
	//	bezier->CreateCurve(p,subPoints->size(),curveList);
	//	CvPoint* curvePoint=curveList.data();
	//	for(int i=0;i<curveList.size()-1;i++){
	//		cvDrawLine(sm->getOutFrame(),curvePoint[i],curvePoint[i+1],cvScalar(0,0,255));
	//	}
	//}
	//delete subPoints;

	// ¿¨¶ûÂüÂË²¨
	 

}