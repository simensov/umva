/************************************************************/
/*    NAME: Simen Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.h                                          */
/*    DATE: Apr 2. 2019                                     */
/************************************************************/

#ifndef PointAssign_HEADER
#define PointAssign_HEADER

#include "XYPoint.h"
#include "MOOS/libMOOS/MOOSLib.h"
#include "Geometry.h"
#include <vector>

using namespace std;

class PointAssign : public CMOOSApp
{
 public:
   PointAssign();
   ~PointAssign();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();

   vector<Point> getAllPoints() const {return m_all_points;};

   void postViewPoint(double x, double y, string label, string color);

 protected:

  vector<string> m_all_vehicles;  // name of incoming vehicles
  vector<Point> m_all_points;     // all waypoints 

  bool m_last_point_received;     // checks if received point is lastpoint
  bool m_assign_by_region;        // config of how to distribute points
};

#endif 
