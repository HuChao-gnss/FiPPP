#pragma ident "$Id$"

/**
 * @file ComputeStaTides.cpp
 * This class computes station tides.
 */


#include "ComputeStaTides.hpp"

using namespace std;

namespace gpstk
{

    // Return a string identifying this object.
    std::string ComputeStaTides::getClassName() const
    { return "ComputeStaTides"; }



    Triple ComputeStaTides::getTides( const CommonTime& time,
                                      const string& station )
        noexcept(false)
    {

        try
        {
            Triple solidCorr, oceanCorr, poleCorr;

            solidCorr = solid.getSolidTide(time, nominalPos);
            if(correctOTL)
            {
                try
                {
                    oceanCorr = ocean.getOceanLoading(time, station);
                }
                catch (...)
                {
                    oceanCorr = Triple(0.0, 0.0, 0.0);
                }
            }
            else
            {
                oceanCorr = Triple(0.0, 0.0, 0.0);
            }

            poleCorr = pole.getPoleTide(time, nominalPos);

            return solidCorr + oceanCorr + poleCorr;
        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );
            GPSTK_THROW(e);
        }

    }  // End of method 'ComputeStaTides::Process()'

    /** Return a satTypeValueMap object, adding the new data generated when
     *  calling this object.
     *
     * @param time      Epoch corresponding to the data.
     * @param gData     Data object holding the data.
     */
    satTypeValueMap& ComputeStaTides::Process( const CommonTime& time,
                                              satTypeValueMap& gData )
        noexcept(false)
    {
        Triple tide;
        tide = getTides(time, station);
        sourceTideData[source] = tide;
        return gData;//add at 09 25
    }


    void ComputeStaTides::Process(gnssRinex& gData)
        noexcept(false)
    {
        // solid
        solid.setReferenceSystem(*pRefSys);
        solid.setSolarSystem(*pSolSys);
        // ocean
        ocean.setBLQDataReader((*pBLQReader));
        ocean.setReferenceSystem(*pRefSys);
        // pole
        pole.setReferenceSystem(*pRefSys);

        // get receiver position from gnssDataHeader
        source = gData.header.source;
        station = source.sourceName;

        Process( gData.header.epoch, gData.body );

    }  // End of method 'ComputeStaTides::Process()'


}  // End of namespace gpstk
