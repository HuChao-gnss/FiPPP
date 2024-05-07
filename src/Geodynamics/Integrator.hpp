#pragma ident "$Id$"

/**
 * @file Integrator.hpp
 *
 * This is an abstract base class for objects solving
 * a ODE system with integrator.
 */


#ifndef GPSTK_INTEGRATOR_HPP
#define GPSTK_INTEGRATOR_HPP

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
//============================================================================


#include "EquationOfMotion.hpp"


namespace gpstk
{
    /** @addtogroup GeoDynamics */
    //@{

    /** This is an abstract base class for objects solving
     * a ODE system with integrator.
     */
    class Integrator
    {
    public:
        /// Default constructor
        Integrator(double step=1.0)
            : stepSize(step)
        { pEOM = NULL; };


        /// Default deconstructor
        virtual ~Integrator() {};


        /// Set step size
        inline Integrator& setStepSize(const double& step)
        { stepSize = step; return (*this); };

        /// Get step size
        inline double getStepSize() const
        { return stepSize; };


        /// Set EquationOfMotion
        inline Integrator& setEquationOfMotion(EquationOfMotion& EOM)
        { pEOM = &EOM; };

        /// Get EquationOfMotion
        inline EquationOfMotion* getEquationOfMotion() const
        { return pEOM; };


        /// Real implementation
        virtual satVectorMap integrateTo( const CommonTime& t_next ) = 0;

    protected:

        /// Step Size
        double stepSize;

        /// Pointer to EquationOfMotion
        EquationOfMotion* pEOM;

    }; // End of class 'Integrator'

    // @}

}  // End of namespace 'gpstk'

#endif  // GPSTK_INTEGRATOR_HPP
