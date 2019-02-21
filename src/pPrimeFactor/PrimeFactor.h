/************************************************************/
/*    NAME: Simen Sem Oevereng                                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeFactor.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef PrimeFactor_HEADER
#define PrimeFactor_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include <cstdint>

class PrimeFactor : public CMOOSApp
{
 public:
   PrimeFactor();
   ~PrimeFactor();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

   bool isPrime(uint64_t &integer);
   std::list<uint64_t> primeCalculator(uint64_t &integer);

 protected:
   void RegisterVariables();

   bool m_first_reading;
   uint64_t m_previous_prime;
   uint64_t m_current_prime;
   std::list<uint64_t> m_primes;
   std::string m_prime_name;

};

#endif 
