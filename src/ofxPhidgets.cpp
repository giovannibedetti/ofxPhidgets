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

#include "ofxPhidgets.h"

// - Stepper simple -
// This simple example sets up a Stepper object, hooks the event handlers and opens it for device connections.
// Once an Advanced Servo is attached it will move the motor to various positions.
//
// Please note that this example was designed to work with only one Phidget Stepper connected.
//
// Copyright 2008 Phidgets Inc.  All rights reserved.
// This work is licensed under the Creative Commons Attribution 2.5 Canada License.
// view a copy of this license, visit http://creativecommons.org/licenses/by/2.5/ca/

#include <stdio.h>
#include <phidget21.h>
//#include <unistd.h>

int CCONV ofxPhidgetsStepper::PositionChangeHandler(CPhidgetStepperHandle stepper, void *usrptr, int Index, __int64 Value)
{
	printf("Motor: %d > Current Position: %lld\n", Index, Value);
	return 0;
}

//Display the properties of the attached phidget to the screen.  We will be displaying the name, serial number and version of the attached device.
int ofxPhidgetsStepper::display_properties(CPhidgetStepperHandle phid)
{
	int serialNo, version, numMotors;
	const char* ptr;
	
	CPhidget_getDeviceType((CPhidgetHandle)phid, &ptr);
	CPhidget_getSerialNumber((CPhidgetHandle)phid, &serialNo);
	CPhidget_getDeviceVersion((CPhidgetHandle)phid, &version);
	
	CPhidgetStepper_getMotorCount (phid, &numMotors);
	
	printf("%s\n", ptr);
	printf("Serial Number: %10d\nVersion: %8d\n# Motors: %d\n", serialNo, version, numMotors);
	
	return 0;
}

void ofxPhidgetsStepper::init()
{
	target_pos = 0;
	
	//create the stepper object
	CPhidgetStepper_create(&stepper);
	
	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)stepper, ofxPhidgets::AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)stepper, ofxPhidgets::DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)stepper, ofxPhidgets::ErrorHandler, NULL);
	
	//Registers a callback that will run when the motor position is changed.
	//Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetStepper_set_OnPositionChange_Handler(stepper, ofxPhidgetsStepper::PositionChangeHandler, NULL);
	
	//open the device for connections
	CPhidget_open((CPhidgetHandle)stepper, -1);
	
	//get the program to wait for an stepper device to be attached
	printf("Waiting for Phidget to be attached....");
	if((result = CPhidget_waitForAttachment((CPhidgetHandle)stepper, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);
		printf("Problem waiting for attachment: %s\n", err);
		return 0;
	}
	
	//Display the properties of the attached device
	display_properties(stepper);
	
	//read event data
	printf("Reading.....\n");
	
	//This example assumes stepper motor is attached to index 0
	
	//Set up some initial acceleration and velocity values
	CPhidgetStepper_getAccelerationMin(stepper, 0, &minAccel);
	CPhidgetStepper_setAcceleration(stepper, 0, minAccel*2);
	CPhidgetStepper_getVelocityMax(stepper, 0, &maxVel);
	CPhidgetStepper_setVelocityLimit(stepper, 0, maxVel/2);
	
	//display current motor position if available
	if(CPhidgetStepper_getCurrentPosition(stepper, 0, &curr_pos) == EPHIDGET_OK)
	printf("Motor: 0 > Current Position: %lld\n", curr_pos);
	
	//Step 1: Position 0 - also engage stepper
	printf("Set to position 0 and engage.\n");
	
	CPhidgetStepper_setCurrentPosition(stepper, 0, 0);
	CPhidgetStepper_setEngaged(stepper, 0, 1);
}

void ofxPhidgetsStepper::update()
{
	CPhidgetStepper_getCurrentPosition(stepper, 0, &curr_pos);
	if( curr_pos != target_pos ) {
		CPhidgetStepper_setTargetPosition (stepper, 0, target_pos);
	}
}

void ofxPhidgetsStepper::setTargetPosition(int _target_pos)
{
	target_pos = _target_pos;
}

int ofxPhidgetsStepper::getTargetPosition() const {
	return target_pos;
}

int ofxPhidgetsStepper::getCurrentPosition() const {
	return curr_pos;
}

bool ofxPhidgetsStepper::isMoving() const {
	return curr_pos != target_pos;
}

void ofxPhidgetsStepper::exit()
{
	CPhidgetStepper_setTargetPosition(stepper, 0, 0);
	
	stopped = PFALSE;
	while(!stopped)
	{
		CPhidgetStepper_getStopped(stepper, 0, &stopped);
		//usleep(100000);
	}
	
	CPhidgetStepper_setEngaged(stepper, 0, 0);
	
	//since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
	printf("Closing...\n");
	CPhidget_close((CPhidgetHandle)stepper);
	CPhidget_delete((CPhidgetHandle)stepper);
}

int CCONV ofxPhidgets::AttachHandler(CPhidgetHandle IFK, void *userptr)
{
	int serialNo;
	const char *name;
	
	CPhidget_getDeviceName(IFK, &name);
	CPhidget_getSerialNumber(IFK, &serialNo);
	
	printf("%s %10d attached!\n", name, serialNo);
	
	return 0;
}

int CCONV ofxPhidgets::DetachHandler(CPhidgetHandle IFK, void *userptr)
{
	int serialNo;
	const char *name;
	
	CPhidget_getDeviceName (IFK, &name);
	CPhidget_getSerialNumber(IFK, &serialNo);
	
	printf("%s %10d detached!\n", name, serialNo);
	
	return 0;
}

int CCONV ofxPhidgets::ErrorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown)
{
	printf("Error handled. %d - %s", ErrorCode, unknown);
	return 0;
}

//callback that will run if an input changes.
//Index - Index of the input that generated the event, State - boolean (0 or 1) representing the input state (on or off)
int CCONV ofxPhidgets::InputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int State)
{
	printf("Digital Input: %d > State: %d\n", Index, State);
	return 0;
}

//callback that will run if an output changes.
//Index - Index of the output that generated the event, State - boolean (0 or 1) representing the output state (on or off)
int CCONV ofxPhidgets::OutputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int State)
{
	printf("Digital Output: %d > State: %d\n", Index, State);
	return 0;
}

//callback that will run if the sensor value changes by more than the OnSensorChange trigger.
//Index - Index of the sensor that generated the event, Value - the sensor read value
int CCONV ofxPhidgets::SensorChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int Value)
{
	printf("Sensor: %d > Value: %d\n", Index, Value);
	return 0;
}

//Display the properties of the attached phidget to the screen.  We will be displaying the name, serial number and version of the attached device.
//Will also display the number of inputs, outputs, and analog inputs on the interface kit as well as the state of the ratiometric flag
//and the current analog sensor sensitivity.
int ofxPhidgetsIfkit::display_properties(CPhidgetInterfaceKitHandle phid)
{
	int serialNo, version, numInputs, numOutputs, numSensors, triggerVal, ratiometric, i;
	const char* ptr;
	
	CPhidget_getDeviceType((CPhidgetHandle)phid, &ptr);
	CPhidget_getSerialNumber((CPhidgetHandle)phid, &serialNo);
	CPhidget_getDeviceVersion((CPhidgetHandle)phid, &version);
	
	CPhidgetInterfaceKit_getInputCount(phid, &numInputs);
	CPhidgetInterfaceKit_getOutputCount(phid, &numOutputs);
	CPhidgetInterfaceKit_getSensorCount(phid, &numSensors);
	CPhidgetInterfaceKit_getRatiometric(phid, &ratiometric);
	
	printf("%s\n", ptr);
	printf("Serial Number: %10d\nVersion: %8d\n", serialNo, version);
	printf("# Digital Inputs: %d\n# Digital Outputs: %d\n", numInputs, numOutputs);
	printf("# Sensors: %d\n", numSensors);
	printf("Ratiometric: %d\n", ratiometric);
	
	for(i = 0; i < numSensors; i++)
	{
		CPhidgetInterfaceKit_getSensorChangeTrigger (phid, i, &triggerVal);
		
		printf("Sensor#: %d > Sensitivity Trigger: %d\n", i, triggerVal);
	}
	
	return 0;
}


void ofxPhidgetsIfkit::init()
{
	//create the InterfaceKit object
	CPhidgetInterfaceKit_create(&ifKit);
	
	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)ifKit, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)ifKit, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)ifKit, ErrorHandler, NULL);
	
	//Registers a callback that will run if an input changes.
	//Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetInterfaceKit_set_OnInputChange_Handler (ifKit, InputChangeHandler, NULL);
	
	//Registers a callback that will run if the sensor value changes by more than the OnSensorChange trig-ger.
	//Requires the handle for the IntefaceKit, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetInterfaceKit_set_OnSensorChange_Handler (ifKit, SensorChangeHandler, NULL);
	
	//Registers a callback that will run if an output changes.
	//Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetInterfaceKit_set_OnOutputChange_Handler (ifKit, OutputChangeHandler, NULL);
	
	//open the interfacekit for device connections
	CPhidget_open((CPhidgetHandle)ifKit, -1);
	
	//get the program to wait for an interface kit device to be attached
	printf("Waiting for interface kit to be attached....");
	if((result = CPhidget_waitForAttachment((CPhidgetHandle)ifKit, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);
		printf("Problem waiting for attachment: %s\n", err);
		return 0;
	}
	
	//Display the properties of the attached interface kit device
	display_properties(ifKit);
	
	//read interface kit event data
	printf("Reading.....\n");
}

void ofxPhidgetsIfkit::update()
{
	printf("Modifying sensor sensitivity triggers....\n");
	
	//get the number of sensors available
	CPhidgetInterfaceKit_getSensorCount(ifKit, &numSensors);
	
	//Change the sensitivity trigger of the sensors
	for(i = 0; i < numSensors; i++)
	{
		CPhidgetInterfaceKit_setSensorChangeTrigger(ifKit, i, 100);  //we'll just use 10 for fun
	}
	
	//read interface kit event data
	printf("Reading.....\n");
	
	//keep displaying interface kit data until user input is read
	printf("Press any key to go to next step\n");
	getchar();
	
	printf("Toggling Ratiometric....\n");
	
	CPhidgetInterfaceKit_setRatiometric(ifKit, 0);
	
	//read interface kit event data
	printf("Reading.....\n");
	
	//keep displaying interface kit data until user input is read
	printf("Press any key to end\n");
	getchar();
	
}

void ofxPhidgetsIfkit::exit()
{
	//since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
	printf("Closing...\n");
	CPhidget_close((CPhidgetHandle)ifKit);
	CPhidget_delete((CPhidgetHandle)ifKit);
}
