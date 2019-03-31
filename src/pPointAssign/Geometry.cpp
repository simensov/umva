/************************************************************/
/*    NAME: Simen Sem Oevereng                              
      ORGN: MIT                                             
      FILE: Geometry.cpp                          
      DATE: Mar 5 2019                                      
      EDIT: -  

      Declarations in Geometry.h.

      Stores point x, y val + id for PointAssign application
*/
/************************************************************/

#include "Geometry.h"
using namespace std;

/////////////
/// POINT ///
/////////////

//---------------------------------------------------------
// Procedure: printPoint
// PURPOSE:   creates a string that prints the position and ID of Point object
// @param     no inputs 
// @edits     no edits
// @return    a string containing the x,y position of the Point, and ID
string Point::printPoint() const {
  string s = "(" + to_string(m_x) + "," + to_string(m_z) + ") : " + to_string(ID);
  return s;
}