/************************************************************/
/*    NAME: Simen Sem Oevereng                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.h                                    */
/*    DATE: Feb 25 2019  
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


#include "MOOS/libMOOS/MOOSLib.h"
#include <string> 
#include <list> 
#include <cstdint>

#ifndef PRIME_ENTRY_HEADER
#define PRIME_ENTRY_HEADER

class PrimeEntry
{
   public:

      // constructor, delegating constructor and empty destructor
      PrimeEntry();
      PrimeEntry(uint64_t num, uint64_t ID, double time);
      ~PrimeEntry() {};

      // mutators
      void setOriginalVal(uint64_t v)     {m_orig = v; m_current_int = v; };
      void setDone(bool v)                {m_done = v; };
      void setID(uint64_t ID, double t)   {m_entry_id = ID; m_entry_time = t;}
      void setSolveID(uint64_t sID)       {m_solve_id = sID; };
      void setInitiated(bool state)       {m_initiated = state;};

      bool reachedMax(unsigned long int index, unsigned long int &max_steps);
      bool factor(unsigned long int max_steps);

      // accessors
      uint64_t getIterations()      const { return m_iterations; }; 
      uint64_t getCurrentInt()      const { return m_current_int; };
      uint64_t getCurrentIndex()    const { return m_current_index; };
      uint64_t getOriginalPrime()   const { return m_orig; }; 
      uint64_t getEntryID()         const { return m_entry_id; };
      uint64_t getSolveID()         const { return m_solve_id; };
      double   getEntryTime()       const { return m_entry_time; };
      bool     getInitiated()       const { return m_initiated; };
      std::string getPrimes()       const;
      
   protected:
      uint64_t      m_orig;         // Original number
      uint64_t      m_current_int;  // Current number after factoring
      uint64_t      m_current_index;// Next beginning index
      bool          m_done;         // Done with iterations or not 
      uint64_t      m_iterations;   // Number of iterations done on m_orig
      uint64_t      m_entry_id;     // Holds number of entry in a list holder
      double        m_entry_time;   // Holds time of entry in list holder
      uint64_t      m_solve_id;     // Holds number of solving from list holder

      bool          m_initiated;

      std::list<uint64_t> m_factors;// Holds factors found so far

};
#endif 