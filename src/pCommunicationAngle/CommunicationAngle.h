/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: CommunicationAngle.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef CommunicationAngle_HEADER
#define CommunicationAngle_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <stdlib.h>
#include <string>
#include <vector>

#include "RayEntry.h"

using namespace std;

class CommunicationAngle : public CMOOSApp
{
 public:
   CommunicationAngle();
   ~CommunicationAngle();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

   // some logical test on RayEntry and depth
   // some recalculating of needed horizontal position

 protected:
   void RegisterVariables();

   bool m_first_reading;

   // Config var
   double m_surface_sound_speed;
   double m_sound_speed_gradient;
   double m_water_depth;
   double m_time_interval;

   // Other var
   double m_current_x;

   string m_vehicle_name;
   string m_collaborator_name;

   RayEntry m_current_ray;   
   
};

#endif 
