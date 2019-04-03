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
  // config 
  vector<string> m_all_vehicles;

  // work
  vector<Point> m_all_points;

  bool m_last_point_received;
  bool m_assign_by_region;
  bool m_points_are_published;
};

#endif 
