// File: PrimeEntry.cpp
// Simen Sem Oevereng, 24. feb 2019
// Header file copied from http://oceanai.mit.edu/ivpman/pmwiki/pmwiki.php?n=Lab.ClassPrimeFactors, chapter 4.2.
// Implemented in PrimeEntry.cpp by me.

#include <iterator>

#include "math.h"
#include "PrimeEntry.h"

using namespace std;

const bool debug = true;

PrimeEntry::PrimeEntry()
{
   m_current_int        = 0;
   m_orig               = 0;
   m_done               = false;
   m_factors            = {};
   m_iterations         = 0;
   m_current_index      = 3;
}

//---------------------------------------------------------
// Procedure: factor
// Purpose:   find next primes, given a maximum number of steps
// @ params   max_steps: maximum steps of trying to find primes
// @ edits    m_factors: vector holding all current primes. m_done: if all
//            primes are found
// @ return   nothing

/*
bool PrimeEntry::factor(unsigned long int max_steps){

   // Print the number of 2s that divide n 
   while (m_current_int % 2 == 0) 
   {  
      m_iterations++;
      m_factors.push_back(2); 
      m_current_int = m_current_int / 2;

      if(m_iterations > max_steps){
         m_iterations = 0;
         return(false); // We have to wait to a later iteration
      }
   } 

   // n must be odd at this point.  So we can skip  
   // one element (Note i = i + 2) 
   for (uint64_t i = m_current_index; i <= sqrt(m_current_int); i = i + 2) 
   { 
      m_iterations++;
      m_current_index = i;

      if(m_iterations > max_steps){
         m_iterations = 0;
         cout << "m_iter > max: Current int and index: " << m_current_int << " - " << m_current_index << endl;
         return(false);
      }
      
      // While i divides n, push back i and divide current int
      while (m_current_int % i == 0) 
      {
         m_factors.push_back(i); 
         m_current_int = m_current_int / i;

         m_iterations++;

         if(m_iterations > max_steps){
            m_iterations = 0;
            cout << "m_iter > max: Current int and index: " << m_current_int << " - " << m_current_index << endl;
            return(false); // We have to wait to a later iteration
         }
      } 
   }

   // This condition is to handle the case when n  
   // is a prime number greater than 2 
   if (m_current_int > 2) 
      m_factors.push_back(m_current_int);

   if(debug){ cout << "Setting done" << endl;}
   return(true); // If the function reaches this point, it's done

}
*/


bool PrimeEntry::factor(unsigned long int max_steps){

   unsigned long int localIterations = 0;

   // Print the number of 2s that divide n 
   while (m_current_int % 2 == 0) 
   {   
     localIterations++;
     m_factors.push_back(2); 
     m_current_int = m_current_int / 2;
   } 

   // n must be odd at this point.  So we can skip  
   // one element (Note i = i + 2) 
   for (uint64_t i = m_current_index; i <= sqrt(m_orig) + 1; i = i + 2) 
   { 
     localIterations++;

     if(localIterations > max_steps){
       m_current_index = i;
       return(false);
     }
     
     // While i divides n, print i and divide n 
     while (m_current_int % i == 0) 
     { 
         m_factors.push_back(i); 
         m_current_int = m_current_int / i;

         localIterations++;

         if(localIterations > max_steps){
           m_current_index = i;
           return(false);
         }
     } 
   } 

   // This condition is to handle the case when n  
   // is a prime number greater than 2 
   if (m_current_int > 2) 
     m_factors.push_back(m_current_int); 

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
   string stringList = "primes of " + to_string(m_orig) + " = ";
   list<uint64_t>::const_iterator it;

   for(it = m_factors.begin(); it != m_factors.end(); ++it){
      if (it == m_factors.begin())
         stringList = stringList + to_string(*it);
      else
         stringList = stringList + ":" + to_string(*it);
   }

   return(stringList);
}; 
