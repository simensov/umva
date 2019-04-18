
// SIMEN SEM OEVERENG

#include "Classification.h"
using namespace std;

string Classification::printClassification(){
  return(to_string(m_label) + "," + to_string(m_hazard) + "," + to_string(m_prob));
}