#ifndef normal_track_h__
#define normal_track_h__
#include <cv.h>
#include <vector>
using namespace std;
class MovingObjectDetection{
public:
	MovingObjectDetection();
	void movingObjectDetect(IplImage* frontFrame);
private:
	IplImage* temp;
	IplConvKernel *element;
	CvMemStorage* storage ;
	CvSeq* contour;
	vector<CvRect>* rects;//������������ϴ�ľ��εļ���
//	vector<CvPoint>* points;// �������ĵ㼯��
	double dist(CvPoint  p1, CvPoint p2);
	CvPoint CvtRectToPoint(CvRect rect);
};
#endif // normal_track_h__
