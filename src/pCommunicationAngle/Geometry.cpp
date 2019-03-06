/************************************************************/
/*    NAME: Simen Sem Oevereng                              
      ORGN: MIT                                             
      FILE: Geometry.cpp                          
      DATE: Mar 5 2019                                      
      EDIT: -  

      Declarations in Geometry.h.

      This file contains the a support classes, Point and
      Line, to the implementation of lab 6, in which the
      goal was to calculate transmission angle and loss 
      between two underwater vehicles trying to communicate.
*/
/************************************************************/


#include "Geometry.h"
using namespace std;

/////////////
/// POINT ///
/////////////

//---------------------------------------------------------
// Procedure: printPoint
// PURPOSE:   creates a string that prints the position of a Point object
// @param     no inputs 
// @edits     no edits
// @return    a string containing the x and z position of the Point
string Point::printPoint() const {
  string s = "(" + to_string(m_x) + "," + to_string(m_z) + ")";
  return s;
}

/////////////
/// LINE  ///
/////////////

//---------------------------------------------------------
// Procedure: length
// PURPOSE:   returns length of a line 
// @param     no inputs
// @edits     no edits
// @return    the length of the line
double Line::length() const {
  return(sqrt(pow(m_p2.getX() - m_p1.getX(),2) + pow(m_p2.getZ() - m_p1.getZ(),2)));
}

//---------------------------------------------------------
// Procedure: setAngle
// PURPOSE:   sets the angle (instead of slope) of a line
// @param     no inputs
// @edits     m_theta: the angle of the line
// @return    nothing
void Line::setAngle(){
  // remember: FIRST quadrant is down, right in cartesian coord syst since theta is defined positive down from horizon, clockwise

  // if x-vals are not the same - get angle through arctan
  double dz = m_p2.getZ() - m_p1.getZ();
  double dx = m_p2.getX() - m_p1.getX();

  // check for dx = 0. if they are equal, two cases are treated later
  if(dx){
    // adjust for which quadrant we are in by assuming first point is always in the first quadrant so that it can be used as reference

    if(dz < 0){
      // first point is DEEPER than second (hence larger z-val)
      if(dx < 0){
        // second point is BEHIND first (hence lower x-val)
        m_theta = M_PI + atan(dz / dx);
      } else {
        // second point is in FRONT, but not as deep
        m_theta = -atan(abs(dz) / abs(dx));
      }
    }
  }
  else{
    // if x2 = x1 and second point is deeper than first - 90 degrees
    if(m_p2.getZ() > m_p1.getZ()){
      m_theta = M_PI/2;
    }
    else{
      // 270 degrees
      m_theta = 3*M_PI/2;
    }
  }
}

//---------------------------------------------------------
// Procedure: midpoint
// PURPOSE:   returns a Point in the middle of the line 
// @param     no inputs
// @edits     no edits
// @return    p: the midpoint on the line as a Point object
Point Line::midpoint() const {
  // theta defined positive WITH clock, starting horiontally to the right

  double dz = (length()/2) * sin(m_theta);
  double dx = (length()/2) * cos(m_theta);

  Point p(m_p1.getX() + dx, m_p1.getZ() + dz);

  return(p);
}

//---------------------------------------------------------
// Procedure: circleCenter
// PURPOSE:   returns circle of a center in which the line makes the chord to
// @param     z_end: the z-position of the circle center
// @edits     no edits
// @return    a Point object representing the circle center
Point Line::circleCenter(double z_end) const {
  Point midpt = midpoint();
  double dx = tan(m_theta) * ( midpt.getZ() + abs(z_end));

  Point p2( midpt.getX() + dx, z_end);

  return(p2);
}

//---------------------------------------------------------
// Procedure: getCircleRadius
// PURPOSE:   calculates the radius of the circle made from using self as 
//            chordline 
// @param     z_end: the z-position of the circle center 
// @edits     no edits
// @return    a double representing the radius
double Line::getCircleRadius(double z_end) const {
  Line l(m_p1,circleCenter(z_end));
  return(l.length());
}

//---------------------------------------------------------
// Procedure: printLine
// PURPOSE:   creates a string to use with cout for printing the line as 
//            "(Point1),(Point2):angle"
// @param     no inputs
// @edits     no edits
// @return    a string with two points and an angle
string Line::printLine() const {
  return(m_p1.printPoint()+","+m_p2.printPoint()+":"+to_string(getAngle()));
}