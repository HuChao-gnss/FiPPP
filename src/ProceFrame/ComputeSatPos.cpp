/**
 * Class to compute satellite relativity corrections, position and velocity.
 */

#include "ComputeSatPos.hpp"
#include "YDSTime.hpp"
#include "GNSSconstants.hpp"
#include "DataStructures.hpp"

using namespace std;
using namespace gpstk;

#define debug 0

namespace gpstk
{


    // Return a string identifying this object.
    std::string ComputeSatPos::getClassName() const
    {
        return "ComputeSatPos";
    }


    /* Return a satTypeValueMap object, adding the new data generated when
     * calling a modeling object.
     *
     * @param time      Epoch.
     * @param gData     Data object holding the data.
     */
    satTypeValueMap& ComputeSatPos::Process(const CommonTime& time,
        std::map<RinexSatID/*SatID*/, TypeIDVec>& satShortTypes,
        satTypeValueMap& gData)
        noexcept(false)
    {

        try
        {
            SatIDSet satRejectedSet;

            // Loop through all the satellites
            for (satTypeValueMap::iterator it = gData.begin();
                it != gData.end();
                ++it)
            {
                SatID sat(it->first);

                if (debug)
                {
                    cout << getClassName() << "sat:" << sat << endl;
                }

                try
                {
                    Xvt svPosVel;

                    // compute satellite ephemeris at transmitting time
                    // Scalar to hold temporal value
                    double obs(0.0);

                    // firstly, find the optimal code observable for current satellite 

                    // 2-char Types for current satellite
                    vector<TypeID> shortTypes = satShortTypes[sat];

                    TypeID codeType;
                    for (int i = 0; i < shortTypes.size(); i++)
                    {
                        string typeStr = shortTypes[i].asString();
                        if (typeStr[0] == 'C')
                        {
                            codeType = shortTypes[i];
                            break;
                        }
                    }

                    // code obs
                    try
                    {
                        obs = (*it).second(codeType);
                    }
                    catch (TypeIDNotFound& e)
                    {
                        // remove this satellite
                        satRejectedSet.insert(sat);

                        // the next satellite
                        continue;
                    }

                    // now, compute svPosVel
                    try
                    {
                        svPosVel = ComputeAtTransmitTime(time, obs, sat);
                    }
                    catch (InvalidRequest& e)
                    {
                        // If some problem appears, then schedule this satellite
                        // for removal
                        cerr << e << endl;
                        satRejectedSet.insert(sat);
                        continue;
                    }

                    //
                    // transmitting-time related parameters
                    //

                    // relativity
                    double relativity(0.0);
                    relativity = svPosVel.computeRelativityCorrection() * C_MPS;

                    // clock bias, clock drift
                    double svClkBias(0.0), svClkDrift(0.0);

                    svClkBias = svPosVel.getClockBias() * C_MPS;
                    svClkDrift = svPosVel.getClockDrift() * C_MPS;

                    // warning: the sign is changed!
                    // see that in LinearCombination.
                    (*it).second[TypeID::relativity] = -relativity;

                    // Let's insert satellite clock bias at transmit time
                    (*it).second[TypeID::cdtSat] = svClkBias;
                    (*it).second[TypeID::cdtSatDot] = svClkDrift;

                    // Let's insert satellite position at transmit time
                    (*it).second[TypeID::satXECEF] = svPosVel.x[0];
                    (*it).second[TypeID::satYECEF] = svPosVel.x[1];
                    (*it).second[TypeID::satZECEF] = svPosVel.x[2];

                    // Let's insert satellite velocity at transmit time
                    (*it).second[TypeID::satVXECEF] = svPosVel.v[0];
                    (*it).second[TypeID::satVYECEF] = svPosVel.v[1];
                    (*it).second[TypeID::satVZECEF] = svPosVel.v[2];

                }
                catch (InvalidRequest& e)
                {
                    // If some problem appears, then schedule this satellite
                    // for removal
                    cerr << e << endl;
                    satRejectedSet.insert(sat);
                    continue;
                }


            } // End of loop for(satTypeValueMap = gData.begin()...

            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;

        }   // End of try...
        catch (Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e(getClassName() + ":" + u.what());
            GPSTK_THROW(u);
        }

    }  // End of method 'ComputeSatPos::Process()'


    Xvt ComputeSatPos::ComputeAtTransmitTime(const CommonTime& tr,
        const double& pr,
        const RinexSatID/*SatID*/& sat)
        noexcept(false)
    {
        Xvt svPosVel;

        CommonTime tt;
        CommonTime transmit(tr);
        transmit -= pr / C_MPS;
        tt = transmit;

        for (int i = 0; i < 2; i++)
        {
            try
            {
                if (pEphStore != NULL)
                {
                    svPosVel = pEphStore->getXvt(sat, tt);
                }
                else
                {
                    cerr << getClassName() << "pEphStore should be given" << endl;
                    exit(-1);
                }
            }
            catch (InvalidRequest& e)
            {
                GPSTK_RETHROW(e);
            }

            tt = transmit;
            tt -= (svPosVel.clkbias + svPosVel.relcorr);
        }

        // transmitting time
////
//      double dt = tt - tr;
////

        double dt = std::abs(tr - tt);

        if (debug)
        {
            cout << getClassName() << ":" << dt << endl;
        }

        // earth rotation
        rotateEarth(dt, svPosVel);

        return svPosVel;

    };

    void ComputeSatPos::rotateEarth(double dt, Xvt& svPosVel)
    {

        double wt(0.0);
        wt = OMEGA_EARTH * dt;

        double sx(0.0), sy(0.0);

        sx = +std::cos(wt) * svPosVel.x[0] + std::sin(wt) * svPosVel.x[1];
        sy = -std::sin(wt) * svPosVel.x[0] + std::cos(wt) * svPosVel.x[1];

        svPosVel.x[0] = sx;
        svPosVel.x[1] = sy;

        double vx(0.0), vy(0.0);

        vx = +std::cos(wt) * svPosVel.v[0] + std::sin(wt) * svPosVel.v[1];
        vy = -std::sin(wt) * svPosVel.v[0] + std::cos(wt) * svPosVel.v[1];

        svPosVel.v[0] = vx;
        svPosVel.v[1] = vy;
    }


}  // End of namespace gpstk
