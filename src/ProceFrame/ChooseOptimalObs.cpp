
/**
 *@file ChooseOptimalObs.cpp
 *
 * this class choose the optimal observation for current carrier-band according
 * to the defined priori order.
 **/

using namespace std;

#include "ChooseOptimalObs.hpp"
#include "SatelliteSystem.hpp"

#define debug 0

namespace gpstk
{
    using namespace StringUtils;
   // this defines the priority order of all observed types
   // characters are ORDERED from best to worst

   // see references:
   // PRANGE L,ORLIAC E,DACH R,et al.
   // CODE's five-system orbit and clock solution:
   // The challenges of multi-GNSS data analysis[J].Journal of Geodesy,2017,91(4):345-360.

   // Notes:
   // according to PRANGE(2017), the priority order of the observed signals with a high 
   // availability are preferred (except GPS C1C)

   CharCharStringMap ChooseOptimalObs::priorityCodes = ChooseOptimalObs::init();
   
   // initialize
   CharCharStringMap ChooseOptimalObs::init()
   {
       CharCharStringMap codes;

       // GPS
       codes['G']['1'] = "WCSLXPYMN";   // except no C1N
       //codes['G']['1'] = "CPWYMNSL";   // except no C1N
       codes['G']['2'] = "WCDSLXPYMN";  // except no C2N
       //codes['G']['2'] = "PYWCMNDSLX";  // except no C2N
       codes['G']['5'] = "XQI";

       // BDS
       codes['C']['1'] = "DPXAN"; // no C1N
       codes['C']['2'] = "IXQ";
       codes['C']['5'] = "DPX";
       codes['C']['6'] = "IXQA";
       codes['C']['8'] = "DPX";
       // BDS-2(IQX) and BDS-3(DPZ) B2b band
       codes['C']['7'] = "IXQDPZ";

       // Glonass
       codes['R']['1'] = "CP";
       //codes['R']['1'] = "PC";
       codes['R']['2'] = "PC";
       codes['R']['3'] = "IQX";
       codes['R']['4'] = "ABX";
       codes['R']['6'] = "ABX";

       // Galileo
       codes['E']['1'] = "XCABZ";
       codes['E']['5'] = "XQI";
       codes['E']['6'] = "XCABZ";
       codes['E']['7'] = "XQI";
       codes['E']['8'] = "XQI";

       // SBAS
       codes['S']['1'] = "C";
       codes['S']['5'] = "IQX";

       // QZSS
       codes['J']['1'] = "XCSLZ";
       codes['J']['2'] = "XLS";
       codes['J']['5'] = "XQIDPZ";
       codes['J']['6'] = "XSLEZ";

       // IRNSS
       codes['I']['5'] = "ABCX";
       codes['I']['9'] = "ABCX";

       return codes;
   };


   // Returns a string identifying this object.
   std::string ChooseOptimalObs::getClassName() const
   { return "ChooseOptimalObs"; }


   // Returns a Rx3ObsData object, filtering the target observables.
   //
   // @param gData     Data object holding the data.
   //
   satTypeValueMap& ChooseOptimalObs::Process( SysTypesMap& mapObsTypes,
                                               std::map<RinexSatID/*SatID*/, TypeIDVec>& satTypes,
                                               satTypeValueMap& gData )
      noexcept(false)
   {
       try
       {
           // only choose the optimal CODE(C) and PHASE(L) observables.
           string otStr = "CL";
           for(auto satIt = gData.begin(); satIt!= gData.end(); satIt++)
           {
                 // get the satellite  identify
               SatelliteSystem system = (*satIt).first.system;

               // convert string-system to char-system
               char sysChar = asChar(system);// .toChar();

               std::vector<TypeID> prioriTypes{};

               // get observed types for current system
               std::vector<TypeID> obsTypes = mapObsTypes[string(1, sysChar)];

               // map of <carrier-band, types> 
               std::map<char, std::string> bandCodes = priorityCodes[sysChar];
               for(auto bandIt = bandCodes.begin(); bandIt!= bandCodes.end(); bandIt++ )
               {
                   char band = (*bandIt).first;
                   string codes = (*bandIt).second;
  
                   // construct 4-char TypeID using sysChar/band/obsType/trackingCode
                   for(int i=0; i<otStr.size(); i++)
                   {
                       for(int j=0; j<codes.size(); j++)
                       {
                           char typeStr[5] ={0};  
                           typeStr[0] = otStr[i];
                           typeStr[1] = band;
                           typeStr[2] = codes[j];
                           typeStr[3] = sysChar;

                           // Rinex 3.04 don't have C1NG/C2NG/C1NC
                           if( string(typeStr) == "C1NG" || 
                               string(typeStr) == "C2NG" ||
                               string(typeStr) == "C1NC" ) 
                           {
                               continue;
                           }

                           TypeID type(typeStr);

                           // whether is stored in Rinex header
                           // vector/list don't have find method.
                           std::vector<TypeID>::iterator 
                               it = find(obsTypes.begin(), obsTypes.end(), type);
                           if( it != obsTypes.end())
                           {
                               // if data with this type is found
                               if((*satIt).second.find(type) != (*satIt).second.end())
                               {
                                   prioriTypes.push_back(type);
                                   break;
                               }
                           }
                       }
                   }
               }

               if(debug)
               {
                   cout << "sat:" << (*satIt).first << " prioriTypes are:"<< endl;
                   for(int i=0; i<prioriTypes.size(); i++)
                   {
                       cout << prioriTypes[i] << " ";
                   }
                   cout << endl;
               }

               // insert prioriTypes into Rx3ObsData
               satTypes[(*satIt).first] = prioriTypes;

               // define and remove other useless codes
               vector<TypeID> otherTypes;

               // now, let's remove the other observed types
               for(int i=0; i<obsTypes.size(); i++)
               {
                   std::vector<TypeID>::iterator it 
                       = find(prioriTypes.begin(), prioriTypes.end(), obsTypes[i]);

                   if(it == prioriTypes.end() )
                   {
                       otherTypes.push_back(obsTypes[i]);
                   }
               }

               // now, let's remove other observation types
               for(int i=0; i<otherTypes.size();i++)
               {
                   (*satIt).second.removeTypeID(otherTypes[i]);
               }

           }

           return gData;
       }
       catch(Exception& u)
       {
           // Throw an exception if something unexpected happens
           ProcessingException e( getClassName() + ":"
                   + u.what() );

           GPSTK_THROW(e);
       }

   }  // End of 'ChooseOptimalObs::Process()'



} // End of namespace gpstk
