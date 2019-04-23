/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_ZigLeg.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_ZigLeg.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

#include "XYRangePulse.h" // for XYRangePulse 

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_ZigLeg::BHV_ZigLeg(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "ZigLeg");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");


  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y");
  addInfoVars("FOO", "no_warning");
  addInfoVars("NAV_HEADING");

  m_priority_wt = 0;

  // TODO: Initialize all member variables
  m_nav_x = 0;
  m_nav_y = 0;
  m_nav_h = 0;

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

bool BHV_ZigLeg::setParam(string param, string val)
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

void BHV_ZigLeg::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_ZigLeg::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_ZigLeg::onIdleState()
{
  // TODO: Not do anything, right?
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_ZigLeg::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_ZigLeg::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_ZigLeg::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_ZigLeg::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_ZigLeg::onRunState()
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
  bool ok1, ok2, ok3, ok4;
  m_nav_x = getBufferDoubleVal("NAV_X", ok1);
  m_nav_y = getBufferDoubleVal("NAV_Y", ok2);
  m_nav_h = getBufferDoubleVal("NAV_HEADING", ok3);

  // Only care about the timing, not the value. This line is used for debugging
  m_waypoint_index = getBufferDoubleVal("FOO", ok4); 

  if(!ok1 || !ok2 || !ok3) {
    postWMessage("No ownship X/Y/Heading info in info_buffer.");
    return(0);
  }

  // Find time since last waypt has been UPDATED, since last zigleg has been posted, and since last waypt has been ENCOUNTERED
  double timeSinceWptUpdate = getBufferTimeVal("FOO"); // Always 0 or Appstick
  double timeSinceZigLeg    = getBufferCurrTime() - m_zigleg_time;
  double timeSinceLastWpt   = getBufferCurrTime() - m_waypoint_time;

  // If we have just hit a waypt, FOO has been posted. Time is stored, and variables updated according to next moves
  if(timeSinceWptUpdate == 0){
    m_waypoint_time       = getBufferCurrTime();
    m_pulse_sent          = false;
    m_zigleg_sent         = false;
    m_objective_function  = false;
  }
  else if( m_zigleg_sent && (timeSinceZigLeg > m_zigleg_duration) ){
    // We have performed a zigleg m_zigleg_duration seconds ago.
    m_objective_function  = false; // Turn off objective function
  }
  else if( timeSinceLastWpt > 5 && !m_zigleg_sent && m_waypoint_time != -0.01){
    // Waypt was crossed 5 seconds ago, and zigleg is not yet sent and
    // we have changed the waypoint time from the initialized value 
    // Store times and heading, and update decision parameters.
    m_pulse_time          = getBufferCurrTime() + 0.1;
    m_zigleg_time         = getBufferCurrTime() + 0.1;
    m_zigleg_heading      = m_nav_h;
    m_zigleg_sent         = true;
    m_objective_function  = true;
  }

  // Decide which objective function to returned based on decision parameter
  if(m_objective_function){
    return(ivpf);
  }

  return(ipf);
}

// Based on the first part of lab. Kept for later reference
void BHV_ZigLeg::postRangePulse(){
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

//-----------------------------------------------------------
// Procedure: buildFunctionWithZAIC
// Purpose:   Builds an objective function to be used for sailing on a certain
//            heading away from a designated heading from Waypoint behaviour
// Params:    no input
// Edits:     no edits
// Returns:   an IvPFunction, used in OnRunState()
IvPFunction *BHV_ZigLeg::buildFunctionWithZAIC() 
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