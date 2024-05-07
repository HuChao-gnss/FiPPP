#pragma ident "$Id$"

/**
* @file EOPDataStore.hpp
*
*/

#ifndef EOPDATASTORE_HPP
#define EOPDATASTORE_HPP

#include <iostream>
#include <map>
#include "CommonTime.hpp"
#include "Vector.hpp"


namespace gpstk
{
    /** @addtogroup Precise Orbit Determination */
    //@{

    /**
     * Class to store and manage EOP data.
     *
     */
    class EOPDataStore
    {
    public:

        struct EOPData
        {
            double xp, yp;          /// arcsec
            double UT1mUTC;         /// seconds
            double LOD;             /// seconds/day
            double dX, dY;          /// arcsec

            double err_xp, err_yp;  /// arcsec
            double err_UT1mUTC;     /// seconds
            double err_LOD;         /// seconds/day
            double err_dX, err_dY;  /// arcsec
        };

        typedef std::map<CommonTime, EOPData> EOPDataMap;

    public:

        /// Default constructor
        EOPDataStore()
            : initialTime(CommonTime::END_OF_TIME),
              finalTime(CommonTime::BEGINNING_OF_TIME),
              maxInterval(86400.0),
              interpPoints(8),
              useBulletinB(false),
              regularization(false)
        {
            clear();
        }

        void clear()
        {
            allData.clear();
        }

        /// Default deconstructor
        virtual ~EOPDataStore()
        {
            allData.clear();
        }


        /// Set interpolation points
        inline EOPDataStore& setInterpPoints(int points)
        {
            interpPoints = points;

            return (*this);
        }


        /// Set use Bulletin B, only valid for IERS EOP data.
        /// Note: this method MUST be called before the file
        /// Load methods to be valid.
        inline EOPDataStore& setUseBulletinB(bool use)
        {
            useBulletinB = use;

            return (*this);
        }


        /// Set regularization
        inline EOPDataStore& setRegularization(bool reg)
        {
            regularization = reg;

            return (*this);
        }


        /// Add to the store directly
        void addEOPData(const CommonTime& utc, const EOPData& data)
            noexcept(false);


        /// Get the data at the given epoch
        EOPData getEOPData(const CommonTime& utc) const
            noexcept(false);


        /// Get the data at the given epoch and return it.
        double getXPole(const CommonTime& utc) const
            noexcept(false)
        {
            // Get the EOPData
            EOPData eopData( getEOPData(utc) );

            // Get yp value
            double xp( eopData.xp );

            // return
            return xp;
        }


        /// Get the data at the given epoch and return it.
        double getYPole(const CommonTime& utc) const
            noexcept(false)
        {
            // Get the EOPData
            EOPData eopData( getEOPData(utc) );

            // Get yp value
            double yp( eopData.yp );

            // return
            return yp;
      }


        /// Get the data at the given epoch and return it.
        double getUT1mUTC(const CommonTime& utc) const
            noexcept(false)
        {
            // Get the EOPData
            EOPData eopData( getEOPData(utc) );

            // Get UT1mUTC value
            double UT1mUTC( eopData.UT1mUTC );

            // return
            return UT1mUTC;
        }


        /// Get the data at the given epoch and return it.
        double getLOD(const CommonTime& utc) const
            noexcept(false)
        {
            // Get the EOPData
            EOPData eopData(  getEOPData(utc) );

            // Get LOD value
            double LOD( eopData.LOD );

            // return
            return LOD;
        }


        /// Get the data at the given epoch and return it.
        double getDX(const CommonTime& utc) const
            noexcept(false)
        {
            // Get the EOPData
            EOPData eopData(  getEOPData(utc) );

            // Get dX value
            double dX( eopData.dX );

            // return
            return dX;
        }


        /// Get the data at the given epoch and return it.
        double getDY(const CommonTime& utc) const
            noexcept(false)
        {
            // Get the EOPData
            EOPData eopData(  getEOPData(utc) );

            // Get dY value
            double dY( eopData.dY );

            // return
            return dY;
        }


        /// Add EOPs to the store via a flat IERS file.
        /// get finals.data from http://maia.usno.navy.mil/
        void loadIERSFile(std::string iersFile)
            noexcept(false);


        /// Add EOPs to the store via a flat IGS file.
        /// get igs*.erp data from IGS ftps
        void loadIGSFile(std::string igsFile)
            noexcept(false);


        /// Add EOPs to the store via a flat STK file.
        /// get eop*.txt from http://celestrak.com/
        void loadSTKFile(std::string stkFile)
            noexcept(false);


        /// Evaluate the effects of zonal Earth tides on the rotation of the
        /// Earth, IERS Conventions 2010
        Vector<double> RG_ZONT2(const CommonTime& TT) const;


    protected:

        /// Map holding all the EOP data
        EOPDataMap allData;

        /// Time span for the EOP data
        CommonTime initialTime;
        CommonTime finalTime;

        /// Points of interpolation, default is 8
        int interpPoints;
        double maxInterval;

        /// Use Bulletin B or not, only valid for IERS EOP Data
        bool useBulletinB;

        /// Do regularization or not, default false
        bool regularization;

    }; // End of class 'EOPDataStore'


    std::ostream& operator<<(std::ostream& s, const EOPDataStore::EOPData& d);

    // @}

}  // End of namespace 'gpstk'

#endif   // EOPDATASTORE_HPP
