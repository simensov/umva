/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PrimeFactor.h" // Vector included

using namespace std;

const bool debug = false;

//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
{
  m_first_reading       = true;
  m_current_prime       = 0;
  m_prime_factors       = {};
  m_max_iterations      = 10000;
  m_prime_name          = "NUM_RESULT";
}

//---------------------------------------------------------
// Destructor

PrimeFactor::~PrimeFactor()
{

}

//---------------------------------------------------------
// Procedure: OnNewMail

bool PrimeFactor::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key = msg.GetKey();        // Gets variable name

    // Checks explicitly for variable name we are interested in
    if (key == "NUM_VALUE"){

      string inString = msg.GetString();      // get value as string
      m_current_prime = std::stoll(inString); // convert to long long

      PrimeEntry entry;
      entry.setOriginalVal(m_current_prime);
      m_all_entries.push_back(entry);
      cout << "Adding entry in NewMail" << endl;

      m_first_reading = false;  // changes to false as soon we read a mail
    }

   } // for
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool PrimeFactor::OnConnectToServer()
{ 
  RegisterVariables();
  return(true);
}

string PrimeFactor::getPrimesAsString() const {

  // To view result, notify the list as string of all elements
  string stringList = "primes=";
  list<uint64_t>::const_iterator it;

  for(it = m_prime_factors.begin(); it != m_prime_factors.end(); ++it){
    
    if (it == m_prime_factors.begin())
      stringList =  stringList + to_string(*it);
    else
      stringList = stringList + ":" + to_string(*it);
  }
  
  return stringList;
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PrimeFactor::Iterate()
{
  // If m_all_entries is not empty -> pick front one and calculate
  // .front() returns reference, so we can change its values permanently

  PrimeEntry frontEntry;
  bool complete = false;
  uint64_t current_iterations = 0;

  while(!m_all_entries.empty()){
    
    frontEntry = m_all_entries.front();

    if(debug)
      cout << "In while: " << frontEntry.getCurrentInt() << endl;

    // stops by itself if it doesn't reach all factors
    complete = frontEntry.factor(m_max_iterations); // - current_iterations);

    // if frontEntry finished with iterations to spare, pop and continue
    if(complete){

      cout << frontEntry.getOriginalPrime() << " finished" << endl;

      // notify and update iterations used
      Notify(m_prime_name,frontEntry.getReport());

      // pop from list
      m_all_entries.pop_front();
    
    } else {
      // if frontEntry did not finish within max iterations, push back and hope for better days. Pop front to let next entries be calculated on
      
      cout << frontEntry.getOriginalPrime() << " is on index: " << frontEntry.getCurrentIndex() << endl;

      m_all_entries.push_back(frontEntry);
      m_all_entries.pop_front();

    } // else
  } // while

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PrimeFactor::OnStartUp()
{
  // generated info kept for later reference:
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;
    
    }
  }
  
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void PrimeFactor::RegisterVariables()
{
  // Explicitly register for the MOOS-variable we want
  Register("NUM_VALUE", 0); 
  Register("NUM_RESULT", 0); 
}


//---------------------------------------------------------
// Procedure: setPrimeFactors
// Purpose:   sends all prime factors of a prime number to local variable
// @ params   integer to test
// @ edits    m_prime_factors
// @ return   nothing
void PrimeFactor::setPrimeFactors(uint64_t &integer){

    // Print the number of 2s that divide n 
    while (integer%2 == 0) 
    { 
        m_prime_factors.push_back(2); 
        integer = integer/2; 
    } 
  
    // n must be odd at this point.  So we can skip  
    // one element (Note i = i +2) 
    for (int i = 3; i <= sqrt(integer); i = i+2) 
    { 
        // While i divides n, print i and divide n 
        while (integer%i == 0) 
        { 
            m_prime_factors.push_back(i); 
            integer = integer/i; 
        } 
    } 
  
    // This condition is to handle the case when n  
    // is a prime number greater than 2 
    if (integer > 2) 
        m_prime_factors.push_back(integer); 
} 