/************************************************************/
/*    NAME: Simen Sem Oevereng                              */
/*    ORGN: MIT: 2.680 Spring 2019, http://oceanai.mit.edu  */
/*    FILE: Odometry.cpp                                    */
/*    DATE: Feb 14, 2019                                    */
/*    EDIT: none  

      This file contains the implementation of lab 4, in 
      which the goal was to learn how to write a MOOSApp, 
      in order to interact with a MOOS mission, alder.moos.
      the app simply calculates the total travelled distance
      of a single vessel.
      
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "Odometry.h" // declaration
#include <math.h>     // for sqrt() procedure

using namespace std;

//---------------------------------------------------------
// Constructor

Odometry::Odometry()
{
  // true until first mail is read
  m_first_reading       = true;

  // positions and distances
  m_current_x           = 0;
  m_current_y           = 0;
  m_previous_x          = 0;
  m_previous_y          = 0;
  m_total_distance      = 0;

  // name of published variable. named in the antler configuration file
  m_total_distance_name = "ODOMETRY_DIST"; 
  
}

//---------------------------------------------------------
// Destructor

Odometry::~Odometry()
{
  // No dynamically allocated variables to delete yet
}

//---------------------------------------------------------
// Procedure: OnNewMail
// PURPOSE:   react to subscribed mails sent from (preferably) another MOOSApp 
// @param     NewMail: a MOOSMSG_LIST type variable,
//				containing all information published from other MOOSApps
// @edits     m_current_x, m_current_y, m_first_reading
// @return    true
bool Odometry::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  // Iterate through all mail
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey();        // Gets variable name

    // Checks explicitly for variablename we are interested in
    if (key == "NAV_X"){
      m_current_x = msg.GetDouble();
      m_first_reading = false;	// changes to false as soon we read a mail
    }

    if (key == "NAV_Y"){  
      m_current_y = msg.GetDouble();
      m_first_reading = false;

    }

  }

   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer
// PURPOSE:   ---
// @param     no inputs
// @edits     no edits
// @returns   true

bool Odometry::OnConnectToServer()
{	
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second
// PURPOSE:   contains the main logic of the MOOSApp
// @param     no inputs
// @edits     m_total_distance
// @returns

bool Odometry::Iterate()
{
  // Calling Iterate() from parent class to enable casting
  AppCastingMOOSApp::Iterate();

  // returns in situations where it makes no sense to perform calculations
  if (m_first_reading){
    // startup-case
    return(true);
  }
  else if (m_total_distance < 0){
    // tells user that the distance is negative so that it could be checked
    cout << "Odometry::Iterate: \"Distance is negative.\" " << endl;
    return(true);
  }

  // horizontal and vertical differences
  double xdist = m_current_x - m_previous_x;
  double ydist = m_current_y - m_previous_y;

  // calculating euclidean distance between previous and current position, and publishes the value to the community
  m_total_distance += sqrt(xdist*xdist + ydist*ydist); // From math.h
  Notify(m_total_distance_name,m_total_distance);

  // updates previous position parameters here to avoid errors between apps with different AppsTick and CommsTick
  m_previous_x = m_current_x;
  m_previous_y = m_current_y;

  AppCastingMOOSApp::PostReport(); // posts to AppCast: variable(s) specified in Odometry::buildReport()
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            virtual function
//            happens before connection is open
// PURPOSE:   grabbing configuration parameters relevant for the given app
// @param     no inputs
// @edits     no edits
// @returns   true

bool Odometry::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  // Collects list of parameters from .moos file
  // Since we register hardcode variable names, this loop is not necessary, but is kept for later reference
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = biteStringX(line, '=');
      string value = line;
    }

  }
  
  // Calls function in the end for registering variables to subscribe to relevant mail
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure:   RegisterVariables
// PURPOSE:     selects variables to register 
// @param       no inputs
// @edits       no edits
// @returns     nothing
void Odometry::RegisterVariables()
{
  // Calling function defined in parent class AppCastingMOOSApp
  AppCastingMOOSApp::RegisterVariables();

  // Explicitly register for the MOOS-variables we want
  Register("NAV_X", 0); // second parameter == 0 as we would like every update from NAV_X
  Register("NAV_y", 0);

}

//---------------------------------------------------------
// Procedure: buildReport
// PURPOSE:   prints selected values to AppCastingMOOSApp
// @param     no inputs
// @edits     member variable m_msgs
// @returns   true
bool Odometry::buildReport()
{
  // Sending string and variable value to m_msgs: the output stream of the casting application
  m_msgs << "Total distance travelled: " << m_total_distance << endl;

  return(true);
}