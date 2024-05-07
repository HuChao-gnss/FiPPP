#ifndef COUNTER_H_H
#define COUNTER_H_H

#include <iostream>
#include <map>
#include <ctime>

using namespace std;

namespace gpstk{

class Counter{

   public:
      static void   begin();
      static double now();
      static double end(); 
   
   private:
      static double  m_begin_clock;
};

}

#endif
