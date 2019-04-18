/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_Pulse.cpp                                    */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include <cstdlib>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "BHV_Pulse.h"

#include "XYRangePulse.h" // for XYRangePulse 

using namespace std;

//---------------------------------------------------------------
// Constructor

BHV_Pulse::BHV_Pulse(IvPDomain domain) :
  IvPBehavior(domain)
{
  // Provide a default behavior name
  IvPBehavior::setParam("name", "defaultname");

  // Declare the behavior decision space
  m_domain = subDomain(m_domain, "course,speed");


  // Add any variables this behavior needs to subscribe for
  addInfoVars("NAV_X, NAV_Y");
  addInfoVars("FOO", "no_warning");


  // TODO: Initialize all member variables
  m_nav_x = 0;
  m_nav_y = 0;

  m_current_time = 0;
  m_waypoint_time = 0;
  m_waypoint_index = -1; // initialized to control for first index update in onRunState()

  m_pulse_x = 0;
  m_pulse_y = 0;
  m_pulse_range = 0;
  m_pulse_duration = 0;
  m_pulse_time = 0;
  m_pulse_sent = true;

}

//---------------------------------------------------------------
// Procedure: setParam()

bool BHV_Pulse::setParam(string param, string val)
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

  // If not handled above, then just return false;
  return(false);
}

//---------------------------------------------------------------
// Procedure: onSetParamComplete()
//   Purpose: Invoked once after all parameters have been handled.
//            Good place to ensure all required params have are set.
//            Or any inter-param relationships like a<b.

void BHV_Pulse::onSetParamComplete()
{
}

//---------------------------------------------------------------
// Procedure: onHelmStart()
//   Purpose: Invoked once upon helm start, even if this behavior
//            is a template and not spawned at startup

void BHV_Pulse::onHelmStart()
{
}

//---------------------------------------------------------------
// Procedure: onIdleState()
//   Purpose: Invoked on each helm iteration if conditions not met.

void BHV_Pulse::onIdleState()
{
  // TODO: Not do anything, right?
}

//---------------------------------------------------------------
// Procedure: onCompleteState()

void BHV_Pulse::onCompleteState()
{
}

//---------------------------------------------------------------
// Procedure: postConfigStatus()
//   Purpose: Invoked each time a param is dynamically changed

void BHV_Pulse::postConfigStatus()
{
}

//---------------------------------------------------------------
// Procedure: onIdleToRunState()
//   Purpose: Invoked once upon each transition from idle to run state

void BHV_Pulse::onIdleToRunState()
{
}

//---------------------------------------------------------------
// Procedure: onRunToIdleState()
//   Purpose: Invoked once upon each transition from run to idle state

void BHV_Pulse::onRunToIdleState()
{
}

//---------------------------------------------------------------
// Procedure: onRunState()
//   Purpose: Invoked each iteration when run conditions have been met.

IvPFunction* BHV_Pulse::onRunState()
{

  // Part 1: Build the IvP function
  IvPFunction *ipf = 0;

  // Get vehicle position from InfoBuffer and post a 
  // warning if problem is encountered
  bool ok1, ok2, ok3;
  m_nav_x = getBufferDoubleVal("NAV_X", ok1);
  m_nav_y = getBufferDoubleVal("NAV_Y", ok2);

  double timeSinceUpdate = getBufferTimeVal("FOO"); // Always 0 or Appstick
  // Only care about the timing of this, not the value
  // next two lines used for debugging
  m_waypoint_index = getBufferDoubleVal("FOO", ok3); 
  // postMessage("TIME_SINCE_WPT_UPDATE",timeSinceUpdate);

  if(timeSinceUpdate == 0){
    m_waypoint_time = getBufferCurrTime();
    m_pulse_sent = false;
  }
  else if ( (getBufferCurrTime() - m_waypoint_time) > 5 && !m_pulse_sent){
    m_pulse_time = getBufferCurrTime() + 0.1;
    postRangePulse();
    m_pulse_sent = true;
  }

  if(!ok1 || !ok2) {
    postWMessage("No ownship X/Y info in info_buffer.");
    return(0);
  }
  else if (!ok3){
    // postWMessage("No WPT_INDEX info in info_buffer.");
    // return(0);
  }

  // Part N: Prior to returning the IvP function, apply the priority wt
  // Actual weight applied may be some value different than the configured
  // m_priority_wt, depending on the behavior author's insite.
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

void BHV_Pulse::postRangePulse(){
  
  XYRangePulse pulse;
  pulse.set_x(m_nav_x);
  pulse.set_y(m_nav_y);
  pulse.set_label("bhv_pulse");
  pulse.set_rad(m_pulse_range);
  pulse.set_duration(m_pulse_duration);
  pulse.set_time(m_pulse_time);
  pulse.set_color("edge", "yellow");
  pulse.set_color("fill", "yellow");

  string spec = pulse.get_spec();
  postMessage("VIEW_RANGE_PULSE", spec);
}