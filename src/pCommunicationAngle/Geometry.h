/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: Geometry.h                                        */
/*    DATE:                                                 */
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
      Point() {m_x = 0; m_z = 0;};
      Point(double x, double z) {m_x = x; m_z = z;};
      ~Point(){};

      double getX() const {return m_x;};
      double getZ() const {return m_z;};

      string printPoint() const ;

   protected:
      void set(double &x, double &z);

   protected:
      double m_x;
      double m_z;
};

class Line
{
   public:
      Line();
      Line(Point p1, Point p2){setPoints(p1,p2); setAngle();};
      ~Line(){};

      double length()   const;
      double getAngle() const {return(m_theta);};

      Point getMidpoint() const; 
      Point getCircleCenter(double z_end) const;
      double getCircleRadius(double z_end) const;

      Line getNormalLine(double z_end) const; // Not used

      string printLine() const;

   protected:
      void setPoints(Point &p1, Point &p2) {m_p1 = p1;
      m_p2 = p2 ;};
      void setAngle();


   protected:
      Point m_p1;
      Point m_p2;
      double m_theta;
};

#endif