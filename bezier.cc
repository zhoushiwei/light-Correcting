#include "stdafx.h"
#include "bezier.h"
#include <cv.h>
#include <cxcore.h>
#include <vector>
using namespace std;
void Bezier::CreateCurve(CvPoint *originPoint, int originCount,vector<CvPoint> &curvePoint){
	//���Ƶ�����ϵ�� ��������0.6�Ϻã�CvPoint��opencv�ģ������ж���ṹ��(x,y)
	float scale = 0.6;
	CvPoint* midpoints = new CvPoint[originCount];
	//�����е�     
	for(int i = 0 ;i < originCount ; i++){    
		int nexti = (i + 1) % originCount;
		midpoints[i].x = (originPoint[i].x + originPoint[nexti].x)/2.0;
		midpoints[i].y = (originPoint[i].y + originPoint[nexti].y)/2.0;
	}    

	//ƽ���е�
	CvPoint* extrapoints=new CvPoint[2 * originCount]; 
	for(int i = 0 ;i < originCount ; i++){
		int nexti = (i + 1) % originCount;
		int backi = (i + originCount - 1) % originCount;
		CvPoint midinmid;
		midinmid.x = (midpoints[i].x + midpoints[backi].x)/2.0;
		midinmid.y = (midpoints[i].y + midpoints[backi].y)/2.0;
		int offsetx = originPoint[i].x - midinmid.x;
		int offsety = originPoint[i].y - midinmid.y;
		int extraindex = 2 * i;
		extrapoints[extraindex].x = midpoints[backi].x + offsetx;
		extrapoints[extraindex].y = midpoints[backi].y + offsety;
		//�� originPoint[i]�������� 
		int addx = (extrapoints[extraindex].x - originPoint[i].x) * scale;
		int addy = (extrapoints[extraindex].y - originPoint[i].y) * scale;
		extrapoints[extraindex].x = originPoint[i].x + addx;
		extrapoints[extraindex].y = originPoint[i].y + addy;

		int extranexti = (extraindex + 1)%(2 * originCount);
		extrapoints[extranexti].x = midpoints[i].x + offsetx;
		extrapoints[extranexti].y = midpoints[i].y + offsety;
		//�� originPoint[i]�������� 
		addx = (extrapoints[extranexti].x - originPoint[i].x) * scale;
		addy = (extrapoints[extranexti].y - originPoint[i].y) * scale;
		extrapoints[extranexti].x = originPoint[i].x + addx;
		extrapoints[extranexti].y = originPoint[i].y + addy;

	}    

	CvPoint controlPoint[4];
	//����4���Ƶ㣬��������������
	for(int i = 0 ;i < originCount-1 ; i++){
		controlPoint[0] = originPoint[i];
		int extraindex = 2 * i;
		controlPoint[1] = extrapoints[extraindex + 1];
		int extranexti = (extraindex + 2) % (2 * originCount);
		controlPoint[2] = extrapoints[extranexti];
		int nexti = (i + 1) % originCount;
		controlPoint[3] = originPoint[nexti];    
		float u = 1;
		while(u >= 0){
			int px = bezier3funcX(u,controlPoint);
			int py = bezier3funcY(u,controlPoint);
			//u�Ĳ����������ߵ�����
			u -= 0.005;
			CvPoint tempP = cvPoint(px,py);
			//�������ߵ� 
			curvePoint.push_back(tempP);
		}    
	}
}
float Bezier::bezier3funcX(float uu,CvPoint *controlP){
	float part0 = controlP[0].x * uu * uu * uu;
	float part1 = 3 * controlP[1].x * uu * uu * (1 - uu);
	float part2 = 3 * controlP[2].x * uu * (1 - uu) * (1 - uu);
	float part3 = controlP[3].x * (1 - uu) * (1 - uu) * (1 - uu);   
	return part0 + part1 + part2 + part3; 
}
float Bezier::bezier3funcY(float uu,CvPoint *controlP){
	float part0 = controlP[0].y * uu * uu * uu;
	float part1 = 3 * controlP[1].y * uu * uu * (1 - uu);
	float part2 = 3 * controlP[2].y * uu * (1 - uu) * (1 - uu);
	float part3 = controlP[3].y * (1 - uu) * (1 - uu) * (1 - uu);   
	return part0 + part1 + part2 + part3; 
}