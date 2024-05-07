#pragma ident "$Id$"


/**
 * @file ComputeIonoDelay.hpp
 * This is a class to compute the slant ionospheric delay using
 * Klobuchar model.
 */

#ifndef ComputeIonoDelay_HPP
#define ComputeIonoDelay_HPP


#include "Position.hpp"
#include "TypeID.hpp"
#include "IonoModelStore.hpp"
#include "gnssData.hpp"
#include "DeprecatedConsts.hpp"

using namespace gpstk;

namespace gpstk
{

      /** this is a class to compute ionospheric mapping function
       * and slant ionospheric delay using Klobuchar model.
       *
       * This class is intended to be used with GNSS Data Structures (GDS).
       *
       * A typical way to use this class follows:

       * @code
       *
       * // insert code here for example
       *
       * @endcode
       *
       * @sa IonoModel.hpp
       *
       */
   class ComputeIonoDelay                         
   {
   public:


         /// Default constructor.
      ComputeIonoDelay() 
          : pIonoModel(NULL)               
      {};


         /// Explicit constructor, taking as input a Position object
         /// containing reference station coordinates.
      ComputeIonoDelay(const Position& RxCoordinates)
         noexcept(false);


         /** Explicit constructor, taking as input reference station
          *  coordinates and ionex maps (Ionex Store object) to be used.
          *
          * @param RxCoordinates    Receiver coordinates.
          * @param ionoStore           IonoModel object to be used by default.
          * @param dObservable      Observable type to be used by default.
          * @param applyDCB         Whether or not P1 observable will be
          *                         corrected from DCB effect.
          * @param ionoMap          Type of ionosphere mapping function (string)
          *                         @sa IonoModel::iono_mapping_function
          */
      ComputeIonoDelay( const Position& RxCoordinates,
                        IonoModel& ionoStore )
         noexcept(false);


         /** Return a satTypeValueMap object, adding the new data generated
          *  when calling a modeling object.
          *
          * @param time      Epoch.
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& time,
                                        std::map<RinexSatID, TypeIDVec>& satShortTypes,
                                        std::map<RinexSatID, int>& glonassFreqNo,
                                        satTypeValueMap& gData )
         noexcept(false);


         /** Return a gnssRinex object, adding the new data generated when
          *  calling a modeling object.
          *
          * @param gData    Data object holding the data.
          */
      virtual void Process(gnssRinex& gRin)
          noexcept(false)
      {
          Process(gRin.header.epoch,
              gRin.header.satShortTypes,
              gRin.header.glonassFreqNo,
              gRin.body);
      };



         /** Method to set the default ionex maps (IonoModel object)
          *  to be used with GNSS data structures.
          *
          * @param ionoStore   IonoModel object to be used by default
          */
      virtual ComputeIonoDelay& setIonoModel(IonoModel& ionoStore)
      { pIonoModel = &ionoStore; return (*this); };


         /** Method to set the initial (a priori) position of receiver.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int setInitialRxPosition(const Position& RxCoordinates)
         noexcept(false);


          /** Ionosphere mapping function
          *
          * @param elevation     Elevation of satellite as seen at receiver
          *                      (degrees).
          * @param ionoMapType   Type of ionosphere mapping function (string)
          *                      (0) NONE no mapping function is applied
          *                      (1) SLM  Single Layer Model (IGS)
          *                      (2) MSLM Modified Single Layer Model (CODE)
          *                      (3) ESM  Extended Slab Model (JLP)
          *
          * Details at: http://aiuws.unibe.ch/ionosphere/mslm.pdf
          *
          * @warning No implementation for JPL's mapping function.
          */
      double iono_mapping_function( const double& elevation,
                                    const std::string& ionoMapType = "SLM" ) const;


         /// Return a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~ComputeIonoDelay() {};


   protected:


         /// Pointer to default Ionex object when working with GNSS
         /// data structures.
      IonoModel* pIonoModel;


         /// Either estimated or "a priori" position of receiver
      Position rxPos;


   }; // End of class 'ComputeIonoDelay'

      //@}

}  // End of namespace gpstk

#endif   // ComputeIonoDelay_HPP




