#pragma ident "$Id$"

/**
* @file SolidTides.cpp
* Computes the effect of solid Earth tides on a given position and epoch.
*/



#include "SolidTides.hpp"
#include "CivilTime.hpp"
#include "YDSTime.hpp"
#include "MJD.hpp"
#include "GNSSconstants.hpp"


namespace gpstk
{

    // Love and shida numbers
    const double SolidTides::H20(0.6078), SolidTides::L20(0.0847);
    const double SolidTides::H3(0.2920), SolidTides::L3(0.0150);


    /* Return the effect of solid Earth tides (meters) at the given
     * position and epoch, in the Up-East-Down (UEN) reference frame.
     *
     * @param[in] time  Epoch to look up
     * @param[in] pos   Position of interest
     *
     * @return a Triple with the solid tidal effect, in meters and in
     * the UED reference frame.
     *
     * @throw InvalidRequest If the request can not be completed for any
     * reason, this is thrown. The text may have additional information
     * as to why the request failed.
     */
    Triple SolidTides::getSolidTide( const CommonTime& time,
                                     const Position& pos )
        noexcept(false)
    {

        // We will store here the results
        Triple ret(0.0,0.0,0.0);

        try
        {

            if(pRefSys == NULL) return ret;
            if(pSolSys == NULL) return ret;

            CommonTime utc( pRefSys->GPS2UTC(time) );

            Matrix<double> c2tRaw( pRefSys->C2TMatrix(utc) );


            CommonTime tt( pRefSys->GPS2TT(time) );


            double jd_tt( JulianDate(tt).jd );

            Triple p2(pos.X(),pos.Y(),pos.Z());

            // Fraction of hour
            double FHR = YDSTime(utc).sod/3600.0;

            SolarSystem::Planet center(SolarSystem::Earth);
            SolarSystem::Planet sun(SolarSystem::Sun);
            SolarSystem::Planet moon(SolarSystem::Moon);

            double rv_sun[6] = {0.0};
            pSolSys->computeState(jd_tt, sun, center, rv_sun);

            Vector<double> rSunECI(3,0.0);
            rSunECI(0) = rv_sun[0];
            rSunECI(1) = rv_sun[1];
            rSunECI(2) = rv_sun[2];
            rSunECI *= 1.0e+3;

            Vector<double> rSunECF( c2tRaw*rSunECI );

            double rv_moon[6] = {0.0};
            pSolSys->computeState(jd_tt, moon, center, rv_moon);

            Vector<double> rMoonECI(3,0.0);
            rMoonECI(0) = rv_moon[0];
            rMoonECI(1) = rv_moon[1];
            rMoonECI(2) = rv_moon[2];
            rMoonECI *= 1.0e+3;

            Vector<double> rMoonECF( c2tRaw*rMoonECI );

            // Variables to hold Sun and Moon positions in ECF.
            Triple sunPos(rSunECF[0], rSunECF[1], rSunECF[2]);
            Triple moonPos(rMoonECF[0], rMoonECF[1], rMoonECF[2]);

            double SCS = p2.dot(sunPos);
            double SCM = p2.dot(moonPos);
            double RSTA = std::sqrt( p2.dot(p2) );
            double RSUN = std::sqrt( (sunPos.dot(sunPos)) );
            double RMON = std::sqrt( (moonPos.dot(moonPos)) );

            double SCSUN = SCS/RSTA/RSUN;
            double SCMON = SCM/RSTA/RMON;

            // Compute the new H2 and L2 for degree 2
            double COSPHI = std::sqrt(p2[0]*p2[0]+p2[1]*p2[1])/RSTA;
            double H2 = H20-0.0006*(1.0-3.0/2.0*COSPHI*COSPHI);
            double L2 = L20+0.0002*(1.0-3.0/2.0*COSPHI*COSPHI);

            // P2 term
            double P2SUN = 3.0*(H2/2.0-L2)*SCSUN*SCSUN-H2/2.0;
            double P2MON = 3.0*(H2/2.0-L2)*SCMON*SCMON-H2/2.0;

            // P3 term
            double P3SUN = 5.0/2.0*(H3-3.0*L3)*SCSUN*SCSUN*SCSUN+3.0/2.0*(L3-H3)*SCSUN;
            double P3MON = 5.0/2.0*(H3-3.0*L3)*SCMON*SCMON*SCMON+3.0/2.0*(L3-H3)*SCMON;

            // Term in direction of sun/moom vector
            double X2SUN = 3.0*L2*SCSUN;
            double X2MON = 3.0*L2*SCMON;
            double X3SUN = 3.0*L3/2.0*(5.0*SCSUN*SCSUN-1.0);
            double X3MON = 3.0*L3/2.0*(5.0*SCMON*SCMON-1.0);

            // Factors for sun and moon
            double MASS_RATIO_SUN = 332946.0482;
            double MASS_RATIO_MOON = 0.0123000371;
            double FAC2SUN = MASS_RATIO_SUN*RE_EARTH*std::pow(RE_EARTH/RSUN,3);
            double FAC2MON = MASS_RATIO_MOON*RE_EARTH*std::pow(RE_EARTH/RMON,3);
            double FAC3SUN = FAC2SUN*(RE_EARTH/RSUN);
            double FAC3MON = FAC2MON*(RE_EARTH/RMON);

            // Total displacement for degree 2 and 3(in-phase)
            Triple DXTIDE(0.0,0.0,0.0);
            for(int i = 0; i< 3; ++i)
            {
                DXTIDE[i] = FAC2SUN*( X2SUN*sunPos[i]/RSUN + P2SUN*p2[i]/RSTA ) +
                            FAC2MON*( X2MON*moonPos[i]/RMON + P2MON*p2[i]/RSTA ) +
                            FAC3SUN*( X3SUN*sunPos[i]/RSUN + P3SUN*p2[i]/RSTA ) +
                            FAC3MON*( X3MON*moonPos[i]/RMON + P3MON*p2[i]/RSTA );
            }

            // Then corrects for the out-of-phase part of love numbers(PART H_2^(0)I AND L_2^(0)I)

            // First, the diurnal band
            Triple XCORSTA1 = getStep1Diu(p2, sunPos, moonPos, FAC2SUN, FAC2MON);

            // Second, the semi-diurnal band
            Triple XCORSTA2 = getStep1Sem(p2, sunPos, moonPos, FAC2SUN, FAC2MON);

            // Then corrects for thr latitude dependence of love numbers(PART L^(1) )
            Triple XCORSTA3 = getStep1L1(p2, sunPos, moonPos, FAC2SUN, FAC2MON);

            for (int i=0; i<3; ++i)
            {
                DXTIDE[i] += XCORSTA1[i] + XCORSTA2[i] + XCORSTA3[i];
            }


            // After the three procedures, Step 1 is  finished.
            // Then corrections for step2 and add to the results of step1.

            //double T = ((MJD_TO_JD - 2451545.0) + MJD(epoch3).mjd +  FHR/24.0)/36525.0;
            // Centuries since J2000

            double T( (MJD(tt).mjd - MJD_J2000)/JC );

            // Caculate the diurnal band corrections(in-phase and out-of-phase frequency dependence)
            Triple XCORSTA4 = getStep2Diu(p2,FHR,T);

            // Corretions for the long period band(in-phase and out-of-phase frequency dependence)
            Triple XCORSTA5 = getStep2Lon(p2,T);

            for (int i=0; i<3; ++i)
            {
                DXTIDE[i] += XCORSTA4[i] + XCORSTA5[i];
            }

            // Change the coordinate from ITRF to UEN
            double lat = pos.getGeodeticLatitude() * DEG_TO_RAD;
            double lon = pos.getLongitude() * DEG_TO_RAD;

            ret[0] = std::cos(lon)*std::cos(lat)*DXTIDE[0] +
                     std::sin(lon)*std::cos(lat)*DXTIDE[1] +
                     std::sin(lat)*DXTIDE[2];
            ret[1] = -std::sin(lon)*DXTIDE[0] + std::cos(lon)*DXTIDE[1];
            ret[2] = -std::cos(lon)*std::sin(lat)*DXTIDE[0] -
                     std::sin(lon)*std::sin(lat)*DXTIDE[1] +
                     std::cos(lat)*DXTIDE[2];

        } // End of try block
        catch(InvalidRequest& ir)
        {
            GPSTK_RETHROW(ir);
        }

        return ret;

    } // End SolidTides::getSolidTide()


    Triple SolidTides::getStep1Diu( const Triple& XSTA,
                                    const Triple& XSUN,
                                    const Triple& XMON,
                                    double FAC2SUN,
                                    double FAC2MON )
    {
        Triple XCORSTA(0.0,0.0,0.0);

        double DHI = -0.0025, DLI = -0.0007;

        // Compute the normalized position vector of the IGS station.
        double RSTA = std::sqrt(XSTA.dot(XSTA));
        double SINPHI = XSTA[2]/RSTA;
        double COSPHI = std::sqrt(XSTA[0]*XSTA[0]+XSTA[1]*XSTA[1])/RSTA;
        double COS2PHI = COSPHI*COSPHI-SINPHI*SINPHI;
        double SINLA = XSTA[1]/COSPHI/RSTA;
        double COSLA = XSTA[0]/COSPHI/RSTA;

        // Compute the normalized position vector of the Moon.
        double RMON = std::sqrt(XMON.dot(XMON));

        // Compute the normalized position vector of the Sun.
        double RSUN = std::sqrt(XSUN.dot(XSUN));

        double DRSUN = -3.0*DHI*SINPHI*COSPHI*FAC2SUN*XSUN[2]*
                        (XSUN[0]*SINLA-XSUN[1]*COSLA)/RSUN/RSUN;

        double DRMON = -3.0*DHI*SINPHI*COSPHI*FAC2MON*XMON[2]*
                        (XMON[0]*SINLA-XMON[1]*COSLA)/RMON/RMON;

        double DNSUN = -3.0*DLI*COS2PHI*FAC2SUN*XSUN[2]*
                        (XSUN[0]*SINLA-XSUN[1]*COSLA)/RSUN/RSUN;

        double DNMON = -3.0*DLI*COS2PHI*FAC2MON*XMON[2]*
                        (XMON[0]*SINLA-XMON[1]*COSLA)/RMON/RMON;

        double DESUN = -3.0*DLI*SINPHI*FAC2SUN*XSUN[2]*
                        (XSUN[0]*COSLA+XSUN[1]*SINLA)/RSUN/RSUN;

        double DEMON = -3.0*DLI*SINPHI*FAC2MON*XMON[2]*
                        (XMON[0]*COSLA+XMON[1]*SINLA)/RMON/RMON;

        double DR = DRSUN + DRMON;
        double DN = DNSUN + DNMON;
        double DE = DESUN + DEMON;

        //  Compute the corrections for the station.
        XCORSTA[0] = DR*COSLA*COSPHI - DE*SINLA - DN*SINPHI*COSLA;
        XCORSTA[1] = DR*SINLA*COSPHI + DE*COSLA - DN*SINPHI*SINLA;
        XCORSTA[2] = DR*SINPHI + DN*COSPHI;

        return XCORSTA;

    } // End SolidTides::getStep1Diu()


    Triple SolidTides::getStep1Sem( const Triple& XSTA,
                                    const Triple& XSUN,
                                    const Triple& XMON,
                                    double FAC2SUN,
                                    double FAC2MON )
    {
        Triple XCORSTA(0.0,0.0,0.0);

        double DHI = -0.0022, DLI = -0.0007;

        // Compute the normalized position vector of the IGS station.
        double RSTA = std::sqrt(XSTA.dot(XSTA));
        double SINPHI = XSTA[2]/RSTA;
        double COSPHI = std::sqrt(XSTA[0]*XSTA[0]+XSTA[1]*XSTA[1])/RSTA;
        double SINLA = XSTA[1]/COSPHI/RSTA;
        double COSLA = XSTA[0]/COSPHI/RSTA;
        double COSTWOLA = COSLA*COSLA-SINLA*SINLA;
        double SINTWOLA = 2.0*COSLA*SINLA;

        // Compute the normalized position vector of the Moon.
        double RMON = std::sqrt(XMON.dot(XMON));

        // Compute the normalized position vector of the Sun.
        double RSUN = std::sqrt(XSUN.dot(XSUN));

        double DRSUN = -3.0/4.0*DHI*COSPHI*COSPHI*FAC2SUN*
                        ((XSUN[0]*XSUN[0]-XSUN[1]*XSUN[1])*SINTWOLA
                         -2.0*XSUN[0]*XSUN[1]*COSTWOLA)/RSUN/RSUN;

        double DRMON = -3.0/4.0*DHI*COSPHI*COSPHI*FAC2MON*
                        ((XMON[0]*XMON[0]-XMON[1]*XMON[1])*SINTWOLA
                         -2.0*XMON[0]*XMON[1]*COSTWOLA)/RMON/RMON;

        double DNSUN = 3.0/2.0*DLI*SINPHI*COSPHI*FAC2SUN*
                        ((XSUN[0]*XSUN[0]-XSUN[1]*XSUN[1])*SINTWOLA
                         -2.0*XSUN[0]*XSUN[1]*COSTWOLA)/RSUN/RSUN;

        double DNMON = 3.0/2.0*DLI*SINPHI*COSPHI*FAC2MON*
                        ((XMON[0]*XMON[0]-XMON[1]*XMON[1])*SINTWOLA
                         -2.0*XMON[0]*XMON[1]*COSTWOLA)/RMON/RMON;

        double DESUN = -3.0/2.0*DLI*COSPHI*FAC2SUN*
                        ((XSUN[0]*XSUN[0]-XSUN[1]*XSUN[1])*COSTWOLA
                         +2.0*XSUN[0]*XSUN[1]*SINTWOLA)/RSUN/RSUN;

        double DEMON = -3.0/2.0*DLI*COSPHI*FAC2MON*
                        ((XMON[0]*XMON[0]-XMON[1]*XMON[1])*COSTWOLA
                         +2.0*XMON[0]*XMON[1]*SINTWOLA)/RMON/RMON;

        double DR = DRSUN + DRMON;
        double DN = DNSUN + DNMON;
        double DE = DESUN + DEMON;

        //  Compute the corrections for the station.
        XCORSTA[0] = DR*COSLA*COSPHI - DE*SINLA - DN*SINPHI*COSLA;
        XCORSTA[1] = DR*SINLA*COSPHI + DE*COSLA - DN*SINPHI*SINLA;
        XCORSTA[2] = DR*SINPHI + DN*COSPHI ;

        return XCORSTA;

    } // End SolidTides::getStep1Sem()


    Triple SolidTides::getStep1L1( const Triple& XSTA,
                                   const Triple& XSUN,
                                   const Triple& XMON,
                                   double FAC2SUN,
                                   double FAC2MON )
    {
        Triple XCORSTA(0.0,0.0,0.0);

        double L1D = 0.0012,L1SD = 0.0024;

        // Compute the normalized position vector of the IGS station.
        double RSTA = std::sqrt(XSTA.dot(XSTA));
        double SINPHI = XSTA[2]/RSTA;
        double COSPHI = std::sqrt(XSTA[0]*XSTA[0]+XSTA[1]*XSTA[1])/RSTA;
        double SINLA = XSTA[1]/COSPHI/RSTA;
        double COSLA = XSTA[0]/COSPHI/RSTA;

        // Compute the normalized position vector of the Moon.
        double RMON=std::sqrt(XMON.dot(XMON));

        // Compute the normalized position vector of the Sun.
        double RSUN=std::sqrt(XSUN.dot(XSUN));

        // Compute the station corrections for the diurnal band.
        double L1 = L1D;
        double DNSUN = -L1*SINPHI*SINPHI*FAC2SUN*XSUN[2]*
                        (XSUN[0]*COSLA+XSUN[1]*SINLA)/RSUN/RSUN;

        double DNMON = -L1*SINPHI*SINPHI*FAC2MON*XMON[2]*
                        (XMON[0]*COSLA+XMON[1]*SINLA)/RMON/RMON;

        double DESUN = L1*SINPHI*(COSPHI*COSPHI-SINPHI*SINPHI)*
                        FAC2SUN*XSUN[2]*(XSUN[0]*SINLA-XSUN[1]*COSLA)/RSUN/RSUN;

        double DEMON = L1*SINPHI*(COSPHI*COSPHI-SINPHI*SINPHI)*
                        FAC2MON*XMON[2]*(XMON[0]*SINLA-XMON[1]*COSLA)/RMON/RMON;

        double DE = 3.0*(DESUN + DEMON);
        double DN = 3.0*(DNSUN + DNMON);

        XCORSTA[0] = -DE*SINLA - DN*SINPHI*COSLA;
        XCORSTA[1] =  DE*COSLA - DN*SINPHI*SINLA;
        XCORSTA[2] =  DN*COSPHI;

        // Compute the station corrections for the semi-diurnal band.
        L1 = L1SD;
        double COSTWOLA = COSLA*COSLA - SINLA*SINLA;
        double SINTWOLA = 2.0*COSLA*SINLA;

        DNSUN = -L1/2.0*SINPHI*COSPHI*FAC2SUN*((XSUN[0]*XSUN[0]-XSUN[1]*XSUN[1])*
                        COSTWOLA+2.0*XSUN[0]*XSUN[1]*SINTWOLA)/RSUN/RSUN;

        DNMON = -L1/2.0*SINPHI*COSPHI*FAC2MON*((XMON[0]*XMON[0]-XMON[1]*XMON[1])*
                        COSTWOLA+2.0*XMON[0]*XMON[1]*SINTWOLA)/RMON/RMON;

        DESUN = -L1/2.0*SINPHI*SINPHI*COSPHI*FAC2SUN*((XSUN[0]*XSUN[0]-XSUN[1]*XSUN[1])*
                        SINTWOLA-2.0*XSUN[0]*XSUN[1]*COSTWOLA)/RSUN/RSUN;

        DEMON = -L1/2.0*SINPHI*SINPHI*COSPHI*FAC2MON*((XMON[0]*XMON[0]-XMON[1]*XMON[1])*
                        SINTWOLA-2.0*XMON[0]*XMON[1]*COSTWOLA)/RMON/RMON;

        DE = 3.0*(DESUN + DEMON);
        DN = 3.0*(DNSUN + DNMON);

        XCORSTA[0] = XCORSTA[0] - DE*SINLA - DN*SINPHI*COSLA;
        XCORSTA[1] = XCORSTA[1] + DE*COSLA - DN*SINPHI*SINLA;
        XCORSTA[2] = XCORSTA[2] + DN*COSPHI ;

        return XCORSTA;

    } // End SolidTides::getStep1L1()


    Triple SolidTides::getStep2Diu( const Triple& XSTA, double FHR, double T )
    {

        Triple XCORSTA(0.0,0.0,0.0);

        // IERS 2010, TABLE 7.3a(extended)
        double DATDI[31][9] =
        {
            -3.0, 0.0, 2.0, 0.0, 0.0,-0.01, 0.0,  0.0,  0.0,
            -3.0, 2.0, 0.0, 0.0, 0.0,-0.01, 0.0,  0.0,  0.0,
            -2.0, 0.0, 1.0,-1.0, 0.0,-0.02, 0.0,  0.0,  0.0,
            -2.0, 0.0, 1.0, 0.0, 0.0,-0.08, 0.0, -0.01, 0.01,
            -2.0, 2.0,-1.0, 0.0, 0.0,-0.02, 0.0,  0.0,  0.0,
            -1.0, 0.0, 0.0,-1.0, 0.0,-0.10, 0.0,  0.0,  0.0,
            -1.0, 0.0, 0.0, 0.0, 0.0,-0.51, 0.0, -0.02, 0.03,
            -1.0, 2.0, 0.0, 0.0, 0.0, 0.01, 0.0,  0.0,  0.0,
             0.0,-2.0, 1.0, 0.0, 0.0, 0.01, 0.0,  0.0,  0.0,
             0.0, 0.0,-1.0, 0.0, 0.0, 0.02, 0.0,  0.0,  0.0,
             0.0, 0.0, 1.0, 0.0, 0.0, 0.06, 0.0,  0.0,  0.0,
             0.0, 0.0, 1.0, 1.0, 0.0, 0.01, 0.0,  0.0,  0.0,
             0.0, 2.0,-1.0, 0.0, 0.0, 0.01, 0.0,  0.0,  0.0,
             1.0,-3.0, 0.0, 0.0, 1.0,-0.06, 0.0,  0.0,  0.0,
             1.0,-2.0, 0.0,-1.0, 0.0, 0.01, 0.0,  0.0,  0.0,
             1.0,-2.0, 0.0, 0.0, 0.0,-1.23,-0.07, 0.06, 0.01,
             1.0,-1.0, 0.0, 0.0,-1.0, 0.02, 0.0,  0.0,  0.0,
             1.0,-1.0, 0.0, 0.0, 1.0, 0.04, 0.0,  0.0,  0.0,
             1.0, 0.0, 0.0,-1.0, 0.0,-0.22, 0.01, 0.01, 0.0,
             1.0, 0.0, 0.0, 0.0, 0.0,12.00,-0.80,-0.67,-0.03, // largest,K1
             1.0, 0.0, 0.0, 1.0, 0.0, 1.73,-0.12,-0.10, 0.0,
             1.0, 0.0, 0.0, 2.0, 0.0,-0.04, 0.0,  0.0,  0.0,
             1.0, 1.0, 0.0, 0.0,-1.0,-0.50,-0.01, 0.03, 0.0,
             1.0, 1.0, 0.0, 0.0, 1.0, 0.01, 0.0,  0.0,  0.0,
             0.0, 1.0, 0.0, 1.0,-1.0,-0.01, 0.0,  0.0,  0.0,
             1.0, 2.0,-2.0, 0.0, 0.0,-0.01, 0.0,  0.0,  0.0,
             1.0, 2.0, 0.0, 0.0, 0.0,-0.11, 0.01, 0.01, 0.0,
             2.0,-2.0, 1.0, 0.0, 0.0,-0.01, 0.0,  0.0,  0.0,
             2.0, 0.0,-1.0, 0.0, 0.0,-0.02, 0.0,  0.0,  0.0,
             3.0, 0.0, 0.0, 0.0, 0.0, 0.0,  0.0,  0.0,  0.0,
             3.0, 0.0, 0.0, 1.0, 0.0, 0.0,  0.0,  0.0,  0.0
        };

        // Compute the phase angles in degrees.
        double S = 218.31664563
                + (481267.88194
                + (-0.0014663889
                + (0.00000185139)*T)*T)*T;

        double TAU = FHR*15.0
                + 280.4606184
                + (36000.7700536
                + (0.00038793
                + (-0.0000000258)*T)*T)*T
                + (-S);

        double PR = (1.396971278
                + (0.000308889
                + (0.000000021
                + (0.000000007)*T)*T)*T)*T;

        S = S + PR;

        double H = 280.46645
                + (36000.7697489
                + (0.00030322222
                + (0.000000020
                + (-0.00000000654)*T)*T)*T)*T;

        double P = 83.35324312
                + (4069.01363525
                + (-0.01032172222
                + (-0.0000124991
                + (0.00000005263)*T)*T)*T)*T;

        double ZNS = 234.95544499
                + (1934.13626197
                + (-0.00207561111
                + (-0.00000213944
                + (0.00000001650)*T)*T)*T)*T;

        double PS = 282.93734098
                + (1.71945766667
                + (0.00045688889
                + (-0.00000001778
                + (-0.00000000334)*T)*T)*T)*T;

        // Reduce angles to between the range 0 and 360.
        S = std::fmod(S, 360.0);
        TAU = std::fmod(TAU, 360.0);
        H = std::fmod(H, 360.0);
        P = std::fmod(P, 360.0);
        ZNS = std::fmod(ZNS, 360.0);
        PS = std::fmod(PS, 360.0);

        double RSTA = std::sqrt(XSTA.dot(XSTA));
        double SINPHI = XSTA[2]/RSTA;
        double COSPHI = std::sqrt(XSTA[0]*XSTA[0]+XSTA[1]*XSTA[1])/RSTA;

        double COSLA = XSTA[0]/COSPHI/RSTA;
        double SINLA = XSTA[1]/COSPHI/RSTA;
        double ZLA = std::atan2(XSTA[1],XSTA[0]);

        // Convert from degrees to radians.
        for(int j=0; j<31; ++j)
        {
            double THETAF = (TAU+DATDI[j][0]*S+DATDI[j][1]*H+DATDI[j][2]*P+
                             DATDI[j][3]*ZNS+DATDI[j][4]*PS)*DEG_TO_RAD;

            double DR = DATDI[j][5]*2.0*SINPHI*COSPHI*std::sin(THETAF+ZLA)+
                        DATDI[j][6]*2.0*SINPHI*COSPHI*std::cos(THETAF+ZLA);

            double DN = DATDI[j][7]*(COSPHI*COSPHI-SINPHI*SINPHI)*std::sin(THETAF+ZLA)+
                        DATDI[j][8]*(COSPHI*COSPHI-SINPHI*SINPHI)*std::cos(THETAF+ZLA);

            double DE = DATDI[j][7]*SINPHI*std::cos(THETAF+ZLA)-
                        DATDI[j][8]*SINPHI*std::sin(THETAF+ZLA);

            XCORSTA[0] = XCORSTA[0]+DR*COSLA*COSPHI-DE*SINLA
                                   -DN*SINPHI*COSLA;
            XCORSTA[1] = XCORSTA[1]+DR*SINLA*COSPHI+DE*COSLA
                                   -DN*SINPHI*SINLA;
            XCORSTA[2] = XCORSTA[2]+DR*SINPHI+DN*COSPHI;
        }

        for(int i=0; i<3; ++i)
        {
            XCORSTA[i] = XCORSTA[i]/1000.0;
        }

        return XCORSTA;

    } // End SolidTides::getStep2Diu()


    Triple SolidTides::getStep2Lon( const Triple& XSTA, double T )
    {
        Triple XCORSTA(0.0,0.0,0.0);

        double DATDI[5][9] =
        {
            0, 0, 0, 1, 0,   0.47, 0.23, 0.16, 0.07,
            0, 2, 0, 0, 0,  -0.20,-0.12,-0.11,-0.05,
            1, 0,-1, 0, 0,  -0.11,-0.08,-0.09,-0.04,
            2, 0, 0, 0, 0,  -0.13,-0.11,-0.15,-0.07,
            2, 0, 0, 1, 0,  -0.05,-0.05,-0.06,-0.03
        };

        // Compute the phase angles in degrees.
        double S = 218.31664563
                + (481267.88194
                + (-0.0014663889
                + (0.00000185139)*T)*T)*T;

        double PR = (1.396971278
                + (0.000308889
                + (0.000000021
                + (0.000000007)*T)*T)*T)*T;

        S = S + PR;

        double H = 280.46645
                + (36000.7697489
                + (0.00030322222
                + (0.000000020
                + (-0.00000000654)*T)*T)*T)*T;

        double P = 83.35324312
                + (4069.01363525
                + (-0.01032172222
                + (-0.0000124991
                + (0.00000005263)*T)*T)*T)*T;

        double ZNS = 234.95544499
                + (1934.13626197
                + (-0.00207561111
                + (-0.00000213944
                + (0.00000001650)*T)*T)*T)*T;

        double PS = 282.93734098
                + (1.71945766667
                + (0.00045688889
                + (-0.00000001778
                + (-0.00000000334)*T)*T)*T)*T;

        double RSTA = std::sqrt(XSTA.dot(XSTA));
        double SINPHI = XSTA[2]/RSTA;
        double COSPHI = std::sqrt(XSTA[0]*XSTA[0]+XSTA[1]*XSTA[1])/RSTA;

        double COSLA = XSTA[0]/COSPHI/RSTA;
        double SINLA = XSTA[1]/COSPHI/RSTA;

        // Reduce angles to between the range 0 and 360.
        S =  std::fmod(S,360.0);
        H =  std::fmod(H,360.0);
        P =  std::fmod(P,360.0);
        ZNS = std::fmod(ZNS,360.0);
        PS = std::fmod(PS,360.0);

        double DR_TOT = 0.0;
        double DN_TOT = 0.0;

        for(int j=0; j<5; ++j)
        {
            double THETAF = (DATDI[j][0]*S+DATDI[j][1]*H+DATDI[j][2]*P+
                             DATDI[j][3]*ZNS+DATDI[j][4]*PS)*DEG_TO_RAD;

            double DR = DATDI[j][5]*(3.0*SINPHI*SINPHI-1.0)/2.0*std::cos(THETAF)+
                        DATDI[j][7]*(3.0*SINPHI*SINPHI-1.0)/2.0*std::sin(THETAF);

            double DN = DATDI[j][6]*(COSPHI*SINPHI*2.0)*std::cos(THETAF)+
                        DATDI[j][8]*(COSPHI*SINPHI*2.0)*std::sin(THETAF);

            double DE = 0.0;
            DR_TOT = DR_TOT + DR;
            DN_TOT = DN_TOT + DN;

            XCORSTA[0] = XCORSTA[0] + DR*COSLA*COSPHI - DE*SINLA
                                    - DN*SINPHI*COSLA ;
            XCORSTA[1] = XCORSTA[1] + DR*SINLA*COSPHI + DE*COSLA
                                    - DN*SINPHI*SINLA;
            XCORSTA[2] = XCORSTA[2] + DR*SINPHI + DN*COSPHI;
        }

        for(int i=0; i<3; ++i)
        {
            XCORSTA[i] = XCORSTA[i]/1000.0;
        }

        return XCORSTA;

    } // End SolidTides::getStep2Lon()


} // end namespace gpstk
