#pragma ident "$Id$"

/**
 * @file SolidTides.hpp
 * Computes the effect of solid Earth tides at a given position and epoch.
 */

#ifndef SOLIDTIDES_HPP
#define SOLIDTIDES_HPP

#include <cmath>
#include <string>

#include "CommonTime.hpp"
#include "Triple.hpp"
#include "Position.hpp"
#include "GNSSconstants.hpp"
#include "ReferenceSystem.hpp"
#include "SolarSystem.hpp"


namespace gpstk
{
      /** @addtogroup GPSsolutions */
      //@{

      /** This class computes the effect of solid Earth tides at a given
       * position and epoch.
       * Given the limitations of the algorithms used to compute the Sun
       * and Moon positions, this class is limited to the period between
       * March 1st, 1900 and February 28th, 2100.
       *
       */
    class SolidTides
    {
    public:

         /// Default constructor
        SolidTides()
            : pRefSys(NULL), pSolSys(NULL)
        {};

        /// Destructor
        virtual ~SolidTides()
        {};


        SolidTides& setReferenceSystem(ReferenceSystem& refSys)
        { pRefSys = &refSys; return (*this); };


        SolidTides& setSolarSystem(SolarSystem& solSys)
        { pSolSys = &solSys; return (*this); };


         /** Return the effect of solid Earth tides (meters) at the given
          *  position and epoch, in the Up-East-North (UEN) reference frame.
          *
          * @param[in] time     Epoch to look up(GPST)
          * @param[in] pos      Position of interest
          *
          * @return a Triple with the solid tidal effect, in meters and in
          * the UEN reference frame.
          *
          * @throw InvalidRequest If the request can not be completed for any
          * reason, this is thrown. The text may have additional information
          * as to why the request failed.
          */
        Triple getSolidTide( const CommonTime& time,
                             const Position& pos )
            noexcept(false);


        /** This subroutine gives the out-of-phase corrections induced by
         *  mantle anelasticity in the diurnal band.
         *
         * @param XSTA      Geocentric position of the IGS station
         * @param XSUN      Geocentric position of the Sun
         * @param XMON      Geocentric position of the Moon
         * @param FAC2SUN   Degree 2 TGP factor for the Sun
         * @param FAC2MON   Degree 2 TGP factor for the Moon
         * @return          a Triple with the solid tidal effect, in meters
         *                  and in the ITRF reference frame.
         */
        Triple getStep1Diu( const Triple& XSTA,
                            const Triple& XSUN,
                            const Triple& XMON,
                            double FAC2SUN,
                            double FAC2MON );


        /** This subroutine gives the out-of-phase corrections induced by
         *  mantle anelasticity in the semi-diurnal band.
         *
         * @param XSTA      Geocentric position of the IGS station
         * @param XSUN      Geocentric position of the Sun
         * @param XMON      Geocentric position of the Moon
         * @param FAC2SUN   Degree 2 TGP factor for the Sun
         * @param FAC2MON   Degree 2 TGP factor for the Moon
         * @return          a Triple with the solid tidal effect, in meters
         *                  and in the ITRF reference frame.
         */
        Triple getStep1Sem( const Triple& XSTA,
                            const Triple& XSUN,
                            const Triple& XMON,
                            double FAC2SUN,
                            double FAC2MON );


        /** This subroutine gives the corrections induced by the latitude
         *  dependence given by L^1 in Mathews et al. 1991
         *
         * @param XSTA      Geocentric position of the IGS station
         * @param XSUN      Geocentric position of the Sun
         * @param XMON      Geocentric position of the Moon
         * @param FAC2SUN   Degree 2 TGP factor for the Sun
         * @param FAC2MON   Degree 2 TGP factor for the Moon
         * @return          a Triple with the solid tidal effect, in meters
         *                  and in the ITRF reference frame.
         */
        Triple getStep1L1( const Triple& XSTA,
                           const Triple& XSUN,
                           const Triple& XMON,
                           double FAC2SUN,
                           double FAC2MON );


        /** This subroutine gives the in-phase and out-of-phase corrections
         *  induced by mantle anelasticity in the diurnal band.
         *
         * @param XSTA      Geocentric position of the IGS station
         * @param FHR       Fractional hours in the day
         * @param T         Centuries since J2000
         * @return          a Triple with the solid tidal effect, in meters
         *                  and in the ITRF reference frame.
         */
        Triple getStep2Diu( const Triple& XSTA,double FHR,double T );


        /** This subroutine gives the in-phase and out-of-phase corrections
         *  induced by mantle anelasticity in the long period band.
         * @param XSTA      Geocentric position of the IGS station
         * @param T         Centuries since J2000
         * @return          a Triple with the solid tidal effect, in meters
         *                  and in the ITRF reference frame.
         */
        Triple getStep2Lon( const Triple& XSTA, double T );


    private:

        /// Nominal second degreen and third degree love and shida number
        static const double H20, L20, H3, L3;

        /// Phase lag
        static const double PH_LAG;

        ReferenceSystem* pRefSys;

        SolarSystem* pSolSys;

    }; // end class SolidTides


    //@}

} // namespace gpstk

#endif  // SOLIDTIDES_HPP
