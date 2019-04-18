/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: BHV_Pulse.h                                      */
/*    DATE:                                                 */
/************************************************************/

#ifndef Pulse_HEADER
#define Pulse_HEADER

#include <string>
#include "IvPBehavior.h"

class BHV_Pulse : public IvPBehavior {
public:
  BHV_Pulse(IvPDomain);
  ~BHV_Pulse() {};

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

protected: // Configuration parameters

protected: // State variables
  double m_nav_x;
  double m_nav_y;

  double m_current_time;
  double m_waypoint_time;
  int    m_waypoint_index;

  double m_pulse_x;
  double m_pulse_y;
  double m_pulse_range;
  double m_pulse_duration;
  double m_pulse_time;
  bool m_pulse_sent;

};

#define IVP_EXPORT_FUNCTION

extern "C" {
  IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
  {return new BHV_Pulse(domain);}
}
#endif
