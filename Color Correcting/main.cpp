//
//  main.cpp
//  Color Correcting
//
//  Created by zhou shiwei on 13-9-8.
//  Copyright (c) 2013年 zhou shiwei. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "face_detection.h"
#include <cv.h>
#include <cxcore.h>
using namespace cv;
using namespace std;
void CompensateLight(IplImage* img);
int* histgram;
int main(int argc, const char * argv[])
{
    Mat input,output;
    FaceDetection *face_detection;
    face_detection=new FaceDetection;
    histgram=new int[256];
    for (int i=0; i<256; i++) {
        histgram[i]=0;
    }
   
//    input=imread("/Users/zyun1/Dropbox/光照归一化/12.jpg");
//    imshow("Origin", input);
    IplImage *read=cvLoadImage("/Users/zhoushiwei/Dropbox/光照归一化/12.jpg");
    
    face_detection->CompensateLight(read);
    face_detection->DetectSkinCrCb(read);
    TickMeter time;
    time.start();
   // CompensateLight(&read);
    time.stop();
    printf("CompenstateLight take time %.2f",time.getTimeMilli());
    cvNamedWindow("read");
    cvShowImage("read", read);
    /*
  //  int histogram[256];
    const int thresholdnum = 100; 
    int nl=input.rows;
    int nc=input.cols;
    uchar RedTemp,GreenTemp,BlueTemp;
    for(int i=0;i<nl;i++){
        for (int j=0; j<nc; j++) {
            RedTemp=input.at<cv::Vec3b>(j,i)[2];
            GreenTemp=input.at<cv::Vec3b>(j,i)[1];
            BlueTemp=input.at<cv::Vec3b>(j,i)[0];
          //  printf("%d %d %d\n",RedTemp,GreenTemp,BlueTemp);
            int gray = (RedTemp * 299 + GreenTemp * 587 + BlueTemp * 114)/1000;
            histgram[gray]++; 
        }
    }
    
    int calnum =0;
    int total = input.rows * input.cols;
    int num;
   const float thresholdco=0.05;
    for (int i=0; i<256; i++) {
        if ((float)calnum/total<thresholdco) {
            printf("calnum %d",calnum);
            calnum+= histgram[255-i];//histogram保存的是某一灰度值的像素个数,calnum是边界灰度之上的像素数
            num = i;
        }else
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
    averagegray /=calnum;
    //得到光线补偿的系数
    float co = 255.0/(float)averagegray;
    printf("得到光线补偿的系数 %.2f\n",co);
    
    for(int i=0;i<nl;i++){
        for (int j=0; j<input.cols; j++) {
            input.at<cv::Vec3b>(j,i)[0]=(input.at<cv::Vec3b>(j,i)[0]*co>255) ? 255:(input.at<cv::Vec3b>(j,i)[0]*co);
            input.at<cv::Vec3b>(j,i)[1]=(input.at<cv::Vec3b>(j,i)[1]*co>255) ? 255:(input.at<cv::Vec3b>(j,i)[1]*co);
            input.at<cv::Vec3b>(j,i)[2]=(input.at<cv::Vec3b>(j,i)[2]*co>255) ? 255:(input.at<cv::Vec3b>(j,i)[2]*co);
        }
    }
    imshow("ing", input);
   */
    waitKey(10000000);

    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}

void CompensateLight(IplImage* img){
	unsigned char RedTemp,GreenTemp,BlueTemp;
  
	//	cout<<"compensate"<<endl;
	for (int i=0; i < img->height; i++) {
		uchar* ptr = (uchar*)(img->imageData+i*img->widthStep);
		for (int j=0; j < img->width; j++) {
			BlueTemp=ptr[3*j];
			GreenTemp=ptr[3*j+1];
			RedTemp=ptr[3*j+2];
			int gray = (RedTemp * 299 + GreenTemp * 587 + BlueTemp * 114)/1000;
          //  printf("%d\n",gray);
			histgram[gray]++;
		}
	}
	int calnum =0;
	int total = img->width * img->height;
	int num;
	const float thresholdco = 0.05;
    
	for(int i =0;i<256;i++)
	{
		if((float)calnum/total < thresholdco) //µ√µΩ«∞5%µƒ∏ﬂ¡¡œÒÀÿ°£
		{
			calnum+= histgram[255-i];//histogram±£¥Êµƒ «ƒ≥“ªª“∂»÷µµƒœÒÀÿ∏ˆ ˝,calnum «±ﬂΩÁª“∂»÷Æ…œµƒœÒÀÿ ˝
			num = i;
		}
		else
			break;
	}
	int averagegray = 0;
	calnum =0;
    
	//µ√µΩ¬˙◊„Ãıº˛µƒœÛÀÿ◊‹µƒª“∂»÷µ
	for(int i = 255;i>=255-num;i--)
	{
		averagegray += histgram[i]*i;//◊‹µƒœÒÀÿµƒ∏ˆ ˝*ª“∂»÷µ
		calnum += histgram[i];//◊‹µƒœÒÀÿ ˝
	}
	averagegray /=calnum; // top 5% µƒ∆Ωæ˘ª“∂»÷µ
	//µ√µΩπ‚œﬂ≤π≥•µƒœµ ˝
	float co = 255.0/(float)averagegray;
	//œ¬√Êµƒ—≠ª∑∂‘ÕºœÛΩ¯––π‚œﬂ≤π≥•
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


