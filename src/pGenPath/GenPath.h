/************************************************************/
/*    NAME: Simen Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef GenPath_HEADER
#define GenPath_HEADER

#include "XYSegList.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <vector>
#include "Geometry.h"

using namespace std;


class GenPath : public AppCastingMOOSApp
{
 public:
   GenPath();
   ~GenPath();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

   vector<Point> greedyPath() const;
   void publishGreedyPath(vector<Point> ret_pts);

   vector<Point> m_all_points;

   bool m_last_point_received;
   int m_points_visited;

   bool m_path_calculated; 
   double m_init_x;
   double m_init_y;

   bool m_x_rec;
   bool m_y_rec;
   bool m_nav_rec;

 private: // Configuration variables

 private: // State variables
};

#endif 
