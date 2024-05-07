#pragma ident "$Id: $"

/**
 * @file ComputeElevWeights.hpp
 * This class computes satellites weights simply based on the elevation .
 */

#include "ComputeElevWeights.hpp"
#include "GNSSconstants.hpp"


namespace gpstk
{

    // Return a string identifying this object.
    std::string ComputeElevWeights::getClassName() const
    { return "ComputeElevWeights"; }



      /* Return a satTypeValueMap object, adding the new data
       * generated when calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& ComputeElevWeights::Process( const CommonTime& time,
                                                  satTypeValueMap& gData )
        noexcept(false)
    {

        try
        {
            SatIDSet satRejectedSet;

            // Loop through all the satellites
            for(satTypeValueMap::iterator it = gData.begin();
                it != gData.end();
                ++it)
            {
                SatID sat( it->first );

                double elev( 0.0 );

                try
                {
                    elev = gData.getValue( sat, TypeID::elevation );
                }
                catch(...)
                {
                    satRejectedSet.insert( sat );
                    continue;
                }

                double weight;
                // Compute the weight according to elevation
                if (elev > 30.0)
                {
                    weight = 1.0;
                }
                else
                {
                    // Transform the unit of elevation from degree to radian
                    elev = elev * DEG_TO_RAD;

                    // Compute the elevation weight
                    weight = std::pow(2.0 * std::sin(elev), expotential);
                }
                (*it).second[TypeID::weight] = weight;
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

    }  // End of method 'ComputeElevWeights:Process()'


    void ComputeElevWeights::Process(gnssRinex& gData)
    {

        try
        {
            Process(gData.header.epoch, gData.body);
        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );
            GPSTK_THROW(e);
        }

    }  // End of method 'ComputeElevWeights::Process()'

}  // End of namespace gpstk
