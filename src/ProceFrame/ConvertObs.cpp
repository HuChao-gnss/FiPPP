#pragma ident "$Id$"


#include "ConvertObs.hpp"

#define debug 0

using namespace std;

namespace gpstk
{

      // Return a string identifying this object.
    string ConvertObs::getClassName() const
    { return "ConvertObs"; }


      /* Return a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
    satTypeValueMap& ConvertObs::Process(std::map<RinexSatID/*SatID*/, TypeIDVec>& satTypes,
                                         std::map<RinexSatID/*SatID*/, TypeIDVec>& satShortTypes,
                                         satTypeValueMap& gData )
        noexcept(false)
    {
        try
        {
            SatIDSet satRejectedSet;

            // Loop through all the satellites
            for(auto it = gData.begin(); it != gData.end(); ++it)
            {
                vector<TypeID> prioriTypes = satTypes[(*it).first];

                vector<TypeID> shortTypes;

                for(int i=0; i<prioriTypes.size(); i++)
                {
                    string longTypeStr = prioriTypes[i].asString();
                    // L1CG=> L1G; C1WG=>C1G
                    TypeID shortType = TypeID(longTypeStr.substr(0,2) + longTypeStr.substr(3,1));
                    (*it).second[shortType] = (*it).second[TypeID(longTypeStr)];
                    shortTypes.push_back(shortType);
                }

                if(debug)
                {
                    for(int i=0; i<shortTypes.size(); i++)
                    {
                        cout << getClassName() << shortTypes[i] << " ";
                    }
                    cout << endl;
                }

                // insert shortTypes into map
                satShortTypes[(*it).first] = shortTypes;
            }

            return gData;
        }
        catch(Exception& u)
        {
            // Throw an exception if something unexpected happens
            ProcessingException e( getClassName() + ":" + u.what() );
            GPSTK_THROW(e);
        }

    }  // End of method 'ConvertObs::Process()'

}  // End of namespace gpstk
