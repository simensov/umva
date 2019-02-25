// File: PrimeEntry.h
// Simen Sem Oevereng, 24. feb 2019
// Header file copied from http://oceanai.mit.edu/ivpman/pmwiki/pmwiki.php?n=Lab.ClassPrimeFactors, chapter 4.2.
// Implemented in PrimeEntry.cpp by me.

// #include "MOOS/libMOOS/MOOSLib.h"
#include <iostream>
#include <string> 
#include <list> 
#include <cstdint>

#ifndef PRIME_ENTRY_HEADER
#define PRIME_ENTRY_HEADER

class PrimeEntry
{
   public:

      PrimeEntry();
      ~PrimeEntry() {};

      void setOriginalVal(unsigned long int v) {m_orig   =  v; m_current_int = v; };
      void setDone(bool v)                     {m_done   =  v; };
      bool isDone()                            {return(m_done);};

      // Finding next primes, given a maximum number of steps
      bool factor(unsigned long int max_steps); 

      // Generated a string report of results
      std::string getReport() const;   

      // Collects current number of iterations
      uint64_t getIterations() const  { return m_iterations; }; 
      uint64_t getCurrentInt() const  { return m_current_int; };
      uint64_t getCurrentIndex() const  { return m_current_index; };
      uint64_t getOriginalPrime() const  { return m_orig; }; 

   protected:
      uint64_t      m_orig;         // Original number
      uint64_t      m_current_int;  // Current number after factoring
      uint64_t      m_current_index; // Next beginning index
      bool          m_done;         // Done with iterations or not 
      uint64_t      m_iterations;   // Number of iterations done on m_orig

      std::list<uint64_t> m_factors;    // Holds factors found so far

};
#endif 