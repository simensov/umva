#include <iostream>
#include "Geometry.h"

using namespace std;

int main(){

  Point p1(10,10);
  Point p2(40,50);
  Line chord(p1,p2);

  cout << chord.printLine() << endl;

  double zz = -10;

  cout << (chord.getMidpoint()).printPoint() << endl;
  cout << chord.getCircleRadius(zz) << endl;

  return 0;
}