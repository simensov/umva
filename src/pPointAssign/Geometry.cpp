/************************************************************/
/*    NAME: Simen Sem Oevereng                              
      ORGN: MIT                                             
      FILE: Geometry.cpp                          
      DATE: Mar 31 2019                                      
      EDIT: -  

      Declarations in Geometry.h.

      Stores point x, y val + id for PointAssign application
*/
/************************************************************/

#include "Geometry.h"
using namespace std;

//---------------------------------------------------------
// Procedure: printPoint
// PURPOSE:   creates a string that prints the position and ID of Point object
// @param     no inputs 
// @edits     no edits
// @return    a string containing the x,y position of the Point, and ID
string Point::printPoint() const {
  string s = "x=" + to_string(m_x) + ", y=" + to_string(m_z) + ", id=" + to_string(ID);
  return s;
}