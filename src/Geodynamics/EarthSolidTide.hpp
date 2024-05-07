
/**
* @file EarthSolidTide.hpp
* Class to do Earth Solid Tide correction.
*/

#ifndef GPSTK_EARTH_SOLID_TIDE_HPP
#define GPSTK_EARTH_SOLID_TIDE_HPP


#include "ReferenceSystem.hpp"
#include "SolarSystem.hpp"


namespace gpstk
{
    /** @addtogroup GeoDynamics */
    //@{

    /** Class to do Earth Solid Tide correction
     * see IERS Conventions 2010 Section 6.2 for more details.
     */
    class EarthSolidTide
    {
    public:
        /// Default constructor
        EarthSolidTide()
            : pRefSys(NULL),
              pSolSys(NULL)
        {}

        /// Default destructor
        ~EarthSolidTide() {}


        /// Set reference system
        inline EarthSolidTide& setReferenceSystem(ReferenceSystem& ref)
        {
            pRefSys = &ref;

            return (*this);
        }

        /// Get reference system
        inline ReferenceSystem* getReferenceSystem() const
        {
            return pRefSys;
        }


        /// Set solar system
        inline EarthSolidTide& setSolarSystem(SolarSystem& sol)
        {
            pSolSys = &sol;

            return (*this);
        }

        /// Get solar system
        inline SolarSystem* getSolarSystem() const
        {
            return pSolSys;
        }


        /** Solid tide to normalized earth potential coefficients.
         *
         * @param tt    TT
         * @return      correction to normalized Cnm and Snm
         */
        Matrix<double> getSolidTide(CommonTime tt);


    protected:

        /// Parameters
        static const double Argu_C20[21][7];
        static const double Argu_C21[48][7];
        static const double Argu_C22[2][6];

        /// Reference System
        ReferenceSystem* pRefSys;

        /// Solar System
        SolarSystem* pSolSys;

    }; // End of class 'EarthSolidTide'

    // @}

}  // End of namespace 'gpstk'

#endif   // EARTH_SOLID_TIDE_HPP
