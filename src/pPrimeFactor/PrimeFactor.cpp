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
  m_primes              = {};
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

      m_primes.push_front(m_current_prime);

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

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool PrimeFactor::Iterate()
{
  // Test for prime number
  if (!m_first_reading){

    while(m_primes.size()){

      uint64_t last_element = m_primes.back();

      // Test for prime number
      if (last_element % 2){
        Notify("NUM_RESULT",to_string(last_element) + ",odd");
      } 
      else{
        Notify("NUM_RESULT",to_string(last_element) + ",even");
      }

      // remove element to reduce list size
      m_primes.pop_back();



    } // while


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
// Procedure: isPrime
// Purpose:   checks if an integer is a prime number or not
// @ params   integer to test
// @ return   true or false
bool PrimeFactor::isPrime(uint64_t &integer){
  return(true);
}

//---------------------------------------------------------
// Procedure: primeCalculator
// Purpose:   takes an integer, tests if it is a prime, and returns all the primes the integer consists of
// @ params   integer to test
// @ return   a list of primes of the input integer
std::list<uint64_t> PrimeFactor::primeCalculator(uint64_t &integer){

  if (!isPrime(integer)){
    std::list<uint64_t> primeList = {};

    /*
        TODODTODTODTODTO
    */
    // Do prime calculations here (modulo?) etc


    // To view result, notify the list as string of all elements
    string stringList = "";
    list<uint64_t>::iterator it;
    for(it = primeList.begin(); it != primeList.end(); ++it){
      stringList = stringList + to_string(*it) + ", ";
    }

    Notify("NUM_LIST",stringList);


  }


}