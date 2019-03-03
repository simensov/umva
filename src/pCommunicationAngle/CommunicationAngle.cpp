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

  m_v_name  = ",";
  m_v_nav_x = 0;
  m_v_nav_y = 0;
  m_v_nav_depth = 0;
  m_v_nav_heading = 0;
  m_v_nav_speed = 0;

  m_c_name = "";
  m_c_nav_x = 0;
  m_c_nav_y = 0;
  m_c_nav_depth = 0;
  m_c_nav_heading = 0;
  m_c_nav_speed = 0;

  m_radius  = 0;
  m_z_max   = 0;
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
      m_v_name = msg.GetString();
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
      m_v_nav_x = atof(msg.GetString().c_str()); 
      m_first_reading = false; 
    }
    if (key == "NAV_Y"){
      m_v_nav_y = atof(msg.GetString().c_str());

      m_first_reading = false; 
    }
    if (key == "NAV_DEPTH"){
      m_v_nav_depth = atof(msg.GetString().c_str());
      m_first_reading = false; 
    }
    if (key == "NAV_HEADING"){
      m_v_nav_heading = atof(msg.GetString().c_str());
      m_first_reading = false; 
    }
    if (key == "NAV_SPEED"){
      m_v_nav_speed = atof(msg.GetString().c_str());
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
    // Make two points coinciding with positions of vessels
    Point pos_c(m_c_nav_x,m_c_nav_y);
    Point pos_v(m_v_nav_x,m_v_nav_y);

    // Create the line that goes through them
    Line chordline(pos_c,pos_v);

    // Find corresponding circle radius according to parameters
    m_radius = chordline.getCircleRadius(-m_surface_sound_speed/m_sound_speed_gradient);

    // Use radius and param to find theta0 
    m_theta0 = acos( m_surface_sound_speed/(m_sound_speed_gradient*m_radius) );

    // Find deepest possible point vs. max depth
    m_z_max = m_radius - m_surface_sound_speed / m_sound_speed_gradient;

    // React
    if(m_z_max >= m_water_depth){
      // No direct curve is possible
      // Suggest movement: find a new angle that makes this zmax smaller 
      // Find corresponding point of our vessel (use parameterization and arc length / chord relations if necessary)

      // Notify accordingly

    }
    else{
      // Possible path exists. Calculate transmission loss and notify

    }


    // String containing your answer, and your email for identifation. In case no direct path exists, the published values in ACOUSTIC_PATH must be specified as NaN.
    Notify("ACOUSTIC_PATH","elev_angle=xxx.x,transmission_loss=yyy.y,id=user@mit.edu");
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

