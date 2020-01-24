#include "Aria.h"
#include "Shared_Memory.h"

#define MAX_X 155
#define INDEX_X 1
#define INDEX_Y 2
#define INDEX_VISIBLE 3

class ActionBV : public ArAction
{

public:
	ActionBV(double border);

	virtual ~ActionBV(void) {};
	 // fire, this is what the resolver calls to figure out what this action wants
	virtual ArActionDesired *fire(ArActionDesired currentDesired);
	void setSonarDevice(ArRobot robot);

protected:
	
	// what the action wants to do; used by the action resolver after fire()
    ArActionDesired m_Desired;
  
	Shared_Memory *m_SharedMemory;
	
	float m_Border;
	float *m_X;
	float *m_Y;
	float *m_Visible;
};