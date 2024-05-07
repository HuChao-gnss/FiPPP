#pragma ident "$Id$"

/**
 * @file RequiredObs.cpp
 * This class filters out satellites with observations grossly out of bounds.
 */

#include "RequiredObs.hpp"
#include "DataStructures.hpp"

using namespace fippp;

namespace gpstk
{

    // Return a string identifying this object.
    std::string RequiredObs::getClassName() const
    { return "RequiredObs"; }


      /* Method to add a set of TypeID's to be required.
       *
       * @param typeSet    Set of TypeID's to be required.
       */
    RequiredObs& RequiredObs::addRequiredType( const SatelliteSystem& sys,
                                               const TypeIDSet& typeSet )
    {
        if(sys == SatelliteSystem::GPS)
        {
            requiredTypeSetOfGPS.insert( typeSet.begin(), typeSet.end() );
        }
        else if(sys == SatelliteSystem::Glonass)
        {
            requiredTypeSetOfGLO.insert( typeSet.begin(), typeSet.end() );
        }
        else if(sys == SatelliteSystem::Galileo)
        {
            requiredTypeSetOfGAL.insert( typeSet.begin(), typeSet.end() );
        }
        else if(sys == SatelliteSystem::BeiDou)
        {
            requiredTypeSetOfBDS.insert( typeSet.begin(), typeSet.end() );
        }

        return (*this);

    }  // End of method 'RequiredObs::addRequiredTypeOfGPS()'


      // Return a satTypeValueMap object, filtering the target observables.
      //
      // @param gData     Data object holding the data.
      //
    satTypeValueMap& RequiredObs::Process(satTypeValueMap& gData)
        noexcept(false)
    {

        try
        {
            SatIDSet satRejectedSet;

            TypeIDSet requiredTypeSet;

            // Loop through all the satellites
            for(satTypeValueMap::iterator satIt = gData.begin();
                satIt != gData.end();
                ++satIt)
            {
                SatID sat( satIt->first );

                if(sat.system == SatelliteSystem::GPS)
                {
                    requiredTypeSet = requiredTypeSetOfGPS;
                }
                else if(sat.system == SatelliteSystem::Glonass)
                {
                    requiredTypeSet = requiredTypeSetOfGLO;
                }
                else if(sat.system == SatelliteSystem::Galileo)
                {
                    requiredTypeSet = requiredTypeSetOfGAL;
                }
                else if(sat.system == SatelliteSystem::BeiDou)
                {
                    requiredTypeSet = requiredTypeSetOfBDS;
                }

                // Check all the indicated TypeID's
                if(requiredTypeSet.empty())
                {
                    satRejectedSet.insert( sat );
                    continue;
                }
                // Reserve the required observation type, 
                // when IF12345, IF1234, IF123 and BDSComb must be selected
                if ((opts.bdsComb&&sat.system==SatelliteSystem::BeiDou)||
                    opts.ionoopt==Options::UC1||
                    opts.ionoopt==Options::IF12||
                    opts.ionoopt==Options::IF123||
                    opts.ionoopt==Options::IF1234||
                    opts.ionoopt==Options::IF12345)
                {
                    for (TypeIDSet::const_iterator typeIt = requiredTypeSet.begin();
                        typeIt != requiredTypeSet.end();
                        ++typeIt)
                    {
                        // Try to find required type
                        typeValueMap::iterator it(satIt->second.find(*typeIt));

                        if (sat.system == SatelliteSystem::BeiDou)
                        {
                            TypeIDSet bdsSet;   // not own type
                            if (sat.id < 19)
                            {   // not own type
                                bdsSet.insert(TypeID::C1C);
                                bdsSet.insert(TypeID::C5C);
                                bdsSet.insert(TypeID::C8C);
                                bdsSet.insert(TypeID::L1C);
                                bdsSet.insert(TypeID::L5C);
                                bdsSet.insert(TypeID::L8C);
                            }
                            else
                            {
                                bdsSet.insert(TypeID::C7C);
                                bdsSet.insert(TypeID::L7C);
                            }
                            if (bdsSet.find(*typeIt) != bdsSet.end())
                                continue;
                        }
                        // Now, check if this TypeID exists in this data structure
                        if (it == satIt->second.end())
                        {
                            // If we couldn't find type, then schedule this
                            // satellite for removal
                            satRejectedSet.insert(sat);
                            // It is not necessary to keep looking
                            //typeIt = requiredTypeSet.end();
                            //--typeIt;
                            break;
                        }

                    }   // End of  looping over satellite observation types
                }   // End of if ionoopt options
                
                
            }   // End of looping through all satellite data in the current epoch

            // Let's remove satellites without all TypeID's
            gData.removeSatID(satRejectedSet);

            return gData;

        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );
            GPSTK_THROW(e);
        }

    }  // End of 'RequiredObs::Process()'


} // End of namespace gpstk
