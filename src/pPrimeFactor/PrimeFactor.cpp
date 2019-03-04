/************************************************************/
/*    NAME: Simen Sem Oevereng                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.cpp                                 */
/*    DATE: Feb 25 2019                                     */
/*    EDIT: none yet  

      Header file copied from:
      http://oceanai.mit.edu/ivpman/pmwiki/pmwiki.php?n=Lab.ClassPrimeFactors, chapter 4.2.
      Implemented in PrimeEntry.cpp by me.

      This file contains the implementation of lab 5, in 
      which the goal was to learn how to write a MOOSApp  
      that calculated all the primes of different numbers
      it recieved during a short amount of time while not
      blocking its own calculations during the 0.25 sek of 
      running (the AppsTick was set to 4).                  */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PrimeFactor.h" // list included

using namespace std;

//---------------------------------------------------------
// Constructor

PrimeFactor::PrimeFactor()
{
  m_first_reading       = true;
  m_current_prime       = 0;
  // m_prime_factors       = {};
  m_max_iterations      = 1000000;  // 2 000 000 gives 5 sek total solving time
  m_prime_name          = "PRIME_RESULT";
  m_solved_primes       = 0;
  m_received_primes     = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail
// PURPOSE:   react to subscribed mails sent from (preferably) another MOOSApp 
// @param     NewMail: a MOOSMSG_LIST type variable,
//            containing all information published from other MOOSApps
// @edits     m_current_prime, m_received_primes, m_all_entries,
//            m_first_reading
// @return    true
bool PrimeFactor::OnNewMail(MOOSMSG_LIST &NewMail)
{
  // Gets variable name and check for variable name we are interested in

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    
    string key = msg.GetKey();  
    
    if (key == "NUM_VALUE"){
      // get value as string from msg, and convert to long long
      // increase number of recieved numbers
      // add the number to list by creating entry object and push_back 
      // change first_reading to false     
      
      m_current_prime = std::stoll(msg.GetString()); 
      
      m_received_primes++;

      PrimeEntry entry(m_current_prime,m_received_primes,MOOSTime());;
      
      m_all_entries.push_back(entry);
      
      m_first_reading = false; 
    }

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
// Procedure: calcSolvingTime
// Purpose:   calculate the time in seconds of how long it took to find all
//            the primes of the original number of a PrimeEntry object
// @ params   entry: a PrimeEntry object
// @ edits    no edits
// @ return   the time between the call and the start of the entry's solving
double PrimeFactor::calcSolvingTime(PrimeEntry &entry) const {
  return(MOOSTime() - entry.getEntryTime());
}

//---------------------------------------------------------
// Procedure: getGlobalReport
// Purpose:   produce the specified output format for Lab 5
// @ params   entry: a PrimeEntry object holding all information
// @ edits    no edits
// @ return   a string with specified information from Lab 5
string PrimeFactor::getGlobalReport(PrimeEntry &entry) const {

  // To view result, notify the list as string of all elements
  string stringList = "";
  stringList += "orig="       + to_string(entry.getOriginalPrime()) + ",";
  stringList += "received="   + to_string(entry.getEntryID())       + ",";
  stringList += "calculated=" + to_string(entry.getSolveID())       + ",";
  stringList += "solve_time=" + to_string(calcSolvingTime(entry))   + ",";
  stringList += "primes="     + entry.getPrimes() + ",";

  return(stringList + "username=simensov");
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second
// PURPOSE:   contains the main logic of the MOOSApp
// @param     no inputs
// @edits     m_total_distance
// @returns   true
bool PrimeFactor::Iterate()
{

  // initialize parameters used for logic 
  PrimeEntry frontEntry;
  bool complete = false;
  uint64_t current_iterations = 0;

  // if m_all_entries is not empty: pick front one and do calculation

  while(!m_all_entries.empty()){
    // select first entry in list, and try to calculate factors. 
    // if completed, Notify and store the iterations, then try another entry
    // if not completed, move that entry in the back of the list
    
    frontEntry = m_all_entries.front();

    // set entry time from the very first factorization attempt
    if(!frontEntry.getInitiated()){
      frontEntry.setID(frontEntry.getEntryID(),MOOSTime());
      frontEntry.setInitiated(true);
    }

    complete = frontEntry.factor(m_max_iterations - current_iterations);

    if(complete){

      m_solved_primes++;

      frontEntry.setSolveID(m_solved_primes);

      Notify(m_prime_name,getGlobalReport(frontEntry));

      current_iterations += frontEntry.getIterations();

      m_all_entries.pop_front();
    
    } else {
      // not completed: we have reached max iterations. 
      // move entry to the back of the list and wait on next Tick

      m_all_entries.push_back(frontEntry);
      m_all_entries.pop_front();

      return(true);
    }
  
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open
//            Only used for registering for variables in this app
bool PrimeFactor::OnStartUp()
{
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables
// Purpose:   explicitly register for the MOOS-variable we want
// @ params   no input
// @ edits    no edit
// @ return   nothing
void PrimeFactor::RegisterVariables()
{
  Register("NUM_VALUE", 0); 
}

//---------------------------------------------------------
// Procedure: setPrimeFactors
// Purpose:   sends all prime factors of a prime number to local variable
//            this is used if PrimeFactor should calculate one number by
//            itself instead of using the list PrimeEntry objects.
// @ params   integer to test
// @ edits    m_prime_factors
// @ return   nothing
void PrimeFactor::setPrimeFactors(uint64_t &integer){

    // push the number of 2s that divide the integer 
    while (integer%2 == 0) 
    { 
        m_prime_factors.push_back(2); 
        integer = integer/2; 
    } 
  
    // n must be odd at this point, so we can skip one element each time 
    for (int i = 3; i <= sqrt(integer); i = i+2) 
    { 
        // While i divides n, push i and divide integer
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
