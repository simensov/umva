/************************************************************/
/*    NAME: Simen Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: PointAssign.cpp                                        */
/*    DATE: Apr 2. 2019                                                */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PointAssign.h" // has included Geometry.h and Point class

#include <algorithm> //std::sort() and std::transform

using namespace std;

//---------------------------------------------------------
// Constructor
PointAssign::PointAssign()
{
  m_last_point_received = false;
  m_assign_by_region = true;
  m_points_are_published = false;
}

//---------------------------------------------------------
// Destructor

PointAssign::~PointAssign()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PointAssign::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey(); 

    // look for vehicle names until all 2 expected have reported NODE_REPORT
    if(key == "NODE_REPORT" && m_all_vehicles.size() < 2){
      // get vehicle name
      string line = msg.GetString();
      string name = tokStringParse( line, "NAME" , ',' , '=');

      // transform name to uppercase
      transform(name.begin(), name.end(), name.begin(), static_cast<int(*)(int)>(std::toupper));

      // check if name already exists. if not; add to vector
      vector<string>::iterator it;
      bool exists = false;
      
      // check for existence
      for(it = m_all_vehicles.begin(); it != m_all_vehicles.end(); ++it){
        if(*it == name){
          exists = true;
        }
      }

      if(!exists){
        m_all_vehicles.push_back(name);
      }
    }

    // store all points in a vector if it is not the first or last strings
    if (key == "VISIT_POINT"){

      string line = msg.GetString();

      if (line == "firstpoint"){
        m_all_points = {}; // clear vector
      }
      else if (line == "lastpoint"){
        m_last_point_received = true; // send all points
      } 
      else {

        string x_string = biteStringX(line, ',');
        string dummy = biteStringX(x_string,'='); 
        double x_pos = stod(x_string);

        string y_string = biteStringX(line, ',');
        dummy = biteStringX(y_string,'='); 
        double y_pos = stod(y_string);

        // now line contains 'id=num', so bite looking for '=' instead of ','
        dummy = biteStringX(line,'=');
        double id = stod(line);

        Point pt(x_pos, y_pos, id);
        m_all_points.push_back(pt);

      } 
    }

   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PointAssign::OnConnectToServer()
{
   // register for variables here
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PointAssign::Iterate()
{
  // Act according to assignment rule chosen when all points and vehicles are ready. m_all_points should contain Point objects with printPoint

  if(m_last_point_received && m_all_vehicles.size() == 2){

    vector<Point> west_points;
    vector<Point> east_points;

    if(m_assign_by_region){
      // Operator < has been overloaded to sort on x-values
      sort(m_all_points.begin(),m_all_points.end());

      // Distribute
      for(int i = 0; i < m_all_points.size(); i++){
        if (i < m_all_points.size() / 2){ // flooring the half-way point
          west_points.push_back(m_all_points[i]);
        }
        else{
          east_points.push_back(m_all_points[i]); 
        }
      }

    } else {
      // Alternate between which points are distributed
      for(int i = 0; i  < m_all_points.size(); i++){
        if(i % 2){
          west_points.push_back(m_all_points[i]);
        }
        else{
          east_points.push_back(m_all_points[i]);
        }
      }
    }

    // TODO: change the hardcoding of vehicle names. Use OnNewMail to read NODE_REPORT, which has "name=gilda" as first element (bite on ',').
    string vpt1 = "VISIT_POINT_" + m_all_vehicles[0];
    string vpt2 = "VISIT_POINT_" + m_all_vehicles[1];

    Notify(vpt1,"firstpoint");
    Notify(vpt2,"firstpoint");

    vector<Point>::iterator it;
    for (it = west_points.begin() ; it != west_points.end(); ++it){
      Notify(vpt1, (*it).printPoint() );
      postViewPoint( (*it).getX(), (*it).getZ(), to_string( (*it).getID() ),\
        "yellow");
    }

    for (it = east_points.begin() ; it != east_points.end(); ++it){
      Notify(vpt2, (*it).printPoint());
      postViewPoint( (*it).getX(), (*it).getZ(), to_string( (*it).getID() ), "red");
    }
    Notify(vpt1,"lastpoint");
    Notify(vpt2,"lastpoint");
  
    // To avoid that the points are published in an infinite loop in iterate, we supress it by saying that the last point is false. It is assumed that if it receives new points later, it will go through the same process over again
    m_last_point_received = false;

  } // if all points received

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PointAssign::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
      
      if(param == "foo") {
        //handled
      }
      else if(param == "bar") {
        //handled
      }
    }
  }
  
  RegisterVariables();

  Notify("UTS_PAUSE","false");
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PointAssign::RegisterVariables()
{
  Register("VISIT_POINT", 0);
  Register("NODE_REPORT", 0);
}


//---------------------------------------------------------
// Procedure: postViewPoint
// PURPOSE:   posts a MOOS Variable so that the pMarineViewer can visualize
// @param     
// @edits     
// @return 
void PointAssign::postViewPoint(double x, double y, string label, string color)
{
  XYPoint point(x, y);
  point.set_label(label);
  point.set_color("vertex", color);
  point.set_param("vertex_size", "6");

  string spec = point.get_spec();
  Notify("VIEW_POINT", spec);
}