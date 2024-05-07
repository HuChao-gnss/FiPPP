#include "Counter.hpp"

#ifdef USE_OPENMP
#include <omp.h>
#endif

namespace gpstk{

double  Counter::m_begin_clock = (double)clock();

void Counter::begin(){
#ifndef USE_OPENMP
    m_begin_clock = (double)clock();
#else
    m_begin_clock = omp_get_wtime();
#endif
}

double Counter::now()
{
#ifndef USE_OPENMP
   return (double)clock()/CLOCKS_PER_SEC;
#else
   return omp_get_wtime();
#endif
}

double Counter::end(){
   double t = 0.0;
#ifndef USE_OPENMP
    clock_t end_clock = (double)clock();
    t = (double)(end_clock - m_begin_clock)/CLOCKS_PER_SEC;
#else
    t = omp_get_wtime() - m_begin_clock;
#endif
    return t;
}

}
