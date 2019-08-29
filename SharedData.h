/*Model view Controller:

This is a model class that contain data that is shared between the
Viewer-Opengl and Controller-Widgets

reference: Jaime King, https://www.youtube.com/watch?v=JKeoJ9nuTE0
*/

#ifndef SHARED_DATA_H
#define SHARED_DATA_H

class SharedData
{
public:
	SharedData();
	~SharedData() {};

	//enable the track ball for the scene
	void enableTrackBallFlag() { onTrackBall = true; }
	void disableTrackBallFlag() { onTrackBall = false; }

	bool queryTrackBallFlag() {		return onTrackBall;	}

private:
	//flag to check whether track ball is enabled or not
	bool onTrackBall;

	

};
#endif // !SHARED_DATA_H



