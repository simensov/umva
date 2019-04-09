/*****************************************************************/
/*    NAME: Oliver Os                                            */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardMgr.cpp                                        */
/*    DATE: Apr 3rd 2019                                         */
/*    EDIT: Apr 5rd 2019 (By Simen Sem Oevereng, MIT)            */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "HazardMgr.h"
#include "XYFormatUtilsHazard.h"
#include "XYFormatUtilsPoly.h"
#include "ACTable.h"
#include <time.h>

using namespace std;

const bool debug = true;

//---------------------------------------------------------
// Constructor

HazardMgr::HazardMgr()
{
  // Config variables
  m_swath_width_desired = 25;
  m_pd_desired          = 0.9;

  // State Variables 
  m_sensor_config_requested = false;
  m_sensor_config_set   = false;
  m_swath_width_granted = 0;
  m_pd_granted          = 0;

  m_sensor_config_reqs = 0;
  m_sensor_config_acks = 0;
  m_sensor_report_reqs = 0;
  m_detection_reports  = 0;

  m_summary_reports = 0;

  m_transit_path_width = 0;

  m_name = "";

  m_last_msg_sent = 0;

  // TODO: NEW
  m_max_time               = 0;
  m_mission_start_time     = 0; 
  m_penalty.missed_hazard  = 0;
  m_penalty.nonopt_hazard  = 0;
  m_penalty.false_alarm    = 0;
  m_penalty.max_time_over  = 0;
  m_penalty.max_time_rate  = 0;

  m_search_region_str = "";

}

//---------------------------------------------------------
// Procedure: OnNewMail

bool HazardMgr::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 

    bool handled = false;

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
    
    if(key == "UHZ_CONFIG_ACK"){
      handleMailSensorConfigAck(sval);
      handled = true;
    }

    if(key == "UHZ_OPTIONS_SUMMARY"){
      handleMailSensorOptionsSummary(sval);
      handled = true;
    }

    if(key == "UHZ_DETECTION_REPORT"){
      handleMailDetectionReport(sval);
      handled = true;
    }

    if(key == "HAZARDSET_REQUEST"){
      handleMailReportRequest();
      handled = true;
    }

    if(key == "UHZ_MISSION_PARAMS"){
      handleMailMissionParams(sval);
      handled = true;
    }

    if(key == "NODE_REPORT_LOCAL"){
      if (m_name == ""){
        handleAddName(sval);
      }
      handled = true;
    }

    if (key == "HAZARD_REPORT"){
      handleHazardReport(sval);
      handled = true;
    }

    if(m_name != "" && key == "UHZ_HAZARD_REPORT_" + m_name){
      handleClassificationReport(sval);
      handled = true;
    }
    
    if(!handled) 
      reportRunWarning("Unhandled Mail: " + key);
  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool HazardMgr::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool HazardMgr::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(!m_sensor_config_requested)
    postSensorConfigRequest();

  if(m_sensor_config_set)
    postSensorInfoRequest();

  if(MOOSTime() - m_last_msg_sent > 60.1){
    postHazardMessage();
  }

  // TODO: NEW
  if( (MOOSTime() - m_mission_start_time) > m_max_time * 0.9){
    // Meet up to merge reports and finish mission FEKS
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool HazardMgr::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(true);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if((param == "swath_width") && isNumber(value)) {
      m_swath_width_desired = atof(value.c_str());
      handled = true;
    }
    else if(((param == "sensor_pd") || (param == "pd")) && isNumber(value)) {
      m_pd_desired = atof(value.c_str());
      handled = true;
    }
    else if(param == "report_name") {
      value = stripQuotes(value);
      m_report_name = value;
      handled = true;
    }
    else if(param == "region") {
      XYPolygon poly = string2Poly(value);
      if(poly.is_convex())
        m_search_region = poly;
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  m_hazard_set.setSource(m_host_community);
  m_hazard_set.setName(m_report_name);
  m_hazard_set.setRegion(m_search_region);
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void HazardMgr::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("UHZ_DETECTION_REPORT", 0);
  Register("UHZ_CONFIG_ACK", 0);
  Register("UHZ_OPTIONS_SUMMARY", 0);
  Register("UHZ_MISSION_PARAMS", 0);
  Register("HAZARDSET_REQUEST", 0);
  Register("NODE_REPORT_LOCAL",0);
  Register("HAZARD_REPORT",0);
}

//---------------------------------------------------------
// Procedure: postSensorConfigRequest

void HazardMgr::postSensorConfigRequest()
{
  string request = "vname=" + m_host_community;
  
  request += ",width=" + doubleToStringX(m_swath_width_desired,2);
  request += ",pd="    + doubleToStringX(m_pd_desired,2);

  m_sensor_config_requested = true;
  m_sensor_config_reqs++;
  Notify("UHZ_CONFIG_REQUEST", request);
}

//---------------------------------------------------------
// Procedure: postSensorInfoRequest

void HazardMgr::postSensorInfoRequest()
{
  string request = "vname=" + m_host_community;

  m_sensor_report_reqs++;
  Notify("UHZ_SENSOR_REQUEST", request);
}

//---------------------------------------------------------
// Procedure: handleMailSensorConfigAck

bool HazardMgr::handleMailSensorConfigAck(string str)
{
  // Expected ack parameters:
  string vname, width, pd, pfa, pclass;
  
  // Parse and handle ack message components
  bool   valid_msg = true;
  string original_msg = str;

  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    if(param == "vname")
      vname = value;
    else if(param == "pd")
      pd = value;
    else if(param == "width")
      width = value;
    else if(param == "pfa")
      pfa = value;
    else if(param == "pclass")
      pclass = value;
    else
      valid_msg = false;       

  }


  if((vname=="")||(width=="")||(pd=="")||(pfa=="")||(pclass==""))
    valid_msg = false;
  
  if(!valid_msg)
    reportRunWarning("Unhandled Sensor Config Ack:" + original_msg);

  
  if(valid_msg) {
    m_sensor_config_set = true;
    m_sensor_config_acks++;
    m_swath_width_granted = atof(width.c_str());
    m_pd_granted = atof(pd.c_str());
  }

  return(valid_msg);
}

//---------------------------------------------------------
// Procedure: handleMailDetectionReport
//      Note: The detection report should look something like:
//            UHZ_DETECTION_REPORT = vname=betty,x=51,y=11.3,label=12 

bool HazardMgr::handleMailDetectionReport(string str)
{
  m_detection_reports++;

  XYHazard new_hazard = string2Hazard(str);
  new_hazard.setType("hazard");

  string hazlabel = new_hazard.getLabel();
  
  if(hazlabel == "") {
    reportRunWarning("Detection report received for hazard w/out label");
    return(false);
  }

  int ix = m_hazard_set.findHazard(hazlabel);
  if(ix == -1)
    m_hazard_set.addHazard(new_hazard);
  else
    m_hazard_set.setHazard(ix, new_hazard);

  string event = "New Detection, label=" + new_hazard.getLabel();
  event += ", x=" + doubleToString(new_hazard.getX(),1);
  event += ", y=" + doubleToString(new_hazard.getY(),1);

  TODO: reportEvent(event);

  string req = "vname=" + m_host_community + ",label=" + hazlabel;
  Notify("UHZ_CLASSIFY_REQUEST", req);

  return(true);
}


//---------------------------------------------------------
// Procedure: handleMailReportRequest

void HazardMgr::handleMailReportRequest()
{
  m_summary_reports++;

  m_hazard_set.findMinXPath(20);
  //unsigned int count    = m_hazard_set.findMinXPath(20);
  string summary_report = m_hazard_set.getSpec("final_report");
  
  Notify("HAZARDSET_REPORT", summary_report);
}


//---------------------------------------------------------
// Procedure: handleMailMissionParams
//   Example: UHZ_MISSION_PARAMS = penalty_missed_hazard=100,               
//                       penalty_nonopt_hazard=55,                
//                       penalty_false_alarm=35,                  
//                       penalty_max_time_over=200,               
//                       penalty_max_time_rate=0.45,              
//                       transit_path_width=25,                           
//                       search_region = pts={-150,-75:-150,-50:40,-50:40,-75}

// Store all parameters for the mission. Update member variables and publish search region to a waypoint update moos cariable to decide search region
void HazardMgr::handleMailMissionParams(string str)
{
  vector<string> svector = parseStringZ(str, ',', "{");
  unsigned int vsize = svector.size();
  for(unsigned int i=0; i<vsize; i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    // TODO: NEW
    if(param == "penalty_missed_hazard")
      m_penalty.missed_hazard = stod(value);

    if(param == "penalty_nonopt_hazard")
      m_penalty.nonopt_hazard = stod(value);

    if(param == "penalty_false_alarm")
      m_penalty.false_alarm = stod(value);

    if(param == "penalty_max_time_over")
      m_penalty.max_time_over = stod(value);

    if(param == "penalty_max_time_rate")
      m_penalty.max_time_rate = stod(value);

    if(param == "max_time")
      m_max_time = stod(value);

    if(param == "transit_path_width")
      m_transit_path_width = stod(value);

    if(param == "search_region")
      m_search_region_str = value; //pts={-150,-75:-150,-400:400,-400:400,-75} 
  }
}

// TODO: NEW
//---------------------------------------------------------
// Procedure: handleMailMissionParams
// Purpose:   deals with incoming classification reports from UHZ that the 
//            vehicle has requested classification on
//            Example str: "label=12,type=benign"
void HazardMgr::handleClassificationReport(string str){
  vector<string> svector = parseStringZ(str, ',', "{");
  int lab = -1;
  string haz_str = "";
  bool haz = false;

  unsigned int vsize = svector.size();
  for(unsigned int i=0; i<vsize; i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    // TODO: NEW
    if(param == "label")
      lab = stod(value);

    if(param == "type")
      haz_str = value;

    if(lab != -1 && !haz_str.empty()){
      if(haz_str == "benign"){
        haz = false;
      } else{
        haz = true;
      }

      // TODO: needs to add probability somehow
      Classification c(lab,haz);

      // Add classification to member vector of classifications
      // If it already exists, then compute new probability based on old classification vs. new one
      

      vector<Classification>::iterator it;
      for(it = m_classifications.begin(); it != m_classifications.end(); ++it){
        if( (*it).getLabel() ==  lab){
          // TODO: calculate new prob, e.g.  getProb*getProb so low probabilities will get really low 
        }
        else{
          // Not classified before - add to vector
          m_classifications.push_back(c);
        }
      } // for all former classifications
    } // if we have got a labal and a type
  } // for all elements in current string
} // func

//------------------------------------------------------------
// Procedure: buildReport()

bool HazardMgr::buildReport() 
{
  if(!debug){
    m_msgs << "Config Requested:"                                  << endl;
    m_msgs << "    swath_width_desired: " << m_swath_width_desired << endl;
    m_msgs << "             pd_desired: " << m_pd_desired          << endl;
    m_msgs << "   config requests sent: " << m_sensor_config_reqs  << endl;
    m_msgs << "                  acked: " << m_sensor_config_acks  << endl;
    m_msgs << "------------------------ "                          << endl;
    m_msgs << "Config Result:"                                     << endl;
    m_msgs << "       config confirmed: " << boolToString(m_sensor_config_set) << endl;
    m_msgs << "    swath_width_granted: " << m_swath_width_granted << endl;
    m_msgs << "             pd_granted: " << m_pd_granted          << endl << endl;
    m_msgs << "--------------------------------------------" << endl << endl;

    m_msgs << "               sensor requests: " << m_sensor_report_reqs << endl;
    m_msgs << "             detection reports: " << m_detection_reports  << endl << endl; 

    m_msgs << "   Hazardset Reports Requested: " << m_summary_reports << endl;
    m_msgs << "      Hazardset Reports Posted: " << m_summary_reports << endl;
    m_msgs << "                   Report Name: " << m_report_name << endl;
  }

  return(true);
}


//---------------------------------------------------------
// Procedure: handleAddName(string)
// PURPOSE:   finds the local vehicle's name for use in message sending
// @param     str: a string of the name
// @edits     m_name: member string with name
// @return    no returns
void HazardMgr::handleAddName(string str)
{
  vector<string> report_list = parseString(str,',');

  for (unsigned int i = 0; i < report_list.size() ; i++){
    string def = biteStringX(report_list[i],'=');
    if (def == "NAME"){
      string name = report_list[i];
      for(int i = 0; i < name.length(); i++) {
        name.at(i) = toupper(name.at(i));
      }   
      m_name = name;

      // TODO: discuss this with oliver
      // I would like a string_val as well, and something that needs to be updated if the size of string_val gets too long
      m_msg += "src_node=" + m_name;
      m_msg += ",dest_node=all";
      m_msg += ",var_name=HAZARD_REPORT";
      m_msg += ",string_val=";

      UnRegister("NODE_REPORT_LOCAL");
    }
  }
}

//---------------------------------------------------------
// Procedure: postHazardMessage()
// PURPOSE:   To update the output report string that is sent to shoreside and 
//            other vehicles on which hazards that has been found by LOCAL
// @param     no inputs
// @edits     m_msg: member string for all reports
// @return    no returns
void HazardMgr::postHazardMessage()
{
  // TODO: moved into handleAddName()
  /*
  string msg;
  msg += "src_node=" + m_name;
  msg += ",dest_node=all";
  msg += ",var_name=HAZARD_REPORT";
  msg += ",string_val=";
  */

  string added_msg; // message to be added upon the existing one
  XYHazardSet unsent_hazards; // all hazards that has not been sent yet

  // Get each hazard, and check to see if it is already sent
  // If not, then add to the set of unsent hazards
  for (unsigned int i = 0; i < m_hazard_set.size() ; i++){
    XYHazard current = m_hazard_set.getHazard(i);
    string label = current.getLabel();

    if (! (m_hazard_sent.hasHazard(label)) ){
      unsent_hazards.addHazard(current);
    }
  } 

  // If we found unsent hazards, add it to output string
  // TODO: Handle case where the output is too long for constraint of 100
  if (!(unsent_hazards.size() == 0)){

    // For all unsent, add specs to out member message string
    // After adding to string, add it to set of sent hazards
    for (unsigned int i = 0; i < unsent_hazards.size(); i++){
      XYHazard current_unsent = unsent_hazards.getHazard(i);
      added_msg = current_unsent.getSpec("");

      // TODO remove false
      // if message length is longer than 100 elements, tell by M 
      // here, test must be changed for length of string_val
      if (false && m_msg.length() + added_msg.length() + 2 > 100) {
        m_msg += "M"; // TODO: her sto en ensom m_hazard_set
        break;
      }

      added_msg += "#";
      m_msg += added_msg;

      m_hazard_sent.addHazard(current_unsent);
    }
  }

  // TODO: remove false
  // TODO: this F has to be removed later in a string reading function, if m_msg is kept as member OR could be solved by Notify("NODE_MESSAGE_LOCAL",m_msg + "M");
  if (false && m_msg.find("M") != std::string::npos){
    m_msg += "F";
  }

  Notify("NODE_MESSAGE_LOCAL", m_msg);
  m_last_msg_sent = MOOSTime();

  if(debug)
    cout << "POSTED: " << m_msg << endl;
}

//---------------------------------------------------------
// Procedure: handleHazardReport(string)
// PURPOSE:   Handles an incoming hazard report
// @param     str: a string with a hazard report
// @edits     m_hazard_set, m_hazard_sent
// @return    no returns
void HazardMgr::handleHazardReport(string str)
{
  Notify("WAIT_FOR_HAZARD","true");
  vector<string> received_hazards = parseString(str,'#');
  for (unsigned int i = 0 ; i < received_hazards.size() ; i++){
    string current = received_hazards[i];

    // TODO: maybe we have to control for "" here as well, since # might be put on the end of a message (I can see that such a situation should never occur in postMessage, but seems like god programmeringsskikk)

    if (current != "M" && current != "F"){
      XYHazard new_hazard = string2Hazard(current);
      string label = new_hazard.getLabel();

      // If the hazard is not in the LOCAL set of hazards, then we can add it to 
      if (!m_hazard_set.hasHazard(label)){
        m_hazard_set.addHazard(new_hazard);
        m_hazard_sent.addHazard(new_hazard);
      }

      // All hazards has been received - stop waiting
      // TODO: where is such a variable handled?
      if (current == "F"){
        Notify("WAIT_FOR_HAZARD","false");
      }
    }
  }
}