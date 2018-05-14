#pragma config(UART_Usage, UART1, uartVEXLCD, baudRate19200, IOPins, None, None)
#pragma config(UART_Usage, UART2, uartNotUsed, baudRate4800, IOPins, None, None)
#pragma config(Sensor, in2,    lightSensor,    sensorReflection)
#pragma config(Sensor, in3,    lineFollowerLEFT, sensorLineFollower)
#pragma config(Sensor, in4,    lineFollowerCENTER, sensorLineFollower)
#pragma config(Sensor, in5,    lineFollowerRIGHT, sensorLineFollower)
#pragma config(Sensor, dgtl1,  sonarSensor,    sensorSONAR_cm)
#pragma config(Sensor, dgtl4,  leftEncoder,    sensorQuadEncoder)
#pragma config(Sensor, dgtl6,  rightEncoder,   sensorQuadEncoder)
#pragma config(Sensor, dgtl12, breakSwitch,    sensorTouch)
#pragma config(Motor,  port2,           rightMotor,    tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port3,           leftMotor,     tmotorServoContinuousRotation, openLoop, reversed)
#pragma config(Motor,  port5,           clawMotor,     tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port6,           armMotor,      tmotorServoContinuousRotation, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "constants.h"

/*
			|Analog ----------------------------------|
			|1|...|PM1-Potentiometer Arm							|
			|2|...|LS1-Light sensor 1-Photon sensor		|
			|3|...|LFL-Line Follower Left							|
			|4|...|LFM-Line Follower Middle						|
			|5|...|LFR-Line Follower Right						|
			|Digital ---------------------------------|
			|1 |..|SS1-Sonar Sensor 1									|
			|2 |..|SS2-Sonar Sensor 2									|
			|3 |..|TC1-Touch 1-A� framan							|
			|4 |..|LE1-Left Encoder 1									|
			|5 |..|LE2-Left Encoder 2									|
			|6 |..|RE1-Right Encoder 1								|
			|7 |..|RE2-Right Encoder 2								|
			|11|..|GS1-Gyroscope 1										|
			|12|..|BRS-Break Switch										|
			|Motor -----------------------------------|
			|2|...|RWM-Right Wheel Motor							|
			|3|...|LWM-Left Wheel Motor								|
			|5|...|CLM-M3-Claw Motor									|
			|6|...|ARM-Arm Motor											|
			|UART ------------------------------------|
			|1|...|LCD-LCD Display										|
			|Remote ----------------------------------|
			|Btn8U|BRB-Break Button										|
			|Btn7U|STU-Switch Task Up									|
			|Btn7D|STU-Switch Task Down								|
			|Ch3|.|MFB-Motors Forwards Backwards			|
			|Ch4|.|MLR-Motors Left Right							|
			|Ch2|.|AMU-Arm Motor Up	Down							|
			|Btn6U|CMO-Claw Motor Open								|
			|Btn6D|CMC-Claw Motor Close								|
			|-----------------------------------------|
*/

void resetEncoders()
{
	SensorValue[rightEncoder] = 0;
	SensorValue[leftEncoder]  = 0;
}

void stopMotors()
{
	motor[rightMotor] = 0;
	motor[leftMotor]  = 0;
}

void reset(unsigned int sleepMilli = 0)
{
	stopMotors();
	motor[armMotor] = 0;
	motor[clawMotor] = 0;
	resetEncoders();
	Sleep(sleepMilli);
}

void calibrate(int speed=80)
{
	int direction = speed/abs(speed);
	float Ntime1 = 0, Ntime2 = 0, Ntime3 = 0;
	int Stateout = 0;
	resetEncoders();
	for (int i=1; i<2000; i++)
	{
		motor[rightMotor] = speed;
		motor[leftMotor] = speed;
	}
	resetEncoders();
	Ntime3 = nPgmTime;
  while(Stateout<2)
  {
		Stateout = 0;
  	if (direction*-SensorValue[rightEncoder]<direction*speed*10)
  	{
  		motor[rightMotor] = speed;
  		Ntime1=((float)nPgmTime);
  	}
  	else if (direction*-SensorValue[rightEncoder]==direction*speed*10)
  	{
  		motor[rightMotor] = speed;
  	}
  	else
  	{
  		motor[rightMotor] = 0;
  		Stateout = Stateout + 1;
  	}
  	if (direction*-SensorValue[leftEncoder]<direction*speed*10)
  	{
  		motor[leftMotor] = speed;
  		Ntime2=((float)nPgmTime);
  	}
  	else if (direction*-SensorValue[leftEncoder]==direction*speed*10)
  	{
  		motor[leftMotor] = speed;
  	}
  	else
  	{
  		motor[leftMotor] = 0;
  		Stateout = Stateout + 1;
  	}
  }
  Ntime1 = (Ntime1 - Ntime3);
	Ntime2 = (Ntime2 - Ntime3);
	if (Ntime1 >Ntime2)
	{
		Ntime3=Ntime2;
	}
	else
	{
		Ntime3=Ntime1;
	}
	Ntime1 = Ntime1/Ntime3;
	Ntime2 = Ntime2/Ntime3;
	resetEncoders();
	speedArr[0] = 2*(-speed*(2-abs(direction*speed/126))*(Ntime1-Ntime2));
	speedArr[1] = 2*(-speed*(2-abs(direction*speed/126))*(Ntime2-Ntime1));
}

string moveKey = "";
bool drive(unsigned int distance, const string key, bool reversed = false, int speed=80, unsigned int waitTime = 0)
{
	int amount = ((float)distance/LITTLE_D_C)*300.0;
	if(moveKey != key)
	{
		reset(waitTime);
		moveKey = key;
	}
	if(!reversed)
	{
		if(SensorValue[leftEncoder] > -amount && SensorValue[rightEncoder] > -amount)
		{
			motor[rightMotor] = (speed-speedArr[0]);
			motor[leftMotor]  = (speed-speedArr[1]);
			return false;
		}
	}
	else
	{
		if(SensorValue[leftEncoder] < amount && SensorValue[rightEncoder] < amount)
		{
			motor[rightMotor] = -speed;
			motor[leftMotor]  = -speed;
			return false;
		}
	}
	return true;
}

void drive(unsigned int speed=80)
{
	if(speed>0)
	{
		motor[rightMotor] = speed-speedArr[0];
		motor[leftMotor]  = speed-speedArr[1];
	}
	else
	{
		motor[rightMotor] = -speed;
		motor[leftMotor]  = -speed;
	}
}

void turn(int degs)
{
	stopMotors();
	resetEncoders();
	int speed = 80;
	int amount = ((BIG_D/LITTLE_D)*(float)abs(degs))*CORRECTION_CONSTANT;
	if(degs < 0)
	{
		while(SensorValue[leftEncoder] > -amount && SensorValue[rightEncoder] < amount)
		{
			motor[rightMotor] = -speed;
			motor[leftMotor]  = speed;
		}
	}
	else if(degs > 0)
	{
		while(SensorValue[leftEncoder] < amount && SensorValue[rightEncoder] > -amount)
		{
			motor[rightMotor] = speed;
			motor[leftMotor]  = -speed;
		}
	}
}

int corrected(int channel)
{
	if (vexRT[channel] >= 100)
		return 127;
	else if (vexRT[channel] <= -100)
		return -127;
	else
		return vexRT[channel];
}

int softCorrected(int channel)
{
	if (vexRT[channel] >= 35)
		return 127;
	else if (vexRT[channel] <= -35)
		return -127;
	else
		return vexRT[channel];
}

task pause
{
	bLCDBacklight = true;
  displayLCDCenteredString(0,"paused");
	while(true)
	{
		reset();
	}
}

task verk1
{
	bLCDBacklight = true;
  displayLCDCenteredString(0,"verk1");
	while(!drive(50, "first")){};
	turn(90);
	while(!drive(50, "second")){};
	turn(-90);
	while(!drive(50, "third")){};
	turn(-90);
	while(!drive(50, "fourth")){};
}

task verk2
{
	Timer tim;
	bool downed = false;
	reset();
	bLCDBacklight = true;
  displayLCDCenteredString(0,"verk2");
  while(true)
  {
  	if(corrected(Ch3) == 127 || corrected(Ch3) == -127 || corrected(Ch4) == 127 || corrected(Ch4) == -127)
  	{
  		short mult = softCorrected(Ch3) == 127 ? -1:1;
  		if(corrected(Ch4) == 127)
  		{
  			motor[rightMotor] = -80;
  			motor[leftMotor] = 80;
  		}
  		else if(corrected(Ch4) == -127)
  		{
  			motor[rightMotor] = 80;
  			motor[leftMotor] = -80;
  		}
  		else
  		{
  			motor[rightMotor] = -1*mult*80;
  			motor[leftMotor] = -1*mult*80;
  		}
  	}
  	else
  	{
  		stopMotors();
  	}

    if(corrected(Ch2) == 127)
    {
      motor[armMotor] = -60;
      downed = false;
    }
    else if(corrected(Ch2) == -127)
    {
      motor[armMotor] = 40;
      downed = true;
      startTimer( &tim, 300 );
    }
    else
    {
      motor[armMotor] = downed ? -15:0;
      if (timerPassed( &tim ))
      	downed = false;
    }
    if(vexRT[Btn6U])
    {
      motor[clawMotor] = -60;
    }
    else if(vexRT[Btn6D])
    {
      motor[clawMotor] = 60;
    }
    else
    {
    	motor[clawMotor] = 0;
    }
  }
}

task verk3
{
	bLCDBacklight = true;
  displayLCDCenteredString(0,"verk3");
	for(unsigned int i = 0; true; i++)
	{
		if(!(SensorValue(lightSensor) > 230))
		{
			if(SensorValue(sonarSensor) > minDist && turned)
			{
				moveKey="";
				drive();
			}
			else
			{

				writeDebugStreamLine("turning");
				turned = false;
				if(drive(50, "backup", true))
				{
					writeDebugStreamLine("turning inner");
					turn(45);
					turned = true;
				}
			}
		}
		else
		{
			stopMotors();
		}
	}
}

task verk4
{
	bLCDBacklight = true;
  displayLCDCenteredString(0,"verk4");
	while(true)
  {
    if(SensorValue(lineFollowerCENTER) > threshold)
    {
      motor[leftMotor]  = followSpeed;
      motor[rightMotor] = followSpeed;
    }
    else if(SensorValue(lineFollowerRIGHT) > threshold)
    {
      motor[leftMotor]  = followSpeed;
      motor[rightMotor] = 0;
    }
    else if(SensorValue(lineFollowerLEFT) > threshold)
    {
      motor[leftMotor]  = 0;
      motor[rightMotor] = followSpeed+15;
    }
  }
}

task verk5
{
	bLCDBacklight = true;
  displayLCDCenteredString(0,"verk5");
  StartTask(verk4);
  bLCDBacklight = true;
  displayLCDCenteredString(0,"verk5");
  Timer tim;
  Timer tim2;
  Timer tim3;
  Timer tim4;
  startTimer(&tim, OPEN_CLOSE_TIME);
  while(!timerPassed(&tim))
  {
  	motor[clawMotor] = 60;
	}
	motor[clawMotor] = 0;
  while(true)
  {
  	if(SensorValue(sonarSensor) <= GRAB_DIST)
  	{
  		StopTask(verk4);
  		stopMotors();
  		startTimer(&tim2, OPEN_CLOSE_TIME);
  		while(!timerPassed(&tim2))
  		{
  			motor[clawMotor] = -60;
			}
			startTimer(&tim3, LIFT_TIME);
			while(!timerPassed(&tim3))
  		{
  			motor[armMotor] = -120;
			}
			motor[armMotor] = -15;
			turn(180);
			break;
  	}
	}
	followSpeed = 70;
	StartTask(verk4);
	bLCDBacklight = true;
  displayLCDCenteredString(0,"verk5");
	while(true)
	{
		if(SensorValue(sonarSensor) <= DROP_DIST)
		{
			StopTask(verk4);
			startTimer(&tim4, OPEN_CLOSE_TIME);
  		while(!timerPassed(&tim4))
  		{
  			motor[clawMotor] = 80;
			}
			break;
		}
	}
	followSpeed = 50;
	StartTask(pause);
}

task picker
{
	unsigned short rTaskID = taskID;
	taskID = 22;
	Timer pressTimer;
	startTimer(&pressTimer, 0);
	unsigned short topTask = 4;
	while(true)
	{
		if(timerPassed(&pressTimer))
		{
			if(vexRT[Btn8U] || SensorValue[breakSwitch])
			{
				rTaskID = 10;
			}
			else if(vexRT(Btn7U))
			{
				writeDebugStreamLine("pressed up");
				if(rTaskID == topTask || rTaskID == 10)
				{
					rTaskID = 0;
				}
				else
				{
					rTaskID++;
				}
				startTimer(&pressTimer, 700);
			}
			else if(vexRT(Btn7D))
			{
				writeDebugStreamLine("pressed down");
				if(rTaskID == 0 || rTaskID == 10)
				{
					rTaskID = topTask;
				}
				else
				{
					rTaskID--;
				}
				startTimer(&pressTimer, 700);
			}
		}
		if(rTaskID != taskID)
		{
			taskID = rTaskID;
			switch(taskID)
			{
				case 0:
					// stoppa alla nema task 1
					reset();
					StartTask(verk1);
					StopTask(verk2);
					StopTask(verk3);
					StopTask(verk4);
					StopTask(pause);
					break;
				case 1:
					// stoppa alla nema task 2
					reset();
					StopTask(verk1);
					StartTask(verk2);
					StopTask(verk3);
					StopTask(verk4);
					StopTask(pause);
					break;
				case 2:
					reset();
					StopTask(verk1);
					StopTask(verk2);
					StartTask(verk3);
					StopTask(verk4);
					StopTask(pause);
					break;
				case 3:
					reset();
					followSpeed=50;
					StopTask(verk1);
					StopTask(verk2);
					StopTask(verk3);
					StartTask(verk4);
					StopTask(pause);
					break;
				case 4:
					reset();
					StopTask(verk1);
					StopTask(verk2);
					StopTask(verk3);
					StopTask(verk4);
					StartTask(verk5);
					StopTask(pause);
					break;
				case 10:
					reset();
					// stoppa alla nema task 10
					StopTask(verk1);
					StopTask(verk1);
					StopTask(verk3);
					StopTask(verk4);
					StartTask(pause);
					break;
			}
		}
	}
}

task main()
{
	//calibrate(80);
	StartTask(picker);
	while(true){}
}