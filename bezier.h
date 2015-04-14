#ifndef bezier_h__
#define bezier_h__
#include <cv.h>
#include <cxcore.h>
#include <vector>
using namespace std;
class Bezier{
public:
	void CreateCurve(CvPoint *originPoint, int originCount,vector<CvPoint> &curvePoint);
private:
	float bezier3funcX(float uu,CvPoint *controlP);
	float bezier3funcY(float uu,CvPoint *controlP);
};

#endif // bezier_h__