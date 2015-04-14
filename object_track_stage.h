#ifndef object_track_stage_h__
#define object_track_stage_h__
#include "moving_object_detection.h"
#include "face_detection.h"
#include "../system/stage.h"
class ObjectTrackStage: public Stage{
public:
	static ObjectTrackStage* Instance();
	virtual void Execute();
protected:
	ObjectTrackStage();
private:
	static ObjectTrackStage* _instance;
	MovingObjectDetection* moving_object_detection;
	FaceDetection* face_detection;
};
#endif // object_track_stage_h__
