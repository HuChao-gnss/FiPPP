/**
 * Function:
 *   This class ease handling Reference System transformation.
 */

#include "ReferenceSystem.hpp"
#include <string>
#include <fstream>
#include <cmath>
#include <iomanip>

#include "Epoch.hpp"

using namespace std;

#define debug 0

namespace gpstk
{

    /// Method from EOPDataStore

    /// Get the x pole displacement parameter, in arcseconds.
    double ReferenceSystem::getXPole(const CommonTime& UTC) const
        noexcept(false)
    {
        double xpole;

        try
        {
            // Get the x pole from EOPDataStore
            if( pEopStore != NULL )
            {
                xpole = (*pEopStore).getXPole( UTC );
            }
        }
        catch(InvalidRequest& ire)
        {
            GPSTK_RETHROW(ire);
        }

        // return
        return xpole;

    }   // End of method 'ReferenceSystem::getXPole()'


    /// Get the y pole displacement parameter, in arcseconds.
    double ReferenceSystem::getYPole(const CommonTime& UTC) const
        noexcept(false)
    {
        double ypole;

        try
        {
            // Get the x pole from EOPDataStore
            if( pEopStore != NULL )
            {
                ypole = (*pEopStore).getYPole( UTC );
            }
        }
        catch(InvalidRequest& ire)
        {
            GPSTK_RETHROW(ire);
        }

        // return
        return ypole;

    }   // End of method 'ReferenceSystem::getYPole()'


    /// Get the value of UT1mUTC, in seconds.
    double ReferenceSystem::getUT1mUTC(const CommonTime& UTC) const
        noexcept(false)
    {
        double UT1mUTC;

    try
    {
        // Get the UT1mUTC from EOPDataStore
        if( pEopStore != NULL )
        {
            UT1mUTC = (*pEopStore).getUT1mUTC( UTC );
        }
    }
    catch(InvalidRequest& ire)
    {
        GPSTK_RETHROW(ire);
    }

        // return
        return UT1mUTC;

    } // End of method 'ReferenceSystem::getUT1mUTC()'


    // Get the value of LOD, in seconds.
    double ReferenceSystem::getLOD(const CommonTime& UTC) const
       noexcept(false)
    {
        double LOD;

        try
        {
            if( pEopStore != NULL )
            {
                LOD = (*pEopStore).getLOD( UTC );
            }
        }
        catch(InvalidRequest& ire)
        {
            GPSTK_RETHROW(ire);
        }

        // return
        return LOD;

    }   // End of method 'ReferenceSystem::getLOD()'


    /// Get the value of dX, in arcseconds.
    double ReferenceSystem::getDX(const CommonTime& UTC) const
        noexcept(false)
    {
        double dX;

        try
        {
            // Get the dX from EOPDataStore
            if( pEopStore != NULL )
            {
                dX = (*pEopStore).getDX( UTC );
            }
        }
        catch(InvalidRequest& ire)
        {
            GPSTK_RETHROW(ire);
        }

        // return
        return dX;

    }    // End of method 'ReferenceSystem::getDX()'


    /// Get the value of dY, in arcseconds.
    double ReferenceSystem::getDY(const CommonTime& UTC) const
        noexcept(false)
    {
        double dY;

        try
        {
            // Get the dY from EOPDataStore
            if( pEopStore != NULL )
            {
                dY = (*pEopStore).getDY( UTC );
            }
        }
        catch(InvalidRequest& ire)
        {
            GPSTK_RETHROW(ire);
        }

        // return
        return dY;

    }    // End of method 'ReferenceSystem::getDY()'


    /// Get the EOP data at the given UTC time.
    EOPDataStore::EOPData ReferenceSystem::getEOPData(const CommonTime& UTC) const
        noexcept(false)
    {
        EOPDataStore::EOPData eopData;

        try
        {
            // Get the EOP data from EOPDataStore
            if( pEopStore != NULL )
            {
                eopData = (*pEopStore).getEOPData( UTC );
            }
        }
        catch(InvalidRequest& ire)
        {
            GPSTK_RETHROW(ire);
        }

        // return
        return eopData;

    }    // End of method 'ReferenceSystem::getEOPData()'



    /// Method from LeapSecondStore

    /// Get the leap second at the given UTC time.
    double ReferenceSystem::getLeapSec(const CommonTime& UTC) const
        noexcept(false)
    {
        double leapSec;

        try
        {
            // Get the leap second from EOPDataStore
            if( pLeapSecStore!= NULL )
            {
                leapSec = (*pLeapSecStore).getLeapSec( UTC );
            }
        }
        catch(InvalidRequest& ire)
        {
            GPSTK_RETHROW(ire);
        }

        // return
        return leapSec;

    } // End of method 'ReferenceSystem::getLeapSec()'


    /// Method to get the difference between time systems

    /// Return the value of (TAI - UTC) (= leapsec).
    double ReferenceSystem::getTAImUTC(const CommonTime& UTC)
        noexcept(false)
    {
        // Leap second = TAI - UTC
        return getLeapSec(UTC);

    } // End of method 'ReferenceSystem::getTAImUTC()


    /// Get the value of (TT - TAI)
    double ReferenceSystem::getTTmTAI( void )
        noexcept(false)
    {
        // The difference is constant : 32.184 second
        return 32.184;

    } // End of method 'ReferenceSystem::getTTmTAI()


    /// Get the value of (TT - UTC)
    double ReferenceSystem::getTTmUTC(const CommonTime& UTC)
        noexcept(false)
    {
        // TT - TAI
        double TTmTAI ( getTTmTAI() );

        // TAI - UTC
        double TAImUTC( getTAImUTC(UTC) );

        // TT - UTC
        double TTmUTC( TTmTAI + TAImUTC );

        // return
        return TTmUTC;

    } // End of method 'ReferenceSystem::getTTmTAI()


    /// Get the value of (TAI - GPS)
    double ReferenceSystem::getTAImGPS(void)
        noexcept(false)
    {
        // The difference is constant : 19.0 second
        return 19.0;

    } // End of method 'ReferenceSystem::getTAImGPS()


    /// Methods to convert between different time systems


    /// Convert GPS to UTC.
    CommonTime ReferenceSystem::GPS2UTC(const CommonTime& GPS)
    {
        // The input time of 'getLeapSec' should be 'UTC'
        // if the input is GPS, you should amends it.
        CommonTime GPS1(GPS);
        GPS1.setTimeSystem(TimeSystem::UTC);

        double leapSec = getLeapSec( GPS1 );

        // The formula of GPS and UTC equals with:
        // GPS = UTC + ( leapSec - 19.0s );

        CommonTime UTC1;
        UTC1  = GPS1 - (leapSec - 19.0);

        ///////////////////////////////////////////////////////////////////
        //
        // Now, correct the leap second by feeding the utc instead
        // of the 'gps', if not fix, the following would happen:
        //
        //  01-JUL-1993 00:00:10 GPS TIME (- 9s) = 01-JUL-1993 00:00:01 UTC TIME
        //  01-JUL-1993 00:00:09 GPS TIME (- 9s) = 01-JUL-1993 00:00:00 UTC TIME
        //  01-JUL-1993 00:00:08 GPS TIME (- 9s) = 01-JUL-1993 00:00:59 UTC TIME
        //  01-JUL-1993 00:00:07 GPS TIME (- 9s) = 30-JUN-1993 23:59:58 UTC TIME
        //  01-JUL-1993 00:00:06 GPS TIME (- 9s) = 30-JUN-1993 23:59:57 UTC TIME
        //  01-JUL-1993 00:00:05 GPS TIME (- 9s) = 30-JUN-1993 23:59:56 UTC TIME
        //  01-JUL-1993 00:00:04 GPS TIME (- 9s) = 30-JUN-1993 23:59:55 UTC TIME
        //  01-JUL-1993 00:00:03 GPS TIME (- 9s) = 30-JUN-1993 23:59:54 UTC TIME
        //  01-JUL-1993 00:00:02 GPS TIME (- 9s) = 30-JUN-1993 23:59:53 UTC TIME
        //  01-JUL-1993 00:00:01 GPS TIME (- 9s) = 30-JUN-1993 23:59:52 UTC TIME
        //  01-JUL-1993 00:00:00 GPS TIME (- 9s) = 30-JUN-1993 23:59:51 UTC TIME
        //  30-JUN-1993 23:59:59 GPS TIME (- 8s) = 30-JUN-1993 23:59:51 UTC TIME
        //
        //
        //  The fix allows the following conversion:
        //
        //  01-JUL-1993 00:00:10 GPS TIME (- 9s) = 01-JUL-1993 00:00:01 UTC TIME
        //  01-JUL-1993 00:00:09 GPS TIME (- 9s) = 01-JUL-1993 00:00:00 UTC TIME
        //  01-JUL-1993 00:00:08 GPS TIME (- 8s) = 01-JUL-1993 00:00:00 UTC TIME
        //  01-JUL-1993 00:00:07 GPS TIME (- 8s) = 30-JUN-1993 23:59:59 UTC TIME
        //  01-JUL-1993 00:00:06 GPS TIME (- 8s) = 30-JUN-1993 23:59:58 UTC TIME
        //  01-JUL-1993 00:00:05 GPS TIME (- 8s) = 30-JUN-1993 23:59:57 UTC TIME
        //  01-JUL-1993 00:00:04 GPS TIME (- 8s) = 30-JUN-1993 23:59:56 UTC TIME
        //  01-JUL-1993 00:00:03 GPS TIME (- 8s) = 30-JUN-1993 23:59:55 UTC TIME
        //  01-JUL-1993 00:00:02 GPS TIME (- 8s) = 30-JUN-1993 23:59:54 UTC TIME
        //  01-JUL-1993 00:00:01 GPS TIME (- 8s) = 30-JUN-1993 23:59:53 UTC TIME
        //  01-JUL-1993 00:00:00 GPS TIME (- 8s) = 30-JUN-1993 23:59:52 UTC TIME
        //  30-JUN-1993 23:59:59 GPS TIME (- 8s) = 30-JUN-1993 23:59:51 UTC TIME
        //
        //  \reference Please refer to the subroutine 'gps2utc' in GRACE
        //  data format converting software from GFZ.
        //
        ///////////////////////////////////////////////////////////////////

        // New leap second
        leapSec = getLeapSec(UTC1);

        CommonTime UTC;
        UTC = GPS1 -  (leapSec - 19.0);

        // The TimeSystem of UTC is already set to TimeSystem::UTC.
        return UTC;

    } // End of method 'ReferenceSystem::GPS2UTC()'


    /// Convert UTC to GPS.
    CommonTime ReferenceSystem::UTC2GPS(const CommonTime& UTC)
    {
        // Now, set gpst with utc
        CommonTime TAI;

        // Convert the UTC time to TAI time by correcting the leap second
        TAI = UTC + getTAImUTC(UTC);

        CommonTime GPS;

        // Convert the TAI time to GPS time
        GPS = TAI - getTAImGPS();
        GPS.setTimeSystem(TimeSystem::GPS);

        // return gps
        return GPS;

    } // End of method 'ReferenceSystem::UTC2GPS()'


    /// Convert UT1 to UTC.
    CommonTime ReferenceSystem::UT12UTC(const CommonTime& UT1)
    {
        // Warnging the input time should be utc, so
        // you must amends it
        CommonTime UTC1( UT1 );
        UTC1.setTimeSystem(TimeSystem::UTC);
        UTC1 = UT1 -  getUT1mUTC(UTC1);
        UTC1.setTimeSystem(TimeSystem::UTC);

        // Correct utc by computing new UT1mUTC with utc1
        CommonTime UTC2;
        UTC2 = UT1 -  getUT1mUTC(UTC1);
        UTC2.setTimeSystem(TimeSystem::UTC);

        // Corrent utc by computing new UT1mUTC with utc2
        CommonTime UTC;
        UTC  = UT1 -  getUT1mUTC(UTC2);
        UTC.setTimeSystem(TimeSystem::UTC);

        return UTC;

    }  // End of method 'ReferenceSystem::UT12UTC()'


    /// Convert UTC to UT1.
    CommonTime ReferenceSystem::UTC2UT1(const CommonTime& UTC)
    {

        CommonTime UT1;
        UT1 =  UTC + getUT1mUTC(UTC);
        UT1.setTimeSystem(TimeSystem::UT1);

        return UT1;

    }  // End of method 'ReferenceSystem::UTC2UT1()'


    CommonTime ReferenceSystem::UTC2UT1(const CommonTime& UTC, const double& UT1mUTC)
    {
        CommonTime UT1;
        UT1 = UTC + UT1mUTC;
        UT1.setTimeSystem(TimeSystem::UT1);

        return UT1;
    }


    /// Convert TAI to UTC.
    CommonTime ReferenceSystem::TAI2UTC(const CommonTime& TAI)
    {
        // Initial UTC1.
        CommonTime UTC1( TAI );
        UTC1.setTimeSystem(TimeSystem::UTC);

        // Get leapSec.
        double leapSec( getLeapSec(UTC1) );

        // Update UTC1.
        UTC1 = UTC1 - leapSec;

        // Update leapSec.
        leapSec = getLeapSec(UTC1);

        // Get UTC.
        CommonTime UTC( TAI - leapSec );
        UTC.setTimeSystem(TimeSystem::UTC);

        // return UTC.
        return UTC;

    }  // End of method 'ReferenceSystem::TAI2UTC()'


    /// Convert UTC to TAI.
    CommonTime ReferenceSystem::UTC2TAI(const CommonTime& UTC)
    {
        // Get leapSec.
        double leapSec( getLeapSec(UTC) );

        // Get TAI.
        CommonTime TAI( UTC + leapSec );
        TAI.setTimeSystem(TimeSystem::TAI);

        // Return TAI.
        return TAI;

    }  // End of method 'ReferenceSystem::UTC2TAI()'


    /// Convert TT to UTC.
    CommonTime ReferenceSystem::TT2UTC(const CommonTime& TT)
    {
        // Convert TT to TAI.
        CommonTime TAI( TT - getTTmTAI() );
        TAI.setTimeSystem(TimeSystem::TAI);

        // Convert TAI to UTC.
        CommonTime UTC( TAI2UTC(TAI) );


        // Return UTC.
        return UTC;

    }  // End of method 'ReferenceSystem::TT2UTC()'


    /// Convert UTC to TT.
    CommonTime ReferenceSystem::UTC2TT(const CommonTime& UTC)
    {
        // Convert UTC to TAI.
        CommonTime TAI( UTC2TAI(UTC) );
        // Convert TAI to TT.
        CommonTime TT( TAI + getTTmTAI() );
        TT.setTimeSystem(TimeSystem::TT);

        // Return TT.
        return TT;

    }  // End of method 'ReferenceSystem::UTC2TT()'



    /// Transformation matrix from ICRS to ITRS.
    Matrix<double> ReferenceSystem::C2TMatrix(const CommonTime& UTC)
    {

        // EOP Data
        EOPDataStore::EOPData eop;
        
        try
        {
            eop = getEOPData(UTC);
        }
        catch(Exception& e)
        {
            cerr << e << endl;
            exit(-1);
        }

        double xp = eop.xp;
        double yp = eop.yp;
        double UT1mUTC = eop.UT1mUTC;
        double dX = eop.dX;
        double dY = eop.dY;

        if(debug)
        {
            cout << "ReferenceSystem" << endl;
            cout << "xp:"<< xp << "yp:"<< yp << "UT1mUTC:"<< UT1mUTC << endl;
        }


        // TT
        CommonTime TT( UTC2TT(UTC) );
        MJD mjd_tt(TT);

        // Ocean Tide Corrections
        Vector<double> cor_ocean = ortho_eop(TT)*1e-6;
        xp += cor_ocean(0);
        yp += cor_ocean(1);
        UT1mUTC += cor_ocean(2);

        // Raw CIP X,Y coordinates
        double X(0.0), Y(0.0);
        iauXy06(JD_TO_MJD, mjd_tt.mjd, &X, &Y);

        // The CIO locator s
        double s = iauS06(JD_TO_MJD, mjd_tt.mjd, X, Y);

        // Corrected CIP X,Y coordinates
        X += dX * AS_TO_RAD;
        Y += dY * AS_TO_RAD;

        // GCRS-to-CIRS matrix
        Matrix<double> Q(3,3,0.0);
        iauC2ixys(X, Y, s, Q);

        // UT1
        CommonTime UT1( UTC2UT1(UTC, UT1mUTC) );
        MJD mjd_ut1(UT1);

        // Earth rotation angle
        double ERA = iauEra00(JD_TO_MJD, mjd_ut1.mjd);

        // CIRS-to-TIRS matrix
        Matrix<double> R(3,3,0.0);
        R(0,0) = 1.0; R(1,1) = 1.0; R(2,2) = 1.0;
        iauRz(ERA, R);

        // The TIO locator s'
        double sp = iauSp00(JD_TO_MJD, mjd_tt.mjd);

        xp *= AS_TO_RAD;
        yp *= AS_TO_RAD;

        // TIRS-to-ITRS matrix
        Matrix<double> W(3,3,0.0);
        iauPom00(xp, yp, sp, W);

        return W * R * Q;

    }  // End of method 'ReferenceSystem::C2TMatrix()'


    /// Transformation matrix from ITRS to ICRS.
    Matrix<double> ReferenceSystem::T2CMatrix(const CommonTime& UTC)
    {

        return transpose( C2TMatrix(UTC) );

    }  // End of method 'ReferenceSystem::T2CMatrix()'



    /// Time derivative of transformation matrix from CRS to TRS coordinates
    /// for a given date
    Matrix<double> ReferenceSystem::dC2TMatrix(const CommonTime& UTC)
    {
        // EOP Data
        EOPDataStore::EOPData eop( getEOPData(UTC) );
        double xp = eop.xp;
        double yp = eop.yp;
        double UT1mUTC = eop.UT1mUTC;
        double dX = eop.dX;
        double dY = eop.dY;

        // TT
        CommonTime TT( UTC2TT(UTC) );
        MJD mjd_tt(TT);

        // Ocean Tide Corrections
        Vector<double> cor_ocean = ortho_eop(TT)*1e-6;
        xp += cor_ocean(0);
        yp += cor_ocean(1);
        UT1mUTC += cor_ocean(2);

        // Raw CIP X,Y coordinates
        double X(0.0), Y(0.0);
        iauXy06(JD_TO_MJD, mjd_tt.mjd, &X, &Y);

        // The CIO locator s
        double s = iauS06(JD_TO_MJD, mjd_tt.mjd, X, Y);

        // Corrected CIP X,Y coordinates
        X += dX * AS_TO_RAD;
        Y += dY * AS_TO_RAD;

        // GCRS-to-CTRS matrix
        Matrix<double> Q(3,3,0.0);
        iauC2ixys(X, Y, s, Q);

        // UT1
        CommonTime UT1( UTC2UT1(UTC, UT1mUTC) );
        MJD mjd_ut1(UT1);

        // Earth rotation angle and its time dot
        double ERA = iauEra00(JD_TO_MJD, mjd_ut1.mjd);
        double dERA = TWO_PI*1.00273781191135448/DAY_TO_SEC;

        // CIRS-to-TIRS matrix time dot
        Matrix<double> dR(3,3,0.0);
        dR(0,0) = -std::sin(ERA);
        dR(0,1) =  std::cos(ERA);
        dR(1,0) = -std::cos(ERA);
        dR(1,1) = -std::sin(ERA);
        dR = dR * dERA;

        xp *= AS_TO_RAD;
        yp *= AS_TO_RAD;

        // TIRS-to-ITRS matrix
        double sp = iauSp00(JD_TO_MJD, mjd_tt.mjd);
        Matrix<double> W(3,3,0.0);
        iauPom00(xp, yp, sp, W);

        return W * dR * Q;

    }  // End of method 'ReferenceSystem::dC2TMatrix()'


    /// Time derivative of transformation matrix from TRS to CRS coordinates
    /// for a given date
    Matrix<double> ReferenceSystem::dT2CMatrix(const CommonTime& UTC)
    {

        return transpose( dC2TMatrix(UTC) );

    }  // End of method 'ReferenceSystem::dT2CMatrix()'



    /**Convert coordinate difference in XYZ to RTN.
     *
     * @param dxyz    Coordinate difference in XYZ
     * @param r       Position used to define RTN
     * @param v       Velocity used to define RTN
     * @return        Coordinate difference in RTN
     */
    Vector<double> ReferenceSystem::XYZ2RTN(const Vector<double>& dxyz,
                                            const Vector<double>& r,
                                            const Vector<double>& v)
    {
        // Unit vectors
        Vector<double> r_unit( normalize(r) );
//        Vector<double> n_unit( normalize(cross(r,v)) );
//        Vector<double> t_unit( cross(r_unit,n_unit) );
        Vector<double> t_unit( normalize(v) );
        Vector<double> n_unit( normalize(cross(r,v)) );

        // Vector to return
        Vector<double> drtn(3,0.0);
        drtn(0) = dot(dxyz, r_unit);
        drtn(1) = dot(dxyz, t_unit);
        drtn(2) = dot(dxyz, n_unit);

        return drtn;

    }  // End of method 'ReferenceSystem::XYZ2RTN()'


}  // End of namespace 'gpstk'
