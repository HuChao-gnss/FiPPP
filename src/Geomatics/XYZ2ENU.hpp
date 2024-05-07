#pragma ident "$Id$"

/**
 * @file XYZ2NEU.hpp
 * This is a class to change the reference base from ECEF XYZ to topocentric
 * North-East-Up (NEU).
 */

#ifndef XYZ2NEU_HPP
#define XYZ2NEU_HPP



#include "Matrix.hpp"
#include "Position.hpp"
#include "gnssData.hpp"
#include "TypeID.hpp"
#include "GNSSconstants.hpp"


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      //@{

      /** This class changes the reference base from an Earth-Centered,
       *  Earth-Fixed (ECEF) system to a North-East-Up (NEU) topocentric
       *  system, centered at the provided reference location.
       *
       * The NEU system is commonly used when comparing the relative accuracy
       * of a given GNSS data processing strategy. Be mindful, however, that
       * NEU is a "left-handed" reference system, whereas geocentric ECEF and
       * topocentric North-East-Down (NED) are "right-handed" systems.
       *
       * A typical way to use this class follows:
       *
       * @code
       *   RinexObsStream rin("ebre0300.02o");
       *
       *      // Reference position of receiver station
       *   Position refPos(4833520.2269, 41537.00768, 4147461.489);
       *
       *      // Some more code and definitions here...
       *
       *      // GDS object
       *   gnssRinex gRin;
       *
       *      // Set model defaults. A typical C1-based modeling is used
       *   ModeledPR modelRef( refPos,
       *                       ionoStore,
       *                       mopsTM,
       *                       bceStore,
       *                       TypeID::C1,
       *                       true );
       *
       *      // Let's define a new equation definition to adapt
       *      // solver object to base change
       *   TypeIDSet typeSet;
       *   typeSet.insert(TypeID::dLat);
       *   typeSet.insert(TypeID::dLon);
       *   typeSet.insert(TypeID::dH);
       *   typeSet.insert(TypeID::cdt);
       *   gnssEquationDefinition newEq(TypeID::prefitC, typeSet);
       *
       *      // Declare (and tune) a SolverLMS object
       *   SolverLMS solver;
       *   solver.setDefaultEqDefinition(newEq);
       *
       *      // Declare the base-changing object setting the reference position
       *   XYZ2NEU baseChange(refPos);
       *
       *   while(rin >> gRin)
       *   {
       *      gRin >> modelRef >> baseChange >> solver;
       *   }
       *
       * @endcode
       *
       * The "XYZ2NEU" object will visit every satellite in the GNSS data
       * structure that is "gRin" and will apply a rotation matrix to
       * coefficients dx, dy and dz of the design matrix, yielding
       * corresponding dLat, dLon and dH for each satellite.
       *
       * Take notice that the design matrix coefficients dx, dy and dz were
       * computed by the "ModeledPR" object, so that step is mandatory.
       *
       * Also, the "XYZ2NEU" class is effective when properly coupled with
       * the "solver" object (be it based on LMS or WMS). In order to get
       * this, you must instruct the "solver" object to get the solution using
       * a geometry/design matrix based on dLat, dLon and dH, instead of the
       * defaults (dx, dy and dz).
       *
       * The later is achieved defining an appropriate "gnssEquationDefinition"
       * object and instructing "solver" to use it as the default equation
       * definition.
       *
       * @sa XYZ2NED.hpp
       */
    class XYZ2NEU                         
    {
    public:

         /// Default constructor.
        XYZ2NEU()
            : refLat(0.0), refLon(0.0), refPos(0.0, 0.0, 0.0)
        { init(); };


         /** Common constructor taking reference point latitude and longitude
          *
          * @param lat       Latitude of the reference point.
          * @param lon       Longitude of the reference point.
          */
        XYZ2NEU( const double& lat,
                 const double& lon )
        { setLatLon(lat, lon); }


         /** Common constructor taking reference point Position object
          *
          * @param refPos    Reference point Position object.
          */
        XYZ2NEU(const Position& refPos);


         /** Method to set the latitude of the reference point, in degrees.
          *
          * @param lat      Latitude of the reference point, in degrees.
          *
          * @warning If parameter 'lat' is outside the +90/-90 degrees range,
          * then latitude will be set to 0 degrees.
          */
        virtual XYZ2NEU& setLat(const double& lat);


         /// Method to get the latitude of the reference point, in degrees.
        virtual double getLat(void) const
        { return (refLat*RAD_TO_DEG); };


         /** Method to set the longitude of the reference point, in degrees.
          *
          * @param lon       Longitude of the reference point, in degrees.
          */
        virtual XYZ2NEU& setLon(const double& lon);


         /// Method to get the longitude of the reference point, in degrees.
        virtual double getLon(void) const
        { return (refLon*RAD_TO_DEG); };

        virtual XYZ2NEU& setRefPos(const Position& pos)
        {
            refPos = pos;
            return (*this);
        };


         /** Method to simultaneously set the latitude and longitude of the
          *  reference point, in degrees.
          *
          * @param lat        Latitude of the reference point, in degrees.
          * @param lon        Longitude of the reference point, in degrees.
          *
          * @warning If parameter 'lat' is outside the +90/-90 degrees range,
          * then latitude will be set to 0 degrees.
          */
        virtual XYZ2NEU& setLatLon( const double& lat,
                                    const double& lon );


         /** Returns a reference to a satTypeValueMap object after
          *  converting from a geocentric reference system to a topocentric
          *  reference system.
          *
          * @param gData     Data object holding the data.
          */
        virtual satTypeValueMap& Process(satTypeValueMap& gData)
            noexcept(false);


        virtual void Process(gnssRinex& gData)
            noexcept(false)
        { 
            if(refPos[0] == 0.0 &&
               refPos[1] == 0.0 &&
               refPos[2] == 0.0 )
            {
                Exception e("refPos must be set!");
                GPSTK_THROW(e);
            }
            if (refPos.mag() == 0.0)
            {
                enuRotationMatrix = neuRotationMatrix = Matrix<double>(3, 3, 0.);
            }
            else
            {
                // set Lat/Lon for current refPos
                setLatLon(refPos.getGeodeticLatitude(), refPos.getLongitude());

                // now, prepare the rotateMatrix
                init();

            }
            gData.header.enuRotateMatrix = enuRotationMatrix;
            gData.header.neuRotateMatrix = neuRotationMatrix;
            // convert now
            //Process(gData.body); 
        };


//       /** Returns a reference to a gnssRinex object after converting
//        *  from a geocentric reference system to a topocentric reference
//        *  system.
//        *
//        * @param gData    Data object holding the data.
//        */
//      virtual void Process(commonHeader& comHeader, gnssRinex& gData)
//          noexcept(false)
//      { 

//          refPos = gData.header.srcPos;
//          
//          // set Lat/Lon for current refPos
//          setLatLon( refPos.getGeodeticLatitude(), refPos.getLongitude() );

//          // now, prepare the rotateMatrix
//          init();

//          // convert now
//          Process(gData.body); 
//      };


        Matrix<double> rotateMatrix()
        { return neuRotationMatrix; };

         /// Returns a string identifying this object.
        virtual std::string getClassName(void) const;
         /// Destructor.
        virtual ~XYZ2NEU() {};


    private:

         /// Latitude of the reference point (topocentric reference), in radians.
        double refLat;

         /// Longitude of the reference point (topocentric reference), in radians.
        double refLon;

        /// Station position.
        Position refPos;

         /// NEU Rotation matrix.
        Matrix<double> neuRotationMatrix;

        Matrix<double> enuRotationMatrix;

         /// Set (TypeIDSet) containing the types of data to be converted
         /// (dx, dy, dz).
        TypeIDSet inputSet;

         /// Set (TypeIDSet) containing the resulting types of data
         /// (dLat, dLon, dH).
        TypeIDSet outputSet;

         /// This method builds the rotation matrix according to 'refLat'
         /// and 'refLon' values.
        virtual void init();


    }; // End of class 'XYZ2NEU'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_XYZ2NEU_HPP
