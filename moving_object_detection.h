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
	vector<CvRect>* rects;//检测出来的面积较大的矩形的集合
//	vector<CvPoint>* points;// 矩形中心点集合
	double dist(CvPoint  p1, CvPoint p2);
	CvPoint CvtRectToPoint(CvRect rect);
};
#endif // normal_track_h__
