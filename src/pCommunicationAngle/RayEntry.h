/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: RayEntry.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef RayEntry_HEADER
#define RayEntry_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <stdlib.h>
#include "math.h"

class RayEntry
{
 public:
   RayEntry();
   RayEntry(double &depth, double &angle);
   ~RayEntry(){};

   // mutator
   void setSoundSpeedParm(double &surfaceSpeed, double &gradient){m_c0 = surfaceSpeed; m_gradient = gradient;};

   void setInitialRay(double &depth, double &angle){m_z0 = depth;  m_theta0 = angle;};

   void setCircle();

   // accessor
   double getSoundSpeed(double &depth) const {return(m_c0+depth*m_gradient);};

   double calculateTargetDepth(double &xdist,double &zdist);

 protected:
   double m_x_pos;
   double m_y_pos;

   double m_c0;         // sound speed at surface
   double m_z0;         // depth at ray source
   double m_theta0;     // grazing angle at ray source
   double m_radius;     // radius of ray trace
   double m_center_d;   // center of the circular ray
   double m_gradient;   // sound speed profile gradient

   double m_zmax;       // max depth
   
};

#endif 
