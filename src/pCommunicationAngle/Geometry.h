/************************************************************/
/*    NAME: Simen Sem Oevereng                              
      ORGN: MIT                                             
      FILE: Geometry.h                          
      DATE: Mar 5 2019                                      
      EDIT: -  

      Implementations in Geometry.cpp.

      This file contains the a support classes, Point and
      Line, to the implementation of lab 6, in which the
      goal was to calculate transmission angle and loss 
      between two underwater vehicles trying to communicate.
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
      Point() {m_x = 0; m_z = 0;}; // default constructor
      Point(double x, double z) {m_x = x; m_z = z;};
      ~Point(){};

      // accessors. public since they are used in Line-class
      double getX() const {return m_x;};
      double getZ() const {return m_z;};

      // utilities
      string printPoint() const ;

   protected:
      // variables with position in x,z-plane
      double m_x;
      double m_z; 
};

class Line
{
   public:
      Line(Point p1, Point p2){setPoints(p1,p2); setAngle();};
      ~Line(){};

      // accessors
      double getAngle()                   const {return(m_theta);};
      double circleRadius(double z_end)   const;

      // tools
      double length()      const;
      Point midpoint()     const; 
      string printLine()   const;
      Point circleCenter(double z_end) const;

   protected:
      void setPoints(Point &p1, Point &p2) {m_p1 = p1; m_p2 = p2 ;};
      void setAngle();

   protected:
      Point m_p1;       // starting point
      Point m_p2;       // end point
      double m_theta;   // angle (instead of slope, measured from p1)
};

#endif