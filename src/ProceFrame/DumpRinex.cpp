#pragma ident "$Id$"

/**
 * @file DumpRinex.hpp
 * This class dumps the values inside a GNSS Data Structure.
 */


#include "DumpRinex.hpp"

using namespace gpstk::StringUtils;

namespace gpstk
{

      // Return a string identifying this object.
   std::string DumpRinex::getClassName() const
   { return "DumpRinex"; }


      /* Dumps data from a gnnsRinex object.
       *
       * @param gData    Data object holding the data.
       */
   satTypeValueMap& DumpRinex::Process( CommonTime& epoch, 
                                         string& markerName,
                                         satTypeValueMap& gData )
      noexcept(false)
   {

      try
      {

            // Iterate through all items in the GNSS Data Structure
         for( satTypeValueMap::const_iterator it = gData.begin();
              it!= gData.end();
              it++ )
         {

             // system
             SatelliteSystem sys = (*it).first.system;

             // get typeSet for current sys
             TypeIDSet typeSet = sysTypes[sys];

             // Declare a 'YDSTime' object to ease printing
             YDSTime time( epoch );

             (*outStr) << fixed
                       << setprecision(6) 
                       << time.year << " "
                       << time.doy << " "
                       << rightJustify(asString(time.sod,6), 12) << " ";

             (*outStr) << markerName << " ";

             // Then, print satellite (system and PRN)
             (*outStr) << (*it).first << " ";

                 // Iterate through all 'tvMap'
             for( typeValueMap::const_iterator itObs = (*it).second.begin();
                  itObs != (*it).second.end();
                  itObs++ )
             {
                 if( typeSet.find((*itObs).first) != typeSet.end() )
                 {

                     (*outStr) << (*itObs).first << " ";

                     (*outStr) << fixed
                               << setprecision(3)
                               << (*itObs).second << " ";

                 }

             }  // End of 'for( typeValueMap::const_iterator itObs = ... )'

             // Print end of line
             (*outStr) << std::endl;

         }  // End of 'for( satTypeValueMap::const_iterator it = ...'

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }
      return gData;

   }  // End of method 'DumpRinex::Process()'


}  // End of namespace gpstk
