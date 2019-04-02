/************************************************************/
/*    NAME: Simen Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "GenPath.h"
#include <map>

using namespace std;

//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
  m_last_point_received = false;
  m_init_x = 0;
  m_init_y = 0;
  m_x_rec = false;
  m_y_rec = false;
  m_nav_rec = false;
  m_path_calculated = false;
}

//---------------------------------------------------------
// Destructor

GenPath::~GenPath()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GenPath::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.GetKey(); 

    cout << msg.GetString() << endl;

    // used for retrieving initial position
    if (key == "NAV_X" && !m_path_calculated){
      m_x_rec = true;
      m_init_x = msg.GetDouble();
    }
    if (key == "NAV_Y" && !m_path_calculated){
      m_y_rec = true;
      m_init_y = msg.GetDouble();
    }
    if (m_x_rec && m_y_rec){
      m_nav_rec = true;
    }

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

    } // IF VISIT_POINT




#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

   }

   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool GenPath::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool GenPath::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!

  if(m_last_point_received && m_path_calculated == false){
    // TODO: is this the correct place to have this function?
    vector<Point> ret_pts = greedyPath();
    publishGreedyPath(ret_pts);
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GenPath::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();


  /*
  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "foo") {
      handled = true;
    }
    else if(param == "bar") {
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  */
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void GenPath::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("VISIT_POINT", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool GenPath::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "File: GenPath.cpp                           " << endl;
  m_msgs << "============================================" << endl;
  m_msgs << "Number of points: " << m_all_points.size() << endl;

  return(true);
}


//---------------------------------------------------------
// Procedure:
// PURPOSE:   
// @param     
// @edits     
// @return 
vector<Point> GenPath::greedyPath() const{

  vector<Point> ret_pts; // to return
  vector<Point> pts = m_all_points; // to browse and remove 

  map<double, int> idx; // a map that will contain distance and a list of all the indexes in m_all_points that has that distance to current 

  Point p(m_init_x,m_init_y);

  for(int i = 0; i < m_all_points.size(); i++){

    for(int i = 0; i < pts.size(); i++){
      idx[pts[i].distanceTo(p)] = i; 
    }

    // choose closest point by the stored index in idx
    // the map is sorted by .first, which contains distance
    int index = idx.begin()->second;
    Point closest = pts[ index ];
    ret_pts.push_back(closest);

    // update p, remove from vector, clear map
    p = closest;
    pts.erase( pts.begin() + index ); 
    idx.clear();

  }

  return(ret_pts);
}

//---------------------------------------------------------
// Procedure:
// PURPOSE:   
// @param     
// @edits     
// @return 
void GenPath::publishGreedyPath(vector<Point> ret_pts){

  XYSegList my_seglist;
  for(vector<Point>::iterator it = ret_pts.begin(); it != ret_pts.end(); ++it){
    my_seglist.add_vertex( (*it).getX(), (*it).getZ());
  }

  string update_str = "points = ";
  update_str += my_seglist.get_spec();

  cout << update_str << endl;
  Notify("UPDATES_TRAVERSE_WAYPT", update_str);

  m_path_calculated = true;

  // TODO: post the deploy variable OR Notify("TRAVERSE","true")
  Notify("TRAVERSE","true");
}