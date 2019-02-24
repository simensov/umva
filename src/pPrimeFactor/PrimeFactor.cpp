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

//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
{
  m_first_reading       = true;
  m_previous_prime      = 0;
  m_current_prime       = 0;
  m_prime_factors       = {};
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

      // m_prime_factors = setPrimeFactors(m_current_prime);

      // m_current_prime = msg.GetDouble(); // Used for the initial implementation when an int was posted, not a string

      m_first_reading = false;  // changes to false as soon we read a mail
    }

    // Intermediate action:

#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   }
	
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
  // Test for prime number
  if (!m_first_reading) {

    setPrimeFactors(m_current_prime);

    Notify(m_prime_name, getPrimesAsString() );


  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool PrimeFactor::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string line  = *p;
      string param = tolower(biteStringX(line, '='));
      string value = line;

      // Do something

    
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


