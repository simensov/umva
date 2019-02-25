/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.h                                          */
/*    DATE: Feb 25 2019                                               */
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

   // These were implemented early in Lab5, and their respective versions has been made in PrimeEntry.h which is processed by the m_all_entries list
   void setPrimeFactors(uint64_t &integer);
   double calcSolvingTime(PrimeEntry &entry) const;
   std::string getGlobalReport(PrimeEntry &entry) const;
   void setCurrentPrime(uint64_t &integer){m_current_prime = integer;};


 protected:
   void RegisterVariables();

   bool                 m_first_reading;
   uint64_t             m_current_prime;
   std::list<uint64_t>  m_prime_factors;
   std::string          m_prime_name;
   uint64_t             m_max_iterations;

   uint64_t             m_solved_primes;
   uint64_t             m_received_primes;

   std::list<PrimeEntry> m_all_entries;

};

#endif 
