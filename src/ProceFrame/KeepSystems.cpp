#pragma ident "$Id$"


#include "KeepSystems.hpp"
#include "SatelliteSystem.hpp"

using namespace std;

#define debug 0

namespace gpstk
{
    using namespace StringUtils;
      // Return a string identifying this object.
    std::string KeepSystems::getClassName() const
    { return "KeepSystems"; }



      /* Return a satTypeValueMap object, adding the new data generated when
       * calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& KeepSystems::Process( const CommonTime& time,
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
                std::vector<SatelliteSystem>::iterator sysIt 
                    = find(sysVec.begin(), sysVec.end(), (*it).first.system);

                // if not found in the sysVec to be kept, then remove the satellite
                if(sysIt == sysVec.end())
                {
                    satRejectedSet.insert((*it).first);
                }
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

    }  // End of method 'KeepSystems::Process()'


    void KeepSystems::setSystems(std::string sysStr)
    {
        sysVec.clear();
        for(int i=0; i<sysStr.size();i++)
        {
            char sysChar = sysStr[i];
            SatelliteSystem satSys = asSatelliteSystem(sysChar);//(sysChar);
            sysVec.push_back(satSys);
        }

        if(debug)
        {
            for(auto it=sysVec.begin(); it!=sysVec.end(); it++)
            {
                cout << asString(*it)/*.toString()*/ << " " ;
            }
            cout << endl;
        }
    }


} // End of namespace gpstk
