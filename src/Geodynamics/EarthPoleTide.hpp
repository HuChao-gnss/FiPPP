
/**
* @file EarthPoleTide.hpp
* Class to do Earth Pole Tide correction.
*
*/

#ifndef EARTH_POLE_TIDE_HPP
#define EARTH_POLE_TIDE_HPP

#include "ReferenceSystem.hpp"


namespace gpstk
{
    /** @addtogroup GeoDynamics */
    //@{

    /**
     * Class to do Earth Pole Tide correction
     * see IERS Conventions 2010 Section 6.4 and 6.5 for more details.
     */
    class EarthPoleTide
    {
    public:
        /// Default constructor
        EarthPoleTide()
            : pRefSys(NULL)
        {}

        /// Default destructor
        ~EarthPoleTide() {}


        /// Set reference system
        inline EarthPoleTide& setReferenceSystem(ReferenceSystem& ref)
        {
            pRefSys = &ref;

            return (*this);
        }


        /// Get reference system
        inline ReferenceSystem* getReferenceSystem() const
        {
            return pRefSys;
        }


        /** Pole tide to normalized earth potential coefficients.
         *
         * @param tt    TT
         * @return      correction to normalized Cnm and Snm
         */
        Matrix<double> getPoleTide(CommonTime tt);


    protected:

        /// Reference System
        ReferenceSystem* pRefSys;

    }; // End of class 'EarthPoleTide'

    // @}

}  // End of namespace 'gpstk'

#endif   // EARTH_POLE_TIDE_HPP
