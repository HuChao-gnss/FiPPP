
/**
 * @file Relativity.cpp
 * Class to do Relativity calculation.
 */

#include "Relativity.hpp"
#include "GNSSconstants.hpp"


using namespace std;


namespace gpstk
{
    // this is the real one
    void Relativity::Compute( const CommonTime&   tt,
                              const satVectorMap& orbits )
    {
        const double GM = GM_EARTH;
        const double C = C_MPS;

        double beta = 1.0;
        double gama = 1.0;

        double c2 = C * C;

        SatID sat;
        Vector<double> orbit;

        Vector<double> r(3,0.0), v(3,0.0);

        Vector<double> a(3,0.0);
        Matrix<double> da_dr(3,3,0.0);
        Matrix<double> da_dv(3,3,0.0);

        for( satVectorMap::const_iterator it = orbits.begin();
             it != orbits.end();
             ++it )
        {
            sat = it->first;
            orbit = it->second;

            r(0) = orbit(0);    r(1) = orbit(1);    r(2) = orbit(2);
            v(0) = orbit(3);    v(1) = orbit(4);    v(2) = orbit(5);

            double r2 = dot(r,r);
            double v2 = dot(v,v);

            double r_mag = norm(r);
            double r3 = r2 * r_mag;

            double p = GM/c2/r3;

            // a
            double pr = 2.0 * (beta + gama) * GM / r_mag - gama * v2;
            double pv = 2.0 * (1.0 + gama) * dot(r,v);

            a = p * ( pr * r + pv * v );

            satAcc[sat] = a;

            // da_dr
            double prr = -(GM/r_mag)*(GM/r_mag)*(2.0*(beta+gama)/c2);
            double pvv = (GM/r3)*(2.0*(1.0+gama)/c2);
            double par = -3.0/r2;
            double ppr = (GM/r3)*((GM/r_mag)*(2.0*(beta+gama)/c2)-gama*v2/c2);

            for(int i=0; i<3; i++)
            {
                for(int j=0; j<3; j++)
                {
                    double det = (i == j) ? 1.0 : 0.0;

                    da_dr(i,j) = prr*r(i)*r(j)+pvv*v(i)*v(j)+par*a(i)*r(j)+ppr*det;
                }
            }

            double prv = -(GM/r3)*(2.0*gama/c2);
            double pvr = (GM/r3)*(2.0*(1.0+gama)/c2);
            double ppv = pvr*dot(r,v);

            for(int i=0;i<3;i++)
            {
                for(int j=0;j<3;j++)
                {
                    double det = (i == j) ? 1.0 : 0.0;

                    da_dr(i,j) = prv*r(i)*v(j)+pvr*v(i)*r(j)+ppv*det;
                }
            }

            satPartialR[sat] = da_dr;

            // da_dv
            satPartialV[sat] = da_dv;

        } // End of 'for(satVectorMap::const_iterator...)'

    }  // End of method 'Relativity::Compute(...)'


}  // End of namespace 'gpstk'
