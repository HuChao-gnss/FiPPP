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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S.
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software.
//
//Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file Legendre.hpp
 */

#ifndef GPSTK_LEGENDRE_HPP
#define GPSTK_LEGENDRE_HPP

#include <cmath>
#include "Vector.hpp"

namespace gpstk
{

   /** @addtogroup math */

   /** Translate degree and order (n,m) from 2-D to 1-D.
    * @param   Degree
    * @param   Order
    *
    * For example:
    * (n,m) = (0,0) <===> return = 1
    *         (1,0) <===>          2
    *         (1,1) <===>          3
    *         (2,0) <===>          4
    *          ...  <===>         ...
    */
   int indexTranslator(int n, int m);


   /** Evaluate the fully normalized associated legendre functions and its gradients.
    * @param deg     Desired degree
    * @param lat     Geocentric latitude in radians
    * @param leg0    Fully normalized associated legendre functions (fnALF)
    * @param leg1    1st-order derivatives of fnALF
    * @param leg2    2nd-order derivatives of fnALF
    * @param idr     Indicator of what to be computed, 0(default) or 1 or 2
    *
    * Ref: E.Fantino, J Geod(2009), Methods of harmonic synthesis for global
    * geopotential models and their first-, second- and third-order gradients
    */
   void legendre(const int&      deg,
                 const double&   lat,
                 Vector<double>& leg0,
                 Vector<double>& leg1,
                 Vector<double>& leg2,
                 const int&      idr=0);


   //@}

} // namespace gpstk


#endif   // GPSTK_LEGENDRE_HPP
