/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.cpp                                        */
/*    DATE: Feb 25 2019                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PrimeFactor.h" // Vector included

using namespace std;

const bool debug = true;

//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
{
  m_first_reading       = true;
  m_current_prime       = 0;
  m_prime_factors       = {};
  m_max_iterations      = 500000;
  m_prime_name          = "PRIME_RESULT";
  m_solved_primes       = 0;
  m_received_primes     = 0;
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

      // get value as string from msg, and convert to long long
      m_current_prime = std::stoll(msg.GetString()); 

      m_received_primes++;

      // add the number to our list by creating entry object
      PrimeEntry entry(m_current_prime,m_received_primes,MOOSTime());;

      m_all_entries.push_back(entry);

      if(debug)
        cout << "Adding "<< entry.getOriginalPrime() << " in NewMail" << endl;

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

double PrimeFactor::calcSolvingTime(PrimeEntry &entry) const {
  return (MOOSTime() - entry.getEntryTime());
}


string PrimeFactor::getGlobalReport(PrimeEntry &entry) const {

  // To view result, notify the list as string of all elements
  string stringList = "orig=" + to_string(entry.getOriginalPrime()) + ",";
  stringList += "received=" + to_string(entry.getEntryID()) + ",";
  stringList += "calculated=" + to_string(entry.getSolveID()) + ",";
  stringList += "solve_time=" + to_string(calcSolvingTime(entry)) + ",";
  stringList += "primes=" + entry.getPrimes() + ",";

  return stringList + "username=simensov";
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
    // Select first entry in list, and try to calculate factors. 
    // If completed, Notify and Store the iterations -> try another entry
    // If not completed, move that entry in the back of the list
    
    frontEntry = m_all_entries.front();

    complete = frontEntry.factor(m_max_iterations - current_iterations);

    if(complete){

      m_solved_primes++;
      frontEntry.setSolveID(m_solved_primes);

      if(debug)
        cout << frontEntry.getOriginalPrime() << " finished" << endl;

      Notify(m_prime_name,getGlobalReport(frontEntry));

      current_iterations += frontEntry.getIterations();

      m_all_entries.pop_front();
    
    } else {
      // not completed: we have reached max iterations. 
      // move entry to the back of the list and wait on next Tick

      m_all_entries.push_back(frontEntry);
      m_all_entries.pop_front();

      return(true);

    } // else
  } // while

  if(debug)
    cout << "List is empty" << endl;

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
