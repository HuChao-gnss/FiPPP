#pragma ident "$Id$"

/**
 * @file PoleTides.cpp
 * Computes the effect of pole tides at a given position and epoch.
 */

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//


#include "PoleTides.hpp"
#include "CivilTime.hpp"
#include "MJD.hpp"

using namespace std;

namespace gpstk
{

      /* Return the effect of pole tides (meters) at the given
       * position and epoch, in the Up-East-North (UEN) reference frame.
       *
       * @param[in]  t Epoch to look up
       * @param[in]  p Position of interest
       *
       * @return a Triple with the pole tide effect, in meters and in
       *    the UEN reference frame.
       *
       * @throw InvalidRequest If the request can not be completed for
       *    any reason, this is thrown. The text may have additional
       *    information about the reason the request failed.
       *
       * @warning In order to use this method, you must have previously
       *    set the current pole displacement parameters
       *
       */
    Triple PoleTides::getPoleTide( const CommonTime& time,
                                   const Position& pos )
        noexcept(false)
    {
        // We will store here the results
        Triple ret(0.0, 0.0, 0.0);

        if(pRefSys == NULL) return ret;

        // UTC time, which will used in ERP interpolation
        CommonTime utc( pRefSys->GPS2UTC(time) );

        try
        {
            // Get current position's latitude and longitude, in radians
            double lat( pos.geodeticLatitude() * DEG_TO_RAD );
            double lon( pos.longitude() * DEG_TO_RAD );

            // Get polar motion from files (UTC)
            xdisp = pRefSys->getXPole(utc);
            ydisp = pRefSys->getYPole(utc);

            // The conventional mean pole from IERS 2010, chap 7, Table 7.7
            double MJD_UTC = MJD(utc).mjd;

            // 51544 for 2000.0 and  51544.5 for J2000
            double ly1 = (MJD_UTC - 51544.0) / 365.25;
            double ly2 = ly1 * ly1;
            double ly3 = ly2 * ly1;

            // until 2010.0, unit: mas/year
            const double xp1[4] = {  55.974, 1.8243,  0.18413,  0.007024 };
            const double yp1[4] = { 346.346, 1.7896, -0.10729, -0.000908 };
            // after 2010.0, unit: mas/year
            const double xp2[2] = {  23.513,  7.6141 };
            const double yp2[2] = { 358.891, -0.6287 };

            // get the mean pole at epoch 2000.0 from IERS Conventions 2010
            // see IERS Conventions 2010, Equation 7.25
            double xpm(0.0), ypm(0.0);

            if(MJD_UTC < 55197.0)    // until 2010.0
            {
                xpm = ( xp1[0] + xp1[1]*ly1 + xp1[2]*ly2 + xp1[3]*ly3 )*1e-3;
                ypm = ( yp1[0] + yp1[1]*ly1 + yp1[2]*ly2 + yp1[3]*ly3 )*1e-3;
            }
            else                     // after 2010.0
            {
                xpm = ( xp2[0] + xp2[1]*ly1 )*1e-3;
                ypm = ( yp2[0] + yp2[1]*ly1 )*1e-3;
            }

            // Now, compute m1 and m2 parameters
            double m1(xdisp - xpm);
            double m2(ypm - ydisp);

            // Now, compute some useful values
            double sin2lat( std::sin(2.0*lat) );
            double cos2lat( std::cos(2.0*lat) );
            double sinlat( std::sin(lat) );
            double sinlon( std::sin(lon) );
            double coslon( std::cos(lon) );

            // Finally, get the pole tide values, in UEN reference
            // frame and meters
            ret[0] = -0.033 * sin2lat * ( m1*coslon + m2*sinlon );
            ret[1] = +0.009 * sinlat  * ( m1*sinlon - m2*coslon );
            ret[2] = -0.009 * cos2lat * ( m1*coslon + m2*sinlon );

            // Please be aware that the former equations take into account
            // that the IERS pole tide equations use CO-LATITUDE instead
            // of LATITUDE. See Wahr, 1985.

        } // End of try block
        catch(InvalidRequest& ir)
        {
            GPSTK_THROW(ir);
        }
        catch(...)
        {
            InvalidRequest ir("Unknown error when computing pole tides.");
            GPSTK_THROW(ir);
        }

        return ret;

    }  // End of method 'PoleTides::getPoleTide()'


      /* Method to set the pole displacement parameters
       *
       * @param x     Pole displacement x, in arcseconds
       * @param y     Pole displacement y, in arcseconds
       *
       * @return This same object
       */
    PoleTides& PoleTides::setXY( const double& x,
                                 const double& y )
    {
        xdisp = x;
        ydisp = y;

        return (*this);

    }  // End of method 'PoleTides::setXY()'


}  // End of namespace gpstk
