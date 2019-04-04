/************************************************************/
/*    NAME: Simen Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: GenPath.cpp                                        */
/*    DATE: Apr 2. 2019                                                */
/************************************************************/

// TODO 4. apr:
// FIX UNDEFINED BEHAVIOUR IN THE END OF THE MISSIONS;
// SEEMS LIKE THE VERY LAST PATH IS DISTURBED IF IT ISN'T COMPLETED ENTIRELY THE LAST RUN. MIGHT HAVE SOMETHING TO DO WITH ENDFLAG IN BEHAVIOUR!


#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "GenPath.h"
#include <map>

using namespace std;

string BoolToString(bool b)
{
  return b ? "true" : "false";
}


//---------------------------------------------------------
// Constructor

GenPath::GenPath()
{
  m_last_point_received = false;
  m_init_x = 0.000001;
  m_init_y = 0.000001;
  m_x_rec = false;
  m_y_rec = false;
  m_nav_rec = false;
  m_path_calculated = false;

  m_searching = false;

  m_nav_x = 0;
  m_nav_y = 0;
  m_visit_radius = 5;

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

    // used for retrieving position. also stores initial position for path gen
    if (key == "NAV_X"){
      if(m_x_rec == false){
        m_x_rec = true;
        m_init_x = msg.GetDouble();
      }

      m_nav_x = msg.GetDouble();
    }
    if (key == "NAV_Y"){
      if(m_y_rec == false){
        m_y_rec = true;
        m_init_y = msg.GetDouble();
      } 
      m_nav_y = msg.GetDouble();
    }

    if (m_x_rec && m_y_rec){
      // store that both initial positions has been received
      m_nav_rec = true;
    }

    if (key == "VISIT_POINT"){

      string line = msg.GetString();

      if (line == "firstpoint"){
        m_all_points = {}; // clear vector
      }
      else if (line == "lastpoint"){
        m_last_point_received = true; // send all points

        m_remaining_points = m_all_points; // initiate first time all points

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


    if (key == "GENPATH_REGENERATIVE"){

      if(msg.GetString() == "true"){
        // the vehicles has to refuel. compare all visited points with remaining vector, and update remaining accordingly. recalculate path

        // iterators for visited list and remaining points
        vector<Point>::iterator it_vis, it_rem;

        for(it_vis = m_visited_points.begin(); it_vis != m_visited_points.end(); ++it_vis){

          for(it_rem = m_remaining_points.begin(); it_rem != m_remaining_points.end(); ){

            // if the point has been visited, erase it from remaining points
            // == Operator has been overloaded in Geometry.h for Point objects
            if( *it_rem == *it_vis ){
              // update iterator 
              it_rem = m_remaining_points.erase(it_rem);
            }
            else{
              ++it_rem;
            }
          } // for rem
        } // for vis
        
        // recalculate instead of iterate to avoid empty remaining vector
        vector<Point> ret_pts = greedyPath();
        publishGreedyPath(ret_pts);    
        

      } // if string test 
    } // if key genpath

    if (key == "REFUEL_NEEDED"){
      // TODO: this handles that the vehicle avoid counting points on the way home. On the way back, the path is recalculated, so closest point will be first visited!

      string line = msg.GetString();

      if(line == "true"){
        m_searching = false;
      }
      else {
        m_searching = true; // this is the only place where m_searching is set to true
      }
    }

    
   } // for all newmail

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

  // TODO: this could be placed to a more fitting place
  if(m_last_point_received && m_path_calculated == false){

    vector<Point> ret_pts = greedyPath();
    publishGreedyPath(ret_pts); // sets path calculated to true
  }

  // only regenerate if we have remaining pts and GENPATH has been sent
  if( m_visited_points.size() != m_all_points.size() ){
    // points has been received
    Point current_pos(m_nav_x, m_nav_y,-1);

    // old and uncorrect note: since we are only going through the remaining points vector, the and pushing to visited from it, visited shouldn't have to be a set of unique points since one point never should be pushed to it twice
    // CORRECTION: that might be right, but the for loop does this a lot of times when a certain point is within reach

    vector<Point>::iterator it;
    for(it = m_remaining_points.begin(); it != m_remaining_points.end(); ++it){

      double distance = current_pos.distanceTo(*it); 

      if(distance <= m_visit_radius){

        bool exists = false;
        // check through if the visited 
        for(int i = 0; i < m_visited_points.size(); i++){
          if(*it == m_visited_points[i]){
            exists = true;
          }
        }

        if(!exists){
          m_visited_points.push_back(*it);
        }
      }

    }

    // test for visit ?? all pts was here

    // return here to avoid for-loop adding one point multiple times
    AppCastingMOOSApp::PostReport();
    return(true);
  }
  else{
      Notify("TRAVERSE","false");
      Notify("RETURN","true");
      m_searching = false;
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
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void GenPath::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("VISIT_POINT", 0);
  Register("GENPATH_REGENERATIVE", 0);
  Register("REFUEL_NEEDED",0);
  Register("NAV_X",0);
  Register("NAV_Y",0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool GenPath::buildReport() 
{
  m_msgs << "============================================"  << endl;
  m_msgs << "File: GenPath.cpp                           "  << endl;
  m_msgs << "============================================"  << endl;
  m_msgs << "Number of points: " << m_all_points.size()     << endl;
  m_msgs << "Points visited: " << m_visited_points.size()   << endl;
  m_msgs << "Points remaining: " << m_remaining_points.size() << endl;
  m_msgs << "Currently searching: " << BoolToString(m_searching)  << endl;
  Point current_pos(m_nav_x, m_nav_y,-1);
  m_msgs << "Current pos: " << current_pos.printPoint()      << endl;


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
 
  vector<Point> pts = m_remaining_points; // to browse and remove 

  map<double, int> idx; // a map that will contain distance and a list of all the indexes in m_all_points that has that distance to current 

  Point p(m_init_x,m_init_y);

  int no_of_points = m_remaining_points.size();

  // for every single point that shall be visited
  for(int i = 0; i < no_of_points; i++){

    // for all points that are currently not added to generated path
    for(int j = 0; j < pts.size(); j++){
      idx[ pts[j].distanceTo(p) ] = j; 
    }

    // choose closest point by the stored index in idx
    // the map is sorted by .first, which contains distance
    int index = idx.begin()->second;
    Point closest = pts[ index ];
    ret_pts.push_back(closest);

    // update p, remove from vector, clear map for next bunch of points
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
  Notify("UPDATES_TRAVERSE_WAYPT", update_str);

  // Notifying so that traverse can happen 
  Notify("TRAVERSE","true");
  m_path_calculated = true;
}