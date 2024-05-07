#pragma ident "$Id$"

/**
 * @file DetectCSMW.cpp
 * This is a class to detect cycle slips using the Melbourne-Wubbena
 * combination.
 */


#include "DetectCSMW.hpp"

using namespace std;

#define debug 0

namespace gpstk
{

    // Return a string identifying this object.
    std::string DetectCSMW::getClassName() const
    { return "DetectCSMW"; }


    /* Common constructor.
     *
     * @param mLambdas      Maximum deviation allowed before declaring
     *                      cycle slip (in number of Melbourne-Wubbena
     *                      wavelenghts).
     * @param dtMax         Maximum interval of time allowed between two
     *                      successive epochs, in seconds.
     */
    DetectCSMW::DetectCSMW( const double& dtMax)
    {
        setDeltaTMax(dtMax);
    }


    /* Return a satTypeValueMap object, adding the new data generated
     * when calling this object.
     *
     * @param epoch     Time of observations.
     * @param gData     Data object holding the data.
     * @param epochflag Epoch flag.
     */
    satTypeValueMap& DetectCSMW::Process( const CommonTime& epoch,
                                          const CommonTime& firstEpoch,
                                            satTypeValueMap& gData,
                                            const short& epochflag )
        noexcept(false)
    {

        try
        {
            double value(0.0);

            SatIDSet satRejectedSet;

            // Loop through all the satellites
            for(satTypeValueMap::iterator it = gData.begin();
                it != gData.end();
                ++it)
            {
                SatID sat = (*it).first;
                SatelliteSystem sys = sat.system;

                // get mwType for current system
                TypeIDSet mwTypeSet = sysTypeSet[sys];

                for(auto typeIt=mwTypeSet.begin(); typeIt!=mwTypeSet.end(); typeIt++)
                {
                    TypeID mwType = (*typeIt);

                    // wavelengthMW of MW-combination, see LinearCombination
                    wavelengthMW = wavelengthOfMW(sys, mwType);

                    // get variance of MW-combinations
                    varianceMW = varOfMW(sys, mwType);

                    try
                    {
                        // Try to extract the values
                        value = (*it).second(mwType);
                    }
                    catch(...)
                    {
                        continue;
                    }

                    // If everything is OK, then get the new values inside the
                    // structure. This way of computing it allows concatenation of
                    // several different cycle slip detectors
                    double csFlag = getDetection( epoch,  firstEpoch,
                                                  sat, 
                                                  epochflag, 
                                                  mwType,
                                                  value);

                    if( (*it).second.find(TypeID::CSFlag) != (*it).second.end() )
                    {
                        double lastCSFlag = (*it).second[TypeID::CSFlag];
                        (*it).second[TypeID::CSFlag] = lastCSFlag || csFlag;
                    }
                    else
                    {
                        (*it).second[TypeID::CSFlag] = csFlag;
                    }

                }


            }

            gData.removeSatID(satRejectedSet);

            return gData;
        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );
            GPSTK_THROW(e);
        }

    }  // End of method 'DetectCSMW::Process()'


    /** Method that implements the Melbourne-Wubbena cycle slip
     *  detection algorithm
     *
     * @param epoch     Time of observations.
     * @param sat       SatID.
     * @param epochflag Epoch flag.
     * @param mw        Current MW observation value.
     */
    double DetectCSMW::getDetection( const CommonTime& epoch,
                                     const CommonTime& firstepoch,
                                     const SatID& sat,
                                     const short& epochflag,
                                     const TypeID& mwType,
                                     const double& mw )
    {

        bool reportCS(false);

        // Difference between current and former epochs, in sec
        double currentDeltaT(0.0);

        // Difference between current and former MW values
        double currentBias(0.0);

        // Get the difference between current epoch and former epoch,
        // in seconds
        currentDeltaT = (epoch - mwData[sat][mwType].formerEpoch);

        if(debug)
        {
            cout << "epoch:" << YDSTime(epoch) << endl;
            cout << "sat:" << sat << endl;
            cout << "currentDeltaT:" << currentDeltaT << endl;
        }

        // Store current epoch as former epoch
        mwData[sat][mwType].formerEpoch = epoch;

        // Difference between current value of MW and average value
        currentBias = std::abs(mw - mwData[sat][mwType].meanMW);

        if(debug)
        {
            cout << "currentBias:" << currentBias << endl;
        }

        // Increment window size
        mwData[sat][mwType].windowSize++;
        /**
         * cycle-slip condition
         * 
         * 1. if data interrupt for a given time gap, then cyce slip should be set
         * 2. if current bias is greater than 1 cycle and greater than 4 sigma of mean mw.
         */
        double sigLimit = 4 * std::sqrt( mwData[sat][mwType].varMW ) ;

        if(debug)
        {
            cout << "deltaTMax:" << deltaTMax << endl;
            cout << "wavelengthMW:" << wavelengthMW << endl;
            cout << "sigLimit:" << sigLimit << endl;
        }

        if( currentDeltaT > deltaTMax || (currentBias > minCycles*wavelengthMW && currentBias > sigLimit) )
        {
            // if cycle slip happened

            // reset the filter window size/meanMW/InitialVarofMW
            mwData[sat][mwType].meanMW     = mw;
            mwData[sat][mwType].varMW      = varianceMW;
            mwData[sat][mwType].windowSize = 1;

            if(debug)
            {
                cout << "CS happened!" << endl;
            }

            return 1.0;
        }


        // MW bias from the mean value
        double mwBias(mw - mwData[sat][mwType].meanMW);
        double size( static_cast<double>(mwData[sat][mwType].windowSize) );

        // Compute average
        mwData[sat][mwType].meanMW += mwBias / size;

        // Compute variance
        // Var(i) = Var(i-1) + [ ( mw(i) - meanMW)^2/(i)- 1*Var(i-1) ]/(i);
        mwData[sat][mwType].varMW  += ( mwBias*mwBias - mwData[sat][mwType].varMW ) / size;

        return 0.0;

    }  // End of method 'DetectCSMW::getDetection()'


}  // End of namespace gpstk
