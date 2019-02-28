/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: RayEntry.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "RayEntry.h"

using namespace std;

RayEntry::RayEntry(){
  m_x_pos = 0;
  m_y_pos = 0;

  m_c0 = 0;         // sound speed at surface
  m_z0 = 0;         // depth at ray source
  m_theta0 = 0;     // grazing angle at ray source
  m_radius = 0;     // radius of ray trace
  m_center_d = 0;
  m_gradient = 0;   // sound speed profile gradient

}

RayEntry::RayEntry(double &depth, double &angle) : RayEntry() {
  setInitialRay(depth,angle);
}

double RayEntry::calculateTargetDepth(double &xdist,double &zdist){

  // take in distances between ray source and vehicle from MOOS App
  // use distances to find chord length between points
  // use formula relating arc length and chord length to theta

  double theta = m_theta0 - asin(sqrt(xdist*xdist + zdist*zdist) / 2*m_radius);

  // use theta to find the parameterized z-value
  // does not consider water depth at all
  return(m_radius*cos(theta) - m_c0/m_gradient);
}

void RayEntry::setCircle(){

  // use try / catch exeption here instead?
  // check for c also since it should never be 0
  if(m_c0 != 0 && m_gradient != 0 && cos(m_theta0) != 0){
    m_radius = m_c0 / (m_gradient * cos(m_theta0));
    m_center_d = m_c0 / m_gradient;
  }

}