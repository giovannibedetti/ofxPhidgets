// - InterfaceKit simple -
// This simple example simply creates an InterfaceKit handle, hooks the event handlers and opens it.  It then waits
// for an InterfaceKit to be attached and waits for events to be fired. We progress through three steps, 1. Normal settings,
// 2. Setting analog sensor sensitivity to 100, 3. Toggling Ratiometric, waiting for user input to proceed to next step to allow
// data to be read.
//
// Copyright 2008 Phidgets Inc.  All rights reserved.
// This work is licensed under the Creative Commons Attribution 2.5 Canada License.
// view a copy of this license, visit http://creativecommons.org/licenses/by/2.5/ca/

// ofxaddon extension by Naoto HIEDA, 2014

#include <stdio.h>
#include <phidget21.h>

#include "ofMain.h"

class ofxPhidgets {
protected:
	static int CCONV AttachHandler(CPhidgetHandle IFK, void *userptr);
	static int CCONV DetachHandler(CPhidgetHandle IFK, void *userptr);
	static int CCONV ErrorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown);
	static int CCONV InputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int State);
	static int CCONV OutputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int State);
	static int CCONV SensorChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int Value);
	bool initialized;
	
public:
	virtual void init() = 0;
	virtual void update() = 0;
	virtual void exit() = 0;
};

class ofxPhidgetsStepper : public ofxPhidgets {
	
public:
	void init();
	void update();
	void exit();
	~ofxPhidgetsStepper() { exit(); };
	void setTargetPosition(int);
	int getTargetPosition() const;
	int getCurrentPosition() const;
	bool isMoving() const;
	
private:
	static int CCONV PositionChangeHandler(CPhidgetStepperHandle stepper, void *usrptr, int Index, __int64 Value);
	int display_properties(CPhidgetStepperHandle phid);
	
	int result;
	__int64 curr_pos, target_pos;
	const char *err;
	double minAccel, maxVel;
	int stopped;
	
	//Declare an stepper handle
	CPhidgetStepperHandle stepper = 0;
};

class ofxPhidgetsIfkit : public ofxPhidgets {
	
public:
	void init();
	void update();
	void exit();
	~ofxPhidgetsIfkit() { exit(); };

private:
	int display_properties(CPhidgetInterfaceKitHandle phid);
	
	int result, numSensors, i;
	const char *err;
	
	//Declare an InterfaceKit handle
	CPhidgetInterfaceKitHandle ifKit = 0;
};


class ofxPhidgetsManager : public ofxPhidgets {
public:
	void init();
	void update();
	void exit();
	CPhidgetHandle* getDevices();
	bool isInitialized();

	int getDevicesNumber();
	~ofxPhidgetsManager() { exit(); };

private:
	 
	 static int CCONV AttachHandler(CPhidgetHandle MAN, void *userptr);
	 static int CCONV DetachHandler(CPhidgetHandle MAN, void *userptr);
	 static int CCONV ErrorHandler(CPhidgetManagerHandle MAN, void *userptr, int ErrorCode, const char *unknown);
	int display_devices(CPhidgetManagerHandle MAN);
	 int numDevices;
	 CPhidgetHandle* devices;
	 CPhidgetManagerHandle manager;
};

class ofxPhidgetsRFID : public ofxPhidgets {

public:
	void init();
	void update();
	void exit();

	char* LastTag();
	bool  TagStatus();
	void setOfAppPtr(void *usrPtr);

	~ofxPhidgetsRFID() { exit(); };


private:
	int display_properties(CPhidgetRFIDHandle phid);
	static int CCONV TagHandler(CPhidgetRFIDHandle RFID, void *usrptr, char *TagVal, CPhidgetRFID_Protocol proto);
	static int CCONV TagLostHandler(CPhidgetRFIDHandle RFID, void *usrptr, char *TagVal, CPhidgetRFID_Protocol proto);

	 void setTag(CPhidgetRFIDHandle RFID, char* Tag);

	

	//int result, i;
	const char *err;
	//is true when we handle a new tag
	 bool newTagRead;
	 char* currentTagValue;
	 //void *ofAppPtr;
	CPhidgetRFIDHandle currRFID;

	
};
