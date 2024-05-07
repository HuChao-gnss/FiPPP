#pragma ident "$Id$"

/**
 * @file ComputeWindUp.hpp
 * This class computes the wind-up effect on the phase observables, in radians.
 */

#ifndef ComputeWindUp_HPP
#define ComputeWindUp_HPP

//============================================================================
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009, 2011
//
//============================================================================



#include <string>
#include "Triple.hpp"
#include "Position.hpp"
#include "SunPosition.hpp"
#include "XvtStore.hpp"
#include "AntexReader.hpp"
#include "GNSSconstants.hpp"
#include "EclipsedSatFilter.hpp"
#include "DataStructures.hpp"
#include "gnssData.hpp"


namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class computes the wind-up effect on the phase observables,
       *  in radians.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       *      // Create the input obs file stream
       *   RinexObsStream rin("ebre0300.02o");
       *
       *      // Loads precise ephemeris object with file data
       *   SP3EphemerisStore SP3EphList;
       *   SP3EphList.loadFile("igs11513.sp3");
       *
       *      // Sets nominal position of receiver
       *   Position nominalPos(4833520.3800, 41536.8300, 4147461.2800);
       *
       *   gnssRinex gRin;
       *
       *   ComputeWindUp windup( SP3EphList,
       *                         nominalPos );
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> windup;
       *   }
       * @endcode
       *
       * The "ComputeWindUp" object will visit every satellite in the GNSS
       * data structure that is "gRin" and will compute the corresponding
       * receiver-satellite wind-up effect, in radians.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the wind-up inserted in it. Be warned
       * that if a given satellite does not have the observations required,
       * it will be summarily deleted from the data structure.
       *
       * \warning ComputeWindUp objects store their internal state, so
       * you MUST NOT use the SAME object to process DIFFERENT data streams.
       *
       * \warning It is recommended that ComputeWindUp objects are used after
       * calling a SatArcMarker object, because they work better when cycle
       * slips are properly tracked.
       *
       */
   class ComputeWindUp                         
   {
   public:

         /// Default constructor
      ComputeWindUp()
         : pEphemeris(NULL), nominalPos(0.0, 0.0, 0.0),
           pAntexReader(NULL),
		   peclipse(NULL)
      {
          Init();
      };


         /** Common constructor
          *
          * @param ephem     Satellite ephemeris.
          * @param stapos    Nominal position of receiver station.
          * @param filename  Name of "PRN_GPS"-like file containing
          *                  satellite data.
          *
          * @warning If filename is not given, this class will look for a
          * file named "PRN_GPS" in the current directory.
          */
      ComputeWindUp( XvtStore<SatID>& ephem,
                     const Position& stapos)
         : pEphemeris(&ephem), nominalPos(stapos), 
		   pAntexReader(NULL),peclipse(NULL)
      { 
          Init();
      };


         /** Common constructor. Uses satellite antenna data from an Antex file.
          *
          * @param ephem     Satellite ephemeris.
          * @param stapos    Nominal position of receiver station.
          * @param antexObj  AntexReader object containing satellite
          *                  antenna data.
          *
          * @warning If 'AntexReader' object holds an Antex file with relative
          * antenna data, a simple satellite phase center model will be used.
          */
      ComputeWindUp( XvtStore<SatID>& ephem,
                     const Position& stapos,
                     AntexReader& antexObj )
         : pEphemeris(&ephem), nominalPos(stapos), pAntexReader(&antexObj),
		   peclipse(NULL)
      { 
          Init();
      };


         /** Common constructor. Uses satellite antenna data from an Antex file.
          *
          * @param stapos    Nominal position of receiver station.
          * @param antexObj  AntexReader object containing satellite
          *                  antenna data.
          *
          * @warning If 'AntexReader' object holds an Antex file with relative
          * antenna data, a simple satellite phase center model will be used.
          */
      ComputeWindUp( const Position& stapos,
                     AntexReader& antexObj )
         : nominalPos(stapos), pAntexReader(&antexObj),
           pEphemeris(NULL), 
		   peclipse(NULL)                 
      { 
          Init();
      };


      virtual void Init();


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& time,
                                        satTypeValueMap& gData )
         noexcept(false);


      virtual void Process(gnssRinex& gData)
         noexcept(false);

         /// Returns nominal position of receiver station.
      virtual Position getNominalPosition(void) const
      { return nominalPos; };


         /** Sets  nominal position of receiver station.
          * @param stapos    Nominal position of receiver station.
          */
      virtual ComputeWindUp& setNominalPosition(const Position& stapos)
        { nominalPos = stapos; return (*this); };


         /// Returns a pointer to the satellite ephemeris object
         /// currently in use.
      virtual XvtStore<SatID> *getEphemeris(void) const
      { return pEphemeris; };


         /** Sets satellite ephemeris object to be used.
          * @param ephem     Satellite ephemeris object.
          */
      virtual ComputeWindUp& setEphemeris(XvtStore<SatID>& ephem)
      { pEphemeris = &ephem; return (*this); };


         /// Returns a pointer to the AntexReader object currently in use.
      virtual AntexReader *getAntexReader(void) const
      { return pAntexReader; };


         /** Sets AntexReader object to be used.
          *
          * @param antexObj  AntexReader object containing satellite
          *                  antenna data.
          */
      virtual ComputeWindUp& setAntexReader(AntexReader& antexObj)
      { pAntexReader = &antexObj; return (*this); };


         /** Sets eclipse object to be used.
          *
          * @param eclipseObj  Eclipse object containing satellite
          *                  attitude information.
          */
	  virtual ComputeWindUp& setEclipse(EclipsedSatFilter& eclipseObj)
	  { peclipse = &eclipseObj; return (*this); };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~ComputeWindUp() {};


   private:


      std::map<SatelliteSystem, TypeIDSet> sysTypes;


         /// Satellite ephemeris to be used
      XvtStore<SatID> *pEphemeris;


         /// Receiver position
      Position nominalPos;


         /// Pointer to object containing satellite antenna data, if available.
      AntexReader* pAntexReader;

         /// A structure used to store phase data.
      struct phaseData
      {
            // Default constructor initializing the data in the structure
         phaseData() 
             : previousPhaseSat(0.0), previousPhaseStation(0.0), arcNum(0)
         {};

         double previousPhaseSat;
         double previousPhaseStation;      ///< Previous phase.
         double arcNum;
      };

      typedef std::map<SatID, phaseData> SatPhaseDataMap;
      typedef std::map<SourceID, SatPhaseDataMap> SourceSatPhaseDataMap;

      SatPhaseDataMap satPhaseData;
      SourceSatPhaseDataMap sourceSatPhaseData;

	  /// For computing satellite windup
	  EclipsedSatFilter* peclipse;


         /** Compute the value of the wind-up, in radians.
          * @param sat       Satellite ID
          * @param time      Epoch of interest
          * @param satpos    Satellite position, as a Triple
          * @param sunpos    Sun position, as a Triple
          *
          * @return Wind-up computation, in radians
          */
      virtual double getWindUp( const SatID& sat,
                                const CommonTime& time,
                                const Triple& satpos,
                                const Triple& sunpos );


   }; // End of class 'ComputeWindUp'

      //@}

}  // End of namespace gpstk

#endif // ComputeWindUp_HPP
