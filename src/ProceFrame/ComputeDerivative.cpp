/**
 * ComputeDerivative
 */

#include "ComputeDerivative.hpp"
#include "YDSTime.hpp"
#include "GNSSconstants.hpp"
#include "DataStructures.hpp"

using namespace std;
#define debug 0

namespace gpstk 
{

      // Return a string identifying this object.
    std::string ComputeDerivative::getClassName() const
    { return "ComputeDerivative"; }


      /* Explicit constructor, taking as input reference station
       * coordinates, ephemeris to be used, default observable
       * and whether TGD will be computed or not.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param dEphemeris    EphemerisStore object to be used by default.
       * @param dObservable   Observable type to be used by default.
       * @param applyTGD      Whether or not C1 observable will be
       *                      corrected from TGD effect or not.
       *
       */
    ComputeDerivative::ComputeDerivative( const Position& StaPos)
    {
        minElev = 10.0;
        nominalPos = StaPos;
    }  // End of 'ComputeDerivative::ComputeDerivative()'

    void ComputeDerivative::computeDOPs(gnssRinex& gRin)
        noexcept(false)
    {
        double ele(0.0), azi(0.), sinel, sinaz, cosel, cosaz;
        Vector<double> dops(4, 0.);
        int ns(gRin.body.size()), i(0);
        Matrix<double> H(ns, 4, 0.);
        // init
        gRin.header.dops.resize(4, -9999.);
        // for loop all the data
        auto itData = gRin.body.begin();
        for (itData;
            itData != gRin.body.end();
            ++itData)
        {
            ele = itData->second(TypeID::elevation);
            azi = itData->second(TypeID::azimuth);
            if(ele<=minElev||azi<=0.)   continue;
            ele *= DEG_TO_RAD;
            azi *= DEG_TO_RAD;
            sinel = ::sin(ele);
            sinaz = ::sin(azi);
            cosel = ::cos(ele);
            cosaz = ::cos(azi);
            // Assign to H matrix
            H(i, 0) = cosel * sinaz;
            H(i, 1) = cosel * cosaz;
            H(i, 2) = sinel;
            H(i, 3) = 1, 0;

            ++i;
        }   // End of for looping all the data
        if (i < 4) return;
        // compute
        try
        {
            Matrix<double> Q(transpose(H) * H);
            Matrix<double> invQ(inverse(Q));
            dops(0) = SQRT(invQ(0, 0)+invQ(1, 1)+invQ(2, 2)+invQ(3, 3));    // GDOP
            dops(1) = SQRT(invQ(0, 0)+invQ(1, 1)+invQ(2, 2));               // PDOP
            dops(2) = SQRT(invQ(0, 0)+invQ(1, 1));                          // HDOP
            dops(3) = SQRT(invQ(2, 2));                                     // VDOP
            gRin.header.dops = dops;
            if (debug)
            {
                std::cout << "Q" << std::endl;
                std::cout << Q << std::endl;
                std::cout << "gdop pdop hdop vdop" << std::endl;
                std::cout << dops << std::endl;
            }
        }
        catch (...)
        {
            InvalidParameter e("Cannot compute Matrix!");
            std::cout << e.getLocation() << std::endl;
            return;
        }
    }

      /* Return a satTypeValueMap object, adding the new data generated when
       * calling a modeling object.
       *
       * @param time      Epoch.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& ComputeDerivative::Process( const CommonTime& time,
                                                 satTypeValueMap& gData )
            noexcept(false)
    {

        try
        {
            SatIDSet satRejectedSet;
            TypeID defaultObs;

            // Loop through all the satellites
            for(satTypeValueMap::iterator it = gData.begin();
                it != gData.end();
                ++it)
            {
                SatID sat = (*it).first;

                Triple svPos;
                svPos[0] = (*it).second[TypeID::satXECEF];
                svPos[1] = (*it).second[TypeID::satYECEF];
                svPos[2] = (*it).second[TypeID::satZECEF];

                if(debug)
                {
                    cout << getClassName() << " sat:" << sat << svPos << endl;
                }

                // rho
                double rho(0.0);
                rho = RSS(svPos[0] - nominalPos.X(), 
                          svPos[1] - nominalPos.Y(), 
                          svPos[2] - nominalPos.Z());

                // partials
                Triple cosines;
                cosines[0] = (nominalPos.X() - svPos[0]) / rho;
                cosines[1] = (nominalPos.Y() - svPos[1]) / rho;
                cosines[2] = (nominalPos.Z() - svPos[2]) / rho;

                // elevation, azimuth
                double elevation(0.0), azimuth(0.0);

                try
                {
                    elevation = nominalPos.elevationGeodetic(svPos);
                    azimuth = nominalPos.azimuth(svPos);
                }
                catch(Exception& e)
                {
                    satRejectedSet.insert((*it).first);
                    continue;
                }

                // Let's test if satellite has enough elevation over horizon
                if ( elevation < minElev )
                {
                    // Mark this satellite if it doesn't have enough elevation
                    satRejectedSet.insert( (*it).first );
                    continue;
                }

                // now, let's compute the prefitC for spp
                double relativity, cdtSat;

                try
                {
                    relativity = (*it).second(TypeID::relativity);
                    cdtSat     = (*it).second(TypeID::cdtSat);
                }
                catch(Exception& e)
                {
                    satRejectedSet.insert((*it).first);
                    continue;
                }

                // rho
                (*it).second[TypeID::rho] = rho;

                // Let's insert partials for station position at receive time
                (*it).second[TypeID::dX] = cosines[0];
                (*it).second[TypeID::dY] = cosines[1];
                (*it).second[TypeID::dZ] = cosines[2];
                (*it).second[TypeID::dcdtGPS] = 1.0;

                (*it).second[TypeID::elevation] = elevation;
                (*it).second[TypeID::azimuth] = azimuth;

            } // End of loop for(satTypeValueMap = gData.begin()...

            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;

        }   // End of try...
        catch(SVNumException& u)
        {
            GPSTK_THROW(u);
        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );
            GPSTK_THROW(u);
        }

    }  // End of method 'ComputeDerivative::Process()'

}  // End of namespace gpstk
