#pragma ident "$Id$"

/**
 * @file XYZ2NED.hpp
 * This is a class to change the reference base from ECEF XYZ to topocentric
 * North-East-Down (NED).
 */

#ifndef GPSTK_XYZ2NED_HPP
#define GPSTK_XYZ2NED_HPP

//============================================================================
//
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2011
//
//============================================================================



#include "Matrix.hpp"
#include "Position.hpp"
#include "TypeID.hpp"
#include "DataStructures.hpp"
#include "GNSSconstants.hpp"


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{

      /** This class changes the reference base from an Earth-Centered,
       *  Earth-Fixed (ECEF) system to a North-East-Down (NED) topocentric
       *  system, centered at the provided reference location.
       *
       * The NED system may be used when comparing the relative accuracy
       * of a given GNSS data processing strategy. This is a "right-handed"
       * system, and be aware that "down" is positive and "up" is negative.
       *
       * A typical way to use this class follows:
       *
       * The "XYZ2NED" object will visit every satellite in the GNSS data
       * structure that is "gRin" and will apply a rotation matrix to
       * coefficients dx, dy and dz of the design matrix, yielding
       * corresponding dLat, dLon and dH for each satellite.
       *
       * Take notice that the design matrix coefficients dx, dy and dz were
       * computed by the "ModeledPR" object, so that step is mandatory.
       *
       * Also, the "XYZ2NED" class is effective when properly coupled with
       * the "solver" object (be it based on LMS or WMS). In order to get
       * this, you must instruct the "solver" object to get the solution using
       * a geometry/design matrix based on dLat, dLon and dH, instead of the
       * defaults (dx, dy and dz).
       *
       * The later is achieved defining an appropriate "gnssEquationDefinition"
       * object and instructing "solver" to use it as the default equation
       * definition.
       *
       * @sa XYZ2NEU.hpp
       */
   class XYZ2NED                         
   {
   public:


         /// Default constructor.
      XYZ2NED()
         : refLat(0.0), refLon(0.0)
      { init(); };


         /** Common constructor taking reference point latitude and longitude
          *
          * @param lat       Latitude of the reference point.
          * @param lon       Longitude of the reference point.
          */
      XYZ2NED( const double& lat,
               const double& lon )
      { setLatLon(lat, lon); }


         /** Common constructor taking reference point Position object
          *
          * @param refPos    Reference point Position object.
          */
      XYZ2NED(const Position& refPos);


         /** Method to set the latitude of the reference point, in degrees.
          *
          * @param lat      Latitude of the reference point, in degrees.
          *
          * @warning If parameter 'lat' is outside the +90/-90 degrees range,
          * then latitude will be set to 0 degrees.
          */
      virtual XYZ2NED& setLat(const double& lat);


         /// Method to get the latitude of the reference point, in degrees.
      virtual double getLat(void) const
      { return (refLat*RAD_TO_DEG); };


         /** Method to set the longitude of the reference point, in degrees.
          *
          * @param lon       Longitude of the reference point, in degrees.
          */
      virtual XYZ2NED& setLon(const double& lon);


         /// Method to get the longitude of the reference point, in degrees.
      virtual double getLon(void) const
      { return (refLon*RAD_TO_DEG); };


         /** Method to simultaneously set the latitude and longitude of
          *  the reference point, in degrees.
          *
          * @param lat        Latitude of the reference point, in degrees.
          * @param lon        Longitude of the reference point, in degrees.
          *
          * @warning If parameter 'lat' is outside the +90/-90 degrees range,
          * then latitude will be set to 0 degrees.
          */
      virtual XYZ2NED& setLatLon( const double& lat,
                                  const double& lon );


         /** Returns a reference to a satTypeValueMap object after
          *  converting from a geocentric reference system to a topocentric
          *  reference system.
          *
          * @param gData     Data object holding the data.
          */
      virtual satTypeValueMap& Process(satTypeValueMap& gData)
         noexcept(false);


         /** Returns a reference to a gnssRinex object after converting
          *  from a geocentric reference system to a topocentric reference
          *  system.
//        *
//        * @param gData    Data object holding the data.
//        */
//    virtual void Process(commonHeader& comHeader, gnssRinex& gData)
//       noexcept(false)
//    { Process(gData.body);  };


         /** Returns a reference to a sourceRinex object after converting
          *  from a geocentric reference system to a topocentric reference
          *  system.
          *
          * @param gData    Data object holding the data.
          */
//    virtual void Process(commonHeader& comHeader, sourceRinex& gData)
//       noexcept(false)
//    {cerr << "need to be modifed" << endl;};


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~XYZ2NED() {};


   private:


         /// Latitude of the reference point (topocentric reference),
         /// in radians.
      double refLat;


         /// Longitude of the reference point (topocentric reference),
         /// in radians.
      double refLon;


         /// Rotation matrix.
      Matrix<double> neuRotationMatrix;


         /// Set (TypeIDSet) containing the types of data to be converted
         /// (dx, dy, dz).
      TypeIDSet inputSet;


         /// Set (TypeIDSet) containing the resulting types of data
         /// (dLat, dLon, dH).
      TypeIDSet outputSet;


         /// This method builds the rotation matrix according to 'refLat'
         /// and 'refLon' values.
      virtual void init();


   }; // End of class 'XYZ2NED'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_XYZ2NED_HPP
