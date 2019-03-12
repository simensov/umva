/************************************************************/
/*    NAME: Simen Sem Oevereng                              
      ORGN: MIT                                             
      FILE: CommunicationAngle.h                          
      DATE: Mar 5 2019                                      
      EDIT: -  

      Implementations in CommunicationAngle.cpp.

      This file contains the declaration of functions for
      lab 6, in which the goal was to calculate a transmission
      angle and -loss from the transmitter on "our" vehicle,
      in order to send a signal to communicate with a 
      collaborating underwater vehicle.

*/
/************************************************************/
#ifndef CommunicationAngle_HEADER
#define CommunicationAngle_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <stdlib.h>
#include <string>
#include <vector>

#include "Geometry.h" // Point and Line objects

using namespace std;

class CommunicationAngle : public CMOOSApp
{
 public:
   CommunicationAngle();
   ~CommunicationAngle();

 protected:
   // standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

   // utilities
   double soundSpeed(double depth) const;
   double transmissionLoss() const;
   void notifyAcousticPath(string name, double angle, double loss);
   void notifyConnectivityLocations(string name, Point p);
   void adjustTheta(Point pos_c, Point center);
   Point findNewLocation();

 protected:
   void RegisterVariables();

   bool m_initiated;  // true if all variables has been received in OnNewMail

   // configuration variables
   double m_surface_sound_speed;
   double m_sound_speed_gradient;
   double m_water_depth;
   double m_time_interval;

   // variables containing information about our behicle
   double m_theta0; // angle of transmitter (radians)
   string m_v_name;
   double m_v_nav_x;
   double m_v_nav_y;
   double m_v_nav_depth;
   double m_v_nav_heading;
   double m_v_nav_speed;

   // variables containing information about collaborator
   string m_c_name;
   double m_c_nav_x;
   double m_c_nav_y;
   double m_c_nav_depth;
   double m_c_nav_heading;
   double m_c_nav_speed;

   double m_radius; // radius of the circular arc between us and collaborator
   double m_z_max;  // maximum depth of the same circular arc

};

#endif 
