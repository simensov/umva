/************************************************************/
/*    NAME: Simen Oevereng                                  */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.h                                       */
/*    DATE: Apr 2. 2019                                     */
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

 protected: // Own implementations
   void registerVariables();

   vector<Point> greedyPath() const;
   void publishGreedyPath(vector<Point> ret_pts);

protected: // Variables
   vector<Point> m_all_points;
   vector<Point> m_remaining_points;
   vector<Point> m_visited_points;

   bool m_searching;
   bool m_last_point_received;
   bool m_path_calculated; 
   bool m_all_visited;
   
   bool m_x_rec;
   bool m_y_rec;
   double m_init_x;
   double m_init_y;

   double m_nav_x;
   double m_nav_y;
   double m_visit_radius;
};

#endif 
