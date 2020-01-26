#include "ActionBV.h"

#include <iostream>

ArRangeDevice *mySonar;

double range;
double rangeLeft;
double rangeRight;

double speed;
double maxSpeed = 500;

ArPTZ *ptz;

//Konstruktor der die Action initialisiert.
//@border: Schwellwert für das Drehen und Tilten anhand der Kamerabilder
//
//Initialisiert eine SharedMemory Instanz und beendet das Programm, falls das fehlschlägt.
//Abschließend werden X, Y und Visible aus dem SharedMemory gelesen und schreibt es in Variablen.
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

//Funktion, die die spezifische Action definiert, welche vom resolver aufgerufen wird und eine ArActionDesired zurückgibt.
ArActionDesired *ActionBV::fire(ArActionDesired currentDesired)
{
	//Zurücksetzen der ArActionDesired Variable
	m_Desired.reset();

	//Überprüfen ob das Sonar angelegt wurde und eine leere ArActionDesired zurückgeben falls nicht
	if (mySonar == NULL)
	{
		deactivate();
		return NULL;
	}

	//Abstandswerte aus dem Sonarsensor lesen und diese Werte in Variablen schreiben.
	range = mySonar->currentReadingPolar(-45, 45) - myRobot->getRobotRadius();
	rangeLeft = mySonar->currentReadingPolar(-180, -45);
	rangeRight = mySonar->currentReadingPolar(45, 180);

	//Geschwindigkeit anhand des Abstands zum nächsten Objekt vor dem Roboter berechnen.
	//Range > 800: setze die Geschwindigkeit auf maxSpeed
	//Range 400 - 800: setze die Geschwindigkeit auf 30% der range Variable mit maxSpeed als Maximum
	//Range < 400: setze die Geschwindigkeit auf 0
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

	//Geschwindigkeit anhand der Entfernungen links und rechts neben dem Roboter berechnen.
	//Range left/right < 200: setze die Geschwindigkeit auf 0, um eine Kollision zu verhindern
	//Range left/right < 400: setze die Geschwindigkeit auf 200, um langsam an nahen Objekten vorbeizufahren
	if (rangeLeft < 200 || rangeRight < 200) 
		m_Desired.setVel(0);
	if (rangeLeft < 400 || rangeRight < 400) 
		m_Desired.setVel(200);
	
	//Stehe still, wenn es keinen Rotschwerpunkt gibt
	if (*m_Visible == 0)
	{
		m_Desired.setVel(0);
		m_Desired.setRotVel(0);
		return &m_Desired;
	}

	//Überprüfe ob der Schwellwert erreicht wurde
	//und setze die Rotationsgeschwindigkeit gemäß der X Koordinate.
	if (*m_X > m_Border || *m_X < m_Border * -1)
	{
		m_Desired.setRotVel(*m_X / 3 * -1);
	}

	//Überprüfe ob der Schwellwert erreicht wurde
	//und setze den Tilt des PTZ gemäß der Y Koordinate.
	if (*m_Y > m_Border || *m_Y < m_Border * -1)
	{
		ptz->tilt(*m_Y / 9 * -1);
	}

	//Gb die ArActionDesired zurück.
	return &m_Desired;
}

int main(int argc, char** argv)
{
	//Der Schwellwert für das Drehen und Tilten gemäß des Rotschwerpunktes im Bild
	double border = 10;

	//Initialisieren der Aria Klasse, des ArArgumentParsers, des ArRobots und des ArSonarDevice
	Aria::init();
	ArArgumentParser parser(&argc, argv);
	parser.loadDefaultArguments();
	ArRobot robot;
	ArSonarDevice sonar;
  
	//Instanziierung der Actions
	ActionBV dogbotAction(border);
	ArActionStallRecover recover;
    
	//Parsen der Argumente
	//Falls das parsen fehlschlägt wird das Programm beendet
	if(!Aria::parseArgs())
	{
		Aria::logOptions();
		Aria::exit(1);
	}
  
	//Verbindung mit dem Roboter aufbauen über den ArRobotConnector
	//Falls das fehlschlägt wird eine Fehlermeldung geschrieben und das Programm beendet.
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

	//Definition des ArPTZConnectors
	ArPTZConnector ptzConnector(&parser, &robot);
 
	//Definition & Instanziierung des keyHandlers
	ArKeyHandler keyHandler;
	Aria::setKeyHandler(&keyHandler);
	robot.attachKeyHandler(&keyHandler);
	printf("You may press escape to exit\n");

	//Hinzufügen von Actions zum Roboter mit bestimmten Priotitäten
	robot.addAction(&recover, 100);
	robot.addAction(&dogbotAction, 50);

	//Initialisierung der Sonarsensoren
	robot.addRangeDevice(&sonar);
	mySonar = robot.findRangeDevice("sonar");

	//Einschalten der Motoren
	robot.enableMotors();

	//Starten der Prozessschleife, um den Roboter zu starten
	robot.runAsync(true);
	ptzConnector.connect();
	ptz = ptzConnector.getPTZ(0);


	//Warte auf die Abbruchbedingung
	robot.waitForRunExit();
	Aria::exit(0);
	return 0;
}