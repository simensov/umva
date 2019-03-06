/************************************************************/
/*    NAME: Simen Sem Oevereng                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle.cpp                          */
/*    DATE: Mar 5 2019                                      */
/*    EDIT: -  

      Declarations in CommunicationAngle.h.

      This file contains the implementation of lab 6, in 
      which the goal was to calculate transmission angle 
      and loss between two underwater vehicles trying
      to communicate.

*/
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "CommunicationAngle.h"

#include <iomanip> // for cout << std::setprecision(2) << std::fixed

using namespace std;

const bool debug = true;

//---------------------------------------------------------
// Constructor

CommunicationAngle::CommunicationAngle()
{
  m_first_reading            = true;
  m_initiated                = false;
  m_surface_sound_speed      = 0;
  m_sound_speed_gradient     = 0.001; // avoids dividing by zero
  m_water_depth              = 0;
  m_time_interval            = 0;

  m_v_name  = ",";
  m_v_nav_x = -1;
  m_v_nav_y = -1;
  m_v_nav_depth = -1;
  m_v_nav_heading = -1;
  m_v_nav_speed = -1;

  m_c_name = ",";
  m_c_nav_x = -1;
  m_c_nav_y = -1;
  m_c_nav_depth = -1;
  m_c_nav_heading = -1;
  m_c_nav_speed = -1;

  m_radius  = 0;
  m_z_max   = 0;

  m_path_name = "ACOUSTIC_PATH";
  m_conn_loc_name = "CONNECTIVITY_LOCATION";

}

//---------------------------------------------------------
// Destructor
CommunicationAngle::~CommunicationAngle()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail
// PURPOSE:   react to subscribed mails sent from (preferably) another MOOSApp 
// @param     NewMail: a MOOSMSG_LIST type variable,
//            containing all information published from other MOOSApps
// @edits     m_current_prime, m_received_primes, m_all_entries,
//            m_first_reading
// @return    true
bool CommunicationAngle::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey(); 
    
    if (key == "VEHICLE_NAME"){
      m_v_name = msg.GetString();
    }
    if (key == "NAV_X"){
      m_v_nav_x = msg.GetDouble();
    }
    if (key == "NAV_Y"){
      m_v_nav_y = msg.GetDouble(); 
    }
    if (key == "NAV_DEPTH"){
      m_v_nav_depth = msg.GetDouble(); 
    }
    if (key == "NAV_HEADING"){
      m_v_nav_heading = msg.GetDouble();
    }
    if (key == "NAV_SPEED"){
      m_v_nav_speed = msg.GetDouble();
    }
    if (key == "COLLABORATOR_NAME"){
      // Now we know the name of the collaborator: register for its members
      m_c_name = msg.GetString();
      Register(m_c_name + "_NAV_X", 0);
      Register(m_c_name + "_NAV_Y", 0);
      Register(m_c_name + "_NAV_DEPTH", 0);
      Register(m_c_name + "_NAV_HEADING", 0);
      Register(m_c_name + "_NAV_SPEED", 0);
    }
    if(m_c_name != "," ){
      // find value as for vehicle only if we know name of collaborator
      if (key == (m_c_name + "_NAV_X")) {
        m_c_nav_x = msg.GetDouble();
      }
      if (key == (m_c_name + "_NAV_Y")){
        m_c_nav_y = msg.GetDouble(); 
      }
      if (key == (m_c_name + "_NAV_DEPTH")){
        m_c_nav_depth = msg.GetDouble();
      }
      if (key == (m_c_name + "_NAV_HEADING")){
        m_c_nav_heading = msg.GetDouble(); 
      }
      if (key == (m_c_name + "_NAV_SPEED")){
        m_c_nav_speed = msg.GetDouble();
      }
    }

    bool v_init = (m_v_nav_x != -1 && m_v_nav_y != -1 && m_v_nav_depth != -1 && m_v_nav_heading != -1 && m_v_nav_speed != -1);
    bool c_init = (m_c_nav_x != -1 && m_c_nav_y != -1 && m_c_nav_depth != -1 && m_c_nav_heading != -1 && m_c_nav_speed != -1);

    if(v_init && c_init && !m_initiated){
      cout << "VEHICLE VARIABLES INITIATED" << endl;
      m_initiated = true;
    }


   } // for all mail
  
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
// Procedure: soundSpeed
// PURPOSE:   calculates sound speed at current depth with linear profile
// @param     depth: the current submerged depth
// @edits     no edits
// @return    a double with the sound speed
double CommunicationAngle::soundSpeed(double depth) const {
  return(m_surface_sound_speed + m_sound_speed_gradient * depth);
}

// TO DO: make and document
//---------------------------------------------------------
// Procedure: transmissionLoss
// PURPOSE:   calculates the transmissionloss along a certain arc and depth
// @param     no inputs 
// @edits     no edits
// @return    a double representing the transmission loss
double CommunicationAngle::transmissionLoss() const {
  return(0.0);
}


//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second
bool CommunicationAngle::Iterate()
{

  // only complete task if  variables have been received from OnNewMail
  if(m_initiated){

    // Make two points coinciding with positions of vessels
    Point pos_c(m_c_nav_x,m_c_nav_depth);
    Point pos_v(m_v_nav_x,m_v_nav_depth);

    // create the chordline that goes through them
    Line chordline(pos_c,pos_v);

    // find corresponding circle radius according to parameters
    m_radius = chordline.getCircleRadius(-m_surface_sound_speed/m_sound_speed_gradient);

    // Get x and z position of the circle center
    Point center = chordline.circleCenter(-m_surface_sound_speed/m_sound_speed_gradient);

    if(debug){
      cout << "C pos: " << pos_c.printPoint() << " - V pos: " << pos_v.printPoint() << " - Center pos: " << center.printPoint() << " - R: " << m_radius << endl;
    }

    // angle of transmission is perpendicular to line from sound source to center of circle. depends
    adjustTheta(pos_c, center);

    // find deepest possible point vs. max depth
    m_z_max = m_radius - m_surface_sound_speed / m_sound_speed_gradient;

    // calculate transmission loss along the arc
    double t_loss = transmissionLoss();


    // react according to depth and feasibility
    if(debug){

      if(m_z_max >= m_water_depth){
        cout << "Checking depth loop" << endl;
        // if we are closer than the circle bottom, and the depth is smaller than the maximum depth, then we are good
        if(pos_v.getX() < center.getX() && pos_v.getZ() < m_z_max){
          notifyAcousticPath(m_path_name, m_theta0, t_loss);
          notifyConnectivityLocations(m_conn_loc_name,pos_v);
        }
        else{
          // we have to adjust. setting loss to -1 tells about no path
          notifyAcousticPath(m_path_name, m_theta0, -1);

          // TODO: find a feasible location!
          // suggest movement: find a new angle that makes this zmax smaller 
          // Find corresponding point of our vessel (use parameterization and arc length / chord relations if necessary)
          notifyConnectivityLocations(m_conn_loc_name,pos_v);
        }

      }
      else{

        // path exists. publish transmission angle and loss, and location
        notifyAcousticPath(m_path_name, m_theta0, t_loss);
        notifyConnectivityLocations(m_conn_loc_name,pos_v);

      }
    }
  
  return(true);
  }

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
      string valueString = line;
      double value = atof(valueString.c_str());

      
      if(param == "surface_sound_speed") {
        // Sound speed in m/s at the sea surface
        m_surface_sound_speed = value;
      }
      else if(param == "sound_speed_gradient") {
        // Sound speed gradient with depth, in (m/s)/m.  
        m_sound_speed_gradient = value;      
      }
      else if(param == "water_depth") {
        // Water depth in meters
        m_water_depth = value;
      }
      else if(param == "time_interval") {
        // Time interval in seconds between estimates.
        m_time_interval = value;
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
  Register("COLLABORATOR_NAME",0); // Name of collaborator
  Register("NAV_X",0); // ownship East location in in local metric coordinates.
  Register("NAV_Y",0); //ownship North location in in local metric coordinates.
  Register("NAV_DEPTH",0); // ownship depth in meters.
  Register("NAV_HEADING",0); // ownship heading in degrees.
  Register("NAV_SPEED",0); // ownship speed in meters/second.

}

//---------------------------------------------------------
// Procedure: notifyAcousticPath
// PURPOSE:   publishes acoustic path in given format to MOOSDB community
// @param     name: name of the MOOSDB variable representing path, angle:
//            angle of transmission, loss: transmission loss
// @edits     no edits
// @return    nothing
void CommunicationAngle::notifyAcousticPath(string name, double angle, double loss)
{
    // given format: string containing your answer, and your email for identifation. In case no direct path exists, the published values in ACOUSTIC_PATH must be specified as NaN.

  // taking loss as negative in Iterate() if there is no path
  if(loss < 0 && false){
    double f1 = nan("-1");
    Notify(name,"elev_angle=" + to_string(f1) + ",transmission_loss=" + to_string(f1) + ",id=simensov@mit.edu");
  } 
  else{
    Notify(name,"elev_angle=" + to_string(angle) + ",transmission_loss=" + to_string(loss) + ",id=simensov@mit.edu");
  }

}

//---------------------------------------------------------
// Procedure: notifyConnectivityLocations
// PURPOSE:   publishes location for possible transmission in given format to
//            MOOSDB community
// @param     name: name of the MOOSDB variable representing location, p:
//            a Point object with the relevant location
// @edits     no edits
// @return    nothing
void CommunicationAngle::notifyConnectivityLocations(string name, Point p)
{
  // given format: string containing the location to which to transit at current speed for achieving connectivity. If a path currently exists, the location is obviously the current location. 

  Notify(name,"x=" + to_string(p.getX())+",y=" + to_string(m_v_nav_y) + ",depth=" + to_string(p.getZ()) + ",id=simensov@mit.edu");

}

//---------------------------------------------------------
// Procedure: adjustTheta
// PURPOSE:   corrects theta - transmission angle and keeps it inside 0 - 2PI.
// @param     pos_c: a Point object representing sound source position, center:
//            a Point object with the circle center
// @edits     m_theta0
// @return    nothing
void CommunicationAngle::adjustTheta(Point pos_c, Point center)
{

  Line sourceToCenter(pos_c,center);

  if(pos_c.getX() > m_v_nav_x){
    // collaborator is further out than us: have to subtract 90 degrees
    m_theta0 = sourceToCenter.getAngle() - M_PI/2;
  }
  else{
    // we are in front of the collaborator: add 90 degrees
    m_theta0 = sourceToCenter.getAngle() + M_PI/2;
  }

  // avoid m_theta0 to build
  if(m_theta0 > 2*M_PI) {
    m_theta0 -= 2*M_PI;
  }
  else if (m_theta0 < -2*M_PI){
    m_theta0 += 2*M_PI;
  }

}
