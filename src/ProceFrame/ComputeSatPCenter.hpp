#pragma ident "$Id $"

/**
 * @file ComputeSatPCenter.hpp
 * This class computes the satellite antenna phase correction, in meters.
 */

#ifndef ComputeSatPCenter_HPP
#define ComputeSatPCenter_HPP

//============================================================================
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009, 2011
//============================================================================



#include <cmath>
#include <string>
#include <sstream>
#include "Triple.hpp"
#include "Position.hpp"
#include "SunPosition.hpp"
#include "XvtStore.hpp"
#include "AntexReader.hpp"
#include "GNSSconstants.hpp"
#include "StringUtils.hpp"
#include "EclipsedSatFilter.hpp"
#include "DataStructures.hpp"
#include "gnssData.hpp"

namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class computes the satellite antenna phase correction, in meters.
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
       *   ComputeSatPCenter svPcenter( SP3EphList,
       *                                nominalPos );
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> svPcenter;
       *   }
       * @endcode
       *
       * The "ComputeSatPCenter" object will visit every satellite in the GNSS
       * data structure that is "gRin" and will compute the corresponding
       * satellite antenna phase correction, in meters.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the "satPCenter" TypeID inserted in it.
       * Be warned that if a given satellite does not have the required data,
       * it will be summarily deleted from the data structure.
       *
       * \warning The ComputeSatPCenter objects generate corrections that are
       * interpreted as an "advance" in the signal, instead of a delay.
       * Therefore, those corrections always hava a negative sign.
       *
       */
   class ComputeSatPCenter 
   {
   public:

         /// Default constructor
      ComputeSatPCenter()
         : pEphemeris(NULL), nominalPos(0.0, 0.0, 0.0),BDS_PCO_model(1), 
           pAntexReader(NULL),
		   peclipse(NULL)
      { };


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
      ComputeSatPCenter( XvtStore<RinexSatID/*SatID*/>& ephem,
                         const Position& stapos )
         : pEphemeris(&ephem), nominalPos(stapos), 
           pAntexReader(NULL),BDS_PCO_model(1),
		   peclipse(NULL)
      { };


         /** Common constructor
          *
          * @param stapos    Nominal position of receiver station.
          * @param filename  Name of "PRN_GPS"-like file containing
          *                  satellite data.
          *
          * @warning If filename is not given, this class will look for a
          * file named "PRN_GPS" in the current directory.
          */
      ComputeSatPCenter( const Position& stapos)
         : nominalPos(stapos), 
           pEphemeris(NULL), 
		   BDS_PCO_model(1),
           pAntexReader(NULL),
		   peclipse(NULL)
      { };


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
      ComputeSatPCenter( XvtStore<RinexSatID/*SatID*/>& ephem,
                         const Position& stapos,
                         AntexReader& antexObj )
         : pEphemeris(&ephem), 
		   nominalPos(stapos), pAntexReader(&antexObj),
           BDS_PCO_model(1),
		   peclipse(NULL) 
       { };


         /** Common constructor. Uses satellite antenna data from an Antex file.
          *
          * @param stapos    Nominal position of receiver station.
          * @param antexObj  AntexReader object containing satellite
          *                  antenna data.
          *
          * @warning If 'AntexReader' object holds an Antex file with relative
          * antenna data, a simple satellite phase center model will be used.
          */
      ComputeSatPCenter( const Position& stapos,
                         AntexReader& antexObj )
         : nominalPos(stapos), pAntexReader(&antexObj),
           pEphemeris(NULL), BDS_PCO_model(1),
		   peclipse(NULL)                 
      {};


         /** Returns a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& time,
                                        std::map<RinexSatID/*SatID*/, TypeIDVec>& satShortTypes,
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
      virtual ComputeSatPCenter& setNominalPosition(const Position& stapos)
        { nominalPos = stapos; return (*this); };


         /// Returns a pointer to the satellite ephemeris object
         /// currently in use.
      virtual XvtStore<RinexSatID/*SatID*/> *getEphemeris(void) const
      { return pEphemeris; };


         /** Sets satellite ephemeris object to be used.
          *
          * @param ephem     Satellite ephemeris object.
          */
      virtual ComputeSatPCenter& setEphemeris(XvtStore<RinexSatID/*SatID*/>& ephem)
      { pEphemeris = &ephem; return (*this); };


         /// Returns a pointer to the AntexReader object currently in use.
      virtual AntexReader *getAntexReader(void) const
      { return pAntexReader; };


         /** Sets AntexReader object to be used.
          *
          * @param antexObj  AntexReader object containing satellite
          *                  antenna data.
          */
      virtual ComputeSatPCenter& setAntexReader(AntexReader& antexObj)
      { pAntexReader = &antexObj; return (*this); };


         /** Sets eclipse object to be used.
          *
          * @param eclipseObj  Eclipse object containing satellite
          *                  attitude information.
          */
	  virtual ComputeSatPCenter& setEclipse(EclipsedSatFilter& eclipseObj)
	  { peclipse = &eclipseObj; return (*this); };


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


	  /// set BDS PCO correction model
	  void setBDSPCOModel(int model)
	  { BDS_PCO_model = model;};


	  /// get BDS PCO correction model
	  int getBDSPCOModel()
	  { return BDS_PCO_model; };


         /// Destructor
      virtual ~ComputeSatPCenter() {};


   private:


         /// Satellite ephemeris to be used
      XvtStore<RinexSatID/*SatID*/> *pEphemeris;


         /// Receiver position
      Position nominalPos;



         /// Name of "PRN_GPS"-like file containing satellite data.
      std::string fileData;


         /// Pointer to object containing satellite antenna data, if available.
      AntexReader* pAntexReader;


	  /// 0 = No correction.
	  /// 1 = MGEX antenna file
	  /// 2 = Producer, 634.00  -3.00  1075.00(mm)
	  int BDS_PCO_model;


	  /// For computing satellite PCO correction
	  EclipsedSatFilter* peclipse;

         /** Compute the value of satellite antenna phase correction, in meters
          * @param satid     Satellite ID
          * @param time      Epoch of interest
          * @param satpos    Satellite position, as a Triple
          * @param sunpos    Sun position, as a Triple
          *
          * @return Satellite antenna phase correction, in meters.
          */
      virtual double getSatPCenter( const RinexSatID/*SatID*/& sat,
                                    const Antenna::frequencyType& freqType,
                                    const CommonTime& time,
                                    const Triple& satpos,
                                    const Triple& sunPosition );


   }; // End of class 'ComputeSatPCenter'

      //@}

}  // End of namespace gpstk

#endif // ComputeSatPCenter_HPP
