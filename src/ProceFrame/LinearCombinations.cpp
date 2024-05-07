
/**
 * @file LinearCombinations.cpp
 * This class defines handy linear combinations of GDS data.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2009
//
//============================================================================


#include "LinearCombinations.hpp"

using namespace gpstk::StringUtils;

#define debug 0

namespace gpstk
{

    LinearCombinations::LinearCombinations()
    {

        // Coefficients of GPS PC/LC combinations, L1 and L2
        const double ag21( 1.0            /(  1.0 - GAMMA_GPS_L1L2 ) );
        const double bg21( GAMMA_GPS_L1L2 /(  1.0 - GAMMA_GPS_L1L2 ) );

        // ag12 = (f1^2)/(f1^2-f2^2) = GAMMA_GPS_L1L2/(GAMMA_GPS_L1L2 - 1.0 )
        // bg12 = (f2^2)/(f1^2-f2^2) = 1.0/(GAMMA_GPS_L1L2 - 1.0)
        const double ag12( GAMMA_GPS_L1L2 /(  GAMMA_GPS_L1L2 - 1.0 ) );
        const double bg12( 1.0            /(  GAMMA_GPS_L1L2 - 1.0 ) );
        // ag15 bg15 added by ragnwang
        const double ag15(GAMMA_GPS_L1L5 / (GAMMA_GPS_L1L5 - 1.0));
        const double bg15(1.0            / (GAMMA_GPS_L1L5 - 1.0));
        // e1 e2 e3
        const double denog( 2*( 1.0+SQR(GAMMA_GPS_L1L2)+SQR(GAMMA_GPS_L1L5)-
            GAMMA_GPS_L1L2*GAMMA_GPS_L1L5-GAMMA_GPS_L1L2-GAMMA_GPS_L1L5 ) );

        const double e1g( (SQR(GAMMA_GPS_L1L2)+SQR(GAMMA_GPS_L1L5)-GAMMA_GPS_L1L2-GAMMA_GPS_L1L5)/denog );
        const double e2g( (1.0+SQR(GAMMA_GPS_L1L5)-GAMMA_GPS_L1L2-GAMMA_GPS_L1L2*GAMMA_GPS_L1L5)/denog );
        const double e5g( (1.0+SQR(GAMMA_GPS_L1L2)-GAMMA_GPS_L1L5-GAMMA_GPS_L1L2*GAMMA_GPS_L1L5)/denog );

        // Galileo
        // Coefficients of Galileo PC/LC combinations, L1 and L5
        const double ae51( 1.0            /( 1.0  - GAMMA_GAL_L1L5 ) );
        const double be51( GAMMA_GAL_L1L5 /( 1.0  - GAMMA_GAL_L1L5 ) );

        const double ae15(GAMMA_GAL_L1L5 / (GAMMA_GAL_L1L5 - 1.0));
        const double be15(1.0            / (GAMMA_GAL_L1L5 - 1.0));
        const double ae17(GAMMA_GAL_L1L7 / (GAMMA_GAL_L1L7 - 1.0));
        const double be17(1.0            / (GAMMA_GAL_L1L7 - 1.0));
        const double ae18(GAMMA_GAL_L1L8 / (GAMMA_GAL_L1L8 - 1.0));
        const double be18(1.0            / (GAMMA_GAL_L1L8 - 1.0));
        const double ae16(GAMMA_GAL_L1L6 / (GAMMA_GAL_L1L6 - 1.0));
        const double be16(1.0            / (GAMMA_GAL_L1L6 - 1.0));

        // IF123
        const double deno3e( 2*( 1.0+SQR(GAMMA_GAL_L1L5)+SQR(GAMMA_GAL_L1L7)-
            GAMMA_GAL_L1L5*GAMMA_GAL_L1L7-GAMMA_GAL_L1L5-GAMMA_GAL_L1L7 ) );
        
        const double e31e( (SQR(GAMMA_GAL_L1L5)+SQR(GAMMA_GAL_L1L7)-GAMMA_GAL_L1L5-GAMMA_GAL_L1L7)/deno3e );
        const double e35e( (1.0+SQR(GAMMA_GAL_L1L7)-GAMMA_GAL_L1L5-GAMMA_GAL_L1L5*GAMMA_GAL_L1L7)/deno3e );
        const double e37e( (1.0+SQR(GAMMA_GAL_L1L5)-GAMMA_GAL_L1L7-GAMMA_GAL_L1L5*GAMMA_GAL_L1L7)/denog );
        // IF1234
        const double deno4e( 3*(1.0+SQR(GAMMA_GAL_L1L5)+SQR(GAMMA_GAL_L1L7)+SQR(GAMMA_GAL_L1L8))-
            2*(GAMMA_GAL_L1L5*GAMMA_GAL_L1L7+GAMMA_GAL_L1L5*GAMMA_GAL_L1L8+GAMMA_GAL_L1L7*GAMMA_GAL_L1L8
                +GAMMA_GAL_L1L5+GAMMA_GAL_L1L7+GAMMA_GAL_L1L8) );
        const double e41e( (SQR(GAMMA_GAL_L1L5)+SQR(GAMMA_GAL_L1L7)+SQR(GAMMA_GAL_L1L8)-
            GAMMA_GAL_L1L5-GAMMA_GAL_L1L7-GAMMA_GAL_L1L8)/deno4e );
        const double e45e( (1.0+SQR(GAMMA_GAL_L1L7)+SQR(GAMMA_GAL_L1L8)-GAMMA_GAL_L1L5-
            GAMMA_GAL_L1L5*GAMMA_GAL_L1L7-GAMMA_GAL_L1L5*GAMMA_BDS_L1L8)/deno4e );
        const double e47e( (1.0+SQR(GAMMA_GAL_L1L5)+SQR(GAMMA_GAL_L1L8)-GAMMA_GAL_L1L7-
            GAMMA_GAL_L1L7*GAMMA_GAL_L1L5-GAMMA_GAL_L1L7*GAMMA_GAL_L1L8)/deno4e );
        const double e48e( (1.0+SQR(GAMMA_GAL_L1L5)+SQR(GAMMA_GAL_L1L7)-GAMMA_GAL_L1L8-
            GAMMA_GAL_L1L8*GAMMA_GAL_L1L5-GAMMA_GAL_L1L8*GAMMA_GAL_L1L7)/deno4e );

        //if12345
        const double deno5e( 4*(1.0+SQR(GAMMA_GAL_L1L5)+SQR(GAMMA_GAL_L1L7)+SQR(GAMMA_GAL_L1L8)+SQR(GAMMA_GAL_L1L6))-
        2*(GAMMA_GAL_L1L5*GAMMA_GAL_L1L7+GAMMA_GAL_L1L5*GAMMA_GAL_L1L8+GAMMA_GAL_L1L5*GAMMA_GAL_L1L6+
            GAMMA_GAL_L1L7*GAMMA_GAL_L1L8+GAMMA_GAL_L1L7*GAMMA_GAL_L1L6+GAMMA_GAL_L1L8*GAMMA_GAL_L1L6+
            GAMMA_GAL_L1L5+GAMMA_GAL_L1L7+GAMMA_GAL_L1L8+GAMMA_GAL_L1L6) );
        const double e1e( (SQR(GAMMA_GAL_L1L5)+SQR(GAMMA_GAL_L1L7)+SQR(GAMMA_GAL_L1L8)+SQR(GAMMA_GAL_L1L6)-
            (GAMMA_GAL_L1L5+GAMMA_GAL_L1L7+GAMMA_GAL_L1L8+GAMMA_GAL_L1L6))/deno5e );
        const double e2e( (1.0+SQR(GAMMA_GAL_L1L7)+SQR(GAMMA_GAL_L1L8)+SQR(GAMMA_GAL_L1L6)-
            (GAMMA_GAL_L1L5+GAMMA_GAL_L1L5*GAMMA_GAL_L1L7+GAMMA_GAL_L1L5*GAMMA_GAL_L1L8+GAMMA_GAL_L1L5*GAMMA_GAL_L1L6)
            )/deno5e );
        const double e3e( (1.0+SQR(GAMMA_GAL_L1L5)+SQR(GAMMA_GAL_L1L8)+SQR(GAMMA_GAL_L1L6)-
            (GAMMA_GAL_L1L7+GAMMA_GAL_L1L5*GAMMA_GAL_L1L7+GAMMA_GAL_L1L7*GAMMA_GAL_L1L8+GAMMA_GAL_L1L7*GAMMA_GAL_L1L6)
            )/deno5e );
        const double e4e( (1.0+SQR(GAMMA_GAL_L1L5)+SQR(GAMMA_GAL_L1L7)+SQR(GAMMA_GAL_L1L6)-
            (GAMMA_GAL_L1L8+GAMMA_GAL_L1L5*GAMMA_GAL_L1L8+GAMMA_GAL_L1L7*GAMMA_GAL_L1L8+GAMMA_GAL_L1L8*GAMMA_GAL_L1L6)
            )/deno5e );
        const double e5e( (1.0+SQR(GAMMA_GAL_L1L5)+SQR(GAMMA_GAL_L1L7)+SQR(GAMMA_GAL_L1L8)-
            (GAMMA_GAL_L1L6+GAMMA_GAL_L1L5*GAMMA_GAL_L1L6+GAMMA_GAL_L1L7*GAMMA_GAL_L1L6+GAMMA_GAL_L1L8*GAMMA_GAL_L1L6)
            )/deno5e );


        /*-----------------------------BDS---------------------------------*/
        const double ac72( 1.0            /( 1.0 -  GAMMA_BDS_L2L7 ) );
        const double bc72( GAMMA_BDS_L2L7 /( 1.0 -  GAMMA_BDS_L2L7 ) );

        const double ac26(GAMMA_BDS_L2L6 / (GAMMA_BDS_L2L6 - 1.0));
        const double bc26(1.0            / (GAMMA_BDS_L2L6 - 1.0));

        const double ac27(GAMMA_BDS_L2L7 / (GAMMA_BDS_L2L7 - 1.0));
        const double bc27(1.0            / (GAMMA_BDS_L2L7 - 1.0));

		const double ac21(GAMMA_BDS_L2L1 / (GAMMA_BDS_L2L1 - 1.0));
		const double bc21(1.0            / (GAMMA_BDS_L2L1 - 1.0));

        const double ac61(GAMMA_BDS_L6L1 / (GAMMA_BDS_L6L1 - 1.0));
        const double bc61(1.0            / (GAMMA_BDS_L6L1 - 1.0));

		const double ac25(GAMMA_BDS_L2L5 / (GAMMA_BDS_L2L5 - 1.0));
		const double bc25(1.0            / (GAMMA_BDS_L2L5 - 1.0));

		const double ac28(GAMMA_BDS_L2L8 / (GAMMA_BDS_L2L8 - 1.0));
		const double bc28(1.0            / (GAMMA_BDS_L2L8 - 1.0));
        // bdsfopt
        const double ac15(GAMMA_BDS_L1L5 / (GAMMA_BDS_L1L5 - 1.0));
        const double bc15(1.0            / (GAMMA_BDS_L1L5 - 1.0));

        const double ac12(GAMMA_BDS_L1L2 / (GAMMA_BDS_L1L2 - 1.0));
        const double bc12(1.0            / (GAMMA_BDS_L1L2 - 1.0));
        // 52
        const double ac52(GAMMA_BDS_L5L2 / (GAMMA_BDS_L5L2 - 1.0));
        const double bc52(1.0            / (GAMMA_BDS_L5L2 - 1.0));

        const double ac16(GAMMA_BDS_L1L6 / (GAMMA_BDS_L1L6 - 1.0));
        const double bc16(1.0            / (GAMMA_BDS_L1L6 - 1.0));

        const double ac18(GAMMA_BDS_L1L8 / (GAMMA_BDS_L1L8 - 1.0));
        const double bc18(1.0            / (GAMMA_BDS_L1L8 - 1.0));

        const double ac17(GAMMA_BDS_L1L7 / (GAMMA_BDS_L1L7 - 1.0));
        const double bc17(1.0            / (GAMMA_BDS_L1L7 - 1.0));
        // IF123
        const double deno3nc( 2*( 1.0+SQR(GAMMA_BDS_L2L6)+SQR(GAMMA_BDS_L2L1)-
            GAMMA_BDS_L2L6*GAMMA_BDS_L2L1-GAMMA_BDS_L2L6-GAMMA_BDS_L2L1 ) );
        
        const double e32nc( (SQR(GAMMA_BDS_L2L6)+SQR(GAMMA_BDS_L2L1)-GAMMA_BDS_L2L6-GAMMA_BDS_L2L1)/deno3nc );
        const double e36nc( (1.0+SQR(GAMMA_BDS_L2L1)-GAMMA_BDS_L2L6-GAMMA_BDS_L2L6*GAMMA_BDS_L2L1)/deno3nc );
        const double e31nc( (1.0+SQR(GAMMA_BDS_L2L6)-GAMMA_BDS_L2L1-GAMMA_BDS_L2L1*GAMMA_BDS_L2L6)/deno3nc );
        // IF1234
        const double deno4nc( 3*(1.0+SQR(GAMMA_BDS_L2L6)+SQR(GAMMA_BDS_L2L1)+SQR(GAMMA_BDS_L2L5))-
            2*(GAMMA_BDS_L2L6*GAMMA_BDS_L2L1+GAMMA_BDS_L2L6*GAMMA_BDS_L2L5+GAMMA_BDS_L2L1*GAMMA_BDS_L2L5
                +GAMMA_BDS_L2L6+GAMMA_BDS_L2L1+GAMMA_BDS_L2L5) );
        const double e42nc( (SQR(GAMMA_BDS_L2L6)+SQR(GAMMA_BDS_L2L1)+SQR(GAMMA_BDS_L2L5)-
            GAMMA_BDS_L2L6-GAMMA_BDS_L2L1-GAMMA_BDS_L2L5)/deno4nc );
        const double e46nc( (1.0+SQR(GAMMA_BDS_L2L1)+SQR(GAMMA_BDS_L2L5)-GAMMA_BDS_L2L6-
            GAMMA_BDS_L2L6*GAMMA_BDS_L2L1-GAMMA_BDS_L2L6*GAMMA_BDS_L2L5)/deno4nc );
        const double e41nc( (1.0+SQR(GAMMA_BDS_L2L6)+SQR(GAMMA_BDS_L2L5)-GAMMA_BDS_L2L1-
            GAMMA_BDS_L2L1*GAMMA_BDS_L2L6-GAMMA_BDS_L2L1*GAMMA_BDS_L2L5)/deno4nc );
        const double e45nc( (1.0+SQR(GAMMA_BDS_L2L6)+SQR(GAMMA_BDS_L2L1)-GAMMA_BDS_L2L5-
            GAMMA_BDS_L2L5*GAMMA_BDS_L2L6-GAMMA_BDS_L2L5*GAMMA_BDS_L2L1)/deno4nc );
        //if12345
        const double deno5nc( 4*(1.0+SQR(GAMMA_BDS_L2L6)+SQR(GAMMA_BDS_L2L1)+SQR(GAMMA_BDS_L2L5)+SQR(GAMMA_BDS_L2L8))-
        2*(GAMMA_BDS_L2L6*GAMMA_BDS_L2L1+GAMMA_BDS_L2L6*GAMMA_BDS_L2L5+GAMMA_BDS_L2L6*GAMMA_BDS_L2L8+
            GAMMA_BDS_L2L1*GAMMA_BDS_L2L5+GAMMA_BDS_L2L1*GAMMA_BDS_L2L8+GAMMA_BDS_L2L5*GAMMA_BDS_L2L8+
            GAMMA_BDS_L2L6+GAMMA_BDS_L2L1+GAMMA_BDS_L2L5+GAMMA_BDS_L2L8) );
        const double e1nc( 
            ( SQR(GAMMA_BDS_L2L6)+SQR(GAMMA_BDS_L2L1)+SQR(GAMMA_BDS_L2L5)+SQR(GAMMA_BDS_L2L8)-
            (GAMMA_BDS_L2L6+GAMMA_BDS_L2L1+GAMMA_BDS_L2L5+GAMMA_BDS_L2L8) )/deno5nc 
        );
        const double e2nc( 
            (1.0+SQR(GAMMA_BDS_L2L1)+SQR(GAMMA_BDS_L2L5)+SQR(GAMMA_BDS_L2L8)-
            (GAMMA_BDS_L2L6+GAMMA_BDS_L2L6*GAMMA_BDS_L2L1+GAMMA_BDS_L2L6*GAMMA_BDS_L2L5+GAMMA_BDS_L2L6*GAMMA_BDS_L2L8)
            )/deno5nc 
        );
        const double e3nc( 
            (1.0+SQR(GAMMA_BDS_L2L6)+SQR(GAMMA_BDS_L2L5)+SQR(GAMMA_BDS_L2L8)-
            (GAMMA_BDS_L2L1+GAMMA_BDS_L2L1*GAMMA_BDS_L2L6+GAMMA_BDS_L2L1*GAMMA_BDS_L2L5+GAMMA_BDS_L2L1*GAMMA_BDS_L2L8)
            )/deno5nc 
        );
        const double e4nc( 
            (1.0+SQR(GAMMA_BDS_L2L6)+SQR(GAMMA_BDS_L2L1)+SQR(GAMMA_BDS_L2L8)-
            (GAMMA_BDS_L2L5+GAMMA_BDS_L2L5*GAMMA_BDS_L2L6+GAMMA_BDS_L2L5*GAMMA_BDS_L2L1+GAMMA_BDS_L2L5*GAMMA_BDS_L2L8)
            )/deno5nc 
        );
        const double e5nc( 
            (1.0+SQR(GAMMA_BDS_L2L6)+SQR(GAMMA_BDS_L2L1)+SQR(GAMMA_BDS_L2L5)-
            (GAMMA_BDS_L2L8+GAMMA_BDS_L2L8*GAMMA_BDS_L2L6+GAMMA_BDS_L2L8*GAMMA_BDS_L2L1+GAMMA_BDS_L2L8*GAMMA_BDS_L2L5)
            )/deno5nc
        );

        
        // BDS: B1c/B2a/B1I/B3I/B2
        // IF123
        const double deno3c( 
            2*( 1.0+SQR(GAMMA_BDS_L1L5)+SQR(GAMMA_BDS_L1L2)-
            GAMMA_BDS_L1L5*GAMMA_BDS_L1L2-GAMMA_BDS_L1L5-GAMMA_BDS_L1L2 ) 
        );
        
        const double e31c( (SQR(GAMMA_BDS_L1L5)+SQR(GAMMA_BDS_L1L2)-GAMMA_BDS_L1L5-GAMMA_BDS_L1L2)/deno3nc );
        const double e35c( (1.0+SQR(GAMMA_BDS_L1L2)-GAMMA_BDS_L1L5-GAMMA_BDS_L1L5*GAMMA_BDS_L1L2)/deno3nc );
        const double e32c( (1.0+SQR(GAMMA_BDS_L1L5)-GAMMA_BDS_L1L2-GAMMA_BDS_L1L2*GAMMA_BDS_L1L5)/deno3nc );
        // IF1234
        const double deno4c( 
            3*(1.0+SQR(GAMMA_BDS_L1L5)+SQR(GAMMA_BDS_L1L2)+SQR(GAMMA_BDS_L1L6))-
            2*(GAMMA_BDS_L1L5*GAMMA_BDS_L1L2+GAMMA_BDS_L1L5*GAMMA_BDS_L1L6+GAMMA_BDS_L1L2*GAMMA_BDS_L1L6
                +GAMMA_BDS_L1L5+GAMMA_BDS_L1L2+GAMMA_BDS_L1L6) 
        );
        const double e41c( 
            (SQR(GAMMA_BDS_L1L5)+SQR(GAMMA_BDS_L1L2)+SQR(GAMMA_BDS_L1L6)-
            GAMMA_BDS_L1L5-GAMMA_BDS_L1L2-GAMMA_BDS_L1L6)/deno4nc 
        );
        const double e45c( 
            (1.0+SQR(GAMMA_BDS_L1L2)+SQR(GAMMA_BDS_L1L6)-GAMMA_BDS_L1L5-
            GAMMA_BDS_L1L5*GAMMA_BDS_L1L2-GAMMA_BDS_L1L5*GAMMA_BDS_L1L6)/deno4nc 
        );
        const double e42c( 
            (1.0+SQR(GAMMA_BDS_L1L5)+SQR(GAMMA_BDS_L1L6)-GAMMA_BDS_L1L2-
            GAMMA_BDS_L1L2*GAMMA_BDS_L1L5-GAMMA_BDS_L1L2*GAMMA_BDS_L1L6)/deno4nc 
        );
        const double e46c( 
            (1.0+SQR(GAMMA_BDS_L1L5)+SQR(GAMMA_BDS_L1L2)-GAMMA_BDS_L1L6-
            GAMMA_BDS_L1L6*GAMMA_BDS_L1L5-GAMMA_BDS_L1L6*GAMMA_BDS_L1L2)/deno4nc 
        );
        //if12345
        const double deno5c( 
            4*(1.0+SQR(GAMMA_BDS_L1L5)+SQR(GAMMA_BDS_L1L2)+SQR(GAMMA_BDS_L1L6)+SQR(GAMMA_BDS_L1L8))-
        2*(GAMMA_BDS_L1L5*GAMMA_BDS_L1L2+GAMMA_BDS_L1L5*GAMMA_BDS_L1L6+GAMMA_BDS_L1L5*GAMMA_BDS_L1L8+
            GAMMA_BDS_L1L2*GAMMA_BDS_L1L6+GAMMA_BDS_L1L2*GAMMA_BDS_L1L8+GAMMA_BDS_L1L6*GAMMA_BDS_L1L8+
            GAMMA_BDS_L1L5+GAMMA_BDS_L1L2+GAMMA_BDS_L1L6+GAMMA_BDS_L1L8) 
        );
        const double e1c( 
            (SQR(GAMMA_BDS_L1L5)+SQR(GAMMA_BDS_L1L2)+SQR(GAMMA_BDS_L1L6)+SQR(GAMMA_BDS_L1L8)-
            (GAMMA_BDS_L1L5+GAMMA_BDS_L1L2+GAMMA_BDS_L1L6+GAMMA_BDS_L1L8))/deno5c 
        );
        const double e2c( 
            (1.0+SQR(GAMMA_BDS_L1L2)+SQR(GAMMA_BDS_L1L6)+SQR(GAMMA_BDS_L1L8)-
            (GAMMA_BDS_L1L5+GAMMA_BDS_L1L5*GAMMA_BDS_L1L2+GAMMA_BDS_L1L5*GAMMA_BDS_L1L6+GAMMA_BDS_L1L5*GAMMA_BDS_L1L8)
            )/deno5c 
        );
        const double e3c( 
            (1.0+SQR(GAMMA_BDS_L1L5)+SQR(GAMMA_BDS_L1L6)+SQR(GAMMA_BDS_L1L8)-
            (GAMMA_BDS_L1L2+GAMMA_BDS_L1L2*GAMMA_BDS_L1L5+GAMMA_BDS_L1L2*GAMMA_BDS_L1L6+GAMMA_BDS_L1L2*GAMMA_BDS_L1L8)
            )/deno5c 
        );
        const double e4c( 
            (1.0+SQR(GAMMA_BDS_L1L5)+SQR(GAMMA_BDS_L1L2)+SQR(GAMMA_BDS_L1L8)-
            (GAMMA_BDS_L1L6+GAMMA_BDS_L1L6*GAMMA_BDS_L1L5+GAMMA_BDS_L1L6*GAMMA_BDS_L1L2+GAMMA_BDS_L1L6*GAMMA_BDS_L1L8)
            )/deno5c 
        );
        const double e5c( 
            (1.0+SQR(GAMMA_BDS_L1L5)+SQR(GAMMA_BDS_L1L2)+SQR(GAMMA_BDS_L1L6)-
            (GAMMA_BDS_L1L8+GAMMA_BDS_L1L8*GAMMA_BDS_L1L5+GAMMA_BDS_L1L8*GAMMA_BDS_L1L2+GAMMA_BDS_L1L8*GAMMA_BDS_L1L6)
            )/deno5c 
        );

        // if158
        const double deno158c( 2*( 1.0+SQR(GAMMA_BDS_L1L5)+SQR(GAMMA_BDS_L1L8)-
            GAMMA_BDS_L1L5*GAMMA_BDS_L1L8-GAMMA_BDS_L1L5-GAMMA_BDS_L1L8 ) );
        const double e1581c( (SQR(GAMMA_BDS_L1L5)+SQR(GAMMA_BDS_L1L8)-GAMMA_BDS_L1L5-GAMMA_BDS_L1L8)/deno158c );
        const double e1585c( (1.0+SQR(GAMMA_BDS_L1L8)-GAMMA_BDS_L1L5-GAMMA_BDS_L1L5*GAMMA_BDS_L1L8)/deno158c );
        const double e1588c( (1.0+SQR(GAMMA_BDS_L1L5)-GAMMA_BDS_L1L8-GAMMA_BDS_L1L8*GAMMA_BDS_L1L5)/deno158c );

        //267
        const double deno267c( 2*( 1.0+SQR(GAMMA_BDS_L2L6)+SQR(GAMMA_BDS_L2L7)-
            GAMMA_BDS_L2L6*GAMMA_BDS_L2L7-GAMMA_BDS_L2L6-GAMMA_BDS_L2L7 ) );
        const double e2672c( (SQR(GAMMA_BDS_L2L6)+SQR(GAMMA_BDS_L2L7)-GAMMA_BDS_L2L6-GAMMA_BDS_L2L7)/ deno267c);
        const double e2676c( (1.0+SQR(GAMMA_BDS_L2L7)-GAMMA_BDS_L2L6-GAMMA_BDS_L2L6*GAMMA_BDS_L2L7)/ deno267c);
        const double e2677c( (1.0+SQR(GAMMA_BDS_L2L6)-GAMMA_BDS_L2L7-GAMMA_BDS_L2L7*GAMMA_BDS_L2L6)/ deno267c);

        // Definition to compute PC combination of GPS, C1 and C2
        pc21CombOfGPS.header                =   TypeID::PC21G;
        pc21CombOfGPS.body[TypeID::C2G]     =   +ag21;
        pc21CombOfGPS.body[TypeID::C1G]     =   -bg21;

        // Definition to compute LC combination of GPS, L1 and L2
        lc21CombOfGPS.header                =   TypeID::LC21G;
        lc21CombOfGPS.body[TypeID::L2G]     =   +ag21;
        lc21CombOfGPS.body[TypeID::L1G]     =   -bg21;
        /*----------------------------------------------------*/
        pc12CombOfGPS.header                =   TypeID::PC12G;
        pc12CombOfGPS.body[TypeID::C1G]     =   +ag12;
        pc12CombOfGPS.body[TypeID::C2G]     =   -bg12;

        // Definition to compute LC combination of GPS, L1 and L2
        lc12CombOfGPS.header                =   TypeID::LC12G;
        lc12CombOfGPS.body[TypeID::L1G]     =   +ag12;
        lc12CombOfGPS.body[TypeID::L2G]     =   -bg12;

        //added by ragnwang
        pc15CombOfGPS.header = TypeID::PC15G;
        pc15CombOfGPS.body[TypeID::C1G] = +ag15;
        pc15CombOfGPS.body[TypeID::C5G] = -bg15;

		lc15CombOfGPS.header = TypeID::LC15G;
		lc15CombOfGPS.body[TypeID::L1G] = +ag15;
		lc15CombOfGPS.body[TypeID::L5G] = -bg15;
        // IF123
        pc125CombOfGPS.header = TypeID::PC125G;
        pc125CombOfGPS.body[TypeID::C1G] = e1g;
        pc125CombOfGPS.body[TypeID::C2G] = e2g;
        pc125CombOfGPS.body[TypeID::C5G] = e5g;

		lc125CombOfGPS.header = TypeID::LC125G;
		lc125CombOfGPS.body[TypeID::L1G] = e1g;
		lc125CombOfGPS.body[TypeID::L2G] = e2g;
		lc125CombOfGPS.body[TypeID::L5G] = e5g;

        // Definition to compute PC combination of Galileo, C1 and C5
        pc51CombOfGAL.header                =   TypeID::PC51E;
        pc51CombOfGAL.body[TypeID::C5E]     =   +ae51;
        pc51CombOfGAL.body[TypeID::C1E]     =   -be51;

        // Definition to compute LC combination of Galileo, L1 and L5
        lc51CombOfGAL.header                =   TypeID::LC51E;
        lc51CombOfGAL.body[TypeID::L5E]     =   +ae51;
        lc51CombOfGAL.body[TypeID::L1E]     =   -be51;

        // 15786
        // GAL 15E  
        pc15CombOfGAL.header = TypeID::PC15E;
        pc15CombOfGAL.body[TypeID::C1E] = +ae15;
        pc15CombOfGAL.body[TypeID::C5E] = -be15;

		lc15CombOfGAL.header = TypeID::LC15E;
		lc15CombOfGAL.body[TypeID::L1E] = +ae15;
		lc15CombOfGAL.body[TypeID::L5E] = -be15;
        // 17
        pc17CombOfGAL.header = TypeID::PC17E;
        pc17CombOfGAL.body[TypeID::C1E] = +ae17;
        pc17CombOfGAL.body[TypeID::C7E] = -be17;
        lc17CombOfGAL.header = TypeID::LC17E;
        lc17CombOfGAL.body[TypeID::L1E] = +ae17;
        lc17CombOfGAL.body[TypeID::L7E] = -be17;
        // 18
        pc18CombOfGAL.header = TypeID::PC18E;
        pc18CombOfGAL.body[TypeID::C1E] = +ae18;
        pc18CombOfGAL.body[TypeID::C8E] = -be18;
        lc18CombOfGAL.header = TypeID::LC18E;
        lc18CombOfGAL.body[TypeID::L1E] = +ae18;
        lc18CombOfGAL.body[TypeID::L8E] = -be18;
        // 16
        pc16CombOfGAL.header = TypeID::PC16E;
        pc16CombOfGAL.body[TypeID::C1E] = +ae16;
        pc16CombOfGAL.body[TypeID::C6E] = -be16;
        lc16CombOfGAL.header = TypeID::LC16E;
        lc16CombOfGAL.body[TypeID::L1E] = +ae16;
        lc16CombOfGAL.body[TypeID::L6E] = -be16;
        // IF123
        pc157CombOfGAL.header = TypeID::PC157E;
        pc157CombOfGAL.body[TypeID::C1E] = e31e;
        pc157CombOfGAL.body[TypeID::C5E] = e35e;
        pc157CombOfGAL.body[TypeID::C7E] = e37e;
        lc157CombOfGAL.header = TypeID::LC157E;
        lc157CombOfGAL.body[TypeID::L1E] = e31e;
        lc157CombOfGAL.body[TypeID::L5E] = e35e;
        lc157CombOfGAL.body[TypeID::L7E] = e37e;
        //IF1234
        pc1578CombOfGAL.header = TypeID::PC1578E;
        pc1578CombOfGAL.body[TypeID::C1E] = e41e;
        pc1578CombOfGAL.body[TypeID::C5E] = e45e;
        pc1578CombOfGAL.body[TypeID::C7E] = e47e;
        pc1578CombOfGAL.body[TypeID::C8E] = e48e;
        lc1578CombOfGAL.header = TypeID::LC1578E;
        lc1578CombOfGAL.body[TypeID::L1E] = e41e;
        lc1578CombOfGAL.body[TypeID::L5E] = e45e;
        lc1578CombOfGAL.body[TypeID::L7E] = e47e;
        lc1578CombOfGAL.body[TypeID::L8E] = e48e;
        
        // IF12345
        pc15786CombOfGAL.header = TypeID::PC15786E;
        pc15786CombOfGAL.body[TypeID::C1E] = e1e;
        pc15786CombOfGAL.body[TypeID::C5E] = e2e;
        pc15786CombOfGAL.body[TypeID::C7E] = e3e;
        pc15786CombOfGAL.body[TypeID::C8E] = e4e;
        pc15786CombOfGAL.body[TypeID::C6E] = e5e;

		lc15786CombOfGAL.header = TypeID::LC15786E;
		lc15786CombOfGAL.body[TypeID::L1E] = e1e;
		lc15786CombOfGAL.body[TypeID::L5E] = e2e;
		lc15786CombOfGAL.body[TypeID::L7E] = e3e;
		lc15786CombOfGAL.body[TypeID::L8E] = e4e;
		lc15786CombOfGAL.body[TypeID::L6E] = e5e;

         // Definition to compute PC combination of BDS, C2 and C6
        pc26CombOfBDS.header                =   TypeID::PC26C;
        pc26CombOfBDS.body[TypeID::C2C]     =   +ac26;
        pc26CombOfBDS.body[TypeID::C6C]     =   -bc26;

        lc26CombOfBDS.header                =   TypeID::LC26C;
        lc26CombOfBDS.body[TypeID::L2C]     =   +ac26;
        lc26CombOfBDS.body[TypeID::L6C]     =   -bc26;

        // bds2 27
        pc27CombOfBDS.header = TypeID::PC27C;
        pc27CombOfBDS.body[TypeID::C2C] = +ac27;
        pc27CombOfBDS.body[TypeID::C7C] = -bc27;

		lc27CombOfBDS.header = TypeID::LC27C;
		lc27CombOfBDS.body[TypeID::L2C] = +ac27;
		lc27CombOfBDS.body[TypeID::L7C] = -bc27;

        // 267
        pc267CombOfBDS.header = TypeID::PC267C;
        pc267CombOfBDS.body[TypeID::C2C] = e2672c;
        pc267CombOfBDS.body[TypeID::C6C] = e2676c;
        pc267CombOfBDS.body[TypeID::C7C] = e2677c;

		lc267CombOfBDS.header = TypeID::LC267C;
		lc267CombOfBDS.body[TypeID::L2C] = e2672c;
		lc267CombOfBDS.body[TypeID::L6C] = e2676c;
		lc267CombOfBDS.body[TypeID::L7C] = e2677c;

        // added by ragnwang
        pc21CombOfBDS.header = TypeID::PC21C;
        pc21CombOfBDS.body[TypeID::C2C] = +ac21;
        pc21CombOfBDS.body[TypeID::C1C] = -bc21;

		lc21CombOfBDS.header = TypeID::LC21C;
		lc21CombOfBDS.body[TypeID::L2C] = +ac21;
		lc21CombOfBDS.body[TypeID::L1C] = -bc21;
        //61
        pc61CombOfBDS.header = TypeID::PC61C;
        pc61CombOfBDS.body[TypeID::C6C] = +ac61;
        pc61CombOfBDS.body[TypeID::C1C] = -bc61;

        lc61CombOfBDS.header = TypeID::LC61C;
        lc61CombOfBDS.body[TypeID::L6C] = +ac61;
        lc61CombOfBDS.body[TypeID::L1C] = -bc61;
        //25
        pc25CombOfBDS.header = TypeID::PC25C;
        pc25CombOfBDS.body[TypeID::C2C] = +ac25;
        pc25CombOfBDS.body[TypeID::C5C] = -bc25;

        lc25CombOfBDS.header = TypeID::LC25C;
        lc25CombOfBDS.body[TypeID::L2C] = +ac25;
        lc25CombOfBDS.body[TypeID::L5C] = -bc25;
        // 28
        pc28CombOfBDS.header = TypeID::PC28C;
        pc28CombOfBDS.body[TypeID::C2C] = +ac28;
        pc28CombOfBDS.body[TypeID::C8C] = -bc28;

        lc28CombOfBDS.header = TypeID::LC28C;
        lc28CombOfBDS.body[TypeID::L2C] = +ac28;
        lc28CombOfBDS.body[TypeID::L8C] = -bc28;
        // 261
        pc261CombOfBDS.header = TypeID::PC261C;
        pc261CombOfBDS.body[TypeID::C2C] = e32nc;
        pc261CombOfBDS.body[TypeID::C6C] = e36nc;
        pc261CombOfBDS.body[TypeID::C1C] = e31nc;
        lc261CombOfBDS.header = TypeID::LC261C;
        lc261CombOfBDS.body[TypeID::L2C] = e32nc;
        lc261CombOfBDS.body[TypeID::L6C] = e36nc;
        lc261CombOfBDS.body[TypeID::L1C] = e31nc;
        // 2615
        pc2615CombOfBDS.header = TypeID::PC2615C;
        pc2615CombOfBDS.body[TypeID::C2C] = e42nc;
        pc2615CombOfBDS.body[TypeID::C6C] = e46nc;
        pc2615CombOfBDS.body[TypeID::C1C] = e41nc;
        pc2615CombOfBDS.body[TypeID::C5C] = e45nc;
        lc2615CombOfBDS.header = TypeID::LC2615C;
        lc2615CombOfBDS.body[TypeID::L2C] = e42nc;
        lc2615CombOfBDS.body[TypeID::L6C] = e46nc;
        lc2615CombOfBDS.body[TypeID::L1C] = e41nc;
        lc2615CombOfBDS.body[TypeID::L5C] = e45nc;

        //26158
        pc26158CombOfBDS.header = TypeID::PC26158C;
        pc26158CombOfBDS.body[TypeID::C2C] = e1nc;
        pc26158CombOfBDS.body[TypeID::C6C] = e2nc;
        pc26158CombOfBDS.body[TypeID::C1C] = e3nc;
        pc26158CombOfBDS.body[TypeID::C5C] = e4nc;
        pc26158CombOfBDS.body[TypeID::C8C] = e5nc;

		lc26158CombOfBDS.header = TypeID::LC26158C;
		lc26158CombOfBDS.body[TypeID::L2C] = e1nc;
		lc26158CombOfBDS.body[TypeID::L6C] = e2nc;
		lc26158CombOfBDS.body[TypeID::L1C] = e3nc;
		lc26158CombOfBDS.body[TypeID::L5C] = e4nc;
		lc26158CombOfBDS.body[TypeID::L8C] = e5nc;

        //bdsfopt
        // 15
        pc15CombOfBDS.header = TypeID::PC15C;
        pc15CombOfBDS.body[TypeID::C1C] = +ac15;
        pc15CombOfBDS.body[TypeID::C5C] = -bc15;

		lc15CombOfBDS.header = TypeID::LC15C;
		lc15CombOfBDS.body[TypeID::L1C] = +ac15;
		lc15CombOfBDS.body[TypeID::L5C] = -bc15;
        // 12
        pc12CombOfBDS.header = TypeID::PC12C;
        pc12CombOfBDS.body[TypeID::C1C] = +ac12;
        pc12CombOfBDS.body[TypeID::C2C] = -bc12;
        
		lc12CombOfBDS.header = TypeID::LC12C;
		lc12CombOfBDS.body[TypeID::L1C] = +ac12;
		lc12CombOfBDS.body[TypeID::L2C] = -bc12;
        //52
        pc52CombOfBDS.header = TypeID::PC52C;
        pc52CombOfBDS.body[TypeID::C5C] = +ac52;
        pc52CombOfBDS.body[TypeID::C2C] = -bc52;

        lc52CombOfBDS.header = TypeID::LC52C;
        lc52CombOfBDS.body[TypeID::L5C] = +ac52;
        lc52CombOfBDS.body[TypeID::L2C] = -bc52;
        //16
        pc16CombOfBDS.header = TypeID::PC16C;
        pc16CombOfBDS.body[TypeID::C1C] = +ac16;
        pc16CombOfBDS.body[TypeID::C6C] = -bc16;

		lc16CombOfBDS.header = TypeID::LC16C;
		lc16CombOfBDS.body[TypeID::L1C] = +ac16;
		lc16CombOfBDS.body[TypeID::L6C] = -bc16;
        // 18
        pc18CombOfBDS.header = TypeID::PC18C;
        pc18CombOfBDS.body[TypeID::C1C] = +ac18;
        pc18CombOfBDS.body[TypeID::C8C] = -bc18;

		lc18CombOfBDS.header = TypeID::LC18C;
		lc18CombOfBDS.body[TypeID::L1C] = +ac18;
		lc18CombOfBDS.body[TypeID::L8C] = -bc18;
        // 152
        pc152CombOfBDS.header = TypeID::PC152C;
        pc152CombOfBDS.body[TypeID::C1C] = e31c;
        pc152CombOfBDS.body[TypeID::C5C] = e35c;
        pc152CombOfBDS.body[TypeID::C2C] = e32c;
        lc152CombOfBDS.header = TypeID::LC152C;
        lc152CombOfBDS.body[TypeID::L1C] = e31c;
        lc152CombOfBDS.body[TypeID::L5C] = e35c;
        lc152CombOfBDS.body[TypeID::L2C] = e32c;
        // 1526
        pc1526CombOfBDS.header = TypeID::PC1526C;
        pc1526CombOfBDS.body[TypeID::C1C] = e41c;
        pc1526CombOfBDS.body[TypeID::C5C] = e45c;
        pc1526CombOfBDS.body[TypeID::C2C] = e42c;
        pc1526CombOfBDS.body[TypeID::C6C] = e46c;
        lc1526CombOfBDS.header = TypeID::LC1526C;
        lc1526CombOfBDS.body[TypeID::L1C] = e41c;
        lc1526CombOfBDS.body[TypeID::L5C] = e45c;
        lc1526CombOfBDS.body[TypeID::L2C] = e42c;
        lc1526CombOfBDS.body[TypeID::L6C] = e46c;
        // 15268
        pc15268CombOfBDS.header = TypeID::PC15268C;
        pc15268CombOfBDS.body[TypeID::C1C] = e1c;
        pc15268CombOfBDS.body[TypeID::C5C] = e2c;
        pc15268CombOfBDS.body[TypeID::C2C] = e3c;
        pc15268CombOfBDS.body[TypeID::C6C] = e4c;
        pc15268CombOfBDS.body[TypeID::C8C] = e5c;

		lc15268CombOfBDS.header = TypeID::LC15268C;
		lc15268CombOfBDS.body[TypeID::L1C] = e1c;
		lc15268CombOfBDS.body[TypeID::L5C] = e2c;
		lc15268CombOfBDS.body[TypeID::L2C] = e3c;
		lc15268CombOfBDS.body[TypeID::L6C] = e4c;
		lc15268CombOfBDS.body[TypeID::L8C] = e5c;

        // if158
        pc158CombOfBDS.header = TypeID::PC158C;
        pc158CombOfBDS.body[TypeID::C1C] = e1581c;
        pc158CombOfBDS.body[TypeID::C5C] = e1585c;
        pc158CombOfBDS.body[TypeID::C8C] = e1588c;

		lc158CombOfBDS.header = TypeID::LC158C;
		lc158CombOfBDS.body[TypeID::L1C] = e1581c;
		lc158CombOfBDS.body[TypeID::L5C] = e1585c;
		lc158CombOfBDS.body[TypeID::L8C] = e1588c;

        //*---------------------------MW----------------------*//
        double cg12( L1_FREQ_GPS/(L1_FREQ_GPS - L2_FREQ_GPS) );
        double dg12( L2_FREQ_GPS/(L1_FREQ_GPS - L2_FREQ_GPS) );
        double eg12( L1_FREQ_GPS/(L1_FREQ_GPS + L2_FREQ_GPS) );
        double fg12( L2_FREQ_GPS/(L1_FREQ_GPS + L2_FREQ_GPS) );

        double cg15(L1_FREQ_GPS / (L1_FREQ_GPS - L5_FREQ_GPS));
        double dg15(L5_FREQ_GPS / (L1_FREQ_GPS - L5_FREQ_GPS));
        double eg15(L1_FREQ_GPS / (L1_FREQ_GPS + L5_FREQ_GPS));
        double fg15(L5_FREQ_GPS / (L1_FREQ_GPS + L5_FREQ_GPS));

        // Coefficients of GPS PW/WL/PN/LN combinations, L1 and L2
        double cg21( L2_FREQ_GPS/(L2_FREQ_GPS - L1_FREQ_GPS) );
        double dg21( L1_FREQ_GPS/(L2_FREQ_GPS - L1_FREQ_GPS) );
        double eg21( L2_FREQ_GPS/(L2_FREQ_GPS + L1_FREQ_GPS) );
        double fg21( L1_FREQ_GPS/(L2_FREQ_GPS + L1_FREQ_GPS) );

        // Coefficients of GPS PW/WL/PN/LN combinations, L2 and L5
        double cg25( L2_FREQ_GPS/(L2_FREQ_GPS - L5_FREQ_GPS) );
        double dg25( L5_FREQ_GPS/(L2_FREQ_GPS - L5_FREQ_GPS) );
        double eg25( L2_FREQ_GPS/(L2_FREQ_GPS + L5_FREQ_GPS) );
        double fg25( L5_FREQ_GPS/(L2_FREQ_GPS + L5_FREQ_GPS) );

        // Coefficients of Galileo MW combinations, L1 and L5
        double ce51( L5_FREQ_GAL/(L5_FREQ_GAL - L1_FREQ_GAL) );
        double de51( L1_FREQ_GAL/(L5_FREQ_GAL - L1_FREQ_GAL) );
        double ee51( L5_FREQ_GAL/(L5_FREQ_GAL + L1_FREQ_GAL) );
        double fe51( L1_FREQ_GAL/(L5_FREQ_GAL + L1_FREQ_GAL) );
        // MW combinations, L1 and L5
        double ce15(L1_FREQ_GAL/(L1_FREQ_GAL-L5_FREQ_GAL));
        double de15(L5_FREQ_GAL/(L1_FREQ_GAL-L5_FREQ_GAL));
        double ee15(L1_FREQ_GAL/(L1_FREQ_GAL+L5_FREQ_GAL));
        double fe15(L5_FREQ_GAL/(L1_FREQ_GAL+L5_FREQ_GAL));

        // Coefficients of Galileo PW/WL/PN/LN combinations, L5 and L7
        double ce57( L5_FREQ_GAL/(L5_FREQ_GAL - L7_FREQ_GAL) );
        double de57( L7_FREQ_GAL/(L5_FREQ_GAL - L7_FREQ_GAL) );
        double ee57( L5_FREQ_GAL/(L5_FREQ_GAL + L7_FREQ_GAL) );
        double fe57( L7_FREQ_GAL/(L5_FREQ_GAL + L7_FREQ_GAL) );

        // MW combinations, L1 and L7
        double ce17(L1_FREQ_GAL / (L1_FREQ_GAL - L7_FREQ_GAL));
        double de17(L7_FREQ_GAL / (L1_FREQ_GAL - L7_FREQ_GAL));
        double ee17(L1_FREQ_GAL / (L1_FREQ_GAL + L7_FREQ_GAL));
        double fe17(L7_FREQ_GAL / (L1_FREQ_GAL + L7_FREQ_GAL));

        // Coefficients of Galileo PW/WL/PN/LN combinations, L5 and L8
        double ce58( L5_FREQ_GAL/(L5_FREQ_GAL - L8_FREQ_GAL) );
        double de58( L8_FREQ_GAL/(L5_FREQ_GAL - L8_FREQ_GAL) );
        double ee58( L5_FREQ_GAL/(L5_FREQ_GAL + L8_FREQ_GAL) );
        double fe58( L8_FREQ_GAL/(L5_FREQ_GAL + L8_FREQ_GAL) );

        // MW combinations, L1 and L8
        double ce18(L1_FREQ_GAL / (L1_FREQ_GAL - L8_FREQ_GAL));
        double de18(L8_FREQ_GAL / (L1_FREQ_GAL - L8_FREQ_GAL));
        double ee18(L1_FREQ_GAL / (L1_FREQ_GAL + L8_FREQ_GAL));
        double fe18(L8_FREQ_GAL / (L1_FREQ_GAL + L8_FREQ_GAL));

         // Coefficients of Galileo PW/WL/PN/LN combinations, L5 and L6
        double ce56( L5_FREQ_GAL/(L5_FREQ_GAL - L6_FREQ_GAL) );
        double de56( L6_FREQ_GAL/(L5_FREQ_GAL - L6_FREQ_GAL) );
        double ee56( L5_FREQ_GAL/(L5_FREQ_GAL + L6_FREQ_GAL) );
        double fe56( L6_FREQ_GAL/(L5_FREQ_GAL + L6_FREQ_GAL) );

        // MW combinations, L1 and L6
        double ce16(L1_FREQ_GAL / (L1_FREQ_GAL - L6_FREQ_GAL));
        double de16(L6_FREQ_GAL / (L1_FREQ_GAL - L6_FREQ_GAL));
        double ee16(L1_FREQ_GAL / (L1_FREQ_GAL + L6_FREQ_GAL));
        double fe16(L6_FREQ_GAL / (L1_FREQ_GAL + L6_FREQ_GAL));

         // Coefficients of BDS PW/WL/PN/LN combinations, L2 and L7
        double cc27( L2_FREQ_BDS/(L2_FREQ_BDS - L7_FREQ_BDS) );
        double dc27( L7_FREQ_BDS/(L2_FREQ_BDS - L7_FREQ_BDS) );
        double ec27( L2_FREQ_BDS/(L2_FREQ_BDS + L7_FREQ_BDS) );
        double fc27( L7_FREQ_BDS/(L2_FREQ_BDS + L7_FREQ_BDS) );

        double cc26( L2_FREQ_BDS/(L2_FREQ_BDS - L6_FREQ_BDS) );
        double dc26( L6_FREQ_BDS/(L2_FREQ_BDS - L6_FREQ_BDS) );
        double ec26( L2_FREQ_BDS/(L2_FREQ_BDS + L6_FREQ_BDS) );
        double fc26( L6_FREQ_BDS/(L2_FREQ_BDS + L6_FREQ_BDS) );

        double cc28( L2_FREQ_BDS/(L2_FREQ_BDS - L8_FREQ_BDS) );
        double dc28( L8_FREQ_BDS/(L2_FREQ_BDS - L8_FREQ_BDS) );
        double ec28( L2_FREQ_BDS/(L2_FREQ_BDS + L8_FREQ_BDS) );
        double fc28( L8_FREQ_BDS/(L2_FREQ_BDS + L8_FREQ_BDS) );

        double cc25( L2_FREQ_BDS/(L2_FREQ_BDS - L5_FREQ_BDS) );
        double dc25( L5_FREQ_BDS/(L2_FREQ_BDS - L5_FREQ_BDS) );
        double ec25( L2_FREQ_BDS/(L2_FREQ_BDS + L5_FREQ_BDS) );
        double fc25( L5_FREQ_BDS/(L2_FREQ_BDS + L5_FREQ_BDS) );

        double cc21( L2_FREQ_BDS/(L2_FREQ_BDS - L1_FREQ_BDS) );
        double dc21( L1_FREQ_BDS/(L2_FREQ_BDS - L1_FREQ_BDS) );
        double ec21( L2_FREQ_BDS/(L2_FREQ_BDS + L1_FREQ_BDS) );
        double fc21( L1_FREQ_BDS/(L2_FREQ_BDS + L1_FREQ_BDS) );

        double cc61(L6_FREQ_BDS / (L6_FREQ_BDS - L1_FREQ_BDS));
        double dc61(L1_FREQ_BDS / (L6_FREQ_BDS - L1_FREQ_BDS));
        double ec61(L6_FREQ_BDS / (L6_FREQ_BDS + L1_FREQ_BDS));
        double fc61(L1_FREQ_BDS / (L6_FREQ_BDS + L1_FREQ_BDS));
         
        // added by ragnwang bdsfopt
        // Coefficients of BDS PW/WL/PN/LN combination L5 and L1
        double cc51( L5_FREQ_BDS/(L5_FREQ_BDS-L1_FREQ_BDS) );
        double dc51( L1_FREQ_BDS/(L5_FREQ_BDS-L1_FREQ_BDS) );
        double ec51( L5_FREQ_BDS/(L5_FREQ_BDS+L1_FREQ_BDS) );
        double fc51( L1_FREQ_BDS/(L5_FREQ_BDS+L1_FREQ_BDS) );

        // MW combinations, L1 and L5
        double cc15(L1_FREQ_BDS / (L1_FREQ_BDS - L5_FREQ_BDS));
        double dc15(L5_FREQ_BDS / (L1_FREQ_BDS - L5_FREQ_BDS));
        double ec15(L1_FREQ_BDS / (L1_FREQ_BDS + L5_FREQ_BDS));
        double fc15(L5_FREQ_BDS / (L1_FREQ_BDS + L5_FREQ_BDS));

        // Coefficients of BDS PW/WL/PN/LN combination L5 and L2
        double cc52(L5_FREQ_BDS / (L5_FREQ_BDS - L2_FREQ_BDS));
        double dc52(L2_FREQ_BDS / (L5_FREQ_BDS - L2_FREQ_BDS));
        double ec52(L5_FREQ_BDS / (L5_FREQ_BDS + L2_FREQ_BDS));
        double fc52(L2_FREQ_BDS / (L5_FREQ_BDS + L2_FREQ_BDS));

        // MW combinations, L1 and L2
        double cc12(L1_FREQ_BDS / (L1_FREQ_BDS - L2_FREQ_BDS));
        double dc12(L2_FREQ_BDS / (L1_FREQ_BDS - L2_FREQ_BDS));
        double ec12(L1_FREQ_BDS / (L1_FREQ_BDS + L2_FREQ_BDS));
        double fc12(L2_FREQ_BDS / (L1_FREQ_BDS + L2_FREQ_BDS));

        // Coefficients of BDS PW/WL/PN/LN combination L5 and L6
        double cc56(L5_FREQ_BDS / (L5_FREQ_BDS - L6_FREQ_BDS));
        double dc56(L6_FREQ_BDS / (L5_FREQ_BDS - L6_FREQ_BDS));
        double ec56(L5_FREQ_BDS / (L5_FREQ_BDS + L6_FREQ_BDS));
        double fc56(L6_FREQ_BDS / (L5_FREQ_BDS + L6_FREQ_BDS));

        // MW combinations, L1 and L6
        double cc16(L1_FREQ_BDS / (L1_FREQ_BDS - L6_FREQ_BDS));
        double dc16(L6_FREQ_BDS / (L1_FREQ_BDS - L6_FREQ_BDS));
        double ec16(L1_FREQ_BDS / (L1_FREQ_BDS + L6_FREQ_BDS));
        double fc16(L6_FREQ_BDS / (L1_FREQ_BDS + L6_FREQ_BDS));
        // Coefficients of BDS PW/WL/PN/LN combination L5 and L8
        double cc58(L5_FREQ_BDS / (L5_FREQ_BDS - L8_FREQ_BDS));
        double dc58(L8_FREQ_BDS / (L5_FREQ_BDS - L8_FREQ_BDS));
        double ec58(L5_FREQ_BDS / (L5_FREQ_BDS + L8_FREQ_BDS));
        double fc58(L8_FREQ_BDS / (L5_FREQ_BDS + L8_FREQ_BDS));

        // MW combinations, L1 and L8
        double cc18(L1_FREQ_BDS / (L1_FREQ_BDS - L8_FREQ_BDS));
        double dc18(L8_FREQ_BDS / (L1_FREQ_BDS - L8_FREQ_BDS));
        double ec18(L1_FREQ_BDS / (L1_FREQ_BDS + L8_FREQ_BDS));
        double fc18(L8_FREQ_BDS / (L1_FREQ_BDS + L8_FREQ_BDS));

        // MW combinations, L1 and L7
        double cc17(L1_FREQ_BDS / (L1_FREQ_BDS - L7_FREQ_BDS));
        double dc17(L7_FREQ_BDS / (L1_FREQ_BDS - L7_FREQ_BDS));
        double ec17(L1_FREQ_BDS / (L1_FREQ_BDS + L7_FREQ_BDS));
        double fc17(L7_FREQ_BDS / (L1_FREQ_BDS + L7_FREQ_BDS));

        // Coefficients of QZSS PW/WL/PN/LN combinations, L1 and L2
        double cj21( L2_FREQ_QZS/(L2_FREQ_QZS - L1_FREQ_QZS) );
        double dj21( L1_FREQ_QZS/(L2_FREQ_QZS - L1_FREQ_QZS) );
        double ej21( L2_FREQ_QZS/(L2_FREQ_QZS + L1_FREQ_QZS) );
        double fj21( L1_FREQ_QZS/(L2_FREQ_QZS + L1_FREQ_QZS) );

        // Coefficients of QZSS PW/WL/PN/LN combinations, L2 and L5
        double cj25( L2_FREQ_QZS/(L2_FREQ_QZS - L5_FREQ_QZS) );
        double dj25( L5_FREQ_QZS/(L2_FREQ_QZS - L5_FREQ_QZS) );
        double ej25( L2_FREQ_QZS/(L2_FREQ_QZS + L5_FREQ_QZS) );
        double fj25( L5_FREQ_QZS/(L2_FREQ_QZS + L5_FREQ_QZS) );

        // coefficients for BL1/BL2 combination using P1/P2/L1/L2
        // lambdaN1*N1 = L1 - [ (alfa+1)*P1 - 2*P2 ]/(alfa-1)
        // lambdaN2*N2 = L2 - [ (2*alfa)*P1 - (alfa+1)*P2]/(alfa-1)
        double agBL1 = 1.0;  
        double bgBL1 = (GAMMA_GPS_L1L2 + 1.0)/(GAMMA_GPS_L1L2 - 1.0);  
        double cgBL1 = -2.0/(GAMMA_GPS_L1L2 - 1.0);  

        double agBL2 = 1.0;  
        double bgBL2 = 2.0*GAMMA_GPS_L1L2/(GAMMA_GPS_L1L2 - 1.0);  
        double cgBL2 = -(GAMMA_GPS_L1L2 + 1.0)/(GAMMA_GPS_L1L2 - 1.0);  


        mw12CombOfGPS.header                =   TypeID::MW12G;
        mw12CombOfGPS.body[TypeID::L1G]      =   +cg12;
        mw12CombOfGPS.body[TypeID::L2G]      =   -dg12;
        mw12CombOfGPS.body[TypeID::C1G]      =   -eg12;
        mw12CombOfGPS.body[TypeID::C2G]      =   -fg12;

        mw15CombOfGPS.header = TypeID::MW15G;
        mw15CombOfGPS.body[TypeID::L1G] = +cg15;
        mw15CombOfGPS.body[TypeID::L5G] = -dg15;
        mw15CombOfGPS.body[TypeID::C1G] = -eg15;
        mw15CombOfGPS.body[TypeID::C5G] = -fg15;

        // Definition to compute the MW combination of GPS, L1 and L2
        mw21CombOfGPS.header                =   TypeID::MW21G;
        mw21CombOfGPS.body[TypeID::L2G]      =   +cg21;
        mw21CombOfGPS.body[TypeID::L1G]      =   -dg21;
        mw21CombOfGPS.body[TypeID::C2G]      =   -eg21;
        mw21CombOfGPS.body[TypeID::C1G]      =   -fg21;

        // Definition to compute the MW combination of GPS, L2 and L5
        mw25CombOfGPS.header                =   TypeID::MW25G;
        mw25CombOfGPS.body[TypeID::L2G]      =   +cg25;
        mw25CombOfGPS.body[TypeID::L5G]      =   -dg25;
        mw25CombOfGPS.body[TypeID::C2G]      =   -eg25;
        mw25CombOfGPS.body[TypeID::C5G]      =   -fg25;

        // Definition to compute the MW combination of Galileo, L1 and L5
        mw58CombOfGAL.header                =   TypeID::MW58E;
        mw58CombOfGAL.body[TypeID::L5E]      =   +ce58;
        mw58CombOfGAL.body[TypeID::L8E]      =   -de58;
        mw58CombOfGAL.body[TypeID::C5E]      =   -ee58;
        mw58CombOfGAL.body[TypeID::C8E]      =   -fe58;

        mw57CombOfGAL.header                =   TypeID::MW57E;
        mw57CombOfGAL.body[TypeID::L5E]      =   +ce57;
        mw57CombOfGAL.body[TypeID::L7E]      =   -de57;
        mw57CombOfGAL.body[TypeID::C5E]      =   -ee57;
        mw57CombOfGAL.body[TypeID::C7E]      =   -fe57;

        mw56CombOfGAL.header                =   TypeID::MW56E;
        mw56CombOfGAL.body[TypeID::L5E]      =   +ce56;
        mw56CombOfGAL.body[TypeID::L6E]      =   -de56;
        mw56CombOfGAL.body[TypeID::C5E]      =   -ee56;
        mw56CombOfGAL.body[TypeID::C6E]      =   -fe56;

        mw51CombOfGAL.header                =   TypeID::MW51E;
        mw51CombOfGAL.body[TypeID::L5E]      =   +ce51;
        mw51CombOfGAL.body[TypeID::L1E]      =   -de51;
        mw51CombOfGAL.body[TypeID::C5E]      =   -ee51;
        mw51CombOfGAL.body[TypeID::C1E]      =   -fe51;

        mw15CombOfGAL.header = TypeID::MW15E;
        mw15CombOfGAL.body[TypeID::L1E] = +ce15;
        mw15CombOfGAL.body[TypeID::L5E] = -de15;
        mw15CombOfGAL.body[TypeID::C1E] = -ee15;
        mw15CombOfGAL.body[TypeID::C5E] = -fe15;

        mw17CombOfGAL.header = TypeID::MW17E;
        mw17CombOfGAL.body[TypeID::L1E] = +ce17;
        mw17CombOfGAL.body[TypeID::L7E] = -de17;
        mw17CombOfGAL.body[TypeID::C1E] = -ee17;
        mw17CombOfGAL.body[TypeID::C7E] = -fe17;

        mw18CombOfGAL.header = TypeID::MW18E;
        mw18CombOfGAL.body[TypeID::L1E] = +ce18;
        mw18CombOfGAL.body[TypeID::L8E] = -de18;
        mw18CombOfGAL.body[TypeID::C1E] = -ee18;
        mw18CombOfGAL.body[TypeID::C8E] = -fe18;

        mw16CombOfGAL.header = TypeID::MW16E;
        mw16CombOfGAL.body[TypeID::L1E] = +ce16;
        mw16CombOfGAL.body[TypeID::L6E] = -de16;
        mw16CombOfGAL.body[TypeID::C1E] = -ee16;
        mw16CombOfGAL.body[TypeID::C6E] = -fe16;

        // Definition to compute the MW combination of BDS, L2 and L7
        mw27CombOfBDS.header                =   TypeID::MW27C;
        mw27CombOfBDS.body[TypeID::L2C]      =   +cc27;
        mw27CombOfBDS.body[TypeID::L7C]      =   -dc27;
        mw27CombOfBDS.body[TypeID::C2C]      =   -ec27;
        mw27CombOfBDS.body[TypeID::C7C]      =   -fc27;

        // Definition to compute the MW combination of BDS, L2 and L6
        mw26CombOfBDS.header                =   TypeID::MW26C;
        mw26CombOfBDS.body[TypeID::L2C]      =   +cc26;
        mw26CombOfBDS.body[TypeID::L6C]      =   -dc26;
        mw26CombOfBDS.body[TypeID::C2C]      =   -ec26;
        mw26CombOfBDS.body[TypeID::C6C]      =   -fc26;

        // Definition to compute the MW combination of BDS, L2 and L5
        mw25CombOfBDS.header                =   TypeID::MW25C;
        mw25CombOfBDS.body[TypeID::L2C]      =   +cc25;
        mw25CombOfBDS.body[TypeID::L5C]      =   -dc25;
        mw25CombOfBDS.body[TypeID::C2C]      =   -ec25;
        mw25CombOfBDS.body[TypeID::C5C]      =   -fc25;

        // Definition to compute the MW combination of BDS, L2 and L1
        mw21CombOfBDS.header               =   TypeID::MW21C;
        mw21CombOfBDS.body[TypeID::L2C]    =   +cc21;
        mw21CombOfBDS.body[TypeID::L1C]    =   -dc21;
        mw21CombOfBDS.body[TypeID::C2C]    =   -ec21;
        mw21CombOfBDS.body[TypeID::C1C]    =   -fc21;

        mw61CombOfBDS.header = TypeID::MW61C;
        mw61CombOfBDS.body[TypeID::L6C] = +cc61;
        mw61CombOfBDS.body[TypeID::L1C] = -dc61;
        mw61CombOfBDS.body[TypeID::C6C] = -ec61;
        mw61CombOfBDS.body[TypeID::C1C] = -fc61;

        mw28CombOfBDS.header = TypeID::MW28C;
        mw28CombOfBDS.body[TypeID::L2C] = +cc28;
        mw28CombOfBDS.body[TypeID::L8C] = -dc28;
        mw28CombOfBDS.body[TypeID::C2C] = -ec28;
        mw28CombOfBDS.body[TypeID::C8C] = -fc28;

        // definition to compute the MW combination of BDS, L5 L1
        mw51CombOfBDS.header = TypeID::MW51C;
        mw51CombOfBDS.body[TypeID::L5C] = +cc51;
        mw51CombOfBDS.body[TypeID::L1C] = -dc51;
        mw51CombOfBDS.body[TypeID::C5C] = -ec51;
        mw51CombOfBDS.body[TypeID::C1C] = -fc51;
        // Definition to compute the MW combination of BDS, L5L2
        mw52CombOfBDS.header = TypeID::MW52C;
        mw52CombOfBDS.body[TypeID::L5C] = +cc52;
        mw52CombOfBDS.body[TypeID::L2C] = -dc52;
        mw52CombOfBDS.body[TypeID::C5C] = -ec52;
        mw52CombOfBDS.body[TypeID::C2C] = -fc52;
        // definition to compute the MW combination of BDS L5L6
        mw56CombOfBDS.header = TypeID::MW56C;
        mw56CombOfBDS.body[TypeID::L5C] = +cc56;
        mw56CombOfBDS.body[TypeID::L6C] = -dc56;
        mw56CombOfBDS.body[TypeID::C5C] = -ec56;
        mw56CombOfBDS.body[TypeID::C6C] = -fc56;
        // definition to compute the MW combination of BDS L5L8
        mw58CombOfBDS.header = TypeID::MW58C;
        mw58CombOfBDS.body[TypeID::L5C] = +cc58;
        mw58CombOfBDS.body[TypeID::L8C] = -dc58;
        mw58CombOfBDS.body[TypeID::C5C] = -ec58;
        mw58CombOfBDS.body[TypeID::C8C] = -fc58;

        mw15CombOfBDS.header = TypeID::MW15C;
        mw15CombOfBDS.body[TypeID::L1C] = +cc15;
        mw15CombOfBDS.body[TypeID::L5C] = -dc15;
        mw15CombOfBDS.body[TypeID::C1C] = -ec15;
        mw15CombOfBDS.body[TypeID::C5C] = -fc15;


        mw12CombOfBDS.header = TypeID::MW12C;
        mw12CombOfBDS.body[TypeID::L1C] = +cc12;
        mw12CombOfBDS.body[TypeID::L2C] = -dc12;
        mw12CombOfBDS.body[TypeID::C1C] = -ec12;
        mw12CombOfBDS.body[TypeID::C2C] = -fc12;

        mw16CombOfBDS.header = TypeID::MW16C;
        mw16CombOfBDS.body[TypeID::L1C] = +cc16;
        mw16CombOfBDS.body[TypeID::L6C] = -dc16;
        mw16CombOfBDS.body[TypeID::C1C] = -ec16;
        mw16CombOfBDS.body[TypeID::C6C] = -fc16;


        mw18CombOfBDS.header = TypeID::MW18C;
        mw18CombOfBDS.body[TypeID::L1C] = +cc18;
        mw18CombOfBDS.body[TypeID::L8C] = -dc18;
        mw18CombOfBDS.body[TypeID::C1C] = -ec18;
        mw18CombOfBDS.body[TypeID::C8C] = -fc18;

        mw17CombOfBDS.header = TypeID::MW17C;
        mw17CombOfBDS.body[TypeID::L1C] = +cc17;
        mw17CombOfBDS.body[TypeID::L7C] = -dc17;
        mw17CombOfBDS.body[TypeID::C1C] = -ec17;
        mw17CombOfBDS.body[TypeID::C7C] = -fc17;


        // Definition to compute the MW combination of QZS, L1 and L5
        mw21CombOfQZS.header               =   TypeID::MW21J;
        mw21CombOfQZS.body[TypeID::L2J]    =   +cj21;
        mw21CombOfQZS.body[TypeID::L1J]    =   -dj21;
        mw21CombOfQZS.body[TypeID::C2J]    =   -ej21;
        mw21CombOfQZS.body[TypeID::C1J]    =   -fj21;

        // Definition to compute the MW combination of QZS, L2 and L5
        mw25CombOfQZS.header               =   TypeID::MW25J;
        mw25CombOfQZS.body[TypeID::L2J]    =   +cj25;
        mw25CombOfQZS.body[TypeID::L5J]    =   -dj25;
        mw25CombOfQZS.body[TypeID::C2J]    =   -ej25;
        mw25CombOfQZS.body[TypeID::C5J]    =   -fj25;

        gf12CombOfGPS.header = TypeID::GF12G;
        gf12CombOfGPS.body[TypeID::L1G] = 1.0;
        gf12CombOfGPS.body[TypeID::L2G] = -1.0;

        gf15CombOfGPS.header = TypeID::GF15G;
        gf15CombOfGPS.body[TypeID::L1G] = 1.0;
        gf15CombOfGPS.body[TypeID::L5G] = -1.0;

        gf15CombOfGAL.header = TypeID::GF15E;
        gf15CombOfGAL.body[TypeID::L1E] = 1.0;
        gf15CombOfGAL.body[TypeID::L5E] = -1.0;

        gf17CombOfGAL.header = TypeID::GF17E;
        gf17CombOfGAL.body[TypeID::L1E] = 1.0;
        gf17CombOfGAL.body[TypeID::L7E] = -1.0;

        gf18CombOfGAL.header = TypeID::GF18E;
        gf18CombOfGAL.body[TypeID::L1E] = 1.0;
        gf18CombOfGAL.body[TypeID::L8E] = -1.0;

        gf16CombOfGAL.header = TypeID::GF16E;
        gf16CombOfGAL.body[TypeID::L1E] = 1.0;
        gf16CombOfGAL.body[TypeID::L6E] = -1.0;

        // bds
        gf26CombOfBDS.header = TypeID::GF26C;
        gf26CombOfBDS.body[TypeID::L2C] = 1.0;
        gf26CombOfBDS.body[TypeID::L6C] = -1.0;

        gf21CombOfBDS.header = TypeID::GF21C;
        gf21CombOfBDS.body[TypeID::L2C] = 1.0;
        gf21CombOfBDS.body[TypeID::L1C] = -1.0;

        gf61CombOfBDS.header = TypeID::GF61C;
        gf61CombOfBDS.body[TypeID::L6C] = 1.0;
        gf61CombOfBDS.body[TypeID::L1C] = -1.0;

        gf25CombOfBDS.header = TypeID::GF25C;
        gf25CombOfBDS.body[TypeID::L2C] = 1.0;
        gf25CombOfBDS.body[TypeID::L5C] = -1.0;

        gf28CombOfBDS.header = TypeID::GF28C;
        gf28CombOfBDS.body[TypeID::L2C] = 1.0;
        gf28CombOfBDS.body[TypeID::L8C] = -1.0;

        gf27CombOfBDS.header = TypeID::GF27C;
        gf27CombOfBDS.body[TypeID::L2C] = 1.0;
        gf27CombOfBDS.body[TypeID::L7C] = -1.0;
        /// bds3 new combination
        gf15CombOfBDS.header = TypeID::GF15C;
        gf15CombOfBDS.body[TypeID::L1C] = 1.0;
        gf15CombOfBDS.body[TypeID::L5C] = -1.0;

        gf12CombOfBDS.header = TypeID::GF12C;
        gf12CombOfBDS.body[TypeID::L1C] = 1.0;
        gf12CombOfBDS.body[TypeID::L2C] = -1.0;

        gf52CombOfBDS.header = TypeID::GF52C;
        gf52CombOfBDS.body[TypeID::L5C] = 1.0;
        gf52CombOfBDS.body[TypeID::L2C] = -1.0;

        gf16CombOfBDS.header = TypeID::GF16C;
        gf16CombOfBDS.body[TypeID::L1C] = 1.0;
        gf16CombOfBDS.body[TypeID::L6C] = -1.0;

        gf18CombOfBDS.header = TypeID::GF18C;
        gf18CombOfBDS.body[TypeID::L1C] = 1.0;
        gf18CombOfBDS.body[TypeID::L8C] = -1.0;

        gf17CombOfBDS.header = TypeID::GF17C;
        gf17CombOfBDS.body[TypeID::L1C] = 1.0;
        gf17CombOfBDS.body[TypeID::L7C] = -1.0;


        BL1CombOfGPS.header                = TypeID::BL1G;
        BL1CombOfGPS.body[TypeID::L1G]     =   agBL1;
        BL1CombOfGPS.body[TypeID::C1G]     =  -bgBL1;
        BL1CombOfGPS.body[TypeID::C2G]     =  -cgBL1;

        BL2CombOfGPS.header                =   TypeID::BL2G;
        BL2CombOfGPS.body[TypeID::L2G]     =   agBL2;
        BL2CombOfGPS.body[TypeID::C1G]     =  -bgBL2;
        BL2CombOfGPS.body[TypeID::C2G]     =  -cgBL2;

        // GPS

        // Definition to compute prefit residual of GPS C1
        c1PrefitOfGPS.header                     =   TypeID::prefitC1G;
        c1PrefitOfGPS.body[TypeID::C1G]          =   +1.0;
        c1PrefitOfGPS.body[TypeID::rho]          =   -1.0;
        c1PrefitOfGPS.body[TypeID::cdtSat]       =   +1.0;
        c1PrefitOfGPS.body[TypeID::relativity]   =   -1.0;
        c1PrefitOfGPS.body[TypeID::satPCenterC1G]=   -1.0;
        c1PrefitOfGPS.body[TypeID::gravDelay]    =   -1.0;
        c1PrefitOfGPS.body[TypeID::tropoSlant]   =   -1.0;
        c1PrefitOfGPS.body[TypeID::rcvCorrC1G]   =   -1.0;
        // ionoMap, GPS SPP
        c1PrefitOfGPS.body[TypeID::ionoMapC1G] = +1.0;
        c1PrefitOfGPS.addOptionalType(TypeID::ionoMapC1G);

         // Definition to compute prefit residual of GPS C2
        c2PrefitOfGPS.header                     =   TypeID::prefitC2G;
        c2PrefitOfGPS.body[TypeID::C2G]          =   +1.0;
        c2PrefitOfGPS.body[TypeID::rho]          =   -1.0;
        c2PrefitOfGPS.body[TypeID::cdtSat]       =   +1.0;
        c2PrefitOfGPS.body[TypeID::satPCenterC2G]=   -1.0;
        c2PrefitOfGPS.body[TypeID::relativity]   =   -1.0;
        c2PrefitOfGPS.body[TypeID::gravDelay]    =   -1.0;
        c2PrefitOfGPS.body[TypeID::tropoSlant]   =   -1.0;
        c2PrefitOfGPS.body[TypeID::rcvCorrC2G]   =   -1.0;
        // ionoMap, GPS SPP
        c2PrefitOfGPS.body[TypeID::ionoMapC2G] = +1.0;
        c2PrefitOfGPS.addOptionalType(TypeID::ionoMapC2G);

        // Definition to compute prefit residual of GPS C5
        c5PrefitOfGPS.header                     =   TypeID::prefitC5G;
        c5PrefitOfGPS.body[TypeID::C5G]          =   +1.0;
        c5PrefitOfGPS.body[TypeID::rho]          =   -1.0;
        c5PrefitOfGPS.body[TypeID::cdtSat]       =   +1.0;
        c5PrefitOfGPS.body[TypeID::satPCenterC5G]=   -1.0;
        c5PrefitOfGPS.body[TypeID::relativity]   =   -1.0;
        c5PrefitOfGPS.body[TypeID::gravDelay]    =   -1.0;
        c5PrefitOfGPS.body[TypeID::tropoSlant]   =   -1.0;
        c5PrefitOfGPS.body[TypeID::rcvCorrC5G]   =   -1.0;
        // ionoMap, GPS SPP
        c5PrefitOfGPS.body[TypeID::ionoMapC5G] = +1.0;
        c5PrefitOfGPS.addOptionalType(TypeID::ionoMapC5G);
        // GAL

        // Definition to compute prefit residual of Galileo C1E
        c1PrefitOfGAL.header                     =   TypeID::prefitC1E;
        c1PrefitOfGAL.body[TypeID::C1E]          =   +1.0;
        c1PrefitOfGAL.body[TypeID::rho]          =   -1.0;
        c1PrefitOfGAL.body[TypeID::cdtSat]       =   +1.0;
        c1PrefitOfGAL.body[TypeID::satPCenterC1E]=   -1.0;
        c1PrefitOfGAL.body[TypeID::relativity]   =   -1.0;
        c1PrefitOfGAL.body[TypeID::gravDelay]    =   -1.0;
        c1PrefitOfGAL.body[TypeID::tropoSlant]   =   -1.0;
        c1PrefitOfGAL.body[TypeID::rcvCorrC1E]     =   -1.0;
        // ionoMap, GAL SPP
        c1PrefitOfGAL.body[TypeID::ionoMapC1E] = +1.0;
        c1PrefitOfGAL.addOptionalType(TypeID::ionoMapC1E);

        // Definition to compute prefit residual of Galileo C5E
        c5PrefitOfGAL.header                    =   TypeID::prefitC5E;
        c5PrefitOfGAL.body[TypeID::C5E]         =   +1.0;
        c5PrefitOfGAL.body[TypeID::rho]         =   -1.0;
        c5PrefitOfGAL.body[TypeID::cdtSat]      =   +1.0;
        c5PrefitOfGAL.body[TypeID::satPCenterC5E]=   -1.0;
        c5PrefitOfGAL.body[TypeID::relativity]  =   -1.0;
        c5PrefitOfGAL.body[TypeID::gravDelay]   =   -1.0;
        c5PrefitOfGAL.body[TypeID::tropoSlant]  =   -1.0;
        c5PrefitOfGAL.body[TypeID::rcvCorrC5E]  =   -1.0;
        // ionoMap, GAL SPP
        c5PrefitOfGAL.body[TypeID::ionoMapC5E] = +1.0;
        c5PrefitOfGAL.addOptionalType(TypeID::ionoMapC5E);
        // Definition to compute prefit residual of Galileo C6E
        c6PrefitOfGAL.header                    =   TypeID::prefitC6E;
        c6PrefitOfGAL.body[TypeID::C6E]         =   +1.0;
        c6PrefitOfGAL.body[TypeID::rho]         =   -1.0;
        c6PrefitOfGAL.body[TypeID::cdtSat]      =   +1.0;
        c6PrefitOfGAL.body[TypeID::satPCenterC6E]=   -1.0;
        c6PrefitOfGAL.body[TypeID::relativity]  =   -1.0;
        c6PrefitOfGAL.body[TypeID::gravDelay]   =   -1.0;
        c6PrefitOfGAL.body[TypeID::tropoSlant]  =   -1.0;
        c6PrefitOfGAL.body[TypeID::rcvCorrC6E]  =   -1.0;
        // ionoMap, GAL SPP
        c6PrefitOfGAL.body[TypeID::ionoMapC6E] = +1.0;
        c6PrefitOfGAL.addOptionalType(TypeID::ionoMapC6E);

        // Definition to compute prefit residual of Galileo C7E
        c7PrefitOfGAL.header                    =   TypeID::prefitC7E;
        c7PrefitOfGAL.body[TypeID::C7E]         =   +1.0;
        c7PrefitOfGAL.body[TypeID::rho]         =   -1.0;
        c7PrefitOfGAL.body[TypeID::cdtSat]      =   +1.0;
        c7PrefitOfGAL.body[TypeID::satPCenterC7E]  =   -1.0;
        c7PrefitOfGAL.body[TypeID::relativity]  =   -1.0;
        c7PrefitOfGAL.body[TypeID::gravDelay]   =   -1.0;
        c7PrefitOfGAL.body[TypeID::tropoSlant]  =   -1.0;
        c7PrefitOfGAL.body[TypeID::rcvCorrC7E]  =   -1.0;
        // ionoMap, GAL SPP
        c7PrefitOfGAL.body[TypeID::ionoMapC7E] = +1.0;
        c7PrefitOfGAL.addOptionalType(TypeID::ionoMapC7E);
        // Definition to compute prefit residual of Galileo C6
        c8PrefitOfGAL.header                    =   TypeID::prefitC8E;
        c8PrefitOfGAL.body[TypeID::C8E]         =   +1.0;
        c8PrefitOfGAL.body[TypeID::rho]         =   -1.0;
        c8PrefitOfGAL.body[TypeID::cdtSat]      =   +1.0;
        c8PrefitOfGAL.body[TypeID::satPCenterC8E]  =   -1.0;
        c8PrefitOfGAL.body[TypeID::relativity]  =   -1.0;
        c8PrefitOfGAL.body[TypeID::gravDelay]   =   -1.0;
        c8PrefitOfGAL.body[TypeID::tropoSlant]  =   -1.0;
        c8PrefitOfGAL.body[TypeID::rcvCorrC8E]  =   -1.0;
        // ionoMap, GAL SPP
        c8PrefitOfGAL.body[TypeID::ionoMapC8E] = +1.0;
        c8PrefitOfGAL.addOptionalType(TypeID::ionoMapC8E);

        // BDS

        // Definition to compute prefit residual of BDS C1C
        c1PrefitOfBDS.header                    =   TypeID::prefitC1C;
        c1PrefitOfBDS.body[TypeID::C1C]         =   +1.0;
        c1PrefitOfBDS.body[TypeID::rho]         =   -1.0;
        c1PrefitOfBDS.body[TypeID::cdtSat]      =   +1.0;
        c1PrefitOfBDS.body[TypeID::satPCenterC1C]  =   -1.0;
        c1PrefitOfBDS.body[TypeID::relativity]  =   -1.0;
        c1PrefitOfBDS.body[TypeID::gravDelay]   =   -1.0;
        c1PrefitOfBDS.body[TypeID::tropoSlant]  =   -1.0;
        c1PrefitOfBDS.body[TypeID::rcvCorrC1C]  =   -1.0;
        // ionoMap, BDS SPP
        c1PrefitOfBDS.body[TypeID::ionoMapC1C] = +1.0;
        c1PrefitOfBDS.addOptionalType(TypeID::ionoMapC1C);
        // Definition to compute prefit residual of BDS C2C
        c2PrefitOfBDS.header                    =   TypeID::prefitC2C;
        c2PrefitOfBDS.body[TypeID::C2C]         =   +1.0;
        c2PrefitOfBDS.body[TypeID::rho]         =   -1.0;
        c2PrefitOfBDS.body[TypeID::cdtSat]      =   +1.0;
        c2PrefitOfBDS.body[TypeID::satPCenterC2C]  =   -1.0;
        c2PrefitOfBDS.body[TypeID::relativity]  =   -1.0;
        c2PrefitOfBDS.body[TypeID::gravDelay]   =   -1.0;
        c2PrefitOfBDS.body[TypeID::tropoSlant]  =   -1.0;
        c2PrefitOfBDS.body[TypeID::rcvCorrC2C]  =   -1.0;
        // ionoMap, BDS SPP
        c2PrefitOfBDS.body[TypeID::ionoMapC2C] = +1.0;
        c2PrefitOfBDS.addOptionalType(TypeID::ionoMapC2C);
        // Definition to compute prefit residual of BDS C5C
        c5PrefitOfBDS.header                    =   TypeID::prefitC5C;
        c5PrefitOfBDS.body[TypeID::C5C]         =   +1.0;
        c5PrefitOfBDS.body[TypeID::rho]         =   -1.0;
        c5PrefitOfBDS.body[TypeID::cdtSat]      =   +1.0;
        c5PrefitOfBDS.body[TypeID::satPCenterC5C]  =   -1.0;
        c5PrefitOfBDS.body[TypeID::relativity]  =   -1.0;
        c5PrefitOfBDS.body[TypeID::gravDelay]   =   -1.0;
        c5PrefitOfBDS.body[TypeID::tropoSlant]  =   -1.0;
        c5PrefitOfBDS.body[TypeID::rcvCorrC5C]  =   -1.0;
        // ionoMap, BDS SPP
        c5PrefitOfBDS.body[TypeID::ionoMapC5C] = +1.0;
        c5PrefitOfBDS.addOptionalType(TypeID::ionoMapC5C);

        // Definition to compute prefit residual of BDS C7C
        c7PrefitOfBDS.header                    =   TypeID::prefitC7C;
        c7PrefitOfBDS.body[TypeID::C7C]          =   +1.0;
        c7PrefitOfBDS.body[TypeID::rho]         =   -1.0;
        c7PrefitOfBDS.body[TypeID::cdtSat]      =   +1.0;
        c7PrefitOfBDS.body[TypeID::satPCenterC7C]  =   -1.0;
        c7PrefitOfBDS.body[TypeID::relativity]  =   -1.0;
        c7PrefitOfBDS.body[TypeID::gravDelay]   =   -1.0;
        c7PrefitOfBDS.body[TypeID::tropoSlant]  =   -1.0;
        c7PrefitOfBDS.body[TypeID::rcvCorrC7C]  =   -1.0;
        // ionoMap, BDS SPP
        c7PrefitOfBDS.body[TypeID::ionoMapC7C] = +1.0;
        c7PrefitOfBDS.addOptionalType(TypeID::ionoMapC7C);
        // Definition to compute prefit residual of BDS C8C
        c8PrefitOfBDS.header                    =   TypeID::prefitC8C;
        c8PrefitOfBDS.body[TypeID::C8C]         =   +1.0;
        c8PrefitOfBDS.body[TypeID::rho]         =   -1.0;
        c8PrefitOfBDS.body[TypeID::cdtSat]      =   +1.0;
        c8PrefitOfBDS.body[TypeID::satPCenterC8C]  =   -1.0;
        c8PrefitOfBDS.body[TypeID::relativity]  =   -1.0;
        c8PrefitOfBDS.body[TypeID::gravDelay]   =   -1.0;
        c8PrefitOfBDS.body[TypeID::tropoSlant]  =   -1.0;
        c8PrefitOfBDS.body[TypeID::rcvCorrC8C]  =   -1.0;
        // ionoMap, BDS SPP
        c8PrefitOfBDS.body[TypeID::ionoMapC8C] = +1.0;
        c8PrefitOfBDS.addOptionalType(TypeID::ionoMapC8C);
        // Definition to compute prefit residual of BDS C6C
        c6PrefitOfBDS.header                    =   TypeID::prefitC6C;
        c6PrefitOfBDS.body[TypeID::C6C]         =   +1.0;
        c6PrefitOfBDS.body[TypeID::rho]         =   -1.0;
        c6PrefitOfBDS.body[TypeID::cdtSat]      =   +1.0;
        c6PrefitOfBDS.body[TypeID::satPCenterC6C]  =   -1.0;
        c6PrefitOfBDS.body[TypeID::relativity]  =   -1.0;
        c6PrefitOfBDS.body[TypeID::gravDelay]   =   -1.0;
        c6PrefitOfBDS.body[TypeID::tropoSlant]  =   -1.0;
        c6PrefitOfBDS.body[TypeID::rcvCorrC6C]  =   -1.0;
        // ionoMap, BDS SPP
        c6PrefitOfBDS.body[TypeID::ionoMapC6C] = +1.0;
        c6PrefitOfBDS.addOptionalType(TypeID::ionoMapC6C);

        // GLO-code

        // Definition to compute prefit residual of GLO C1R
        c1PrefitOfGLO.header                    =   TypeID::prefitC1R;
        c1PrefitOfGLO.body[TypeID::C1R]         =   +1.0;
        c1PrefitOfGLO.body[TypeID::rho]         =   -1.0;
        c1PrefitOfGLO.body[TypeID::cdtSat]      =   +1.0;
        c1PrefitOfGLO.body[TypeID::satPCenterC1R]  =   -1.0;
        c1PrefitOfGLO.body[TypeID::relativity]  =   -1.0;
        c1PrefitOfGLO.body[TypeID::gravDelay]   =   -1.0;
        c1PrefitOfGLO.body[TypeID::tropoSlant]  =   -1.0;
        c1PrefitOfGLO.body[TypeID::rcvCorrC1R]  =   -1.0;
        // ionoMap, GLO SPP
        c1PrefitOfGLO.body[TypeID::ionoMapC1R] = +1.0;
        c1PrefitOfGLO.addOptionalType(TypeID::ionoMapC1R);
        // Definition to compute prefit residual of GLO C2R
        c2PrefitOfGLO.header                    =   TypeID::prefitC2R;
        c2PrefitOfGLO.body[TypeID::C2R]         =   +1.0;
        c2PrefitOfGLO.body[TypeID::rho]         =   -1.0;
        c2PrefitOfGLO.body[TypeID::cdtSat]      =   +1.0;
        c2PrefitOfGLO.body[TypeID::satPCenterC2R]  =   -1.0;
        c2PrefitOfGLO.body[TypeID::relativity]  =   -1.0;
        c2PrefitOfGLO.body[TypeID::gravDelay]   =   -1.0;
        c2PrefitOfGLO.body[TypeID::tropoSlant]  =   -1.0;
        c2PrefitOfGLO.body[TypeID::rcvCorrC2R]  =   -1.0;
        // ionoMap, GLO SPP
        c2PrefitOfGLO.body[TypeID::ionoMapC2R] = +1.0;
        c2PrefitOfGLO.addOptionalType(TypeID::ionoMapC2R);
        // Definition to compute prefit residual of GLO C3
        c3PrefitOfGLO.header                    =   TypeID::prefitC3R;
        c3PrefitOfGLO.body[TypeID::C3R]         =   +1.0;
        c3PrefitOfGLO.body[TypeID::rho]         =   -1.0;
        c3PrefitOfGLO.body[TypeID::cdtSat]      =   +1.0;
        c3PrefitOfGLO.body[TypeID::satPCenterC3R]  =   -1.0;
        c3PrefitOfGLO.body[TypeID::relativity]  =   -1.0;
        c3PrefitOfGLO.body[TypeID::gravDelay]   =   -1.0;
        c3PrefitOfGLO.body[TypeID::tropoSlant]  =   -1.0;
        c3PrefitOfGLO.body[TypeID::rcvCorrC3R]  =   -1.0;
        // ionoMap, GLO SPP
        c3PrefitOfGLO.body[TypeID::ionoMapC3R] = +1.0;
        c3PrefitOfGLO.addOptionalType(TypeID::ionoMapC3R);
        // Definition to compute prefit residual of GLO C4R
        c4PrefitOfGLO.header                    =   TypeID::prefitC4R;
        c4PrefitOfGLO.body[TypeID::C4R]         =   +1.0;
        c4PrefitOfGLO.body[TypeID::rho]         =   -1.0;
        c4PrefitOfGLO.body[TypeID::cdtSat]      =   +1.0;
        c4PrefitOfGLO.body[TypeID::satPCenterC4R]  =   -1.0;
        c4PrefitOfGLO.body[TypeID::relativity]  =   -1.0;
        c4PrefitOfGLO.body[TypeID::gravDelay]   =   -1.0;
        c4PrefitOfGLO.body[TypeID::tropoSlant]  =   -1.0;
        c4PrefitOfGLO.body[TypeID::rcvCorrC4R]  =   -1.0;
        // ionoMap, GLO SPP
        c4PrefitOfGLO.body[TypeID::ionoMapC4R] = +1.0;
        c4PrefitOfGLO.addOptionalType(TypeID::ionoMapC4R);
        // Definition to compute prefit residual of GLO C6R
        c6PrefitOfGLO.header                    =   TypeID::prefitC6R;
        c6PrefitOfGLO.body[TypeID::C6R]         =   +1.0;
        c6PrefitOfGLO.body[TypeID::rho]         =   -1.0;
        c6PrefitOfGLO.body[TypeID::cdtSat]      =   +1.0;
        c6PrefitOfGLO.body[TypeID::satPCenterC6R]  =   -1.0;
        c6PrefitOfGLO.body[TypeID::relativity]  =   -1.0;
        c6PrefitOfGLO.body[TypeID::gravDelay]   =   -1.0;
        c6PrefitOfGLO.body[TypeID::tropoSlant]  =   -1.0;
        c6PrefitOfGLO.body[TypeID::rcvCorrC6R]  =   -1.0;
        // ionoMap, GLO SPP
        c6PrefitOfGLO.body[TypeID::ionoMapC6R] = +1.0;
        c6PrefitOfGLO.addOptionalType(TypeID::ionoMapC6R);
        // QZS-code

        // Definition to compute prefit residual of QZS C1J
        c1PrefitOfQZS.header                    =   TypeID::prefitC1J;
        c1PrefitOfQZS.body[TypeID::C1J]          =   +1.0;
        c1PrefitOfQZS.body[TypeID::rho]         =   -1.0;
        c1PrefitOfQZS.body[TypeID::cdtSat]      =   +1.0;
        c1PrefitOfQZS.body[TypeID::satPCenterC1J]  =   -1.0;
        c1PrefitOfQZS.body[TypeID::relativity]  =   -1.0;
        c1PrefitOfQZS.body[TypeID::gravDelay]   =   -1.0;
        c1PrefitOfQZS.body[TypeID::tropoSlant]  =   -1.0;
        c1PrefitOfQZS.body[TypeID::rcvCorrC1J]    =   -1.0;
        // ionoMap, QZS SPP
        c1PrefitOfQZS.body[TypeID::ionoMapC1J] = +1.0;
        c1PrefitOfQZS.addOptionalType(TypeID::ionoMapC1J);
        // Definition to compute prefit residual of QZS C2J
        c2PrefitOfQZS.header                    =   TypeID::prefitC2J;
        c2PrefitOfQZS.body[TypeID::C2J]         =   +1.0;
        c2PrefitOfQZS.body[TypeID::rho]         =   -1.0;
        c2PrefitOfQZS.body[TypeID::cdtSat]      =   +1.0;
        c2PrefitOfQZS.body[TypeID::satPCenterC2J]  =   -1.0;
        c2PrefitOfQZS.body[TypeID::relativity]  =   -1.0;
        c2PrefitOfQZS.body[TypeID::gravDelay]   =   -1.0;
        c2PrefitOfQZS.body[TypeID::tropoSlant]  =   -1.0;
        c2PrefitOfQZS.body[TypeID::rcvCorrC2J]  =   -1.0;
        // ionoMap, QZS SPP
        c2PrefitOfQZS.body[TypeID::ionoMapC2J] = +1.0;
        c2PrefitOfQZS.addOptionalType(TypeID::ionoMapC2J);
        // Definition to compute prefit residual of QZS C5J
        c5PrefitOfQZS.header                    =   TypeID::prefitC5J;
        c5PrefitOfQZS.body[TypeID::C5J]         =   +1.0;
        c5PrefitOfQZS.body[TypeID::rho]         =   -1.0;
        c5PrefitOfQZS.body[TypeID::cdtSat]      =   +1.0;
        c5PrefitOfQZS.body[TypeID::satPCenterC5J]  =   -1.0;
        c5PrefitOfQZS.body[TypeID::relativity]  =   -1.0;
        c5PrefitOfQZS.body[TypeID::gravDelay]   =   -1.0;
        c5PrefitOfQZS.body[TypeID::tropoSlant]  =   -1.0;
        c5PrefitOfQZS.body[TypeID::rcvCorrC5J]  =   -1.0;
        // ionoMap, QZS SPP
        c5PrefitOfQZS.body[TypeID::ionoMapC5J] = +1.0;
        c5PrefitOfQZS.addOptionalType(TypeID::ionoMapC5J);
        // Definition to compute prefit residual of QZS C6J
        c6PrefitOfQZS.header                    =   TypeID::prefitC6J;
        c6PrefitOfQZS.body[TypeID::C6J]         =   +1.0;
        c6PrefitOfQZS.body[TypeID::rho]         =   -1.0;
        c6PrefitOfQZS.body[TypeID::cdtSat]      =   +1.0;
        c6PrefitOfQZS.body[TypeID::satPCenterC6J]  =   -1.0;
        c6PrefitOfQZS.body[TypeID::relativity]  =   -1.0;
        c6PrefitOfQZS.body[TypeID::gravDelay]   =   -1.0;
        c6PrefitOfQZS.body[TypeID::tropoSlant]  =   -1.0;
        c6PrefitOfQZS.body[TypeID::rcvCorrC6J]  =   -1.0;
        // ionoMap, QZS SPP
        c6PrefitOfQZS.body[TypeID::ionoMapC6J] = +1.0;
        c6PrefitOfQZS.addOptionalType(TypeID::ionoMapC6J);
        // GPS-phase

        // Definition to compute prefit residual of GPS L1G
        l1PrefitOfGPS.header                    =   TypeID::prefitL1G;
        l1PrefitOfGPS.body[TypeID::L1G]         =   +1.0;
        l1PrefitOfGPS.body[TypeID::rho]         =   -1.0;
        l1PrefitOfGPS.body[TypeID::cdtSat]      =   +1.0;
        l1PrefitOfGPS.body[TypeID::satPCenterL1G]  =   -1.0;
        l1PrefitOfGPS.body[TypeID::relativity]  =   -1.0;
        l1PrefitOfGPS.body[TypeID::gravDelay]   =   -1.0;
        l1PrefitOfGPS.body[TypeID::tropoSlant]  =   -1.0;
        l1PrefitOfGPS.body[TypeID::windUpL1G]   =   -1.0;
        l1PrefitOfGPS.body[TypeID::rcvCorrL1G]  =   -1.0;
        // ionoMap, GPS SPP
        l1PrefitOfGPS.body[TypeID::ionoMapL1G] = +1.0;
        l1PrefitOfGPS.addOptionalType(TypeID::ionoMapL1G);
        // Definition to compute prefit residual of GPS L2G
        l2PrefitOfGPS.header                    =   TypeID::prefitL2G;
        l2PrefitOfGPS.body[TypeID::L2G]         =   +1.0;
        l2PrefitOfGPS.body[TypeID::rho]         =   -1.0;
        l2PrefitOfGPS.body[TypeID::cdtSat]      =   +1.0;
        l2PrefitOfGPS.body[TypeID::satPCenterL2G]  =   -1.0;
        l2PrefitOfGPS.body[TypeID::relativity]  =   -1.0;
        l2PrefitOfGPS.body[TypeID::gravDelay]   =   -1.0;
        l2PrefitOfGPS.body[TypeID::tropoSlant]  =   -1.0;
        l2PrefitOfGPS.body[TypeID::windUpL2G]   =   -1.0;
        l2PrefitOfGPS.body[TypeID::rcvCorrL2G]  =   -1.0;
        // ionoMap, GPS SPP
        l2PrefitOfGPS.body[TypeID::ionoMapL2G] = +1.0;
        l2PrefitOfGPS.addOptionalType(TypeID::ionoMapL2G);
        // Definition to compute prefit residual of GPS L5G
        l5PrefitOfGPS.header                    =   TypeID::prefitL5G;
        l5PrefitOfGPS.body[TypeID::L5G]         =   +1.0;
        l5PrefitOfGPS.body[TypeID::rho]         =   -1.0;
        l5PrefitOfGPS.body[TypeID::cdtSat]      =   +1.0;
        l5PrefitOfGPS.body[TypeID::satPCenterL5G]  =   -1.0;
        l5PrefitOfGPS.body[TypeID::relativity]  =   -1.0;
        l5PrefitOfGPS.body[TypeID::gravDelay]   =   -1.0;
        l5PrefitOfGPS.body[TypeID::tropoSlant]  =   -1.0;
        l5PrefitOfGPS.body[TypeID::windUpL5G]   =   -1.0;
        l5PrefitOfGPS.body[TypeID::rcvCorrL5G]  =   -1.0;
        // ionoMap, GPS SPP
        l5PrefitOfGPS.body[TypeID::ionoMapL5G] = +1.0;
        l5PrefitOfGPS.addOptionalType(TypeID::ionoMapL5G);
        // GAL

        // Definition to compute prefit residual of Galileo L1E
        l1PrefitOfGAL.header                    =   TypeID::prefitL1E;
        l1PrefitOfGAL.body[TypeID::L1E]         =   +1.0;
        l1PrefitOfGAL.body[TypeID::rho]         =   -1.0;
        l1PrefitOfGAL.body[TypeID::cdtSat]      =   +1.0;
        l1PrefitOfGAL.body[TypeID::satPCenterL1E]  =   -1.0;
        l1PrefitOfGAL.body[TypeID::relativity]  =   -1.0;
        l1PrefitOfGAL.body[TypeID::gravDelay]   =   -1.0;
        l1PrefitOfGAL.body[TypeID::tropoSlant]  =   -1.0;
        l1PrefitOfGAL.body[TypeID::windUpL1E]   =   -1.0;
        l1PrefitOfGAL.body[TypeID::rcvCorrL1E]  =   -1.0;
        // ionoMap, GAL SPP
        l1PrefitOfGAL.body[TypeID::ionoMapL1E] = +1.0;
        l1PrefitOfGAL.addOptionalType(TypeID::ionoMapL1E);
        // Definition to compute prefit residual of Galileo L5E
        l5PrefitOfGAL.header                    =   TypeID::prefitL5E;
        l5PrefitOfGAL.body[TypeID::L5E]         =   +1.0;
        l5PrefitOfGAL.body[TypeID::rho]         =   -1.0;
        l5PrefitOfGAL.body[TypeID::cdtSat]      =   +1.0;
        l5PrefitOfGAL.body[TypeID::satPCenterL5E]  =   -1.0;
        l5PrefitOfGAL.body[TypeID::relativity]  =   -1.0;
        l5PrefitOfGAL.body[TypeID::gravDelay]   =   -1.0;
        l5PrefitOfGAL.body[TypeID::tropoSlant]  =   -1.0;
        l5PrefitOfGAL.body[TypeID::windUpL5E]   =   -1.0;
        l5PrefitOfGAL.body[TypeID::rcvCorrL5E]  =   -1.0;
        // ionoMap, GAL SPP
        l5PrefitOfGAL.body[TypeID::ionoMapL5E] = +1.0;
        l5PrefitOfGAL.addOptionalType(TypeID::ionoMapL5E);
        // Definition to compute prefit residual of Galileo L6
        l6PrefitOfGAL.header                    =   TypeID::prefitL6E;
        l6PrefitOfGAL.body[TypeID::L6E]         =   +1.0;
        l6PrefitOfGAL.body[TypeID::rho]         =   -1.0;
        l6PrefitOfGAL.body[TypeID::cdtSat]      =   +1.0;
        l6PrefitOfGAL.body[TypeID::satPCenterL6E]  =   -1.0;
        l6PrefitOfGAL.body[TypeID::relativity]  =   -1.0;
        l6PrefitOfGAL.body[TypeID::gravDelay]   =   -1.0;
        l6PrefitOfGAL.body[TypeID::tropoSlant]  =   -1.0;
        l6PrefitOfGAL.body[TypeID::windUpL6E]   =   -1.0;
        l6PrefitOfGAL.body[TypeID::rcvCorrL6E]  =   -1.0;
        // ionoMap, GAL SPP
        l6PrefitOfGAL.body[TypeID::ionoMapL6E] = +1.0;
        l6PrefitOfGAL.addOptionalType(TypeID::ionoMapL6E);
        // Definition to compute prefit residual of Galileo L7
        l7PrefitOfGAL.header                    =   TypeID::prefitL7E;
        l7PrefitOfGAL.body[TypeID::L7E]         =   +1.0;
        l7PrefitOfGAL.body[TypeID::rho]         =   -1.0;
        l7PrefitOfGAL.body[TypeID::cdtSat]      =   +1.0;
        l7PrefitOfGAL.body[TypeID::satPCenterL7E]  =   -1.0;
        l7PrefitOfGAL.body[TypeID::relativity]  =   -1.0;
        l7PrefitOfGAL.body[TypeID::gravDelay]   =   -1.0;
        l7PrefitOfGAL.body[TypeID::tropoSlant]  =   -1.0;
        l7PrefitOfGAL.body[TypeID::windUpL7E]   =   -1.0;
        l7PrefitOfGAL.body[TypeID::rcvCorrL7E]  =   -1.0;
        // ionoMap, GAL SPP
        l7PrefitOfGAL.body[TypeID::ionoMapL7E] = +1.0;
        l7PrefitOfGAL.addOptionalType(TypeID::ionoMapL7E);
        // Definition to compute prefit residual of Galileo L8
        l8PrefitOfGAL.header                      =   TypeID::prefitL8E;
        l8PrefitOfGAL.body[TypeID::L8E]            =   +1.0;
        l8PrefitOfGAL.body[TypeID::rho]           =   -1.0;
        l8PrefitOfGAL.body[TypeID::cdtSat]        =   +1.0;
        l8PrefitOfGAL.body[TypeID::satPCenterL8E]    =   -1.0;
        l8PrefitOfGAL.body[TypeID::relativity]    =   -1.0;
        l8PrefitOfGAL.body[TypeID::gravDelay]     =   -1.0;
        l8PrefitOfGAL.body[TypeID::tropoSlant]    =   -1.0;
        l8PrefitOfGAL.body[TypeID::windUpL8E]      =   -1.0;
        l8PrefitOfGAL.body[TypeID::rcvCorrL8E]      =   -1.0;
        // ionoMap, GAL SPP
        l8PrefitOfGAL.body[TypeID::ionoMapL8E] = +1.0;
        l8PrefitOfGAL.addOptionalType(TypeID::ionoMapL8E);
        // BDS

        // Definition to compute prefit residual of BDS L1
        l1PrefitOfBDS.header                      =   TypeID::prefitL1C;
        l1PrefitOfBDS.body[TypeID::L1C]            =   +1.0;
        l1PrefitOfBDS.body[TypeID::rho]           =   -1.0;
        l1PrefitOfBDS.body[TypeID::cdtSat]        =   +1.0;
        l1PrefitOfBDS.body[TypeID::satPCenterL1C]    =   -1.0;
        l1PrefitOfBDS.body[TypeID::relativity]    =   -1.0;
        l1PrefitOfBDS.body[TypeID::gravDelay]     =   -1.0;
        l1PrefitOfBDS.body[TypeID::tropoSlant]    =   -1.0;
        l1PrefitOfBDS.body[TypeID::windUpL1C]      =   -1.0;
        l1PrefitOfBDS.body[TypeID::rcvCorrL1C]      =   -1.0;
        // ionoMap, BDS SPP
        l1PrefitOfBDS.body[TypeID::ionoMapL1C] = +1.0;
        l1PrefitOfBDS.addOptionalType(TypeID::ionoMapL1C);
        // Definition to compute prefit residual of BDS L2
        l2PrefitOfBDS.header                      =   TypeID::prefitL2C;
        l2PrefitOfBDS.body[TypeID::L2C]            =   +1.0;
        l2PrefitOfBDS.body[TypeID::rho]           =   -1.0;
        l2PrefitOfBDS.body[TypeID::cdtSat]        =   +1.0;
        l2PrefitOfBDS.body[TypeID::satPCenterL2C]    =   -1.0;
        l2PrefitOfBDS.body[TypeID::relativity]    =   -1.0;
        l2PrefitOfBDS.body[TypeID::gravDelay]     =   -1.0;
        l2PrefitOfBDS.body[TypeID::tropoSlant]    =   -1.0;
        l2PrefitOfBDS.body[TypeID::windUpL2C]      =   -1.0;
        l2PrefitOfBDS.body[TypeID::rcvCorrL2C]      =   -1.0;
        // ionoMap, BDS SPP
        l2PrefitOfBDS.body[TypeID::ionoMapL2C] = +1.0;
        l2PrefitOfBDS.addOptionalType(TypeID::ionoMapL2C);
        // Definition to compute prefit residual of BDS L5
        l5PrefitOfBDS.header                      =   TypeID::prefitL5C;
        l5PrefitOfBDS.body[TypeID::L5C]           =   +1.0;
        l5PrefitOfBDS.body[TypeID::rho]           =   -1.0;
        l5PrefitOfBDS.body[TypeID::cdtSat]        =   +1.0;
        l5PrefitOfBDS.body[TypeID::satPCenterL5C]    =   -1.0;
        l5PrefitOfBDS.body[TypeID::relativity]    =   -1.0;
        l5PrefitOfBDS.body[TypeID::gravDelay]     =   -1.0;
        l5PrefitOfBDS.body[TypeID::tropoSlant]    =   -1.0;
        l5PrefitOfBDS.body[TypeID::windUpL5C]     =   -1.0;
        l5PrefitOfBDS.body[TypeID::rcvCorrL5C]    =   -1.0;
        // ionoMap, BDS SPP
        l5PrefitOfBDS.body[TypeID::ionoMapL5C] = +1.0;
        l5PrefitOfBDS.addOptionalType(TypeID::ionoMapL5C);
        // Definition to compute prefit residual of BDS L7
        l7PrefitOfBDS.header                      =   TypeID::prefitL7C;
        l7PrefitOfBDS.body[TypeID::L7C]           =   +1.0;
        l7PrefitOfBDS.body[TypeID::rho]           =   -1.0;
        l7PrefitOfBDS.body[TypeID::cdtSat]        =   +1.0;
        l7PrefitOfBDS.body[TypeID::satPCenterL7C]    =   -1.0;
        l7PrefitOfBDS.body[TypeID::relativity]    =   -1.0;
        l7PrefitOfBDS.body[TypeID::gravDelay]     =   -1.0;
        l7PrefitOfBDS.body[TypeID::tropoSlant]    =   -1.0;
        l7PrefitOfBDS.body[TypeID::windUpL7C]     =   -1.0;
        l7PrefitOfBDS.body[TypeID::rcvCorrL7C]    =   -1.0;
        // ionoMap, BDS SPP
        l7PrefitOfBDS.body[TypeID::ionoMapL7C] = +1.0;
        l7PrefitOfBDS.addOptionalType(TypeID::ionoMapL7C);
        // Definition to compute prefit residual of BDS L8
        l8PrefitOfBDS.header                      =   TypeID::prefitL8C;
        l8PrefitOfBDS.body[TypeID::L8C]           =   +1.0;
        l8PrefitOfBDS.body[TypeID::rho]           =   -1.0;
        l8PrefitOfBDS.body[TypeID::cdtSat]        =   +1.0;
        l8PrefitOfBDS.body[TypeID::satPCenterL8C]    =   -1.0;
        l8PrefitOfBDS.body[TypeID::relativity]    =   -1.0;
        l8PrefitOfBDS.body[TypeID::gravDelay]     =   -1.0;
        l8PrefitOfBDS.body[TypeID::tropoSlant]    =   -1.0;
        l8PrefitOfBDS.body[TypeID::windUpL8C]     =   -1.0;
        l8PrefitOfBDS.body[TypeID::rcvCorrL8C]    =   -1.0;
        // ionoMap, BDS SPP
        l8PrefitOfBDS.body[TypeID::ionoMapL8C] = +1.0;
        l8PrefitOfBDS.addOptionalType(TypeID::ionoMapL8C);
        // Definition to compute prefit residual of BDS L6
        l6PrefitOfBDS.header                      =   TypeID::prefitL6C;
        l6PrefitOfBDS.body[TypeID::L6C]           =   +1.0;
        l6PrefitOfBDS.body[TypeID::rho]           =   -1.0;
        l6PrefitOfBDS.body[TypeID::cdtSat]        =   +1.0;
        l6PrefitOfBDS.body[TypeID::satPCenterL6C]    =   -1.0;
        l6PrefitOfBDS.body[TypeID::relativity]    =   -1.0;
        l6PrefitOfBDS.body[TypeID::gravDelay]     =   -1.0;
        l6PrefitOfBDS.body[TypeID::tropoSlant]    =   -1.0;
        l6PrefitOfBDS.body[TypeID::windUpL6C]     =   -1.0;
        l6PrefitOfBDS.body[TypeID::rcvCorrL6C]    =   -1.0;
        // ionoMap, BDS SPP
        l6PrefitOfBDS.body[TypeID::ionoMapL6C] = +1.0;
        l6PrefitOfBDS.addOptionalType(TypeID::ionoMapL6C);
        // GLO-phase
        // To be done: wavelength for windup should be modified. 

        // Definition to compute prefit residual of BDS L1
        l1PrefitOfGLO.header                      =   TypeID::prefitL1R;
        l1PrefitOfGLO.body[TypeID::L1R]            =   +1.0;
        l1PrefitOfGLO.body[TypeID::rho]           =   -1.0;
        l1PrefitOfGLO.body[TypeID::cdtSat]        =   +1.0;
        l1PrefitOfGLO.body[TypeID::satPCenterL1R]    =   -1.0;
        l1PrefitOfGLO.body[TypeID::relativity]    =   -1.0;
        l1PrefitOfGLO.body[TypeID::gravDelay]     =   -1.0;
        l1PrefitOfGLO.body[TypeID::tropoSlant]    =   -1.0;
        l1PrefitOfGLO.body[TypeID::windUpL1R]     =    -1.0;
        l1PrefitOfGLO.body[TypeID::rcvCorrL1R]      =   -1.0;
        // ionoMap, GLO SPP
        l1PrefitOfGLO.body[TypeID::ionoMapL1R] = +1.0;
        l1PrefitOfGLO.addOptionalType(TypeID::ionoMapL1R);
        // Definition to compute prefit residual of GLO L2
        l2PrefitOfGLO.header                      =   TypeID::prefitL2R;
        l2PrefitOfGLO.body[TypeID::L2R]            =   +1.0;
        l2PrefitOfGLO.body[TypeID::rho]           =   -1.0;
        l2PrefitOfGLO.body[TypeID::cdtSat]        =   +1.0;
        l2PrefitOfGLO.body[TypeID::satPCenterL2R]    =   -1.0;
        l2PrefitOfGLO.body[TypeID::relativity]    =   -1.0;
        l2PrefitOfGLO.body[TypeID::gravDelay]     =   -1.0;
        l2PrefitOfGLO.body[TypeID::tropoSlant]    =   -1.0;
        l2PrefitOfGLO.body[TypeID::windUpL2R]      =   -1.0;
        l2PrefitOfGLO.body[TypeID::rcvCorrL2R]      =   -1.0;
        // ionoMap, GLO SPP
        l2PrefitOfGLO.body[TypeID::ionoMapL2R] = +1.0;
        l2PrefitOfGLO.addOptionalType(TypeID::ionoMapL2R);
        // Definition to compute prefit residual of GLO L3
        l3PrefitOfGLO.header                      =   TypeID::prefitL3R;
        l3PrefitOfGLO.body[TypeID::L3R]            =   +1.0;
        l3PrefitOfGLO.body[TypeID::rho]           =   -1.0;
        l3PrefitOfGLO.body[TypeID::cdtSat]        =   +1.0;
        l3PrefitOfGLO.body[TypeID::satPCenterL3R]    =   -1.0;
        l3PrefitOfGLO.body[TypeID::relativity]    =   -1.0;
        l3PrefitOfGLO.body[TypeID::gravDelay]     =   -1.0;
        l3PrefitOfGLO.body[TypeID::tropoSlant]    =   -1.0;
        l3PrefitOfGLO.body[TypeID::windUpL3R]     =    -1.0;
        l3PrefitOfGLO.body[TypeID::rcvCorrL3R]      =   -1.0;
        // ionoMap, GLO SPP
        l3PrefitOfGLO.body[TypeID::ionoMapL3R] = +1.0;
        l3PrefitOfGLO.addOptionalType(TypeID::ionoMapL3R);
        // Definition to compute prefit residual of GLO L4
        l4PrefitOfGLO.header                      =   TypeID::prefitL4R;
        l4PrefitOfGLO.body[TypeID::L4R]            =   +1.0;
        l4PrefitOfGLO.body[TypeID::rho]           =   -1.0;
        l4PrefitOfGLO.body[TypeID::cdtSat]        =   +1.0;
        l4PrefitOfGLO.body[TypeID::satPCenterL4R]    =   -1.0;
        l4PrefitOfGLO.body[TypeID::relativity]    =   -1.0;
        l4PrefitOfGLO.body[TypeID::gravDelay]     =   -1.0;
        l4PrefitOfGLO.body[TypeID::tropoSlant]    =   -1.0;
        l4PrefitOfGLO.body[TypeID::windUpL4R]      =   -1.0;
        l4PrefitOfGLO.body[TypeID::rcvCorrL4R]      =   -1.0;
        // ionoMap, GLO SPP
        l4PrefitOfGLO.body[TypeID::ionoMapL4R] = +1.0;
        l4PrefitOfGLO.addOptionalType(TypeID::ionoMapL4R);
        // Definition to compute prefit residual of GLO L6
        l6PrefitOfGLO.header                      =   TypeID::prefitL6R;
        l6PrefitOfGLO.body[TypeID::L6R]            =   +1.0;
        l6PrefitOfGLO.body[TypeID::rho]           =   -1.0;
        l6PrefitOfGLO.body[TypeID::cdtSat]        =   +1.0;
        l6PrefitOfGLO.body[TypeID::satPCenterL6R]    =   -1.0;
        l6PrefitOfGLO.body[TypeID::relativity]    =   -1.0;
        l6PrefitOfGLO.body[TypeID::gravDelay]     =   -1.0;
        l6PrefitOfGLO.body[TypeID::tropoSlant]    =   -1.0;
        l6PrefitOfGLO.body[TypeID::windUpL6R]      =   -1.0;
        l6PrefitOfGLO.body[TypeID::rcvCorrL6R]      =   -1.0;
        // ionoMap, GLO SPP
        l6PrefitOfGLO.body[TypeID::ionoMapL6R] = +1.0;
        l6PrefitOfGLO.addOptionalType(TypeID::ionoMapL6R);
        // QZSS

        // Definition to compute prefit residual of QZS L1
        l1PrefitOfQZS.header                      =   TypeID::prefitL1J;
        l1PrefitOfQZS.body[TypeID::L1J]            =   +1.0;
        l1PrefitOfQZS.body[TypeID::rho]           =   -1.0;
        l1PrefitOfQZS.body[TypeID::cdtSat]        =   +1.0;
        l1PrefitOfQZS.body[TypeID::satPCenterL1J]    =   -1.0;
        l1PrefitOfQZS.body[TypeID::relativity]    =   -1.0;
        l1PrefitOfQZS.body[TypeID::gravDelay]     =   -1.0;
        l1PrefitOfQZS.body[TypeID::tropoSlant]    =   -1.0;
        l1PrefitOfQZS.body[TypeID::windUpL1J]      =   -1.0;
        l1PrefitOfQZS.body[TypeID::rcvCorrL1J]      =   -1.0;
        // ionoMap, QZS SPP
        l1PrefitOfQZS.body[TypeID::ionoMapL1J] = +1.0;
        l1PrefitOfQZS.addOptionalType(TypeID::ionoMapL1J);
        // Definition to compute prefit residual of QZS L2
        l2PrefitOfQZS.header                      =   TypeID::prefitL2J;
        l2PrefitOfQZS.body[TypeID::L2J]            =   +1.0;
        l2PrefitOfQZS.body[TypeID::rho]           =   -1.0;
        l2PrefitOfQZS.body[TypeID::cdtSat]        =   +1.0;
        l2PrefitOfQZS.body[TypeID::satPCenterL2J]    =   -1.0;
        l2PrefitOfQZS.body[TypeID::relativity]    =   -1.0;
        l2PrefitOfQZS.body[TypeID::gravDelay]     =   -1.0;
        l2PrefitOfQZS.body[TypeID::tropoSlant]    =   -1.0;
        l2PrefitOfQZS.body[TypeID::windUpL2J]      =   -1.0;
        l2PrefitOfQZS.body[TypeID::rcvCorrL2J]      =   -1.0;
        // ionoMap, QZS SPP
        l2PrefitOfQZS.body[TypeID::ionoMapL2J] = +1.0;
        l2PrefitOfQZS.addOptionalType(TypeID::ionoMapL2J);
        // Definition to compute prefit residual of QZS L5
        l5PrefitOfQZS.header                      =   TypeID::prefitL5J;
        l5PrefitOfQZS.body[TypeID::L5J]            =   +1.0;
        l5PrefitOfQZS.body[TypeID::rho]           =   -1.0;
        l5PrefitOfQZS.body[TypeID::cdtSat]        =   +1.0;
        l5PrefitOfQZS.body[TypeID::satPCenterL5J]    =   -1.0;
        l5PrefitOfQZS.body[TypeID::relativity]    =   -1.0;
        l5PrefitOfQZS.body[TypeID::gravDelay]     =   -1.0;
        l5PrefitOfQZS.body[TypeID::tropoSlant]    =   -1.0;
        l5PrefitOfQZS.body[TypeID::windUpL5J]     =   -1.0;
        l5PrefitOfQZS.body[TypeID::rcvCorrL5J]    =   -1.0;
        // ionoMap, QZS SPP
        l5PrefitOfQZS.body[TypeID::ionoMapL5J] = +1.0;
        l5PrefitOfQZS.addOptionalType(TypeID::ionoMapL5J);

        // Definition to compute prefit residual of QZS L6
        l6PrefitOfQZS.header                      =   TypeID::prefitL6J;
        l6PrefitOfQZS.body[TypeID::L6J]            =   +1.0;
        l6PrefitOfQZS.body[TypeID::rho]           =   -1.0;
        l6PrefitOfQZS.body[TypeID::cdtSat]        =   +1.0;
        l6PrefitOfQZS.body[TypeID::satPCenterL6J]    =   -1.0;
        l6PrefitOfQZS.body[TypeID::relativity]    =   -1.0;
        l6PrefitOfQZS.body[TypeID::gravDelay]     =   -1.0;
        l6PrefitOfQZS.body[TypeID::tropoSlant]    =   -1.0;
        l6PrefitOfQZS.body[TypeID::windUpL6J]     =   -1.0;
        l6PrefitOfQZS.body[TypeID::rcvCorrL6J]    =   -1.0;
        // ionoMap, QZS SPP
        l6PrefitOfQZS.body[TypeID::ionoMapL6J] = +1.0;
        l6PrefitOfQZS.addOptionalType(TypeID::ionoMapL6J);

         // Definition to compute prefit residual of GPS PC, L1 + L2
        pc21PrefitOfGPS.header                    =   TypeID::prefitPC21G;
        pc21PrefitOfGPS.body[TypeID::prefitC2G]   =   +ag21;
        pc21PrefitOfGPS.body[TypeID::prefitC1G]   =   -bg21;

         // Definition to compute prefitG residual of GPS LC, L1 + L2
        lc21PrefitOfGPS.header                    =   TypeID::prefitLC21G;
        lc21PrefitOfGPS.body[TypeID::prefitL2G]   =   +ag21;
        lc21PrefitOfGPS.body[TypeID::prefitL1G]   =   -bg21;

         // Definition to compute prefit residual of GPS PC, L1 + L2
        pc12PrefitOfGPS.header                    =   TypeID::prefitPC12G;
        pc12PrefitOfGPS.body[TypeID::prefitC1G]   =   +ag12;
        pc12PrefitOfGPS.body[TypeID::prefitC2G]   =   -bg12;

         // Definition to compute prefitG residual of GPS LC, L1 + L2
        lc12PrefitOfGPS.header                    =   TypeID::prefitLC12G;
        lc12PrefitOfGPS.body[TypeID::prefitL1G]   =   +ag12;
        lc12PrefitOfGPS.body[TypeID::prefitL2G]   =   -bg12;
        // added by ragnwang
        // gps
        pc15PrefitOfGPS.header = TypeID::prefitPC15G;
        pc15PrefitOfGPS.body[TypeID::prefitC1G] = +ag15;
        pc15PrefitOfGPS.body[TypeID::prefitC5G] = -bg15;

        lc15PrefitOfGPS.header = TypeID::prefitLC15G;
        lc15PrefitOfGPS.body[TypeID::prefitL1G] = +ag15;
        lc15PrefitOfGPS.body[TypeID::prefitL5G] = -bg15;

        pc125PrefitOfGPS.header = TypeID::prefitPC125G;
        pc125PrefitOfGPS.body[TypeID::prefitC1G] = e1g;
        pc125PrefitOfGPS.body[TypeID::prefitC2G] = e2g;
        pc125PrefitOfGPS.body[TypeID::prefitC5G] = e5g;

        lc125PrefitOfGPS.header = TypeID::prefitLC125G;
        lc125PrefitOfGPS.body[TypeID::prefitL1G] = e1g;
        lc125PrefitOfGPS.body[TypeID::prefitL2G] = e2g;
        lc125PrefitOfGPS.body[TypeID::prefitL5G] = e5g;
        // Galiloe-related

        // Definition to compute prefit residual of GAL PC, L1 + L5
        pc51PrefitOfGAL.header                    =   TypeID::prefitPC51E;
        pc51PrefitOfGAL.body[TypeID::prefitC5E]   =   +ae51;
        pc51PrefitOfGAL.body[TypeID::prefitC1E]   =   -be51;

        // Definition to compute prefit residual of Galileo LC, L1 + L5
        lc51PrefitOfGAL.header                    =   TypeID::prefitLC51E;
        lc51PrefitOfGAL.body[TypeID::prefitL5E]   =   +ae51;
        lc51PrefitOfGAL.body[TypeID::prefitL1E]   =   -be51;

        // BDS-related combination

        pc26PrefitOfBDS.header                    =   TypeID::prefitPC26C;
        pc26PrefitOfBDS.body[TypeID::prefitC2C]   =   +ac26;
        pc26PrefitOfBDS.body[TypeID::prefitC6C]   =   -bc26;

         // Definition to compute prefitC residual of BDS LC, L6 + L2
        lc26PrefitOfBDS.header                    =   TypeID::prefitLC26C;
        lc26PrefitOfBDS.body[TypeID::prefitL2C]   =   +ac26;
        lc26PrefitOfBDS.body[TypeID::prefitL6C]   =   -bc26;

        // bds normal
        pc27PrefitOfBDS.header = TypeID::prefitPC27C;
        pc27PrefitOfBDS.body[TypeID::prefitC2C] = +ac27;
        pc27PrefitOfBDS.body[TypeID::prefitC7C] = -bc27;

		lc27PrefitOfBDS.header = TypeID::prefitLC27C;
		lc27PrefitOfBDS.body[TypeID::prefitL2C] = +ac27;
		lc27PrefitOfBDS.body[TypeID::prefitL7C] = -bc27;
        //267
        pc267PrefitOfBDS.header = TypeID::prefitPC267C;
        pc267PrefitOfBDS.body[TypeID::prefitC2C] = e2672c;
        pc267PrefitOfBDS.body[TypeID::prefitC6C] = e2676c;
        pc267PrefitOfBDS.body[TypeID::prefitC7C] = e2677c;

		lc267PrefitOfBDS.header = TypeID::prefitLC267C;
		lc267PrefitOfBDS.body[TypeID::prefitL2C] = e2672c;
		lc267PrefitOfBDS.body[TypeID::prefitL6C] = e2676c;
		lc267PrefitOfBDS.body[TypeID::prefitL7C] = e2677c;
        // 21
        pc21PrefitOfBDS.header = TypeID::prefitPC21C;
        pc21PrefitOfBDS.body[TypeID::prefitC2C] = +ac21;
        pc21PrefitOfBDS.body[TypeID::prefitC1C] = -bc21;
        
		lc21PrefitOfBDS.header = TypeID::prefitLC21C;
		lc21PrefitOfBDS.body[TypeID::prefitL2C] = +ac21;
		lc21PrefitOfBDS.body[TypeID::prefitL1C] = -bc21;

        // 61
        pc61PrefitOfBDS.header = TypeID::prefitPC61C;
        pc61PrefitOfBDS.body[TypeID::prefitC6C] = +ac61;
        pc61PrefitOfBDS.body[TypeID::prefitC1C] = -bc61;

        lc61PrefitOfBDS.header = TypeID::prefitPC61C;
        lc61PrefitOfBDS.body[TypeID::prefitC6C] = +ac61;
        lc61PrefitOfBDS.body[TypeID::prefitC1C] = -bc61;

        //25
        pc25PrefitOfBDS.header = TypeID::prefitPC25C;
        pc25PrefitOfBDS.body[TypeID::prefitC2C] = +ac25;
        pc25PrefitOfBDS.body[TypeID::prefitC5C] = -bc25;

		lc25PrefitOfBDS.header = TypeID::prefitLC25C;
		lc25PrefitOfBDS.body[TypeID::prefitL2C] = +ac25;
		lc25PrefitOfBDS.body[TypeID::prefitL5C] = -bc25;
        //28
        pc28PrefitOfBDS.header = TypeID::prefitPC28C;
        pc28PrefitOfBDS.body[TypeID::prefitC2C] = +ac28;
        pc28PrefitOfBDS.body[TypeID::prefitC8C] = -bc28;

		lc28PrefitOfBDS.header = TypeID::prefitLC28C;
		lc28PrefitOfBDS.body[TypeID::prefitL2C] = +ac28;
		lc28PrefitOfBDS.body[TypeID::prefitL8C] = -bc28;
        // 261
        pc261PrefitOfBDS.header = TypeID::prefitPC261C;
        pc261PrefitOfBDS.body[TypeID::prefitC2C] = e32nc;
        pc261PrefitOfBDS.body[TypeID::prefitC6C] = e36nc;
        pc261PrefitOfBDS.body[TypeID::prefitC1C] = e31nc;

        lc261PrefitOfBDS.header = TypeID::prefitLC261C;
        lc261PrefitOfBDS.body[TypeID::prefitL2C] = e32nc;
        lc261PrefitOfBDS.body[TypeID::prefitL6C] = e36nc;
        lc261PrefitOfBDS.body[TypeID::prefitL1C] = e31nc;
        // 2615
        pc2615PrefitOfBDS.header = TypeID::prefitPC2615C;
        pc2615PrefitOfBDS.body[TypeID::prefitC2C] = e42nc;
        pc2615PrefitOfBDS.body[TypeID::prefitC6C] = e46nc;
        pc2615PrefitOfBDS.body[TypeID::prefitC1C] = e41nc;
        pc2615PrefitOfBDS.body[TypeID::prefitC5C] = e45nc;

        lc2615PrefitOfBDS.header = TypeID::prefitLC2615C;
        lc2615PrefitOfBDS.body[TypeID::prefitL2C] = e42nc;
        lc2615PrefitOfBDS.body[TypeID::prefitL6C] = e46nc;
        lc2615PrefitOfBDS.body[TypeID::prefitL1C] = e41nc;
        lc2615PrefitOfBDS.body[TypeID::prefitL5C] = e45nc;

        // 26158
        pc26158PrefitOfBDS.header = TypeID::prefitPC26158C;
        pc26158PrefitOfBDS.body[TypeID::prefitC2C] = e1nc;
        pc26158PrefitOfBDS.body[TypeID::prefitC6C] = e2nc;
        pc26158PrefitOfBDS.body[TypeID::prefitC1C] = e3nc;
        pc26158PrefitOfBDS.body[TypeID::prefitC5C] = e4nc;
        pc26158PrefitOfBDS.body[TypeID::prefitC8C] = e5nc;

		lc26158PrefitOfBDS.header = TypeID::prefitLC26158C;
		lc26158PrefitOfBDS.body[TypeID::prefitL2C] = e1nc;
		lc26158PrefitOfBDS.body[TypeID::prefitL6C] = e2nc;
		lc26158PrefitOfBDS.body[TypeID::prefitL1C] = e3nc;
		lc26158PrefitOfBDS.body[TypeID::prefitL5C] = e4nc;
		lc26158PrefitOfBDS.body[TypeID::prefitL8C] = e5nc;

        // bdsfopt 15
        pc15PrefitOfBDS.header = TypeID::prefitPC15C;
        pc15PrefitOfBDS.body[TypeID::prefitC1C] = +ac15;
        pc15PrefitOfBDS.body[TypeID::prefitC5C] = -bc15;

		lc15PrefitOfBDS.header = TypeID::prefitLC15C;
		lc15PrefitOfBDS.body[TypeID::prefitL1C] = +ac15;
		lc15PrefitOfBDS.body[TypeID::prefitL5C] = -bc15;
        //12
		pc12PrefitOfBDS.header = TypeID::prefitPC12C;
		pc12PrefitOfBDS.body[TypeID::prefitC1C] = +ac12;
		pc12PrefitOfBDS.body[TypeID::prefitC2C] = -bc12;

		lc12PrefitOfBDS.header = TypeID::prefitLC12C;
		lc12PrefitOfBDS.body[TypeID::prefitL1C] = +ac12;
		lc12PrefitOfBDS.body[TypeID::prefitL2C] = -bc12;
        // 52
        pc52PrefitOfBDS.header = TypeID::prefitPC52C;
        pc52PrefitOfBDS.body[TypeID::prefitC5C] = +ac52;
        pc52PrefitOfBDS.body[TypeID::prefitC2C] = -bc52;
        lc52PrefitOfBDS.header = TypeID::prefitLC52C;
        lc52PrefitOfBDS.body[TypeID::prefitL5C] = +ac52;
        lc52PrefitOfBDS.body[TypeID::prefitL2C] = -bc52;
        //16
		pc16PrefitOfBDS.header = TypeID::prefitPC16C;
		pc16PrefitOfBDS.body[TypeID::prefitC1C] = +ac16;
		pc16PrefitOfBDS.body[TypeID::prefitC6C] = -bc16;

		lc16PrefitOfBDS.header = TypeID::prefitLC16C;
		lc16PrefitOfBDS.body[TypeID::prefitL1C] = +ac16;
		lc16PrefitOfBDS.body[TypeID::prefitL6C] = -bc16;
        //18
		pc18PrefitOfBDS.header = TypeID::prefitPC18C;
		pc18PrefitOfBDS.body[TypeID::prefitC1C] = +ac18;
		pc18PrefitOfBDS.body[TypeID::prefitC8C] = -bc18;

		lc18PrefitOfBDS.header = TypeID::prefitLC18C;
		lc18PrefitOfBDS.body[TypeID::prefitL1C] = +ac18;
		lc18PrefitOfBDS.body[TypeID::prefitL8C] = -bc18;
        // 152
        pc152PrefitOfBDS.header = TypeID::prefitPC1526C;
        pc152PrefitOfBDS.body[TypeID::prefitC1C] = e31c;
        pc152PrefitOfBDS.body[TypeID::prefitC5C] = e35c;
        pc152PrefitOfBDS.body[TypeID::prefitC2C] = e32c;

        lc152PrefitOfBDS.header = TypeID::prefitLC152C;
        lc152PrefitOfBDS.body[TypeID::prefitL1C] = e31c;
        lc152PrefitOfBDS.body[TypeID::prefitL5C] = e35c;
        lc152PrefitOfBDS.body[TypeID::prefitL2C] = e32c;

        // 1526
        pc1526PrefitOfBDS.header = TypeID::prefitPC1526C;
        pc1526PrefitOfBDS.body[TypeID::prefitC1C] = e41c;
        pc1526PrefitOfBDS.body[TypeID::prefitC5C] = e45c;
        pc1526PrefitOfBDS.body[TypeID::prefitC2C] = e42c;
        pc1526PrefitOfBDS.body[TypeID::prefitC6C] = e46c;

        lc1526PrefitOfBDS.header = TypeID::prefitLC1526C;
        lc1526PrefitOfBDS.body[TypeID::prefitL1C] = e41c;
        lc1526PrefitOfBDS.body[TypeID::prefitL5C] = e45c;
        lc1526PrefitOfBDS.body[TypeID::prefitL2C] = e42c;
        lc1526PrefitOfBDS.body[TypeID::prefitL6C] = e46c;


        // 15268
        pc15268PrefitOfBDS.header = TypeID::prefitPC15268C;
        pc15268PrefitOfBDS.body[TypeID::prefitC1C] = e1c;
        pc15268PrefitOfBDS.body[TypeID::prefitC5C] = e2c;
        pc15268PrefitOfBDS.body[TypeID::prefitC2C] = e3c;
        pc15268PrefitOfBDS.body[TypeID::prefitC6C] = e4c;
        pc15268PrefitOfBDS.body[TypeID::prefitC8C] = e5c;
        
		lc15268PrefitOfBDS.header = TypeID::prefitLC15268C;
		lc15268PrefitOfBDS.body[TypeID::prefitL1C] = e1c;
		lc15268PrefitOfBDS.body[TypeID::prefitL5C] = e2c;
		lc15268PrefitOfBDS.body[TypeID::prefitL2C] = e3c;
		lc15268PrefitOfBDS.body[TypeID::prefitL6C] = e4c;
		lc15268PrefitOfBDS.body[TypeID::prefitL8C] = e5c;
        // 158
        pc158PrefitOfBDS.header = TypeID::prefitPC158C;
        pc158PrefitOfBDS.body[TypeID::prefitC1C] = e1581c;
        pc158PrefitOfBDS.body[TypeID::prefitC5C] = e1585c;
        pc158PrefitOfBDS.body[TypeID::prefitC8C] = e1588c;

		lc158PrefitOfBDS.header = TypeID::prefitLC158C;
		lc158PrefitOfBDS.body[TypeID::prefitL1C] = e1581c;
		lc158PrefitOfBDS.body[TypeID::prefitL5C] = e1585c;
		lc158PrefitOfBDS.body[TypeID::prefitL8C] = e1588c;

        

        // gal;
        pc15PrefitOfGAL.header = TypeID::prefitPC15E;
        pc15PrefitOfGAL.body[TypeID::prefitC1E] = +ae15;
        pc15PrefitOfGAL.body[TypeID::prefitC5E] = -be15;

		lc15PrefitOfGAL.header = TypeID::prefitLC15E;
		lc15PrefitOfGAL.body[TypeID::prefitL1E] = +ae15;
		lc15PrefitOfGAL.body[TypeID::prefitL5E] = -be15;

        //17
        pc17PrefitOfGAL.header = TypeID::prefitPC17E;
        pc17PrefitOfGAL.body[TypeID::prefitC1E] = +ae17;
        pc17PrefitOfGAL.body[TypeID::prefitC5E] = -be17;

		lc17PrefitOfGAL.header = TypeID::prefitLC17E;
		lc17PrefitOfGAL.body[TypeID::prefitL1E] = +ae17;
		lc17PrefitOfGAL.body[TypeID::prefitL5E] = -be17;
        // 18
        pc18PrefitOfGAL.header = TypeID::prefitPC18E;
        pc18PrefitOfGAL.body[TypeID::prefitC1E] = +ae18;
        pc18PrefitOfGAL.body[TypeID::prefitC8E] = -be18;

		lc18PrefitOfGAL.header = TypeID::prefitLC18E;
		lc18PrefitOfGAL.body[TypeID::prefitL1E] = +ae18;
		lc18PrefitOfGAL.body[TypeID::prefitL8E] = -be18;
        // 16
        pc16PrefitOfGAL.header = TypeID::prefitPC16E;
        pc16PrefitOfGAL.body[TypeID::prefitC1E] = +ae16;
        pc16PrefitOfGAL.body[TypeID::prefitC6E] = -be16;

		lc16PrefitOfGAL.header = TypeID::prefitLC16E;
		lc16PrefitOfGAL.body[TypeID::prefitL1E] = +ae16;
		lc16PrefitOfGAL.body[TypeID::prefitL6E] = -be16;
        //157
        pc157PrefitOfGAL.header = TypeID::prefitPC157E;
        pc157PrefitOfGAL.body[TypeID::prefitC1E] = e31e;
        pc157PrefitOfGAL.body[TypeID::prefitC5E] = e35e;
        pc157PrefitOfGAL.body[TypeID::prefitC7E] = e37e;

        lc157PrefitOfGAL.header = TypeID::prefitLC157E;
        lc157PrefitOfGAL.body[TypeID::prefitL1E] = e31e;
        lc157PrefitOfGAL.body[TypeID::prefitL5E] = e35e;
        lc157PrefitOfGAL.body[TypeID::prefitL7E] = e37e;

        //1578
        pc1578PrefitOfGAL.header = TypeID::prefitPC1578E;
        pc1578PrefitOfGAL.body[TypeID::prefitC1E] = e41e;
        pc1578PrefitOfGAL.body[TypeID::prefitC5E] = e45e;
        pc1578PrefitOfGAL.body[TypeID::prefitC7E] = e47e;
        pc1578PrefitOfGAL.body[TypeID::prefitC8E] = e48e;

        lc1578PrefitOfGAL.header = TypeID::prefitLC1578E;
        lc1578PrefitOfGAL.body[TypeID::prefitL1E] = e41e;
        lc1578PrefitOfGAL.body[TypeID::prefitL5E] = e45e;
        lc1578PrefitOfGAL.body[TypeID::prefitL7E] = e47e;
        lc1578PrefitOfGAL.body[TypeID::prefitL8E] = e48e;

        //15786
        pc15786PrefitOfGAL.header = TypeID::prefitPC15786E;
        pc15786PrefitOfGAL.body[TypeID::prefitC1E] = e1e;
        pc15786PrefitOfGAL.body[TypeID::prefitC5E] = e2e;
        pc15786PrefitOfGAL.body[TypeID::prefitC7E] = e3e;
        pc15786PrefitOfGAL.body[TypeID::prefitC8E] = e4e;
        pc15786PrefitOfGAL.body[TypeID::prefitC6E] = e5e;

		lc15786PrefitOfGAL.header = TypeID::prefitLC15786E;
		lc15786PrefitOfGAL.body[TypeID::prefitL1E] = e1e;
		lc15786PrefitOfGAL.body[TypeID::prefitL5E] = e2e;
		lc15786PrefitOfGAL.body[TypeID::prefitL7E] = e3e;
		lc15786PrefitOfGAL.body[TypeID::prefitL8E] = e4e;
		lc15786PrefitOfGAL.body[TypeID::prefitL6E] = e5e;

        ///************** clock and ambiguity *************//
        c1ResOfGPS.header = TypeID::prefitC1G;
        c1ResOfGPS.body[TypeID::dcdtGPS] = -1.0;
        l1ResOfGPS.header = TypeID::prefitL1G;
        l1ResOfGPS.body[TypeID::dcdtGPS] = -1.0;
        l1ResOfGPS.body[TypeID::BL1G] = -1.0;
        // L2
        c2ResOfGPS.header = TypeID::prefitC2G;
        c2ResOfGPS.body[TypeID::dcdtGPS] = -1.0;
        l2ResOfGPS.header = TypeID::prefitL2G;
        l2ResOfGPS.body[TypeID::dcdtGPS] = -1.0;
        l2ResOfGPS.body[TypeID::BL2G] = -1.0;
        // L5
        c5ResOfGPS.header = TypeID::prefitC5G;
        c5ResOfGPS.body[TypeID::dcdtGPS] = -1.0;
        l5ResOfGPS.header = TypeID::prefitL5G;
        l5ResOfGPS.body[TypeID::dcdtGPS] = -1.0;
        l5ResOfGPS.body[TypeID::BL5G] = -1.0;
        // IF12
        pc12ResOfGPS.header = TypeID::prefitPC12G;
        pc12ResOfGPS.body[TypeID::dcdtGPS] = -1.0;
        lc12ResOfGPS.header = TypeID::prefitLC12G;
        lc12ResOfGPS.body[TypeID::dcdtGPS] = -1.0;
        lc12ResOfGPS.body[TypeID::BLC12G] = -1.0;
        // IF15
        pc15ResOfGPS.header = TypeID::prefitPC15G;
        pc15ResOfGPS.body[TypeID::dcdtGPS] = -1.0;
        lc15ResOfGPS.header = TypeID::prefitLC15G;
        lc15ResOfGPS.body[TypeID::dcdtGPS] = -1.0;
        lc15ResOfGPS.body[TypeID::BLC15G] = -1.0;
        // IF125
        pc125ResOfGPS.header = TypeID::prefitPC125G;
        pc125ResOfGPS.body[TypeID::dcdtGPS] = -1.0;
        lc125ResOfGPS.header = TypeID::prefitLC125G;
        lc125ResOfGPS.body[TypeID::dcdtGPS] = -1.0;
        lc125ResOfGPS.body[TypeID::BLC125G] = -1.0;
        // GAL
        c1ResOfGAL.header = TypeID::prefitC1E;
        c1ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        l1ResOfGAL.header = TypeID::prefitL1E;
        l1ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        l1ResOfGAL.body[TypeID::BL1E] = -1.0;
        // L5E
        c5ResOfGAL.header = TypeID::prefitC5E;
        c5ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        l5ResOfGAL.header = TypeID::prefitL5E;
        l5ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        l5ResOfGAL.body[TypeID::BL5E] = -1.0;
        // L7E
        c7ResOfGAL.header = TypeID::prefitC7E;
        c7ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        l7ResOfGAL.header = TypeID::prefitL7E;
        l7ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        l7ResOfGAL.body[TypeID::BL7E] = -1.0;
        // L8E
        c8ResOfGAL.header = TypeID::prefitC8E;
        c8ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        l8ResOfGAL.header = TypeID::prefitL8E;
        l8ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        l8ResOfGAL.body[TypeID::BL8E] = -1.0;
        // L6E
        c6ResOfGAL.header = TypeID::prefitC6E;
        c6ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        l6ResOfGAL.header = TypeID::prefitL6E;
        l6ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        l6ResOfGAL.body[TypeID::BL6E] = -1.0;
        // IF pair-wise
        pc15ResOfGAL.header = TypeID::prefitPC15E;
        pc15ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        lc15ResOfGAL.header = TypeID::prefitLC15E;
        lc15ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        lc15ResOfGAL.body[TypeID::BLC15E] = -1.0;
        // IF17
        pc17ResOfGAL.header = TypeID::prefitPC17E;
        pc17ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        lc17ResOfGAL.header = TypeID::prefitLC17E;
        lc17ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        lc17ResOfGAL.body[TypeID::BLC17E] = -1.0;
        // IF18
        pc18ResOfGAL.header = TypeID::prefitPC18E;
        pc18ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        lc18ResOfGAL.header = TypeID::prefitLC18E;
        lc18ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        lc18ResOfGAL.body[TypeID::BLC18E] = -1.0;
        // IF16
        pc16ResOfGAL.header = TypeID::prefitPC16E;
        pc16ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        lc16ResOfGAL.header = TypeID::prefitLC16E;
        lc16ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        lc16ResOfGAL.body[TypeID::BLC16E] = -1.0;
        // IF linear combination
        pc157ResOfGAL.header = TypeID::prefitPC157E;
        pc157ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        lc157ResOfGAL.header = TypeID::prefitLC157E;
        lc157ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        lc157ResOfGAL.body[TypeID::BLC157E] = -1.0;
        //
        pc1578ResOfGAL.header = TypeID::prefitPC1578E;
        pc1578ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        lc1578ResOfGAL.header = TypeID::prefitLC1578E;
        lc1578ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        lc1578ResOfGAL.body[TypeID::BLC1578E] = -1.0;
        //
        pc15786ResOfGAL.header = TypeID::prefitPC15786E;
        pc15786ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        lc15786ResOfGAL.header = TypeID::prefitLC15786E;
        lc15786ResOfGAL.body[TypeID::dcdtGAL] = -1.0;
        lc15786ResOfGAL.body[TypeID::BLC15786E] = -1.0;
        ///////////////////// BDS ////////////////////
        // l1
        c1ResOfBDS.header = TypeID::prefitC1C;
        c1ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        l1ResOfBDS.header = TypeID::prefitL1C;
        l1ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        l1ResOfBDS.body[TypeID::BL1C] = -1.0;
        // L5
        c5ResOfBDS.header = TypeID::prefitC5C;
        c5ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        l5ResOfBDS.header = TypeID::prefitL5C;
        l5ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        l5ResOfBDS.body[TypeID::BL5C] = -1.0;
        // L2
        c2ResOfBDS.header = TypeID::prefitC2C;
        c2ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        l2ResOfBDS.header = TypeID::prefitL2C;
        l2ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        l2ResOfBDS.body[TypeID::BL2C] = -1.0;
        // L6
        c6ResOfBDS.header = TypeID::prefitC6C;
        c6ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        l6ResOfBDS.header = TypeID::prefitL6C;
        l6ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        l6ResOfBDS.body[TypeID::BL6C] = -1.0;
        // L8
        c8ResOfBDS.header = TypeID::prefitC8C;
        c8ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        l8ResOfBDS.header = TypeID::prefitL8C;
        l8ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        l8ResOfBDS.body[TypeID::BL8C] = -1.0;
        // l7
        c7ResOfBDS.header = TypeID::prefitC7C;
        c7ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        l7ResOfBDS.header = TypeID::prefitL7C;
        l7ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        l7ResOfBDS.body[TypeID::BL7C] = -1.0;
        //IF pair-wise IF15
        pc15ResOfBDS.header = TypeID::prefitPC15C;
        pc15ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc15ResOfBDS.header = TypeID::prefitLC15C;
        lc15ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc15ResOfBDS.body[TypeID::BLC15C] = -1.0;
        //IF pair-wise IF52
        pc52ResOfBDS.header = TypeID::prefitPC52C;
        pc52ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc52ResOfBDS.header = TypeID::prefitLC52C;
        lc52ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc52ResOfBDS.body[TypeID::BLC52C] = -1.0;
        //IF pair-wise IF16
        pc16ResOfBDS.header = TypeID::prefitPC16C;
        pc16ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc16ResOfBDS.header = TypeID::prefitLC16C;
        lc16ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc16ResOfBDS.body[TypeID::BLC16C] = -1.0;
        //IF pair-wise IF18
        pc18ResOfBDS.header = TypeID::prefitPC18C;
        pc18ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc18ResOfBDS.header = TypeID::prefitLC18C;
        lc18ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc18ResOfBDS.body[TypeID::BLC18C] = -1.0;
        //IF pair-wise IF26
        pc26ResOfBDS.header = TypeID::prefitPC26C;
        pc26ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc26ResOfBDS.header = TypeID::prefitLC26C;
        lc26ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc26ResOfBDS.body[TypeID::BLC26C] = -1.0;
        //IF pair-wise IF26
        pc61ResOfBDS.header = TypeID::prefitPC61C;
        pc61ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc61ResOfBDS.header = TypeID::prefitLC61C;
        lc61ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc61ResOfBDS.body[TypeID::BLC61C] = -1.0;
        //IF pair-wise IF25
        pc25ResOfBDS.header = TypeID::prefitPC25C;
        pc25ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc25ResOfBDS.header = TypeID::prefitLC25C;
        lc25ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc25ResOfBDS.body[TypeID::BLC25C] = -1.0;
        //IF pair-wise IF28
        pc28ResOfBDS.header = TypeID::prefitPC28C;
        pc28ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc28ResOfBDS.header = TypeID::prefitLC28C;
        lc28ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc28ResOfBDS.body[TypeID::BLC28C] = -1.0;
        //IF pair-wise IF27
        pc27ResOfBDS.header = TypeID::prefitPC27C;
        pc27ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc27ResOfBDS.header = TypeID::prefitLC27C;
        lc27ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc27ResOfBDS.body[TypeID::BLC27C] = -1.0;
        // IF linear combination
        //IF pair-wise IF261
        pc261ResOfBDS.header = TypeID::prefitPC261C;
        pc261ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc261ResOfBDS.header = TypeID::prefitLC261C;
        lc261ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc261ResOfBDS.body[TypeID::BLC261C] = -1.0;
        // IF267
        pc267ResOfBDS.header = TypeID::prefitPC267C;
        pc267ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc267ResOfBDS.header = TypeID::prefitLC267C;
        lc267ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc267ResOfBDS.body[TypeID::BLC267C] = -1.0;
        // 2615
        pc2615ResOfBDS.header = TypeID::prefitPC2615C;
        pc2615ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc2615ResOfBDS.header = TypeID::prefitLC2615C;
        lc2615ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc2615ResOfBDS.body[TypeID::BLC2615C] = -1.0;
        // 26158
        pc26158ResOfBDS.header = TypeID::prefitPC26158C;
        pc26158ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc26158ResOfBDS.header = TypeID::prefitLC26158C;
        lc26158ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc26158ResOfBDS.body[TypeID::BLC26158C] = -1.0;
        // 152
        pc152ResOfBDS.header = TypeID::prefitPC152C;
        pc152ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc152ResOfBDS.header = TypeID::prefitLC152C;
        lc152ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc152ResOfBDS.body[TypeID::BLC152C] = -1.0;
        // 158
        pc158ResOfBDS.header = TypeID::prefitPC158C;
        pc158ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc158ResOfBDS.header = TypeID::prefitLC158C;
        lc158ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc158ResOfBDS.body[TypeID::BLC158C] = -1.0;
        // 1526
        pc1526ResOfBDS.header = TypeID::prefitPC1526C;
        pc1526ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc1526ResOfBDS.header = TypeID::prefitLC1526C;
        lc1526ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc1526ResOfBDS.body[TypeID::BLC1526C] = -1.0;
        // 15268
        pc15268ResOfBDS.header = TypeID::prefitPC15268C;
        pc15268ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc15268ResOfBDS.header = TypeID::prefitLC15268C;
        lc15268ResOfBDS.body[TypeID::dcdtBDS] = -1.0;
        lc15268ResOfBDS.body[TypeID::BLC15268C] = -1.0;
    }  // End of constructor 'LinearCombinations::LinearCombinations()'


      // Return the frequency of the combination in cycles: i * L1 + j * L2
    double LinearCombinations::freqOfLC(int i, int j, double f1 , double f2 )
    {
        return ( double(i)*f1+double(j)*f2 );
    }

    /// Return the wavelength of the combination in cycles: i * L1 + j * L2
    double LinearCombinations::wavelengthOfLC(int i,int j,double f1,double f2)
    {
        return C_MPS / freqOfLC(i,j,f1,f2);
    }

    /// Return the f1 factor of the combination in cycles: i * L1 + j * L2
    double LinearCombinations::firstFactorOfLC(int i,int j,double f1,double f2)
    {
        return double(i)*f1/freqOfLC(i,j,f1,f2);
    }

    /// Return the f2 factor of the combination in cycles: i * L1 + j * L2
    double LinearCombinations::secondFactorOfLC(int i,int j,double f1,double f2 )
    {
        return double(j)*f2/freqOfLC(i,j,f1,f2);
    }

    /// Warning:global function
    double wavelengthOfMW(const SatelliteSystem& sys, const TypeID& type)
    {
        string typeStr = type.asString();

        int band1 = asInt(typeStr.substr(2,1));
        int band2 = asInt(typeStr.substr(3,1));

        double f1 = getFreq(sys, band1);
        double f2 = getFreq(sys, band2);

        double wavelength = C_MPS/(f1 - f2);

        if(debug)
        {
            cout << "f1:" << f1 << "f2:" << f2 << endl;
            cout << "wavelength:" << wavelength << endl;
        }

        return wavelength; 

    };

    /// Warning:global function
    double varOfMW(const SatelliteSystem& sys, const TypeID& type)
    {
        string typeStr = type.asString();

        int band1 = asInt(typeStr.substr(2,1));
        int band2 = asInt(typeStr.substr(3,1));

        double f1 = getFreq(sys, band1);
        double f2 = getFreq(sys, band2);

        double varPhase = 0.003*0.003;
        //double varPhase = 0.003*0.003;
        double varCode = 0.3*0.3;

        double var = (f1*f1 + f2*f2)/((f1-f2)*(f1-f2)) * varPhase  + 
                     (f1*f1 + f2*f2)/((f1+f2)*(f1+f2)) * varCode;

        if(debug)
        {
            cout << "f1:" << f1 << "f2:" << f2 << endl;
            cout << "var:" << var << endl;
        }

        return var; 

    };

} // End of namespace gpstk
