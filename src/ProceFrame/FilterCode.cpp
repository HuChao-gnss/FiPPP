
/**
* @file FilterCode.cpp
* This class filters out satellites with observations grossly out of bounds.
*
*/

#include "FilterCode.hpp"
#include "SatelliteSystem.hpp"

#define debug 0

namespace gpstk
{
    using namespace StringUtils;
   // Returns a string identifying this object.
   std::string FilterCode::getClassName() const
   { return "FilterCode"; }


   // Returns a gnssRinex object, filtering the target observables.
   //
   // @param gData     Data object holding the data.
   //
   satTypeValueMap& FilterCode::Process( SysTypesMap& mapObsTypes,
                                         std::map<RinexSatID/*SatID*/, TypeIDVec>& satTypes,
                                         satTypeValueMap& gData )
      noexcept(false)
   {

       try
       {
           // Now, Let's check the range of the code observables.
           TypeIDSet tempFilterTypeSet;

           SatIDSet satRejectedSet;

           // Loop through all the satellites
           for ( satTypeValueMap::iterator satIt = gData.begin();
                 satIt != gData.end();
                 ++satIt )
           {
               // get the satellite  identify
               SatelliteSystem system = (*satIt).first.system;

               // convert string-system to char-system
               char sysChar = asChar(system);//system.toChar();

               // convert 1-char to 1-string
               string sysString = string(1, sysChar);

               std::vector<TypeID> typeVec = mapObsTypes[sysString];

               if(debug)
               {
                   for(int i=0; i<typeVec.size(); i++)
                   {
                       cout << getClassName() << ":" << typeVec[i] << " ";
                   }
                   cout << endl;
               }

               std::vector<TypeID> goodTypes = typeVec;

               // filter observable with zero values
               for(int i=0; i<typeVec.size(); i++)
               {
                   TypeID type = typeVec[i];
                   if((*satIt).second[type] == 0.0)
                   {
                       (*satIt).second.removeTypeID(type);

                       vector<TypeID>::iterator typeIt 
                           = find(goodTypes.begin(), goodTypes.end(), type);

                       if(typeIt != goodTypes.end())
                       {
                          goodTypes.erase(typeIt);
                       }
                   }
               }

               // now, let's check the range 
               for(int i=0; i<typeVec.size(); i++)
               {
                   TypeID type = typeVec[i];

                   // judge whether is is code observable
                   string typeStr = type.asString();

                   // code observable
                   // there are 4-type of observables, i.e. C/L/D/S 
                   if(typeStr[0] == 'C')
                   {
                       // Try to extract the values
                       if((*satIt).second.find(type) != (*satIt).second.end())
                       {
                           double value = (*satIt).second[type];

                           // Now, check that the value is within bounds
                           if ( !( checkValue(value) ) )
                           {

                               // remove current code observable
                               (*satIt).second.removeTypeID(type);

                               // remove type from goodTypes
                               vector<TypeID>::iterator typeIt 
                                   = find(goodTypes.begin(), goodTypes.end(), type);

                               if(typeIt != goodTypes.end())
                               {
                                  goodTypes.erase(typeIt);
                               }

                               // construct corresponding phase type
                               string phaseStr = typeStr;
                               phaseStr[0] = 'L';

                               // remove corresponding phase obs, 
                               TypeID phaseType(phaseStr);
                               if((*satIt).second.find(phaseType) != (*satIt).second.end())
                               {
                                   (*satIt).second.removeTypeID(phaseType);
                               }

                               // remove phaseType from goodTypes
                               vector<TypeID>::iterator typeIt2
                                   = find(goodTypes.begin(), goodTypes.end(), phaseType);

                               if(typeIt2 != goodTypes.end())
                               {
                                  goodTypes.erase(typeIt2);
                               }

                               // signal strength observation
                               string ssStr = typeStr;
                               ssStr[0] = 'S';

                               TypeID ssType(ssStr);
                               if((*satIt).second.find(ssType) != (*satIt).second.end())
                               {
                                   (*satIt).second.removeTypeID(ssType);
                               }

                               // remove ssType from goodTypes
                               vector<TypeID>::iterator typeIt3
                                   = find(goodTypes.begin(), goodTypes.end(), ssType);

                               if(typeIt3 != goodTypes.end())
                               {
                                  goodTypes.erase(typeIt3);
                               }

                               // signal strength observation
                               string dopplerStr = typeStr;
                               dopplerStr[0] = 'D';

                               TypeID dopplerType(dopplerStr);
                               if((*satIt).second.find(dopplerType) != (*satIt).second.end())
                               {
                                   (*satIt).second.removeTypeID(dopplerType);
                               }

                               // remove dopplerType from goodTypes
                               vector<TypeID>::iterator typeIt4
                                   = find(goodTypes.begin(), goodTypes.end(), dopplerType);

                               if(typeIt4 != goodTypes.end())
                               {
                                  goodTypes.erase(typeIt4);
                               }

                               if(debug)
                               {
                                   cout << "sat:" << (*satIt).first 
                                        << "type:" << type
                                        << "value:" << value
                                        << "phaseType:" << phaseType
                                        << endl;;
                               }
                           }
                       }
                   }
               }

               if(goodTypes.size() > 0)
               {
                   // good observation types for current satellite.
                   satTypes[(*satIt).first] = goodTypes;
               }
               else
               {
                   satRejectedSet.insert((*satIt).first);
               }

           }                       

           if(debug)
           {
               cout << "now, after filter code, good types for all sat are:" << endl;
               for(auto satIt=satTypes.begin(); satIt!=satTypes.end(); satIt++)
               {
                   cout << (*satIt).first << " "  << endl;
                   TypeIDVec typeVec = (*satIt).second;

                   for(int i=0; i<typeVec.size(); i++)
                   {
                       cout << typeVec[i] << " ";
                   }

                   cout << endl;
               }
           }

           // Remove satellites with missing data
           gData.removeSatID(satRejectedSet);

           return gData;

       }
       catch(Exception& u)
       {
               // Throw an exception if something unexpected happens
               ProcessingException e( getClassName() + ":"
                       + u.what() );

               GPSTK_THROW(e);
       }


   }  // End of 'FilterCode::Process()'



} // End of namespace gpstk
