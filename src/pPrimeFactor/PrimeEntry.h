// File: PrimeEntry.h
// Simen Sem Oevereng, 24. feb 2019
// Header file copied from http://oceanai.mit.edu/ivpman/pmwiki/pmwiki.php?n=Lab.ClassPrimeFactors, chapter 4.2.
// Implemented in PrimeEntry.cpp by me.

#include <string> 
#include <vector> 
#include <cstdint>
#include "PrimeFactor.h"


#ifndef PRIME_ENTRY_HEADER
#define PRIME_ENTRY_HEADER

class PrimeEntry
{
   public:

      PrimeEntry();
      ~PrimeEntry() {};

      void setOriginalVal(unsigned long int v) {m_orig   =  v; };
      void setDone(bool v)                     {m_done   =  v;};
      bool done()                              {return(m_done);};

      bool factor(unsigned long int max_steps); // Finding next primes, given a maximum number of steps

      std::string getReport() const;   // Generated a string report of results

   protected:
      uint64_t      m_current_int;      // 
      uint64_t      m_orig;               // Original number
      bool          m_done;               // 

      std::list<uint64_t> m_factors;    // Holds factors found so far

};
#endif 