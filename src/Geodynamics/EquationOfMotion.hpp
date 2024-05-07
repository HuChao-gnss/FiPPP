
/**
 * @file EquationOfMotion.hpp
 *
 * Class provides the mechanism for passing a method
 * that computes the derivatives to an integrator.
 */

#ifndef EQUATION_OF_MOTION_HPP
#define EQUATION_OF_MOTION_HPP

#include "CommonTime.hpp"
#include "DataStructures.hpp"


namespace gpstk
{
    /** @addtogroup GeoDynamics */
    //@{

    /**
     * The EquationsOfMotion provides the mechanism for passing a method
     * that computes the derivatives to an integrator.
     *
     */
    class EquationOfMotion
    {
    public:
        /// Default constructor
        EquationOfMotion() {};

        /// Default deconstructor
        virtual ~EquationOfMotion() {};

        /** Compute the derivatives.
         * @params t    time or the independent variable.
         * @params y    the required data.
         * @return      the derivatives.
         */
        virtual satVectorMap getDerivatives( const CommonTime&   time,
                                             const satVectorMap& states ) = 0;

    }; // End of class 'EquationOfMotion'

    // @}

}  // End of namespace 'gpstk'

#endif  // EQUATION_OF_MOTION_HPP
