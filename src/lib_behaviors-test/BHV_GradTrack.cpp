/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_GradTrack.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_GradTrack.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

#include "XYRangePulse.h" // for XYRangePulse 

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_GradTrack::BHV_GradTrack(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "GradTrack");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");

  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y");
  addInfoVars("NAV_HEADING");
  addInfoVars("FOO", "no_warning");
  addInfoVars("COLD_DIR","no_warning");     // 
  addInfoVars("FRONT_GRADIENT","no_warning");
  addInfoVars("UCTD_MSMNT_REPORT","no_warning");
  addInfoVars("AVERAGE_TEMP","no_warning");

  m_priority_wt = 100;

  // TODO: Initialize all member variables
  m_nav_x = 0;
  m_nav_y = 0;
  m_nav_h = 0;

  m_temp_threshold = 0.5; // TODO: Check this size from runs
  m_measured_temp_avg = 20;
  m_global_temp_avg = 20;
  m_global_front_gradient = 0.0; // assumed to be angle in radians, moving in in x,y coordsyst
  m_cold_direction = -1; // initialized to give left turn if sailing east
  m_turn_rate = 5;
  m_started = false;

  m_waypoint_time = -0.01;
  m_waypoint_index = -1; // initialized to control for first index update in onRunState()

  m_pulse_x = 0;
  m_pulse_y = 0;
  m_pulse_range = 0;
  m_pulse_duration = 0;
  m_pulse_time = 0;
  m_pulse_sent = true;

  m_zigleg_offset = 0;
  m_zigleg_duration = 0;
  m_zigleg_time = 0;
  m_zigleg_heading = 0;
  m_zigleg_sent = false;
  m_objective_function = false;

}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_GradTrack::setParam(string param, string val)
{
  // Convert the parameter to lower case for more general matching
  param = tolower(param);

  // Get the numerical value of the param argument for convenience once
  double double_val = atof(val.c_str());
  
  if((param == "pulse_range") && isNumber(val)) {
    if(double_val >= 0){
      m_pulse_range = double_val;
    }
    else {
      postWMessage("Pulse range is negative. Setting it to zero");
      m_pulse_range = 0;
    }
    return(true);
  }
  else if (param == "pulse_duration") {
    if(double_val >= 0){
      m_pulse_duration = double_val;
    }
    else {
      postWMessage("Pulse duration is negative. Setting it to zero");
      m_pulse_duration = 0;
    }
    return(true);
  } 
  else if (param == "zigleg_offset") {
    m_zigleg_offset = double_val;
    return(true);
  }
  else if (param == "zigleg_duration") {
    if(double_val >= 0){
      m_zigleg_duration = double_val;
    }
    else {
      postWMessage("Zigleg duration is negative. Setting it to zero");
      m_zigleg_duration = 0;
    }
    return(true);
  } 
  else if (param == "pwt") {
    if(double_val >= 0){
      m_priority_wt = double_val;
    }
    else {
      postWMessage("Priority weight (pwt) is negative. Setting it to 1");
      m_priority_wt = 1;
    }
    return(true);
  }

  // If not handled above, then just return false;
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_GradTrack::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_GradTrack::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_GradTrack::onIdleState()
{
  // TODO: Not do anything, right?
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_GradTrack::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_GradTrack::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_GradTrack::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_GradTrack::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_GradTrack::onRunState()
{
  // Build the IvP functions that represents no objective function and an objective function defined through ZAIC. See buildFunctionWithZAIC()
  IvPFunction *ipf = 0;
  if(ipf)
    ipf->setPWT(m_priority_wt);

  IvPFunction *ivpf = 0;
  ivpf = buildFunctionWithZAIC();
  if(ivpf == 0) 
    postWMessage("Problem Creating the IvP Function");
  if(ivpf)
    ivpf->setPWT(m_priority_wt);

  // Vehicle params from InfoBuffer. Post warning if problem is encountered
  bool oknx,okny,oknh,okwpti,oktemp,oktempavg,okgrad,okcold,okstart;
  m_nav_x = getBufferDoubleVal("NAV_X", oknx);
  m_nav_y = getBufferDoubleVal("NAV_Y", okny);
  m_nav_h = getBufferDoubleVal("NAV_HEADING", oknh);
  m_global_temp_avg = getBufferDoubleVal("AVERAGE_TEMP",oktempavg);
  m_global_front_gradient = getBufferDoubleVal("FRONT_GRADIENT",okgrad);
  double cold_direction = getBufferDoubleVal("COLD_DIR",okcold);
  string measurement = getBufferStringVal("UCTD_MSMNT_REPORT", oktemp);

  if(okcold){
    m_cold_direction = cold_direction; // 1 if cold is north, -1 if south
  }

  if(oktemp){
    double temp = measurementToTemp(measurement);

    m_last_temps.insert(m_last_temps.begin(),temp); // also returns an iterator to the new 'begin'

    // TODO: only call this function if we have a certain amount of measurements, since there is no average with to few measurements. Maybe start by adding the max and min temp found from another behaviour?
    followGradient();
  }

  if(!oknx || !okny || !oknh) {
    postWMessage("No ownship X/Y/Heading info in info_buffer.");
    return(0);
  }

  /* // Examples on updating based on curr time and last time to hit info buff
  double timeSinceWptUpdate = getBufferTimeVal("FOO"); // Always 0 or Appstick
  double timeSinceZigLeg    = getBufferCurrTime() - m_zigleg_time;
  */

  // Decide which objective function to returned based on decision parameter
  // Keep this parameter to be able to use zigleg behaviour in case we get stuck on a wave
  if(m_objective_function){
    return(ivpf);
  }

  return(ipf);
}

// Based on the first part of lab. Kept for later reference
void BHV_GradTrack::postRangePulse(){
  XYRangePulse pulse;
  pulse.set_x(m_nav_x);
  pulse.set_y(m_nav_y);
  pulse.set_label("BHV_ZigLeg_Pulse");
  pulse.set_rad(m_pulse_range);
  pulse.set_duration(m_pulse_duration);
  pulse.set_time(m_pulse_time);
  pulse.set_color("edge", "yellow");
  pulse.set_color("fill", "yellow");

  string spec = pulse.get_spec();
  postMessage("VIEW_RANGE_PULSE", spec);
}


double BHV_GradTrack::measurementToTemp(string m){
  // String will be like:
  // "vname=v_name,utc=XXX.0,x=123.2,y=412.2,temp=22.34"
  // TODO: Test to see if the four former parses is necessary
  string a = tokStringParse(m, "vname", ',', '=');
  string b = tokStringParse(m, "utc", ',', '=');
  string c = tokStringParse(m, "x", ',', '=');
  string d = tokStringParse(m, "y", ',', '=');
  string t = tokStringParse(m, "temp", ',', '=');
  
  // return temperature as a double
  return(std::stod(t));// stod is from the string library
}

void BHV_GradTrack::followGradient(){
  // Idea: follow gradient of the front by always adjusting by measured temp
  
  // TODO:To avoid getting stuck on riding the wave, the speed can be changed randomly, or one could measure the time since last direction change, and make the vessel to a change in heading

  double turn = 1; // set to zero if no turn 
  int turn_direction = -1; // the same as cold north

  // We are using the global temp average
  if(m_last_temps.size() > 0){
    if(m_last_temps[0] > m_global_temp_avg + m_temp_threshold){
      // turn towards colder. if vehicle sails towards east, it means decreasing heading
      if(m_nav_h >= 0 && m_nav_h <= 180){
        // we are sailing east, so we want to reduce angle. if cold is north, m_cold_direction should be -1. If not, the we turn
        turn_direction = m_cold_direction;
      }
      else{
        turn_direction = -m_cold_direction;
      }
    }
    else if (m_last_temps[0] < m_global_temp_avg - m_temp_threshold) {
      // turn toward warmer
      if(m_nav_h >= 0 && m_nav_h <= 180){
        // we are sailing east. if cold is north, we want to turn the opposite way of m_cold_direction. E.g. if cold is north, then m_cold_direction is -1, and we want to increase angle -> -(m_cold_direction) 
        turn_direction = -m_cold_direction;
      }
      else{
        turn_direction = m_cold_direction;
      }
    } 
    else{
      turn = 0;
      // TODO: Maybe just return here so that CONST_HDG_UPDATES don't get spammed to much at each onRunState? 
    } 
    double change = turn * turn_direction * m_turn_rate;
    postTurnDir(change);
    postMessage("CONST_HDG_UPDATES", "heading=" + to_string(m_nav_h+change));
  } // more than 0 measurements  
} // func

// takes in a change in heading direction and posts a variable that indicates if the vehicle should turn right, left or none
void BHV_GradTrack::postTurnDir(double change){
  
  string outstring;
  if(change == 0){
    outstring = "TURN_NONE";
  }
  else if(change < 0){
    outstring = "TURN_LEFT";
  }
  else{
    outstring = "TURN_RIGHT";
  }
  postMessage("TEST_TURN", outstring);
}

//-----------------------------------------------------------
// Procedure: updateTempAvg()
void BHV_GradTrack::updateTempAvg(){
  double noOfTemps = m_last_temps.size();
  if(noOfTemps == 0){
    postWMessage("updateTempAvg(): No average can be found from empty list");
    return;
  }
  else {
    double tempSum = 0;
    for(vector<double>::iterator it = m_last_temps.begin(); it != m_last_temps.end(); it++){
      tempSum += (*it);
    }
    m_measured_temp_avg = tempSum / (noOfTemps);
  }
}

//-----------------------------------------------------------
// Procedure: buildFunctionWithZAIC
// Purpose:   Builds an objective function to be used for sailing on a certain
//            heading away from a designated heading from Waypoint behaviour
// Params:    no input
// Edits:     no edits
// Returns:   an IvPFunction, used in OnRunState()
IvPFunction *BHV_GradTrack::buildFunctionWithZAIC() 
{
  double offset = m_zigleg_heading + m_zigleg_offset;
  ZAIC_PEAK crs_zaic(m_domain, "course");
  crs_zaic.setSummit(offset);
  crs_zaic.setPeakWidth(0);
  crs_zaic.setBaseWidth(180.0);
  crs_zaic.setSummitDelta(0);  
  crs_zaic.setValueWrap(true);
  if(crs_zaic.stateOK() == false) {
    string warnings = "Course ZAIC problems " + crs_zaic.getWarnings();
    postWMessage(warnings);
    return(0);
  }

  IvPFunction *crs_ipf = crs_zaic.extractIvPFunction();
  return(crs_ipf);
}