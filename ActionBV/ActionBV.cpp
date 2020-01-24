#include "ActionBV.h"

#include <iostream>

ArRangeDevice *mySonar;

double range;
double rangeLeft;
double rangeRight;

double speed;
double maxSpeed = 500;

ArPTZ *ptz;

//Constructor that initialzies the Action.
//@border: threshold for the turning and tilting trigger according to the camera
//
//Initializes the SharedMemory instance and exits the program if that fails.
//Finally it reads X and Y from the SharedMemory and writes it to the protected variables m_X/m_Y.
ActionBV::ActionBV(double border) : ArAction("PioneerBV")
{
	m_Border = border;
	mySonar = NULL;

	m_SharedMemory = new Shared_Memory();
	m_SharedMemory->SM_Init();

	if (m_SharedMemory == NULL)
    {
        deactivate();
		Aria::exit(1);
    }

	m_X = m_SharedMemory->SM_GetFloat(INDEX_X);
	m_Y = m_SharedMemory->SM_GetFloat(INDEX_Y);
	m_Visible = m_SharedMemory->SM_GetFloat(INDEX_VISIBLE);
}

//Function that defines the specific action that gets called by the resolver and returns it as ArActionDesired
ArActionDesired *ActionBV::fire(ArActionDesired currentDesired)
{
	//Reset the ArActionDesired variable
	m_Desired.reset();

	//Check if the sonar has been set and if not return an empty ArActionDesired
	if (mySonar == NULL)
	{
		deactivate();
		return NULL;
	}

	//Read values from the sonar sensor and write them into the 3 different variables for later use.
	range = mySonar->currentReadingPolar(-45, 45) - myRobot->getRobotRadius();
	rangeLeft = mySonar->currentReadingPolar(-180, -45);
	rangeRight = mySonar->currentReadingPolar(45, 180);

	//Calculate the velocity according to the distance in front of the robot.
	//Range > 800: set velocity to the maxSpeed variable
	//Range 400 - 800: set velocity to 30% percent of the range up to the maximum of maxSpeed
	//Range < 400: set velocity to 0
	if (range > 800)
		m_Desired.setVel(maxSpeed);
	else if (range > 400 && range < 800)
	{
		speed = range * .3;

		if (speed >= maxSpeed) speed = maxSpeed;
		m_Desired.setVel(speed);
	}
	else 
		m_Desired.setVel(0);

	//Calculate the velocity according to the distance on the side of the robot.
	//Range left/right < 200: set velocity to 0 to avoid crashing
	//Range left/right < 400: set velocity to 200 to pass objects slowly
	if (rangeLeft < 200 || rangeRight < 200) 
		m_Desired.setVel(0);
	if (rangeLeft < 400 || rangeRight < 400) 
		m_Desired.setVel(200);
	
	//If nothing red was found stand still
	if (*m_Visible == 0)
	{
		m_Desired.setVel(0);
		m_Desired.setRotVel(0);
		return &m_Desired;
	}

	//Check if the treshold is passed
	//and set the rotation speed according to the x coordinate.
	if (*m_X > m_Border || *m_X < m_Border * -1)
	{
		m_Desired.setRotVel(*m_X / 3 * -1);
	}

	//Check if the treshold is passed
	//and set the PTZ tilt according to the y coordinate.
	if (*m_Y > m_Border || *m_Y < m_Border * -1)
	{
		ptz->tilt(*m_Y / 9 * -1);
	}

	//Return the defined ArActionDesired
	return &m_Desired;
}

int main(int argc, char** argv)
{
	//Define the threshold for the turning and tilting trigger according to the camera.
	double border = 10;

	//Initialize Aria class, ArArgumentParser, ArRobot and ArSonarDevice
	Aria::init();
	ArArgumentParser parser(&argc, argv);
	parser.loadDefaultArguments();
	ArRobot robot;
	ArSonarDevice sonar;
  
	//Instantiation of the Actions
	ActionBV dogbotAction(border);
	ArActionStallRecover recover;
    
	//Parsing the arguments 
	//If parsing fails the program exits
	if(!Aria::parseArgs())
	{
		Aria::logOptions();
		Aria::exit(1);
	}
  
	//Connect to the robot via the ArRobotConnector
	//If it fails the program writes an error message and exits
	ArRobotConnector robotConnector(&parser, &robot);
	if (!robotConnector.connectRobot())
	{
		ArLog::log(ArLog::Terse, "actionExample: Could not connect to the robot.");
		if (parser.checkHelpAndWarnUnparsed())
		{
			// -help not given
			Aria::logOptions();
			Aria::exit(1);
		}
	}

	//Define the PTZ connector
	ArPTZConnector ptzConnector(&parser, &robot);
 
	//Define keyHandler and adding them to enable ESC key to exit program
	ArKeyHandler keyHandler;
	Aria::setKeyHandler(&keyHandler);
	robot.attachKeyHandler(&keyHandler);
	printf("You may press escape to exit\n");

	//Add Actions with specific priority to the robot
	robot.addAction(&recover, 100);
	robot.addAction(&dogbotAction, 50);

	//Initialize sonar sensor for the robot
	robot.addRangeDevice(&sonar);
	mySonar = robot.findRangeDevice("sonar");

	//Enabling the motors
	robot.enableMotors();

	//Run process loop to start the robot
	robot.runAsync(true);
	ptzConnector.connect();
	ptz = ptzConnector.getPTZ(0);


	//Wait for break condition
	robot.waitForRunExit();
	Aria::exit(0);
	return 0;
}