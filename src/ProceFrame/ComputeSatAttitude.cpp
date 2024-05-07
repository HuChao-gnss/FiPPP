

#include <cmath>
#include "ComputeSatAttitude.hpp"
#include "GNSSconstants.hpp"
#include "Epoch.hpp"

using namespace std;

namespace gpstk
{

    // Return a string identifying this object.
    std::string ComputeSatAttitude::getClassName() const
    { return "ComputeSatAttitude"; }


    /* Return a satTypeValueMap object, adding the new data generated
    *  when calling this object.
    *
    * @param epoch      Time
    * @param orbits     Orbits
    */
    satYawDataMap ComputeSatAttitude::getAttitude( const CommonTime& epoch,
                                                   const satVectorMap& orbits )
        noexcept(false)
    {
        try
        {
            if(pRefSys == NULL)
            {
                ProcessingException e("Pointer to ReferenceSystem NULL!");
                GPSTK_THROW(e);
            }

            if(pSolSys == NULL)
            {
                ProcessingException e("Pointer to SolarSystem NULL!");
                GPSTK_THROW(e);
            }

            if(pAntexReader == NULL)
            {
                ProcessingException e("Pointer to AntexReader NULL!");
                GPSTK_THROW(e);
            }

            CommonTime utc( pRefSys->GPS2UTC(epoch) );

            Matrix<double> t2cRaw( pRefSys->T2CMatrix(utc) );
            Matrix<double> t2cDot( pRefSys->dT2CMatrix(utc) );

            CommonTime tt( pRefSys->GPS2TT(epoch) );
            double jd_tt( JulianDate(tt).jd );

            // Sun pos and vel in ECI, unit: km, km/day
            SolarSystem::Planet center(SolarSystem::Earth);
            SolarSystem::Planet target(SolarSystem::Sun);

            double rv_sun[6] = {0.0};
            pSolSys->computeState(jd_tt, target, center, rv_sun);

            // Sun pos in ECI, unit: m
            Vector<double> rSunECI(3,0.0);
            rSunECI(0) = rv_sun[0];
            rSunECI(1) = rv_sun[1];
            rSunECI(2) = rv_sun[2];
            rSunECI *= 1.0e+3;


            // Sat pos/vel in ECI
            Vector<double> rSatECI(3,0.0), vSatECI(3,0.0);


            satYawDataMap currentYawData;

            SatIDSet satRejectedSet;

            if(orbits.size() == 0)
            {
                ProcessingException e("sat orbit is empty!!!");
                GPSTK_THROW(e);
                cerr << e << endl;
                exit(-1);
            }

            // Loop through all the satellites
            for(satVectorMap::const_iterator it = orbits.begin();
                it != orbits.end();
                ++it)
            {
                SatID sat( it->first );

                if(sat.system != SatelliteSystem::GPS) continue;

                Vector<double> orbit( it->second );

                if(orbit.size() == 0)
                {
                    satRejectedSet.insert( sat );
                    continue;
                }
                else
                {
                    for(int i=0; i<3; ++i)
                    {
                        rSatECI(i+0) = orbit(i+0);
                        vSatECI(i+0) = orbit(i+3);
                    }
                }

                double rSatECI2( dot(rSatECI,rSatECI) );
                double vSatECI2( dot(vSatECI,vSatECI) );

                // unit vector from Sat to Sun, ECI
                Vector<double> sat2sun( normalize(rSunECI-rSatECI) );

                // unit vector from Sat to Earth, ECI
                Vector<double> rk( ( (-1.0)*normalize(rSatECI) ) );

                // rj = rk x sat2sun, ECI
                Vector<double> rj( normalize(cross(rk,sat2sun)) );

                // ri = rj x rk, ECF
                Vector<double> ri( normalize(cross(rj,rk)) );


                // normal of orbit plane
                Vector<double> unit_nop( normalize(cross(rSatECI,vSatECI)) );

                // morning of orbit plane
                Vector<double> unit_mop( normalize(cross(rSunECI,unit_nop)) );

                // unit vector
                Vector<double> unit_rsat( normalize(rSatECI) );
                Vector<double> unit_rsun( normalize(rSunECI) );

                // elevation angle of Sun above Sat orbit plane
                double beta( PI/2.0 - std::acos(dot(unit_nop,unit_rsun)) );

                // nominal yaw angle
                nominalYaw = std::acos(dot(ri,vSatECI)/std::sqrt(vSatECI2));
                if(beta > 0.0) nominalYaw = -nominalYaw;

                // average orbital anglular velocity
                // ~0.00836 FOR GPS; ~ 0.00888 GLONASS
                double muRate( std::sqrt(vSatECI2/rSatECI2) );

                // sat information
                std::stringstream satstr;
                if(sat.system == SatelliteSystem::GPS)
                {
                    satstr << "G";
                    if(sat.id < 10) satstr << "0";
                    satstr << sat.id;
                }

                Antenna antenna( pAntexReader->getAntenna(satstr.str(),epoch) );
                string type( antenna.getAntennaType() );

                bool blockIIR( type.find("BLOCK IIR") != string::npos );
                bool blockIIF( type.find("BLOCK IIF") != string::npos );

                double maxYawRate(0.0);
                double yawBias(0.0);

                double mu(0.0);
                if(dot(unit_rsat,unit_rsun) <= 0.0)
                {
                    mu = PI/2.0 - std::acos(dot(unit_rsat,unit_mop));
                }
                else
                {
                    mu = PI/2.0 + std::acos(dot(unit_rsat,unit_mop));
                }

                double smu( std::sin(mu) );
                double cmu( std::cos(mu) );
                double tbeta( std::tan(beta) );

                double yawRate(0.0);
                yawRate = muRate*tbeta*cmu/(smu*smu+tbeta*tbeta);


                double beta0( BETA_GPS*DEG_TO_RAD );

                bool noon(false), night(false), shadow(false);


                double beta1(0.0);

                modeledYaw = nominalYaw;

                if(blockIIR)
                {
                    maxYawRate = 0.20*DEG_TO_RAD;
                    yawBias = 0.00*DEG_TO_RAD;

                    beta1 = std::atan(muRate/maxYawRate);

                    if(std::fabs(beta) <= beta0)
                    {
                        if(std::fabs(beta) <= beta1)
                        {
                            // noon
                            if(mu > +0.5*PI && mu < +1.5*PI)
                            {
                                noon = noonManeuver( epoch,
                                                     beta, beta1,
                                                     mu, muRate,
                                                     maxYawRate, yawBias,
                                                     unit_rsat, unit_rsun,
                                                     modeledYaw );
                            }
                            // night
                            else if(mu > -0.5*PI && mu < +0.5*PI)
                            {
                                night = nightManeuver( epoch,
                                                       beta, beta1,
                                                       mu, muRate,
                                                       maxYawRate, yawBias,
                                                       unit_rsat, unit_rsun,
                                                       modeledYaw );
                            }
                        }
                        else
                        {
                            double det( std::sqrt(beta0*beta0-beta*beta) );

                            // orbit angle at shadow start time
                            double mu_s( -det );

                            // orbit angle at shadow end time
                            double mu_e( det );

                            if(mu >= mu_s && mu <= mu_e)
                            {
                                shadow = true;
                            }
                        }
                    }
                }

                if(blockIIF)
                {
                    maxYawRate = 0.11*DEG_TO_RAD;
                    yawBias = 0.70*DEG_TO_RAD;

                    beta1 = std::atan(muRate/maxYawRate);

                    if(std::fabs(beta) <= beta0)
                    {
                        if(std::fabs(beta) <= beta1)
                        {
                            if(mu > +0.5*PI && mu < +1.5*PI)
                            {
                                noon = noonManeuver( epoch,
                                                     beta, beta1,
                                                     mu, muRate,
                                                     maxYawRate, yawBias,
                                                     unit_rsat, unit_rsun,
                                                     modeledYaw );
                            }
                            else
                            {
                                shadow = shadowManeuver( epoch,
                                                         beta, beta0,
                                                         mu, muRate,
                                                         maxYawRate, yawBias,
                                                         unit_rsat, unit_rsun,
                                                         modeledYaw );
                            }
                        }
                        else
                        {
                            if(mu > -0.5*PI && mu < +0.5*PI)
                            {
                                shadow = shadowManeuver( epoch,
                                                         beta, beta0,
                                                         mu, muRate,
                                                         maxYawRate, yawBias,
                                                         unit_rsat, unit_rsun,
                                                         modeledYaw );
                            }
                        }
                    }
                }

                YawData yawData;
                yawData.type = type;
                yawData.beta = beta*RAD_TO_DEG;

                if(noon)        yawData.event = 1;
                else if(night)  yawData.event = 2;
                else if(shadow) yawData.event = 3;
                else            yawData.event = 0;

                yawData.nominal = nominalYaw*RAD_TO_DEG;
                yawData.modeled = modeledYaw*RAD_TO_DEG;

                currentYawData[sat] = yawData;

            } // End for satellite loop

//            cout << endl;

            return currentYawData;
        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );
            cerr << e << endl;
            exit(-1);
        }

    }  // End of 'ComputeSatAttitude::Process()'


    bool ComputeSatAttitude::noonManeuver(const CommonTime& epoch,
                                          const double beta,
                                          const double betaLimit,
                                          const double mu,
                                          const double muRate,
                                          const double maxYawRate,
                                          const double yawBias,
                                          const Vector<double>& unit_rsat,
                                          const Vector<double>& unit_rsun,
                                          double& modeledYaw)
    {
        bool status( false );

        double tbeta( std::tan(beta) );
        double beta2( beta*beta );

        double det( std::sqrt(betaLimit*std::fabs(beta)-beta2) );

        // orbit angle at noon start time
        double mu_s(PI - det);

        // yaw angle at noon start time
        double yaw_s( std::atan2(-tbeta,std::sin(mu_s)) );

        // orbit angle at noon end time, initial guess
        double mu_e(mu_s + muRate*PI/maxYawRate);

        double mu_temp(0.0);
        double yaw_e(0.0);
        double dt(0.0);

        // iterate to get the accurate mu_e
        mu_temp = mu_e;
        while(true)
        {
            yaw_e = std::atan2(-tbeta,std::sin(mu_temp));

            dt = std::fabs(yaw_e-yaw_s)/maxYawRate;
            mu_temp = mu_s + muRate*dt;

            if(std::fabs(mu_temp-mu_e) < 1e-5)
                break;
            else
                mu_e = mu_temp;
        }

        // modeled yaw angle
        double E(0.0);
        CommonTime t_m(epoch), t_s(epoch);
        if(mu >= mu_s && mu <= mu_e)
        {
            status = true;

            E = std::acos(dot(unit_rsat,unit_rsun));

            if(mu > +0.5*PI && mu <= +1.0*PI)
            {
                t_m = epoch + std::sqrt(E*E-beta2)/muRate;
            }
            else
            {
                t_m = epoch - std::sqrt(E*E-beta2)/muRate;
            }

            t_s = t_m - det/muRate;

            dt = epoch - t_s;

            modeledYaw = yaw_s - maxYawRate*((beta+yawBias)>0?1:-1)*dt;
        }

        return status;
    }


    bool ComputeSatAttitude::nightManeuver(const CommonTime& epoch,
                                           const double beta,
                                           const double betaLimit,
                                           const double mu,
                                           const double muRate,
                                           const double maxYawRate,
                                           const double yawBias,
                                           const Vector<double>& unit_rsat,
                                           const Vector<double>& unit_rsun,
                                           double& modeledYaw)
    {
        bool status(false);

        double tbeta( std::tan(beta) );
        double beta2( beta*beta );

        double det( std::sqrt(betaLimit*std::fabs(beta)-beta2) );

        // orbit angle at night start time
        double mu_s( -det );

        // yaw angle at night start time
        double yaw_s( std::atan2(-tbeta,std::sin(mu_s)) );

        // orbit angle at night end time, initial guess
        double mu_e(mu_s + muRate*PI/maxYawRate);

        double mu_temp(0.0);
        double yaw_e(0.0);
        double dt(0.0);

        // iterate to get the accurate mu_e
        mu_temp = mu_e;
        while(true)
        {
            yaw_e = std::atan2(-tbeta,std::sin(mu_temp));

            dt = std::fabs(yaw_e-yaw_s)/maxYawRate;
            mu_temp = mu_s + muRate*dt;

            if(std::fabs(mu_temp-mu_e) < 1e-5)
                break;
            else
                mu_e = mu_temp;
        }

        // modeled yaw angle
        double E(0.0);
        CommonTime t_m(epoch), t_s(epoch);
        if(mu >= mu_s && mu <= mu_e)
        {
            status = true;

            E = PI - std::acos(dot(unit_rsat,unit_rsun));

            if(mu > -0.5*PI && mu <= 0.0)
            {
                t_m = epoch + std::sqrt(E*E-beta2)/muRate;
            }
            else
            {
                t_m = epoch - std::sqrt(E*E-beta2)/muRate;
            }

            t_s = t_m - det/muRate;

            dt = epoch - t_s;

            modeledYaw = yaw_s + maxYawRate*((beta+yawBias)>0?1:-1)*dt;
        }

        return status;
    }


    bool ComputeSatAttitude::shadowManeuver(const CommonTime& epoch,
                                            const double beta,
                                            const double betaLimit,
                                            const double mu,
                                            const double muRate,
                                            const double maxYawRate,
                                            const double yawBias,
                                            const Vector<double>& unit_rsat,
                                            const Vector<double>& unit_rsun,
                                            double& modeledYaw)
    {
        bool status(false);

        double tbeta( std::tan(beta) );
        double beta2( beta*beta );

        double betaLimit2( betaLimit*betaLimit );

        double det( std::sqrt(betaLimit2-beta2) );

        // orbit angle at shadow start time
        double mu_s( -det );

        // yaw angle at shadow start time
        double yaw_s( std::atan2(-tbeta,std::sin(mu_s)) );

        // orbit angle at shadow end time
        double mu_e( det );

        // yaw angle at shadow end time
        double yaw_e( std::atan2(-tbeta,std::sin(mu_e)) );

        double diff(yaw_e - yaw_s);

        if(diff < -PI) diff += 2.0*PI;
        if(diff > +PI) diff -= 2.0*PI;

        // modeled yaw angle
        double E(0.0), dt(0.0);
        CommonTime t_m(epoch), t_s(epoch), t_e(epoch);

        if(mu >= mu_s && mu <= mu_e)
        {
            status = true;

            E = PI - std::acos(dot(unit_rsat,unit_rsun));

            if(mu > -0.5*PI && mu <= 0.0)
            {
                t_m = epoch + std::sqrt(E*E-beta2)/muRate;
            }
            else
            {
                t_m = epoch - std::sqrt(E*E-beta2)/muRate;
            }

            t_s = t_m - det/muRate;
            t_e = t_m + det/muRate;

            dt = epoch - t_s;
            modeledYaw = yaw_s + diff/(t_e-t_s)*dt;
        }

        return status;
    }

    /** Return a satTypeValueMap object, adding the new data generated when
     *  calling this object.
     *
     * @param time      Epoch corresponding to the data.
     * @param gData     Data object holding the data.
     */
    satTypeValueMap& ComputeSatAttitude::Process( const CommonTime& time,
                                                 satTypeValueMap& gData )
        noexcept(false)
    {

        satYawData = getAttitude(time, satOrbits);
        return gData;
    }


} // End of namespace gpstk
