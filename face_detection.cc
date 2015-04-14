#include "stdafx.h"
#include "face_detection.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <vector>
#include <iostream>
using namespace std;
FaceDetection::FaceDetection(){
	faceList=new vector<CvRect>;
	char* cascade_name="haarcascade_frontalface_alt2.xml";
	char* profile_cascade_name="haarcascade_profileface.xml";
	char* some_cascade_name="haarcascade_fullbody.xml";
	cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
	profile_cascade=(CvHaarClassifierCascade*)cvLoad( profile_cascade_name, 0, 0, 0 );
	some_cascade=(CvHaarClassifierCascade*)cvLoad( some_cascade_name, 0, 0, 0 );
	storage=cvCreateMemStorage(0);
	histgram=new int[256];
	storage = cvCreateMemStorage(0);
	contour = 0;
	normalFace=cvCreateImage(cvSize(20,20),8,1);
	for(int i=0;i<256;i++){
		histgram[i]=0;
	}
	element = NULL;

}

// compensate light 
void FaceDetection::CompensateLight(IplImage* img){
	unsigned char RedTemp,GreenTemp,BlueTemp; 
	//	cout<<"compensate"<<endl;
	for (int i=0; i < img->height; i++) { 
		uchar* ptr = (uchar*)(img->imageData+i*img->widthStep); 
		for (int j=0; j < img->width; j++) { 
			BlueTemp=ptr[3*j]; 
			GreenTemp=ptr[3*j+1]; 
			RedTemp=ptr[3*j+2]; 
			int gray = (RedTemp * 299 + GreenTemp * 587 + BlueTemp * 114)/1000; 
			histgram[gray]++; 
		} 
	} 
	int calnum =0; 
	int total = img->width * img->height; 
	int num; 
	const float thresholdco = 0.05; 

	for(int i =0;i<256;i++) 
	{ 
		if((float)calnum/total < thresholdco) //得到前5%的高亮像素。
		{ 
			calnum+= histgram[255-i];//histogram保存的是某一灰度值的像素个数,calnum是边界灰度之上的像素数 
			num = i; 
		} 
		else 
			break; 
	} 
	int averagegray = 0; 
	calnum =0; 

	//得到满足条件的象素总的灰度值 
	for(int i = 255;i>=255-num;i--) 
	{ 
		averagegray += histgram[i]*i;//总的像素的个数*灰度值 
		calnum += histgram[i];//总的像素数 
	} 
	averagegray /=calnum; // top 5% 的平均灰度值
	//得到光线补偿的系数 
	float co = 255.0/(float)averagegray;  
	//下面的循环对图象进行光线补偿 
	for (int i=0; i < img->height; i++) { 
		uchar* ptr = (uchar*)(img->imageData + i*img->widthStep );
		for (int j=0; j < img->width; j++) { 

			for(int n=0;n<3;n++) 
			{ 
				int nTemp=ptr[3*j + n]*co*1.15; 
				ptr[3*j + n]=(nTemp>255)?255:nTemp; 

			} 
		} 
	}

}

// detect skin area by Ycrcb
vector<CvRect>* FaceDetection::DetectSkinCrCb(IplImage* img){
	faceList->clear();
	IplImage* yImg=cvCreateImage(cvGetSize(img),img->depth,3);
	//CompensateLight(img);
	cvCvtColor(img,yImg,CV_BGR2YCrCb);
	IplImage* out=cvCreateImage(cvGetSize(img),img->depth,1);
	cvSet(out,cvScalar(0,0,0));
	IplImage*  skinCrCbHist = cvCreateImage(cvSize(256,256), 8, 1);  
	uchar* skin=(uchar*)skinCrCbHist->imageData;
	uchar* outData=(uchar*)out->imageData;
	for(int i=0; i<skinCrCbHist->height; i++){
		for(int j=0; j<skinCrCbHist->width; j++){
			skin[i*256+j]=0;
		}
	}
	//cvEllipse(skinCrCbHist, cvPoint(105, 135), cvSize(20, 10), 135.0, 0.0, 360.0, cvScalar(255, 255, 255), -1);
	cvEllipse(skinCrCbHist, cvPoint(113, 155.6), cvSize(23.4, 15.2), 43.0, 0.0, 360.0, cvScalar(255, 255, 255), -1);
	for(int i=0; i<img->height; i++){
		uchar* ptrRGB=(uchar*)(img->imageData + i*img->widthStep);
		uchar* ptrY=(uchar*)(yImg->imageData + i*yImg->widthStep);
		for(int j=0; j<img->width; j++){
		//	if(ptrY[3*j+1]>=140 && ptrY[3*j+1]<=170 &&  ptrY[3*j+2]>=90 && ptrY[3*j+2]<=118){
			if(skin[( ptrY[3*j+1] )*256 + ptrY[3*j+2]] > 0 && ptrY[3*j] >140 ){
			
				//	cout<<(int)(uchar)(skinCrCbHist->imageData[ptrY[3*j+1]*256+ptrY[3*j+2]])<<endl;
				/*ptrRGB[3 * j]=0;
				ptrRGB[3 * j + 1]=0;
				ptrRGB[3 * j + 2]=0;*/

			//	skin[ ptrY[3*j+1]*256 + ptrY[3*j+2]] = 255;
			//	cout<<(int)ptrY[3*j+1]<<" "<<(int)ptrY[3*j+2]<<endl;
				outData[i*out->widthStep+j]=255;
				//cout<<i<<" "<<j<<endl;
			}
		}
	}
	/*cvNamedWindow("eclipse");
	cvShowImage("eclipse",skinCrCbHist);*/

	IplImage* temp = cvCreateImage(cvSize(out->width,out->height),out->depth,out->nChannels);
	if(!element){
		element=cvCreateStructuringElementEx( 4, 4, 1, 1, CV_SHAPE_ELLIPSE, 0);
	}
	cvSmooth(out, out, CV_MEDIAN, 3, 0, 0, 0); 
	cvDilate( out, temp, element, 1);
	cvErode( temp, out, element, 1);

	cvThreshold( out, out, 1, 255, CV_THRESH_BINARY );
	//cvNamedWindow("out");
	//cvShowImage("out",out);
	cvFindContours( out, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );

	for( ; contour != 0; contour = contour->h_next )
	{

		float area = fabs(cvContourArea( contour, CV_WHOLE_SEQ )); 
		if(area>30 && area <100){
			//	cout<<"face appear"<<endl;
			CvRect rect=cvBoundingRect(contour);
			double width_height_ratio = (double)rect.width / (double)rect.height;
			if(width_height_ratio > 0.5 && width_height_ratio <1.3){
				faceList->push_back(cvRect(rect.x,rect.y,rect.width,rect.height));
				//	cvRectangle(img,cvPoint(rect.x,rect.y),cvPoint(rect.x+rect.width,rect.y+rect.height),cvScalar(255,0,0),2);
			}
		}
	}
	cvReleaseImage(&temp);
	cvReleaseImage(&out);
	cvReleaseImage(&skinCrCbHist);
	return faceList;
}
//用adaboost验证正脸
int FaceDetection::VerifyFace(IplImage* img){
	//cvRunHaarClassifierCascade(cascade,cvPoint(0,0));
	CvArr *sum, *sqSum, *tilted;
	IplImage* grayImg = cvCreateImage( cvGetSize(img),img->depth,1);
	cvCvtColor(img,grayImg,CV_BGR2GRAY);

	/*cvEqualizeHist(grayImg,grayImg);
	cvClearMemStorage(storage);
	CvSeq* faces = cvHaarDetectObjects( grayImg, cascade, storage,1.1, 2, 0,
		cvSize(10, 10),cvSize(50, 50) );
	if(faces->total>0){
		return 1;
	}
	return -1;*/
	//存入积分图
	cvResize(grayImg,normalFace);
	cvEqualizeHist(normalFace,normalFace);
	sum = cvCreateMat( normalFace->height+1, normalFace->width+1, CV_32SC1 );
	sqSum = cvCreateMat( normalFace->height+1, normalFace->width + 1, CV_64FC1 );
	//icvCreateHidHaarClassifierCascade(cascade);
	tilted = cvCreateMat( normalFace->height+1, normalFace->width + 1, CV_32SC1 ); 
	cvIntegral(normalFace, sum, sqSum, tilted);
	cvSetImagesForHaarClassifierCascade(cascade, sum, sqSum,tilted, 1);
	cvReleaseImage(&grayImg);
	return cvRunHaarClassifierCascade(cascade, cvPoint(0,0), 0); 
}
//用adaboost验证侧脸
int FaceDetection::VerifyProfileFace(IplImage* img){
	//cvRunHaarClassifierCascade(cascade,cvPoint(0,0));
	CvArr *sum, *sqSum, *tilted;
	IplImage* grayImg = cvCreateImage( cvGetSize(img),img->depth,1);
	cvCvtColor(img,grayImg,CV_BGR2GRAY);
	//存入积分图
	cvResize(grayImg,normalFace);
	cvEqualizeHist(normalFace,normalFace);
	sum = cvCreateMat( normalFace->height+1, normalFace->width+1, CV_32SC1 );
	sqSum = cvCreateMat( normalFace->height+1, normalFace->width + 1, CV_64FC1 );
	//icvCreateHidHaarClassifierCascade(cascade);
	tilted = cvCreateMat( normalFace->height+1, normalFace->width + 1, CV_32SC1 ); 
	cvIntegral(normalFace, sum, sqSum, tilted);
	cvSetImagesForHaarClassifierCascade(profile_cascade, sum, sqSum,tilted, 1);
	cvReleaseImage(&grayImg);
	return cvRunHaarClassifierCascade(profile_cascade, cvPoint(0,0), 0); 
}
// detect face by Adaboost
vector<CvRect>* FaceDetection::DetectFaces(IplImage* img){
	IplImage* grayImg=cvCreateImage(cvSize(img->width,img->height),img->depth,1);
	cvCvtColor(img,grayImg,CV_BGR2GRAY);
	cvEqualizeHist(grayImg,grayImg);
	cvClearMemStorage(storage);
	faceList->clear();
	double t = (double)cvGetTickCount();
	CvSeq* faces = cvHaarDetectObjects( grayImg, cascade, storage,1.1, 2, 0,
		cvSize(10, 10),cvSize(50, 50) );
	t = (double)cvGetTickCount() - t;
	//cout<<"detection time = "<< t/((double)cvGetTickFrequency()*1000)<<endl;
	for( int i = 0; i < (faces ? faces->total : 0); i++ )
	{
		cout<<"a face"<<endl;
		CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
		cout<<r->width<<" "<<r->height<<endl;
	//	cvRectangle(img,cvPoint(r->x,r->y),cvPoint(r->x+r->width,r->y+r->height),cvScalar(0,0,255),2);
		faceList->push_back(*r);

		
	}
	cvReleaseImage(&grayImg);
	return faceList;
}
vector<CvRect>* FaceDetection::DetectSome(IplImage* img){
	IplImage* grayImg=cvCreateImage(cvSize(img->width,img->height),img->depth,1);
	cvCvtColor(img,grayImg,CV_BGR2GRAY);
	cvEqualizeHist(grayImg,grayImg);
	cvClearMemStorage(storage);
	double t = (double)cvGetTickCount();
	CvSeq* some = cvHaarDetectObjects( grayImg, some_cascade, storage,1.1, 2, 0,
		cvSize(30, 30),cvSize(200, 200) );
	t = (double)cvGetTickCount() - t;
	//cout<<"detection time = "<< t/((double)cvGetTickFrequency()*1000)<<endl;
	for( int i = 0; i < (some ? some->total : 0); i++ )
	{
		cout<<"success"<<endl;
		CvRect* r = (CvRect*)cvGetSeqElem( some, i );
		cout<<r->width<<" "<<r->height<<endl;
		cvRectangle(img,cvPoint(r->x,r->y),cvPoint(r->x+r->width,r->y+r->height),cvScalar(0,0,255),2);
	}
	cvReleaseImage(&grayImg);
	return NULL;
}