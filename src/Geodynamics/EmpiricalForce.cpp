
/**
 * @file EmpiricalForce.cpp
 * Class to do Empirical Force calculation.
 */

#include "EmpiricalForce.hpp"
#include "GNSSconstants.hpp"


using namespace std;


namespace gpstk
{
    // this is the real one
    void EmpiricalForce::Compute( const CommonTime&   tt,
                                  const satVectorMap& orbits )
    {

        double dt( std::abs(tt-prevTime) );

        Vector<double> r(3,0.0);
        Vector<double> v(3,0.0);

        Vector<double> force(3,0.0);

        Vector<double> a(3,0.0);

        Matrix<double> da_dr(3,3,0.0);
        Matrix<double> da_dv(3,3,0.0);
        Matrix<double> da_dEMP(3,3,0.0);

        for( satVectorMap::const_iterator it = orbits.begin();
             it != orbits.end();
             ++it )
        {
            SatID sat( it->first );

            Vector<double> orbit( it->second );

            r(0) = orbit(0); r(1) = orbit(1); r(2) = orbit(2);
            v(0) = orbit(3); v(1) = orbit(4); v(2) = orbit(5);

            Vector<double> r_unit( normalize(r) );
            Vector<double> n_unit( normalize(cross(r,v)) );
            Vector<double> t_unit( normalize(cross(n_unit,r_unit)) );

            Matrix<double> RTN2ECI(3,3,0.0);
            for(int i=0; i<3; i++)
            {
                RTN2ECI(i,0) = r_unit(i);
                RTN2ECI(i,1) = t_unit(i);
                RTN2ECI(i,2) = n_unit(i);
            }

            if(prevForce.find(sat) != prevForce.end())
            {
                force = prevForce[sat];
            }

            // a
//            a = std::exp(-dt/tau)*force;
            a = RTN2ECI*force;
            satAcc[sat] = a;

            // da_dr
            satPartialR[sat] = da_dr;

            // da_dv
            satPartialV[sat] = da_dv;

            // da_dEMP
//            da_dEMP(0,0) = std::exp(-dt/tau);
//            da_dEMP(1,1) = std::exp(-dt/tau);
//            da_dEMP(2,2) = std::exp(-dt/tau);
            da_dEMP(0,0) = 1.0;
            da_dEMP(1,1) = 1.0;
            da_dEMP(2,2) = 1.0;
            satPartialEMP[sat] = da_dEMP;

        } // End of 'for(satVectorMap::const_iterator...)'

    }  // End of method 'EmpiricalForce::Compute(...)'


}  // End of namespace 'gpstk'
