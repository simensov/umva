/************************************************************/
/*    NAME: Simen Sem Oevereng                              */
/*    ORGN: MIT                                             */
/*    FILE: PrimeEntry.cpp                                  */
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

#include <iterator>
#include "math.h"
#include "PrimeEntry.h"

using namespace std;

//---------------------------------------------------------
// Constructor
PrimeEntry::PrimeEntry()
{
  m_current_int        = 0;
  m_orig               = 0;
  m_done               = false;
  // m_factors            = {};
  m_iterations         = 0;
  m_current_index      = 3;
  m_entry_id           = 0;
  m_solve_id           = 0;
  m_initiated          = false;
}


//---------------------------------------------------------
// Delegating constructor
PrimeEntry::PrimeEntry(uint64_t num, uint64_t ID, double time)
{
  m_current_int        = 0;
  m_orig               = 0;
  m_done               = false;
  // m_factors            = {};
  m_iterations         = 0;
  m_current_index      = 3;
  m_entry_id           = 0;
  m_solve_id           = 0;
  m_initiated          = false;
  setOriginalVal(num);
  setID(ID,time);
}


//---------------------------------------------------------
// Procedure: reachedMax
// Purpose:   tests to see if current iterations have surpassed max iterations
// @ params   index: largest number that the original number has been divided
//            by yet. max steps: maximum iterations allowed
// @ edits    m_iterations: resets to 0. m_current_index: stores index
// @ return   true or false based on reaching max or not
bool PrimeEntry::reachedMax(unsigned long int index, unsigned long int &max_steps){

  // m_iterations is never set to 0 (in reachedMax) if we reach the max.
  // therefore PrimeFactor::Iterate() can extract that value from PrimeEntry::getIterations()

  if(m_iterations >= max_steps){
    m_current_index = index;
    m_iterations = 0;
    return(true);
  }

  return(false);
}

//---------------------------------------------------------
// Procedure: factor
// Purpose:   find next primes, given a maximum number of steps
// @ params   max_steps: maximum steps of trying to find primes
// @ edits    m_factors: vector holding all current primes. m_done: if all
//            primes are found
// @ return   nothing

bool PrimeEntry::factor(unsigned long int max_steps){

   // push the number of 2s that divide current integer
   // here it is assumed that if we get a prime that is only divisable with 2, max steps will not be so small that we will not find the solution
   while (m_current_int % 2 == 0) 
   {   
     m_iterations++;
     m_factors.push_back(2); 
     m_current_int = m_current_int / 2;
   } 

   // current integer must be odd at this point: we can skip even numbers.
   // while i divides the integer, push i and divide current integer.
   // always check iterations vs. reachedMax, which will store progress if we have not finished solving before maximum iterations.
   for (uint64_t i = m_current_index; i <= sqrt(m_orig) + 1; i += 2) 
   { 
     m_iterations++;

     if(reachedMax(i,max_steps)){
       return(false);
     }
     
     while (m_current_int % i == 0) 
     { 
         m_factors.push_back(i); 
         m_current_int = m_current_int / i;
         
         m_iterations++;

         if(reachedMax(i,max_steps)){
           return(false);
         }
     } 
   } 

   // handle the case when the integer is a prime number greater than 2 
   if (m_current_int > 2) 
     m_factors.push_back(m_current_int); 

   return(true);
} 


//---------------------------------------------------------
// Procedure: getPrimes
// Purpose:   generates a string of factors found with a colon in between  
// @ params   no inputs
// @ edits    no edits
// @ return   a string of correct format, e.g. "2:3:3:5"
string PrimeEntry::getPrimes() const {

   string stringList = "";
   list<uint64_t>::const_iterator it;
   for(it = m_factors.begin(); it != m_factors.end(); ++it){

      if (it == m_factors.begin())
         stringList = to_string(*it);
      else
         stringList = stringList + ":" + to_string(*it);
   }

   return(stringList);
}; 
