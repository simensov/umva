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
  addInfoVars("START_POINT","no_warning");
  addInfoVars("FRONT_GRADIENT","no_warning");
  addInfoVars("UCTD_MSMNT_REPORT","no_warning");
  addInfoVars("AVERAGE_TEMP","no_warning");

  m_priority_wt = 0;

  // TODO: Initialize all member variables
  m_nav_x = 0;
  m_nav_y = 0;
  m_nav_h = 0;

  m_temp_threshold = 0.2; // TODO: Check this size from runs
  m_measured_temp_avg = 0.0;
  m_global_temp_avg = 0.0;
  m_global_front_gradient = 0.0; // assumed to be angle in radians, moving in in x,y coordsyst

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
  bool oknx, okny, oknh, okwpti, oktemp, oktempavg, okgrad;
  m_nav_x = getBufferDoubleVal("NAV_X", oknx);
  m_nav_y = getBufferDoubleVal("NAV_Y", okny);
  m_nav_h = getBufferDoubleVal("NAV_HEADING", oknh);
  m_global_temp_avg = getBufferDoubleVal("AVERAGE_TEMP",oktempavg);
  m_global_front_gradient = getBufferDoubleVal("FRONT_GRADIENT",okgrad);
  string measurement = getBufferStringVal("UCTD_MSMNT_REPORT", oktemp);
  double temp = measurementToTemp(measurement);

  m_last_temps.insert(m_last_temps.begin(),temp); // also returns an iterator to the new 'begin'

  // TODO: only call this function if we have a certain amount of measurements, since there is no average with to few measurements. Maybe start by adding the max and min temp found from another behaviour?
  followGradient();

  // Only care about the timing, not the value. This line is used for debugging
  m_waypoint_index = getBufferDoubleVal("FOO", okwpti); 

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
  return(stod(t));// stod is from the string library
}

void BHV_GradTrack::followGradient(){

  // Idea: follow gradient of the front. 
  // To avoid getting stuck on riding the wave, the speed can be changed randomly, or one could measure the time since last direction change, and make the vessel to a change in heading

  bool useLocalTempAvg = false;
  if(useLocalTempAvg){
    updateTempAvg(); // curr just updating local average temp, not used here
    double tempDiff = 0;

    for(int i = 0; i < 2; i++){
      // m_last_temps has most recent measurments as the FRONT element 
      tempDiff = m_last_temps[i] - tempDiff;
    }

    // tempDiff now contains (tempPrevious - tempNow)
    tempDiff = -tempDiff;
    if(tempDiff > m_temp_threshold){
      // We have moved to a warmer area: increment heading northward (or relative to the gradient!!)

      // TODO: post to a const speed behaviour?
    }
    else if (tempDiff < -m_temp_threshold){
      // We have moved to a cooler area
    }
    else{
      // Keep same heading
    }

  } 
  else {
    // We are using the 
    if(m_last_temps[0] > m_global_temp_avg + m_temp_threshold){
      // turn towards colder
      postMessage("HEADING_ADJUST", m_nav_h - 2);
    }
    else if (m_last_temps[0] < m_global_temp_avg - m_temp_threshold) {
      // turn toward warmer
      postMessage("HEADING_ADJUST", m_nav_h + 2);
    } 
    else{
      postMessage("HEADING_ADJUST", m_nav_h);
    }// if elseif else global temp avg
    
  } // else localtempavg
} // func


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