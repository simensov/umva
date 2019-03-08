/************************************************************/
/*    NAME: Simen Sem Oevereng                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle.h                          */
/*    DATE: Mar 5 2019                                      */
/*    EDIT: -  

      Implementations in CommunicationAngle.cpp.

      This file contains the implementation of lab 6, in 
      which the goal was to calculate transmission angle 
      and loss between two underwater vehicles trying
      to communicate.

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

   // tools
   double soundSpeed(double depth) const;
   double transmissionLoss() const;
   void notifyAcousticPath(string name, double angle, double loss);
   void notifyConnectivityLocations(string name, Point p);
   void adjustTheta(Point pos_c, Point center);
   Point findNewLocation();

 protected:
   void RegisterVariables();

   bool m_initiated;

   // Config var
   double m_surface_sound_speed;
   double m_sound_speed_gradient;
   double m_water_depth;
   double m_time_interval;

   // Global var
   double m_theta0;

   // Local var
   string m_v_name;
   double m_v_nav_x;
   double m_v_nav_y;
   double m_v_nav_depth;
   double m_v_nav_heading;
   double m_v_nav_speed;

   string m_c_name;
   double m_c_nav_x;
   double m_c_nav_y;
   double m_c_nav_depth;
   double m_c_nav_heading;
   double m_c_nav_speed;

   double m_radius;
   double m_z_max;
};

#endif 
