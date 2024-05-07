#pragma ident "$Id$"

/**
 * @file DumpTypeSatValueData.hpp
 * This class dumps the values inside a GNSS Data Structure (GDS).
 */

#ifndef DumpTypeSatValueData_HPP
#define DumpTypeSatValueData_HPP



#include <ostream>
#include "gnssData.hpp"


namespace gpstk
{

   class DumpTypeSatValueData                         
   {
   public:

         /// Default constructor
      DumpTypeSatValueData()
      {};



         /** Dumps data from a satTypeValueMap object.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( CommonTime& epoch, 
                                        string& markerName,  
                                        satTypeValueMap& gData )
         noexcept(false);


      virtual void Process(gnssRinex& gRin)
         noexcept(false)
      {
          Process(gRin.header.epoch, 
                  gRin.header.source.sourceName,  
                  gRin.body);
      };

         /** Sets stream object used for output.
          *
          * @param out           Stream object used for output.
          */
      virtual DumpTypeSatValueData& setDumpStream( std::fstream& stream )
      {   
          pDumpStrm = &stream;
          return *this;
      };


         /** Method to set the TypeID to be printed.
          *
          * @param type      TypeID of data values to be printed.
          *
          * \warning The previously set type values will be deleted. If this
          * is not what you want, see method addType.
          *
          * \warning If no TypeIDs are specified, then ALL TypeIDs present in
          * the GDS will be printed.
          */
      virtual DumpTypeSatValueData& setType( const SatelliteSystem& sys, const TypeID& type )
      { 
          dumpSys = sys;
          dumpType = type;

          return (*this); 
      };

         /// Return a string identifying this object.
      virtual std::string getClassName(void) const;

         /// Destructor
      virtual ~DumpTypeSatValueData() {};

   private:

      std::string dumpFile;

      SatelliteSystem dumpSys;
      TypeID dumpType;
      std::fstream* pDumpStrm;

   }; // End of class 'DumpTypeSatValueData'


}  // End of namespace gpstk

#endif  // DumpTypeSatValueData_HPP
