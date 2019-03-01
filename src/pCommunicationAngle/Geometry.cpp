/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: Geometry.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include "Geometry.h"

using namespace std;

///
/// POINT
///

string Point::printPoint() const {
  string s = "(" + to_string(m_x) + "," + to_string(m_z) + ")";
  return s;
}

///
/// LINE
///

double Line::length() const {
  return(sqrt(pow(m_p2.getX() - m_p1.getX(),2) + pow(m_p2.getZ() - m_p1.getZ(),2)));
}

void Line::setAngle(){
  // if x-vals are not the same - get angle through arctan
  if(m_p2.getX() != m_p1.getX()){
    m_theta = atan((m_p2.getZ() - m_p1.getZ()) / (m_p2.getX() - m_p1.getX()) );
  }
  else{
    // if second point is deeper than first - 90 degrees
    if(m_p2.getZ() > m_p1.getZ()){
      m_theta = M_PI/2;
    }
    else{
      // 270 degrees
      m_theta = 3*M_PI/2;
    }
  }
}

Point Line::getMidpoint() const {
  // theta defined positive WITH clock, starting horiontally to the right

  double dz = (length()/2) * sin(m_theta);
  double dx = (length()/2) * cos(m_theta);

  Point p(m_p1.getX() + dx, m_p1.getZ() + dz);

  return(p);
}

Point Line::getCircleCenter(double z_end) const {
  Point midpoint = getMidpoint();
  double dx = tan(m_theta) * ( midpoint.getZ() + abs(z_end));
  Point p2( midpoint.getX() + dx, z_end);
  return(p2);
}

Line Line::getNormalLine(double z_end) const {
  // theta defined positive WITH clock, starting horiontally to the right

  Point midpoint = getMidpoint();
  Point center = getCircleCenter(z_end);
  Line l(midpoint,center);

  return(l);
}

double Line::getCircleRadius(double z_end) const {
  Line l(m_p1,getCircleCenter(z_end));
  return(l.length());
}

string Line::printLine() const {
  return(m_p1.printPoint()+","+m_p2.printPoint()+":"+to_string(getAngle()));
}