/************************************************************/
/*    NAME: Simen Sem Oevereng                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.h                                   */
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


#ifndef PrimeFactor_HEADER
#define PrimeFactor_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <iostream>
#include <cstdint>
#include <list>
#include "PrimeEntry.h"

class PrimeFactor : public CMOOSApp
{
 public:
   PrimeFactor();
   ~PrimeFactor(){};

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

   void setPrimeFactors(uint64_t &integer);
   double calcSolvingTime(PrimeEntry &entry) const;
   std::string getGlobalReport(PrimeEntry &entry) const;
   void setCurrentPrime(uint64_t &integer)  {m_current_prime = integer;};


 protected:
   void RegisterVariables();

   bool                 m_first_reading;  // registers first mail
   uint64_t             m_current_prime;  // current num to calculate primes 
   std::list<uint64_t>  m_prime_factors;  // hold
   std::string          m_prime_name;     // name of MOOS variable to publish
   uint64_t             m_max_iterations; // max iterations per Iterate-loop
   uint64_t             m_solved_primes;  // number of solved primes
   uint64_t             m_received_primes;// number of received primes
   std::list<PrimeEntry> m_all_entries;   // list of all current numbers 

};

#endif 
