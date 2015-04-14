#ifndef face_detection_h__
#define face_detection_h__
#include "cv.h"
#include <vector>

using namespace std;
class FaceDetection{
public:
	FaceDetection();
	vector<CvRect>* DetectFaces(IplImage* img); // Adaboost�������������
	vector<CvRect>* DetectSkinCrCb(IplImage* img); //CrCb��Բģ��
	vector<CvRect>* DetectSome(IplImage* img);
	void CompensateLight(IplImage* img); //���߲���
	int VerifyFace(IplImage* img);
	int VerifyProfileFace(IplImage* img);
private:
	vector<CvRect>* faceList;
	CvHaarClassifierCascade* cascade;
	CvHaarClassifierCascade* profile_cascade;
	CvHaarClassifierCascade* some_cascade;
	CvMemStorage* storage;
	int* histgram;
	IplConvKernel *element;
	CvSeq* contour;
	IplImage* normalFace;
};

#endif // face_detection_h__
