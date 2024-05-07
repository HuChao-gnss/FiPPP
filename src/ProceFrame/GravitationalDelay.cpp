#pragma ident "$Id$"

/**
 * @file GravitationalDelay.cpp
 * This class computes the delay in the signal due to changes in gravity field.
 */

#include "GravitationalDelay.hpp"


using namespace std;


namespace gpstk
{

    // Return a string identifying this object.
    std::string GravitationalDelay::getClassName() const
    { return "GravitationalDelay"; }



      /* Return a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param epoch     Time of observations.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& GravitationalDelay::Process( const CommonTime& epoch,
                                                  satTypeValueMap& gData )
        noexcept(false)
    {

        // Constant value needed for computation. This value comes from:
        //    K = (1+gamma) muE / (c*c)
        // where:
        // - gamma = 1.0   (general relativity)
        // - muE = 3.986004418e14 m^3/s^2  (std gravitational parameter, Earth)
        // - c = 2.99792458e8 m/s  (speed of light)
        const double K = 0.887005608e-2;

        try
        {

            SatIDSet satRejectedSet;

            if(nominalPos.X()==0.0 &&
               nominalPos.Y()==0.0 &&
               nominalPos.Z()==0.0)
            {
                Exception e("nominalPos must be given!");
                GPSTK_THROW(e);
            }

            // Define a Triple that will hold satellite position, in ECEF
            Triple svPos(0.0, 0.0, 0.0);

            // Loop through all the satellites
            for(satTypeValueMap::iterator it = gData.begin();
                it != gData.end();
                ++it)
            {
                SatID sat( it->first );

                // Check if satellite position is not already computed
                if( ( (*it).second.find(TypeID::satXECEF) == (*it).second.end() ) ||
                    ( (*it).second.find(TypeID::satYECEF) == (*it).second.end() ) ||
                    ( (*it).second.find(TypeID::satZECEF) == (*it).second.end() ) )
                {
                    satRejectedSet.insert( sat );
                    continue;
                }
                else
                {
                    // Get satellite position out of GDS
                    svPos[0] = (*it).second[TypeID::satXECEF];
                    svPos[1] = (*it).second[TypeID::satYECEF];
                    svPos[2] = (*it).second[TypeID::satZECEF];

                }  // End of 'if( ( (*it).second.find(TypeID::satX) == ...'

                // Get magnitude of satellite position vector
                double r2(svPos.mag());

                // Get vector from Earth mass center to receiver
                Triple rxPos(nominalPos.X(), nominalPos.Y(), nominalPos.Z());

                // Compute magnitude of receiver position vector
                double r1(rxPos.mag());

                // Compute the difference vector between satellite and
                // receiver positions
                Position difPos(svPos - rxPos);

                // Compute magnitude of the diference between rxPos and svPos
                double r12( difPos.mag() );

                // Compute gravitational delay correction
                double gravDelay( K*std::log( (r1+r2+r12)/(r1+r2-r12) ) );

                // Get the correction into the GDS
                //(*it).second[TypeID::gravDelay] = gravDelay;
                (*it).second[TypeID::gravDelay] = -gravDelay;

            }

            // Remove satellites with missing data
            gData.removeSatID(satRejectedSet);

            return gData;

        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );
            GPSTK_THROW(e);
        }

    }  // End of method 'GravitationalDelay::Process()'


    void GravitationalDelay::Process(gnssRinex& gData)
        noexcept(false)
    {
        try
        {
            // extract receiver position from gnssData
            Process(gData.header.epoch, gData.body);

        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );
            GPSTK_THROW(e);
        }

    }  // End of method 'GravitationalDelay::Process()'


}  // End of namespace gpstk
