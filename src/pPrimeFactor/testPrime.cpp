#include "PrimeEntry.h"
#include "math.h"

using namespace std;


list<uint64_t> setPrimeFactors(uint64_t &integer){

    list<uint64_t> m_prime_factors = {};
    // Print the number of 2s that divide n 
    while (integer%2 == 0) 
    { 
        m_prime_factors.push_back(2); 
        integer = integer/2; 
    } 
  
    // n must be odd at this point.  So we can skip  
    // one element (Note i = i +2) 
    for (int i = 3; i <= sqrt(integer); i = i+2) 
    { 
        // While i divides n, print i and divide n 
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

    return m_prime_factors;
} 

string getPrimesAsString(list<uint64_t> m_prime_factors) {

  // To view result, notify the list as string of all elements
  string stringList = "primes=";
  list<uint64_t>::iterator it;

  for(it = m_prime_factors.begin(); it != m_prime_factors.end(); ++it){
    
    if (it == m_prime_factors.begin())
      stringList =  stringList + to_string(*it);
    else
      stringList = stringList + ":" + to_string(*it);
  }
  
  return stringList;
}


int main(){

  uint64_t num = 10203040501;

  bool complete = false;

  PrimeEntry prime;
  prime.setOriginalVal(num);

  list<uint64_t> m_prime_factors = setPrimeFactors(num);
  cout << getPrimesAsString(m_prime_factors) << endl;


  while(!complete){
    cout << "Round" << endl;
    complete = prime.factor(1000);
    std::cout << prime.getCurrentIndex() << std::endl;

  }

  std::cout << prime.getReport() << std::endl;

}
