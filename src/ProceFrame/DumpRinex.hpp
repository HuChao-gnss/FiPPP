#pragma ident "$Id$"

/**
 * @file DumpRinex.hpp
 * This class dumps the values inside a GNSS Data Structure (GDS).
 */

#ifndef DumpRinex_HPP
#define DumpRinex_HPP


#include <ostream>
#include "gnssData.hpp"

namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class dumps the values inside a GNSS Data Structure (GDS), and
       *  therefore is meant to be used with the GDS objects found in
       *  "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *       // Create the input obs file stream
       *    RinexObsStream rin("ebre0300.02o");
       *
       *   gnssRinex gRin;
       *
       *   DumpRinex dumpObj;
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> dumpObj;
       *   }
       * @endcode
       *
       * The "DumpRinex" object will visit every satellite in the GNSS Data
       * Structure that is "gRin" and will print the information associated
       * with it to a pre-designated place.
       *
       * By default "DumpRinex" will print all information to 'std::cout' output
       * stream (which is a 'std::ostream'), and will include the epoch and the
       * station, as well as the type associated with each data value.
       *
       * The default behavior may be changed using the appropriate constructors
       * and/or methods. For instance, in order to dump information to file
       * 'model.out', not including station ID, and including only data of
       * satellite arc, cycle slip flag, and prefit residuals of code and phase,
       * we could do the following:
       *
       * @code
       *
       *       //// INITIALIZATION CODE HERE FOR OTHER OBJECTS...
       *
       *       // Create the output file stream
       *    ofstream dumperFile;
       *    dumperFile.open( "model.out", ios::out );
       *
       *       // You make want to set an specific precision for printing
       *    dumperFile << fixed << setprecision( 3 );
       *
       *   gnssRinex gRin;
       *
       *   DumpRinex dumpObj( dumperFile );
       *
       *      // Deactivate SourceID printing
       *   dumpObj.setPrintSourceID( false );
       *
       *      // Limit TypeIDs to print. It is a good idea to start with
       *      // 'setType()' to clear all TypeIDs previously set, and then
       *      // follow adding new TypeIDs using 'addType()'.
       *   dumpObj.setType(TypeID::satArc);
       *   dumpObj.addType(TypeID::CSL1);
       *   dumpObj.addType(TypeID::prefitC);
       *   dumpObj.addType(TypeID::prefitL);
       *
       *   while(rin >> gRin)
       *   {
       *      try
       *      {
       *         gRin  >> basicM
       *               >> correctObs
       *               >> compWindup
       *               >> computeTropo
       *               >> linear1      // Compute combinations
       *               >> pcFilter
       *               >> markCSLI2
       *               >> markCSMW
       *               >> markArc
       *               >> linear2      // Compute prefit residuals
       *               >> dumpObj
       *               >> pppSolver;
       *      }
       *      catch(...)
       *      {
       *         cerr << "Unknown exception at epoch: " << time << endl;
       *         continue;
       *      }
       *
       * @endcode
       *
       * Please note that, in order to dump a given TypeID, it must be present
       * in the GNSS Data Structure.
       *
       * A nice feature of "DumpRinex" objects is that they return the incoming
       * GDS without altering it, so they can be inserted wherever you need
       * them.
       */
   class DumpRinex                         
   {
   public:

         /// Default constructor
      DumpRinex()
         : outStr(&std::cout)
      {};


         /** Common constructor
          *
          * @param out           Stream object used for output.
          * @param printtype     Flag to print TypeID's.
          * @param printtime     Flag to print CommonTime's.
          * @param printstation  Flag to print SourceID's.
          *
          */
      DumpRinex( std::ostream& out)
         : outStr(&out)
      { };


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
      virtual DumpRinex& setOutputStream( std::ostream& out )
      { outStr = &out; return (*this); };


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
      virtual DumpRinex& setType( const SatelliteSystem& sys, const TypeID& type )
      { 
          sysTypes.clear(); 
          sysTypes[sys].insert(type); 
          return (*this); 
      };


         /** Method to add a TypeID to be printed.
          *
          * @param type          TypeID of data values to be added to the ones
          *                      being printed.
          */
      virtual DumpRinex& addType( const SatelliteSystem& sys, const TypeID& type )
      { 
          sysTypes[sys].insert(type); 
          return (*this); 
      };


         /** Method to set a set of TypeIDs to be printed.
          *
          * @param printSet       TypeIDSet of data values to be printed.
          *
          * \warning The previously set type values will be deleted. If this
          * is not what you want, see method addDiffType.
          */
      virtual DumpRinex& setTypeSet( const SatelliteSystem& sys, 
                                      const TypeIDSet& printSet )
      { 
          sysTypes.clear(); 
          sysTypes[sys] = printSet;
          return (*this); 
      };


         /** Method to add a set of TypeIDs to be printed.
          *
          * @param types      TypeIDSet of data values to be added to the
          *                   ones being printed.
          */
      virtual DumpRinex& addTypeSet( const SatelliteSystem& sys, 
                                      const TypeIDSet& types)
      {
          for(auto it=types.begin(); it!=types.end(); it++)
          {
              sysTypes[sys].insert((*it));
          }
          return (*this);
      };


         /// Method to clear the set of TypeIDs to be printed. If you do this,
         /// all TypeIDs that are present in GDS will be printed.
      virtual DumpRinex& clearTypeSet( void )
      { 
          sysTypes.clear(); 
          return (*this); 
      };


         /// Return a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~DumpRinex() {};


   private:


      /// Stream object used for output.
      std::ostream* outStr;

      std::map<SatelliteSystem, TypeIDSet> sysTypes;


   }; // End of class 'DumpRinex'

      //@}

}  // End of namespace gpstk

#endif  // DumpRinex_HPP
