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
 * @file Legendre.cpp
 */

#include "Legendre.hpp"


namespace gpstk
{

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
   int indexTranslator(int n, int m)
   {
      return n*(n+1)/2 + (m+1);
   }


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
                 const int&      idr)
   {
      double slat, clat, tlat;
      slat = std::sin(lat);
      clat = std::cos(lat);
      tlat = std::tan(lat);

      int size = indexTranslator(deg, deg);

      if(0 == idr)         // leg0 to be computed
      {
         leg0.resize(size, 0.0);
      }
      else if(1 == idr)    // leg0 and leg1 to be computed
      {
         leg0.resize(size, 0.0);
         leg1.resize(size, 0.0);
      }
      else if(2 == idr)    // leg0, leg1 and leg2 to be computed
      {
         leg0.resize(size, 0.0);
         leg1.resize(size, 0.0);
         leg2.resize(size, 0.0);
      }


      leg0(0) = 1.0;     // P00

      // first, the leg0
      for(int n=1; n<=deg; ++n)
      {
         // Kronecker's delta
         double delta = ( (1==n) ? 1.0 : 0.0 );

         for(int m=0; m<=n; ++m)
         {
            // sectorials
            if(m == n)
            {
               double fn = std::sqrt((1.0+delta)*(2*n+1.0)/(2*n));

               // P(n,m) = fn * cos(lat) * P(n-1,m-1)
               leg0( indexTranslator(n,m)-1 ) = fn*clat*leg0( indexTranslator(n-1,m-1)-1 );
            }
            // zonals and tesserals
            else
            {
               double gnm = std::sqrt((2*n+1.0)*(2*n-1.0)/(n+m)/(n-m));
               double hnm = std::sqrt((2*n+1.0)*(n-m-1.0)*(n+m-1.0)/(2*n-3.0)/(n+m)/(n-m));
               if(m == n-1)
               {
                  // P(n,m) = gnm * sin(lat) * P(n-1,m)
                  leg0( indexTranslator(n,m)-1 ) = gnm*slat*leg0( indexTranslator(n-1,m)-1 );
               }
               else
               {
                  // P(n,m) = gnm * sin(lat) * P(n-1,m) - hnm * P(n-2,m)
                  leg0( indexTranslator(n,m)-1 ) = gnm*slat*leg0( indexTranslator(n-1,m)-1 )
                                       - hnm*leg0( indexTranslator(n-2,m)-1 );
               }
            }
         }
      }


      if(0 == idr) return;

      // then, the leg1
      for(int n=0; n<=deg; ++n)
      {
         for(int m=0; m<=n; ++m)
         {
            // Kronecker's delta
            double delta = ( (0 == m) ? 1.0 : 0.0 );

            double knm = std::sqrt((2.0-delta)*(n-m)*(n+m+1.0)/2);

            if(m == n)
            {
               // dP(n,m) = -m * tan(lat) * P(n,m), n = m
               leg1( indexTranslator(n,m)-1 ) = -m*tlat*leg0( indexTranslator(n,m)-1 );
            }
            else
            {
               // dP(n,m) = knm * P(n,m+1) - m * tan(lat) * P(n,m)
               leg1( indexTranslator(n,m)-1 ) = knm*leg0( indexTranslator(n,m+1)-1 )
                                    - m*tlat*leg0( indexTranslator(n,m)-1 );
            }
         }
      }


      if(1 == idr) return;

      // then, the leg2
      for(int n=0; n<=deg; ++n)
      {
         for(int m=0; m<=n; ++m)
         {
            leg2( indexTranslator(n,m)-1 ) = (m*m/clat/clat-n*(n+1))*leg0( indexTranslator(n,m)-1 )
                                 + tlat*leg1( indexTranslator(n,m)-1 );
         }
      }

   }  // End of function 'legendre()'


} // End of namespace gpstk
