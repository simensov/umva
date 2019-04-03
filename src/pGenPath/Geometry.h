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
      Point() {m_x = 0; m_z = 0; m_ID = -1;}; // default constructor
      Point(double x, double z, int id=0) {m_x = x; m_z = z; m_ID = id;};
      ~Point(){};

      // accessors. public since they are used in Line-class
      double getX()  const {return m_x;};
      double getZ()  const {return m_z;};
      int    getID() const {return m_ID; };

      // utilities
      string printPoint() const;

      double distanceTo(Point p) const;

      bool operator < (const Point& p) const { return (m_x < p.m_x); }

      // TODO: check these operators
      bool operator == (const Point& p) const { 
            return( m_ID == p.getID() ); }
      bool operator != (const Point& p) const { 
            return !(*this == p); }

   protected:
      // variables with position in x,z-plane
      double m_x;
      double m_z; 
      int m_ID;
};

#endif