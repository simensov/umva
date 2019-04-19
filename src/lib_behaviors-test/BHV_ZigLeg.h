/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_ZigLeg.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef ZigLeg_HEADER
#define ZigLeg_HEADER

#include <string>
#include "IvPBehavior.h"

class BHV_ZigLeg : public IvPBehavior {
public:
  BHV_ZigLeg(IvPDomain);
  ~BHV_ZigLeg() {};

  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onCompleteState();
  void         onIdleState();
  void         onHelmStart();
  void         postConfigStatus();
  void         onRunToIdleState();
  void         onIdleToRunState();
  IvPFunction* onRunState();

protected: // Local Utility functions
  void postRangePulse();
  IvPFunction* buildFunctionWithZAIC();

protected: // Configuration parameters

protected: // State variables
  // Vessel x,y,heading
  double m_nav_x;
  double m_nav_y;
  double m_nav_h;

  // Time since last waypt hit and the next waypt's index
  double m_waypoint_time;
  int    m_waypoint_index;

  // Pulse parameters, stored from BHV_Pulse
  double m_pulse_x;
  double m_pulse_y;
  double m_pulse_range;
  double m_pulse_duration;
  double m_pulse_time;
  bool m_pulse_sent;

  // Zigleg specific parameters
  double m_zigleg_offset;   // Heading offset for the objective function
  double m_zigleg_duration; // Duration of zigleg
  double m_zigleg_time;     // Time since last zigleg was started
  double m_zigleg_heading;  // Heading when last zigleg was started
  bool m_zigleg_sent;       // Is the zigleg supposed to be sent?
  bool m_objective_function;// Is the zigleg governed by a non-zero obj func?
};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_ZigLeg(domain);}
}
#endif
