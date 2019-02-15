/************************************************************/
/*    NAME: Simen Sem Oevereng                              */
/*    ORGN: MIT: 2.680 Spring 2019, http://oceanai.mit.edu  */
/*    FILE: Odometry.h                                      */
/*    DATE: Feb 14, 2019                                    */
/*    EDIT: none  

      Implementations: Odometry.cpp

      This file contains the declarations of lab 4, in 
      which the goal was to learn how to write a MOOSApp, 
      in order to interact with a MOOS mission, alder.moos.
      the app simply calculates the total travelled distance
      of a single vessel.      
      
/************************************************************/

#ifndef Odometry_HEADER
#define Odometry_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

// inherits from AppCastingMOOSApp class to enable AppCasting
class Odometry : public AppCastingMOOSApp
{
 public:
   Odometry();
   ~Odometry();

 protected: // Standard MOOSApp functions being overloaded 
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   bool buildReport();

 protected: // App-specific functions / parameters
   void RegisterVariables();           // Registers variables published to the community
   bool m_first_reading;               // Variable for differentiating between first run and the following iterations

   // Variables relating to the position and distance travelled by the vessel
   double m_current_x;
   double m_current_y;
   double m_previous_x;
   double m_previous_y;
   double m_total_distance;

   std::string m_total_distance_name;  // Holds the key of the MOOS variable, defined to hold total distance (in the configuration file)
};

#endif 