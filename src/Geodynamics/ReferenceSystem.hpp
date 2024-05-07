/**
 * @file ReferenceSystem.hpp
 * This class ease handling reference system transformation.
 */

#ifndef REFERENCE_SYSTEM_HPP
#define REFERENCE_SYSTEM_HPP


#include <string>
#include "GNSSconstants.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"
#include "CommonTime.hpp"
#include "EOPDataStore.hpp"
#include "LeapSecStore.hpp"
#include "Exception.hpp"
#include "IERSConv2010.hpp"

namespace gpstk
{

    /** @addtogroup GeoDynamics */
    //@{

    /** Class to handle the reference system transformation, i.e.
     *  Transforming the ICRS to ITRS and vice versa.
     *
     *  All the transformation matrix are computed using the
     *  analytic fomulas.
     *
     *  The Earth Orentation Parameters are from the EOPDataStore,
     *  and the leap seconds data are from LeapSecStore.
     *
     *  When convert the ICRS to ITRS, you will use the time system
     *  conversion, and the relation between different time system
     *  follows as:
     *
     *           -14s
     *     -----------------> BDT(Compass Time)
     *     |
     *     |         +19s             +32.184s           +rel.effects
     *    GPS -------------> TAI ----------------> TT -----------------> TDB
     *                       T |
     *            -(UT1-TAI) | |    -leap seconds
     *    UT1 ---------------| |--------------------> UTC
     *     |
     *     |   earth rotation
     *     ---------------------> GAST
     *
     *  Most of the methods are modified from the code provided by
     *  Montenbruck, Oliver, 2001, as the attachement of the book
     *  "Satellite Orbits: Models, Methods and applications".
     *
     */
    class ReferenceSystem
    {
    public:

        /// Default constructor.
        ReferenceSystem()
            : pEopStore(NULL), pLeapSecStore(NULL), isPrepared(false)
        {}


        /** Constructor.
         */
        ReferenceSystem(EOPDataStore& eopStore,
                        LeapSecStore& leapSecStore)
            : isPrepared(false)
        {
            pEopStore = &eopStore;
            pLeapSecStore = &leapSecStore;
        }


        /// Set the EOP data store.
        ReferenceSystem& setEOPDataStore(EOPDataStore& eopStore)
        { pEopStore = &eopStore; return (*this); };


        /// Get the EOP data store.
        EOPDataStore* getEOPDataStore() const
        { return pEopStore; };


        /// Set the leapsec data store.
        ReferenceSystem& setLeapSecStore(LeapSecStore& leapSecStore)
        { pLeapSecStore = &leapSecStore; return (*this); };


        /// Get the leapsec data store.
        LeapSecStore* getLeapSecStore() const
        { return pLeapSecStore; };


        /// Get the x pole displacement parameter, in arcseconds
        double getXPole(const CommonTime& UTC) const
            noexcept(false);


        /// Get the y pole displacement parameter, in arcseconds
        double getYPole(const CommonTime& UTC) const
            noexcept(false);


        /// Get the value of (UT1 - UTC), in seconds
        double getUT1mUTC(const CommonTime& UTC) const
            noexcept(false);

        double getLOD(const CommonTime& UTC) const
            noexcept(false);

        /// Get the value of dX, in arcseconds
        double getDX(const CommonTime& UTC) const
            noexcept(false);

        /// Get the value of dY, in arcseconds
        double getDY(const CommonTime& UTC) const
            noexcept(false);


        /// Get the EOP data at the given UTC time.
        EOPDataStore::EOPData getEOPData(const CommonTime& UTC) const
            noexcept(false);


        /// Get the leapsec data at the given UTC time.
        double getLeapSec(const CommonTime& UTC) const
            noexcept(false);


        ///------ Methods to get the difference of different time systems ------//


        /// Get the value of (TAI - UTC) (= leapsec).
        double getTAImUTC(const CommonTime& UTC)
            noexcept(false);


        /// Get the value of (TT - UTC) (= 32.184 + leapsec).
        double getTTmUTC(const CommonTime& UTC)
            noexcept(false);


        /// Get the value of (TT - TAI) (= 32.184s).
        double getTTmTAI(void)
            noexcept(false);


        /// Get the value of (TAI - GPS) (= -19s).
        double getTAImGPS(void)
            noexcept(false);



        ///------ Methods to convert between different time systems ------//


        /// Convert GPS to UTC.
        CommonTime GPS2UTC(const CommonTime& GPS);


        /// Convert UTC to GPS.
        CommonTime UTC2GPS(const CommonTime& UTC);


        /// Convert GPS to TAI.
        CommonTime GPS2TAI(const CommonTime& GPS)
        { return (GPS + 19.0); }


        /// Convert TAI to GPS.
        CommonTime TAI2GPS(const CommonTime& TAI)
        { return (TAI - 19.0); }


        /// Convert GPS to TT.
        CommonTime GPS2TT(const CommonTime& GPS)
        { return (GPS + 19.0 + 32.184); }


        /// Convert TT to GPS.
        CommonTime TT2GPS(const CommonTime& TT)
        { return (TT - 19.0 - 32.184); }


        /// Convert UT1 to UTC.
        CommonTime UT12UTC(const CommonTime& UT1);


        /// Convert UTC to UT1.
        CommonTime UTC2UT1(const CommonTime& UTC);
        CommonTime UTC2UT1(const CommonTime& UTC, const double& UT1mUTC);


        /// Convert TAI to UTC.
        CommonTime TAI2UTC(const CommonTime& TAI);


        /// Convert UTC to TAI.
        CommonTime UTC2TAI(const CommonTime& UTC);


        /// Convert TT to UTC.
        CommonTime TT2UTC(const CommonTime& TT);


        /// Convert UTC to TT.
        CommonTime UTC2TT(const CommonTime& UTC);



        /// Convert TDB to UTC.
        CommonTime TDB2UTC(const CommonTime& TDB);


        /// Convert UTC to TDB.
        CommonTime UTC2TDB(const CommonTime& UTC);



        ///------ Methods to do reference system transformation ------///

        /// Transformation matrix from CRS to TRS coordinates for a given date
        Matrix<double> C2TMatrix(const CommonTime& UTC);


        /// Transformation matrix form TRS to CRS coordinates for a given date
        Matrix<double> T2CMatrix(const CommonTime& UTC);


        /// Time derivative of transformation matrix from CRS to TRS coordinates
        /// for a given date
        Matrix<double> dC2TMatrix(const CommonTime& UTC);


        /// Time derivative of transformation matrix from TRS to CRS coordinates
        /// for a given date
        Matrix<double> dT2CMatrix(const CommonTime& UTC);


        /**Convert coordinate difference in XYZ to RTN.
         *
         * @param dxyz     Coordinate difference in XYZ
         * @param r        Position used to define RTN
         * @param v        Velocity used to define RTN
         * @return         Coordinate difference in RTN
         */
        Vector<double> XYZ2RTN(const Vector<double>& dxyz,
                               const Vector<double>& r,
                               const Vector<double>& v);


        /// Deconstrutor
        virtual ~ReferenceSystem() {};


    private:

        /// Pointer to the EOPDataStore
        EOPDataStore* pEopStore;

        /// Pointer to the leap second store
        LeapSecStore* pLeapSecStore;

        /// whether the transformation matrix is prepared
        bool isPrepared;

    }; // End of class 'ReferenceSystem'

    // @}

}  // End of namespace 'gpstk'

#endif  // REFERENCE_SYSTEM_HPP
