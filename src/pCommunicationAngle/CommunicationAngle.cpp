/************************************************************/
/*    NAME: Simen Sem Oevereng                              
      ORGN: MIT                                             
      FILE: CommunicationAngle.cpp                          
      DATE: Mar 5 2019                                      
      EDIT: -  

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
// @edits     
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


//---------------------------------------------------------
// Procedure: transmissionLoss
// PURPOSE:   calculates the transmission loss of the sound signal along a 
//            certain arc and depth
// @param     no inputs 
// @edits     no edits
// @return    a double representing the transmission loss

double CommunicationAngle::transmissionLoss() const
{
  if(true){
    // Calculates the soundspeed at the transmitter and receiver positions

    // Find relevant sound speeds to find angle at receiver and the arclength
    double c_collaborator = soundSpeed(m_c_nav_depth);

    double c_vehicle = soundSpeed(m_v_nav_depth);

    double theta_c = acos( cos(m_theta0) * c_vehicle/c_collaborator);

    double arclength = m_radius * (m_theta0 + theta_c);

    // Setting sound spreading in water
    double d_theta = 0.001;

    // Calculates the length to two r-positions, defining the dissipation of the sound into a cone-looking shape
    double r1 = m_radius*(sin(m_theta0) + sin(arclength/m_radius - m_theta0));
    double r2 = m_radius*(sin(m_theta0 + d_theta) + sin(arclength/m_radius - m_theta0 - d_theta));

    // Area of pressure from the sound at the area at the receivers end
    double J = (r1/sin(theta_c))*((r2 - r1)/d_theta);

    // Calculates the pressure loss
    double ps = (0.25*M_PI)*sqrt(abs((cos(m_theta0)*c_vehicle)/(c_collaborator*J)));
    double p1 = (0.25*M_PI);

    // Returns the transmission loss
    double loss = -20*log10(ps/p1);

    return(loss);

  } else{
    return(0.0);
  }

 }

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second
bool CommunicationAngle::Iterate()
{

  // only complete task if  variables have been received from OnNewMail
  if(m_initiated){

    // make two points coinciding with positions of vessels,
    // create the chordline that goes through them,
    // find corresponding circle radius accordingly,
    // get Point with the the circle center,
    //  - considering 2D in r,z-plane transformed from global x,y,z

    double dx = m_c_nav_x - m_v_nav_x;
    double dy = m_c_nav_y - m_v_nav_y;
    double dr = sqrt(dx*dx + dy*dy);

    Point pos_v(0,m_v_nav_depth);
    Point pos_c(dr,m_c_nav_depth);

    Line chordline(pos_v,pos_c);

    double virtualHeight = m_surface_sound_speed/m_sound_speed_gradient;
    
    m_radius = chordline.circleRadius(-virtualHeight);
    Point center = chordline.circleCenter(-virtualHeight);

    // adjust transmission angle
    // find depth of deepest point on circle
    // calculate transmission loss along the arc

    adjustTheta(pos_v, center);
    m_z_max = m_radius - m_surface_sound_speed / m_sound_speed_gradient;
    
    double t_loss = transmissionLoss();

    if(m_z_max >= m_water_depth){
      // React according to depth and feasibility:
      // - If we are on correct side of bottom, we could still have a path
      // Otherwise: adjust. setting loss to -1 tells about no path and
      // calculate a new point where transmission is possible

      // Using pythagoras to see if we could be closer to source than the point where the circle hits the seabed. Remember that our horizontal position will always be less than collaborator's in r,z-plane
      bool ok = false;

      if(pos_c.getZ() > pos_v.getZ()){
        // We are above collaborator and have to check if it lies on the circular arc BEFORE it hits the seabed. If it is, then there exists a path.
        double x_pos_circle_hits_bottom = center.getX() \
         - sqrt(pow(m_radius,2) - pow(virtualHeight+m_water_depth,2) );

        ok = (pos_c.getX() < x_pos_circle_hits_bottom );
      }
      else{
        // Else, we are below the collaborator and should be doing fine since we could just send a signal upwards the circle path, even though the circle itself might hit the bottom somewhere else
        ok = true;
      }

      if(ok){
        // We figured that we was in an acceptable place on the circle anyways
        notifyAcousticPath("ACOUSTIC_PATH", m_theta0, t_loss);
        notifyConnectivityLocations("CONNECTIVITY_LOCATION",pos_v);
      }
      else{
        // Need to update position 
        // Send -1 to notifyAcoutsitcPath to tell of 'nan'
        notifyAcousticPath("ACOUSTIC_PATH", m_theta0, -1);

        // Only adjusts the vehicle's depth
        findNewLocation();
      }
    }
    else{
      // Path exists. publish transmission angle and loss, and location
      notifyAcousticPath("ACOUSTIC_PATH", m_theta0, t_loss);
      notifyConnectivityLocations("CONNECTIVITY_LOCATION",pos_v);
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
    // Given format: string containing your answer, and your email for identifation. In case no direct path exists, the published values in ACOUSTIC_PATH must be specified as NaN.

  if(loss < 0){
    double f1 = nan("-1");
    Notify(name,"elev_angle=" + to_string(f1) + ",transmission_loss=" + to_string(f1) + ",id=simensov@mit.edu");
  } 
  else{
    // Since my calculations started with defining theta positive clockwise after talking to Henrik in the first lecture, I have to return the negative angle here since the testing is done on defining theta positive the other way.
    Notify(name,"elev_angle=" + to_string(-angle*180/M_PI) + ",transmission_loss=" + to_string(loss) + ",id=simensov@mit.edu");
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
  // Given format: string containing the location to which to transit at current speed for achieving connectivity. If a path currently exists, the location is obviously the current location. 

  Notify(name,"x=" + to_string(p.getX())+",y=" + to_string(m_v_nav_y) + ",depth=" + to_string(p.getZ()) + ",id=simensov@mit.edu");

}

//---------------------------------------------------------
// Procedure: adjustTheta
// PURPOSE:   corrects theta - transmission angle and keeps it inside 0 - 2PI.
// @param     pos_c: a Point object representing sound source position, center:
//            a Point object with the circle center
// @edits     m_theta0
// @return    nothing
void CommunicationAngle::adjustTheta(Point pos, Point center)
{
  // Angle of transmission is perpendicular to line from sound source to center of circle. find angle between us and center, and add 90 degrees

  Line sourceToCenter(pos,center);
  m_theta0 = sourceToCenter.getAngle() + M_PI/2;

  // avoid m_theta0 to build above 360 degrees.
  if(m_theta0 > 2*M_PI) {
    m_theta0 -= 2*M_PI;
  }
  else if (m_theta0 < -2*M_PI){
    m_theta0 += 2*M_PI;
  } 
}

Point CommunicationAngle::findNewLocation()
{
  // using a very naive strategy: look for a new position assuming that the vehicle can move straight up towards the surface
  // if no position is found, start closing in on the horizontal.

  // ONLY changing z here as that was OK with Henrik. 
  double cur_dx = m_c_nav_x - m_v_nav_x;
  double cur_dy = m_c_nav_y - m_v_nav_y;
  double current_r = sqrt(cur_dx*cur_dx + cur_dy*cur_dy);

  double cur_z = m_v_nav_depth;
  double cur_zmax = m_z_max;
  double cur_rad = m_radius;
  double virtualHeight = m_surface_sound_speed/m_sound_speed_gradient;

  Point pos_new(0,cur_z);
  Point pos_c(current_r,m_c_nav_depth);

  while(cur_zmax >= m_water_depth){

    // start looking higher in the water
    if(cur_z > 10){
      cur_z = cur_z * 0.99;
    }
    else{
      double f1 = nan("-1");
      pos_new = Point(f1,f1);
      break;
    }

    // create new point, line and update radius and this zmax
    pos_new = Point(0,cur_z);
    Line chordline(pos_new,pos_c);
    cur_rad = chordline.circleRadius(-virtualHeight);
    cur_zmax = cur_rad - virtualHeight;
  }

  // after the while loop has terminated, we either find new pos or nan
  notifyConnectivityLocations("CONNECTIVITY_LOCATION",pos_new);
}

