#pragma ident "$Id$"

/**
 * @file DumpTypeSatValueData.hpp
 * This class dumps the values inside a GNSS Data Structure.
 */


#include "GNSSconstants.hpp"
#include "DumpTypeSatValueData.hpp"
#include "YDSTime.hpp"
#include "RinexSatID.hpp"

using namespace gpstk::StringUtils;

namespace gpstk
{

      // Return a string identifying this object.
   std::string DumpTypeSatValueData::getClassName() const
   { return "DumpTypeSatValueData"; }


      /* Dumps data from a gnnsRinex object.
       *
       * @param gData    Data object holding the data.
       */
   satTypeValueMap& DumpTypeSatValueData::Process( CommonTime& epoch, 
                                                   string& markerName,
                                                   satTypeValueMap& gData )
      noexcept(false)
   {

      try
      {
          // satValueData for current type
          satValueMap satValueData;

          // now, we will convert satTypeValueMap to typeSatValueMap for ploting using gnuplot 
          if( dumpSys == SatelliteSystem::GPS )
          {
              for(int i=1; i<=MAX_PRN_GPS; i++)
              {
                  RinexSatID sat = /*SatID*/RinexSatID(i, SatelliteSystem::GPS);

                  satTypeValueMap::iterator satIt = gData.find(sat);
                  if(satIt!=gData.end())
                  {
                      if((*satIt).second.find( dumpType )!=(*satIt).second.end())
                      {
                          satValueData[sat] = (*satIt).second[dumpType];
                      }
                      else
                      {
                          satValueData[sat] = 0.0;
                      }
                  }
                  else
                  {
                      satValueData[sat] = 0.0;
                  }
              }
          }
          else if( dumpSys == SatelliteSystem::Galileo )
          {
              for(int i=1; i<=MAX_PRN_GAL; i++)
              {
                  RinexSatID/*SatID*/ sat = /*SatID*/RinexSatID(i, SatelliteSystem::Galileo);

                  satTypeValueMap::iterator satIt = gData.find(sat);
                  if(satIt!=gData.end())
                  {
                      if((*satIt).second.find( dumpType )!=(*satIt).second.end())
                      {
                          satValueData[sat] = (*satIt).second[dumpType];
                      }
                      else
                      {
                          satValueData[sat] = 0.0;
                      }
                  }
                  else
                  {
                      satValueData[sat] = 0.0;
                  }
              }
          }
          else if( dumpSys == SatelliteSystem::BeiDou)
          {
              // BDS
              for(int i=1; i<=MAX_PRN_BDS; i++)
              {
                  RinexSatID/*SatID*/ sat = /*SatID*/RinexSatID(i,SatelliteSystem::BeiDou);

                  satTypeValueMap::iterator satIt = gData.find(sat);
                  if(satIt!=gData.end())
                  {
                      if((*satIt).second.find( dumpType )!=(*satIt).second.end())
                      {
                          satValueData[sat] = (*satIt).second[dumpType];
                      }
                      else
                      {
                          satValueData[sat] = 0.0;
                      }
                  }
                  else
                  {
                      satValueData[sat] = 0.0;
                  }
              }
          }
          else if( dumpSys == SatelliteSystem::Glonass)
          {
              // Glonass
              for(int i=1; i<=MAX_PRN_GLO; i++)
              {
                  RinexSatID/*SatID*/ sat = RinexSatID/*SatID*/(i,SatelliteSystem::Glonass);

                  satTypeValueMap::iterator satIt = gData.find(sat);
                  if(satIt!=gData.end())
                  {
                      if((*satIt).second.find( dumpType )!=(*satIt).second.end())
                      {
                          satValueData[sat] = (*satIt).second[dumpType];
                      }
                      else
                      {
                          satValueData[sat] = 0.0;
                      }
                  }
                  else
                  {
                      satValueData[sat] = 0.0;
                  }
              }
          }

          YDSTime time(epoch);

          // now, let's write data for current type for all satellites
          (*pDumpStrm) 
              << fixed
              << setprecision(6) 
              << time.year << " "
              << time.doy << " "
              << rightJustify( asString(time.sod, 6), 12) << " ";

          (*pDumpStrm) 
              << markerName << " ";
          (*pDumpStrm) 
              << dumpType << " ";

          // now, output satValueData
          for(auto satIt = satValueData.begin(); satIt!=satValueData.end(); satIt++)
          {
              (*pDumpStrm) 
                  << (*satIt).second << " " ;
          }

          (*pDumpStrm) << endl;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }
      return gData;
   }  // End of method 'DumpTypeSatValueData::Process()'


}  // End of namespace gpstk
