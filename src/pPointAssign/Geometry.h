/************************************************************/
/*    NAME: Simen Sem Oevereng                              
      ORGN: MIT                                             
      FILE: Geometry.h                          
      DATE: Mar 31 2019                                      
      EDIT: -  

      Implementations in Geometry.cpp.

      Stores point x, y val + id for PointAssign application
*/
/************************************************************/

#ifndef Geometry_HEADER
#define Geometry_HEADER

#include <iostream>
#include <stdlib.h>
#include "math.h"
#include <string>

using namespace std;

class Point
{
   public:
      Point() {m_x = 0; m_z = 0; ID = -1}; // default constructor
      Point(double x, double z, string id) {m_x = x; m_z = z; ID = id;};
      ~Point(){};

      // accessors. public since they are used in Line-class
      double getX()  const {return m_x;};
      double getZ()  const {return m_z;};
      int    getID() const {return ID; };

      // utilities
      string printPoint() const ;

   protected:
      // variables with position in x,z-plane
      double m_x;
      double m_z; 
      int ID;
};