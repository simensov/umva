/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "CommunicationAngle.h"

using namespace std;

//---------------------------------------------------------
// Constructor

CommunicationAngle::CommunicationAngle()
{
  m_first_reading            = true;
  m_surface_sound_speed      = 0;
  m_sound_speed_gradient     = 0;
  m_water_depth              = 0;
  m_time_interval            = 0;
  m_current_x                = 0;
  m_current_ray              = RayEntry();
}

//---------------------------------------------------------
// Destructor

CommunicationAngle::~CommunicationAngle()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool CommunicationAngle::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey();  
    
    if (key == "VEHICLE_NAME"){
      
      m_first_reading = false; 
    }
    if (key == "COLLABORATOR_NAME"){
      // Now we know the name of the collaborator: register for its members
      Register(msg.GetString() + "_NAV_X", 0);
      Register(msg.GetString() + "_NAV_Y", 0);
      Register(msg.GetString() + "_NAV_DEPTH", 0);
      Register(msg.GetString() + "_NAV_HEADING", 0);
      Register(msg.GetString() + "_NAV_SPEED", 0);
    }
    if (key == "NAV_X"){
      
      m_current_x = atol(msg.GetString().c_str()); 
    }
    if (key == "NAV_Y"){
      
      m_first_reading = false; 
    }
    if (key == "NAV_DEPTH"){
      
      m_first_reading = false; 
    }
    if (key == "NAV_HEADING"){
      
      m_first_reading = false; 
    }
    if (key == "NAV_SPEED"){
      
      m_first_reading = false; 
    }

#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CommunicationAngle::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CommunicationAngle::Iterate()
{
  if(false){

  // String containing your answer, and your email for identifation. In case no direct path exists, the published values in ACOUSTIC_PATH must be specified as NaN.
  Notify("ACOUSTIC_PATH","elev_angle=xxx.x, transmission_loss=yyy.y,id=user@mit.edu");


  // RayEntry ray(collabdepth,collabangle);
  // ray.setSoundSpeedParam(m_surface_sound_speed,m_sound_speed_gradient)




  // String containing the location to which to transit at current speed for achieving connectivity. If a path currently exists, the location is obviously the current location. 
  Notify("CONNECTIVITY_LOCATION","x=xxx.xxx,y=yyy.yyy,depth=ddd.d,id=user@mit.edu");
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CommunicationAngle::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
      if(param == "surface_sound_speed") {
        // Sound speed in m/s at the sea surface
      }
      else if(param == "sound_speed_gradient") {
        // Sound speed gradient with depth, in (m/s)/m.

        
      }
      else if(param == "water_depth") {
        // Water depth in meters
      }
      else if(param == "time_interval") {
        // Time interval in seconds between estimates.
        
      }

    }
  }
  
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void CommunicationAngle::RegisterVariables()
{

  Register("VEHICLE_NAME",0); // Name of the ownship platform
  Register("COLLABORATOR_NAME",0); // Name of collaborator, referred to as 'collaborator' in the following.
  Register("NAV_X",0); // ownship East location in in local metric coordinates.
  Register("NAV_Y",0); //ownship North location in in local metric coordinates.
  Register("NAV_DEPTH",0); // ownship depth in meters.
  Register("NAV_HEADING",0); // ownship heading in degrees.
  Register("NAV_SPEED",0); // ownship speed in meters/second.

}

