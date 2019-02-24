// File: PrimeEntry.cpp
// Simen Sem Oevereng, 24. feb 2019
// Header file copied from http://oceanai.mit.edu/ivpman/pmwiki/pmwiki.php?n=Lab.ClassPrimeFactors, chapter 4.2.
// Implemented in PrimeEntry.cpp by me.

#include "PrimeEntry.h"
using namespace std;

PrimeEntry::PrimeEntry()
{
   m_current_int        = 0;
   m_orig               = 0;
   m_done               = false;
   m_factors            = {};
}

//---------------------------------------------------------
// Procedure: factor
// Purpose:   find next primes, given a maximum number of steps
// @ params   max_steps: maximum steps of trying to find primes
// @ edits    m_factors: vector holding all current primes
// @ return   true if prime is found, false otherwise
bool PrimeEntry::factor(unsigned long int max_steps){

   unsigned int steps = 0;

   // Print the number of 2s that divide n 
   while (m_current_int % 2 == 0) 
   {  
      steps++;
      m_factors.push_back(2); 
      m_current_int = m_current_int / 2;

      if(steps > max_steps){
         return(true);
      }
   } 

   // n must be odd at this point.  So we can skip  
   // one element (Note i = i + 2) 
   for (int i = m_current_int; i <= sqrt(m_orig); i = i+2) 
   { 
      // While i divides n, push back i and divide current int
      while (m_current_int % i == 0) 
      {
         steps++;

         m_factors.push_back(i); 
         m_current_int = m_current_int / i;

         if(steps > max_steps){
            return(true);
         }
      } 
   } 

   // This condition is to handle the case when n  
   // is a prime number greater than 2 
   if (m_current_int > 2) 
      m_factors.push_back(m_current_int);

   setDone(true); // If the function reaches this point, it's over

   return(true);
}

//---------------------------------------------------------
// Procedure: getReport
// Purpose:   Generates a string of factors found  
// @ params   integer to test
// @ edits    m_prime_factors
// @ return   nothing
string PrimeEntry::getReport() const {
   
   // To view result, notify the list as string of all elements
   string stringList = "primes=";
   list<uint64_t>::const_iterator it;

   for(it = m_factors.begin(); it != m_factors.end(); ++it){
    
      if (it == m_factors.begin())
         stringList =  stringList + to_string(*it);
      else
         stringList = stringList + ":" + to_string(*it);
   }

   return(stringList);
}; 

