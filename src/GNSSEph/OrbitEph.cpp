//==============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/// @file OrbElem.cpp Encapsulates the "least common denominator" orbit parameters
/// that determine a satellite ephemeris, that is, clock model, Kepler orbit elements
/// plus harmonic perturbations with time of ephemeris, satellite ID, and begin and
/// end times of validity.
/// Although it can also be used alone, this class is most often to be used as a base
/// class for a fuller implementation of the ephemeris and clock, by adding health
/// and accuracy information, fit interval, ionospheric correction terms and data
/// flags. It serves as the base class for broadcast ephemerides for GPS, QZSS,
/// Galileo, and BeiDou, with RINEX Navigation input, among others.

#include "OrbitEph.hpp"
#include "MathBase.hpp"
#include "GNSSconstants.hpp"
#include "CivilTime.hpp"
#include "GPSWeekSecond.hpp"
#include "GALWeekSecond.hpp"
#include "BDSWeekSecond.hpp"
#include "QZSWeekSecond.hpp"
#include "GPSEllipsoid.hpp"
#include "CGCS2000Ellipsoid.hpp"
#include "TimeString.hpp"

using namespace std;

namespace gpstk
{

   OrbitEph :: OrbitEph()
         : dataLoadedFlag(false),
           af0(0.0), af1(0.0), af2(0.0), M0(0.0), dn(0.0), ecc(0.0), A(0.0),
           OMEGA0(0.0), i0(0.0), w(0.0), OMEGAdot(0.0), idot(0.0),
           dndot(0.0), Adot(0.0),
           Cuc(0.0), Cus(0.0), Crc(0.0), Crs(0.0), Cic(0.0), Cis(0.0),
           beginValid(CommonTime::END_OF_TIME),
           endValid(CommonTime::BEGINNING_OF_TIME)
   { }

   // Returns true if the time, ct, is within the period of validity of
   // this OrbitEph object.
   // throw Invalid Request if the required data has not been stored.
   bool OrbitEph::isValid(const CommonTime& ct) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));
      if(ct < beginValid || ct > endValid) return false;
      return true;
   }

   // Compute the satellite clock bias (seconds) at the given time
   // throw Invalid Request if the required data has not been stored.
   double OrbitEph::svClockBias(const CommonTime& t) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      double dtc, elaptc;
      elaptc = t - ctToc;
      dtc = af0 + elaptc * (af1 + elaptc * af2);
      return dtc;
   }

   // Compute the satellite clock drift (sec/sec) at the given time
   // throw Invalid Request if the required data has not been stored.
   double OrbitEph::svClockDrift(const CommonTime& t) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      double drift, elaptc;
      elaptc = t - ctToc;
      drift = af1 + elaptc * af2;
      return drift;
   }

   // Compute satellite position at the given time.
   // throw Invalid Request if the required data has not been stored.
   Xvt OrbitEph::svXvt(const CommonTime& t) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      Xvt sv;
      double ea;              // eccentric anomaly
      double delea;           // delta eccentric anomaly during iteration
      double elapte;          // elapsed time since Toe
      //double elaptc;          // elapsed time since Toc
      //double dtc,dtr;
      double q,sinea,cosea;
      double GSTA,GCTA;
      double amm;
      double meana;           // mean anomaly
      double F,G;             // temporary real variables
      double alat,talat,c2al,s2al,du,dr,di,U,R,truea,AINC;
      double ANLON,cosu,sinu,xip,yip,can,san,cinc,sinc;
      double xef,yef,zef,dek,dlk,div,domk,duv,drv;
      double dxp,dyp,vxef,vyef,vzef;

      GPSEllipsoid ell;
      double sqrtgm = SQRT(ell.gm(satID));
      double twoPI = 2.0e0 * PI;
      double lecc;            // eccentricity
      double tdrinc;          // dt inclination
      double Ahalf = SQRT(A); // A is semi-major axis of orbit
      double ToeSOW = GPSWeekSecond(ctToe).sow;    // SOW is time-system-independent

      lecc = ecc;
      tdrinc = idot;

      // Compute time since ephemeris & clock epochs
      elapte = t - ctToe;

      // Compute A at time of interest (LNAV: Adot==0)
      double Ak = A + Adot * elapte;

      // Compute mean motion (LNAV: dndot==0)
      double dnA = dn + 0.5*dndot*elapte;
      amm  = (sqrtgm / (A*Ahalf)) + dnA;     // Eqn specifies A0, not Ak

      // In-plane angles
      //     meana - Mean anomaly
      //     ea    - Eccentric anomaly
      //     truea - True anomaly
      meana = M0 + elapte * amm;
      meana = fmod(meana, twoPI);
      ea = meana + lecc * ::sin(meana);

      int loop_cnt = 1;
      do  {
         F = meana - (ea - lecc * ::sin(ea));
         G = 1.0 - lecc * ::cos(ea);
         delea = F/G;
         ea = ea + delea;
         loop_cnt++;
      } while ((fabs(delea) > 1.0e-11) && (loop_cnt <= 20));

      // Compute clock corrections
      sv.relcorr = svRelativity(t);
      sv.clkbias = svClockBias(t);
      sv.clkdrift = svClockDrift(t);
      sv.frame = ReferenceFrame::WGS84;

      // Compute true anomaly
      q     = SQRT(1.0e0 - lecc*lecc);
      sinea = ::sin(ea);
      cosea = ::cos(ea);
      G     = 1.0e0 - lecc * cosea;

      //  G*SIN(TA) AND G*COS(TA)
      GSTA  = q * sinea;
      GCTA  = cosea - lecc;

      //  True anomaly
      truea = atan2 (GSTA, GCTA);

      // Argument of lat and correction terms (2nd harmonic)
      alat  = truea + w;
      talat = 2.0e0 * alat;
      c2al  = ::cos(talat);
      s2al  = ::sin(talat);

      du  = c2al * Cuc +  s2al * Cus;
      dr  = c2al * Crc +  s2al * Crs;
      di  = c2al * Cic +  s2al * Cis;

      // U = updated argument of lat, R = radius, AINC = inclination
      U    = alat + du;
      R    = Ak*G  + dr;
      AINC = i0 + tdrinc * elapte  +  di;

      //  Longitude of ascending node (ANLON)
      ANLON = OMEGA0 + (OMEGAdot - ell.angVelocity()) *
              elapte - ell.angVelocity() * ToeSOW;

      // In plane location
      cosu = ::cos(U);
      sinu = ::sin(U);
      xip  = R * cosu;
      yip  = R * sinu;

      //  Angles for rotation to earth fixed
      can  = ::cos(ANLON);
      san  = ::sin(ANLON);
      cinc = ::cos(AINC);
      sinc = ::sin(AINC);

      // Earth fixed coordinates in meters
      xef  =  xip*can  -  yip*cinc*san;
      yef  =  xip*san  +  yip*cinc*can;
      zef  =              yip*sinc;
      sv.x[0] = xef;
      sv.x[1] = yef;
      sv.x[2] = zef;

      // Compute velocity of rotation coordinates
      dek = amm / G;
      dlk = amm * q / (G*G);
      div = tdrinc - 2.0e0 * dlk * (Cic  * s2al - Cis * c2al);
      domk = OMEGAdot - ell.angVelocity();
      duv = dlk*(1.e0+ 2.e0 * (Cus*c2al - Cuc*s2al));
      drv = Ak * lecc * dek * sinea - 2.e0 * dlk * (Crc * s2al - Crs * c2al) +
         Adot * G;
      dxp = drv*cosu - R*sinu*duv;
      dyp = drv*sinu + R*cosu*duv;

      // Calculate velocities
      vxef = dxp*can - xip*san*domk - dyp*cinc*san
               + yip*(sinc*san*div - cinc*can*domk);
      vyef = dxp*san + xip*can*domk + dyp*cinc*can
               - yip*(sinc*can*div + cinc*san*domk);
      vzef = dyp*sinc + yip*cinc*div;

      // Move results into output variables
      sv.v[0] = vxef;
      sv.v[1] = vyef;
      sv.v[2] = vzef;

      return sv;
   }

   // Compute satellite relativity correction (sec) at the given time
   // throw Invalid Request if the required data has not been stored.
   double OrbitEph::svRelativity(const CommonTime& t) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      GPSEllipsoid ell;

      double twoPI  = 2.0 * PI;
      double sqrtgm = SQRT(ell.gm());
      double elapte = t - ctToe;

      // Compute A at time of interest
      double Ak = A + Adot*elapte;                 // LNAV: Adot==0
      double Ahalf = SQRT(A);
      double amm = (sqrtgm / (A*Ahalf)) + dn;      // Eqn specifies A0 not Ak
      double meana,F,G,delea;

      meana = M0 + elapte * amm;
      meana = fmod(meana, twoPI);
      double ea = meana + ecc * ::sin(meana);

      int loop_cnt = 1;
      do {
         F     = meana - (ea - ecc * ::sin(ea));
         G     = 1.0 - ecc * ::cos(ea);
         delea = F/G;
         ea    = ea + delea;
         loop_cnt++;
      } while ((ABS(delea) > 1.0e-11) && (loop_cnt <= 20));

      return (REL_CONST * ecc * SQRT(Ak) * ::sin(ea));
   }

   // Dump the overhead information as a string containing a single line.
   // @throw Invalid Request if the required data has not been stored.
   string OrbitEph::asString(void) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      try {
         ostringstream os;
         string sys;
         switch(satID.system) {
            case SatelliteSystem::GPS: sys = "G"; break;
            case SatelliteSystem::Galileo: sys = "E"; break;
            case SatelliteSystem::BeiDou: sys = "C"; break;
            case SatelliteSystem::QZSS: sys = "J"; break;
            default:
               os << "EPH Error - invalid satellite system "
                  << convertSatelliteSystemToString(satID.system) << endl;
               return os.str();
         }

         CivilTime ct;
         os << "EPH " << sys << setfill('0') << setw(2) << satID.id << setfill(' ');
         ct = CivilTime(beginValid);
         os << printTime(ct," | %4Y %3j %02H:%02M:%02S |");
         ct = CivilTime(ctToe);
         os << printTime(ct," %3j %02H:%02M:%02S |");
         ct = CivilTime(ctToc);
         os << printTime(ct," %3j %02H:%02M:%02S |");
         ct = CivilTime(endValid);
         os << printTime(ct," %3j %02H:%02M:%02S |");

         return os.str();
      }
      catch(Exception& e) { GPSTK_RETHROW(e);
      }
   }

   // Utility routine for dump(); override if GPSWeekSecond is not right
   string OrbitEph::timeDisplay(const CommonTime& t, bool showHead) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      try {
         if(showHead) return string("Week( mod)     SOW     DOW   UTD     SOD"
                                    "   MM/DD/YYYY   HH:MM:SS SYS");

         ostringstream os;
         WeekSecond *ptr;
         if(     t.getTimeSystem() == TimeSystem::GAL)
            ptr = new GALWeekSecond(t);
         else if(t.getTimeSystem() == TimeSystem::BDT)
            ptr = new BDSWeekSecond(t);
         else if(t.getTimeSystem() == TimeSystem::QZS)
            ptr = new QZSWeekSecond(t);
         else 
            ptr = new GPSWeekSecond(t);

         os << setw(4) << ptr->week << "(";
         os << setw(4) << (ptr->week & ptr->bitmask()) << ")  ";
         os << setw(6) << setfill(' ') << ptr->sow << "   ";

         switch (ptr->getDayOfWeek())
         {
            case 0: os << "Sun-0"; break;
            case 1: os << "Mon-1"; break;
            case 2: os << "Tue-2"; break;
            case 3: os << "Wed-3"; break;
            case 4: os << "Thu-4"; break;
            case 5: os << "Fri-5"; break;
            case 6: os << "Sat-6"; break;
            default: break;
         }
         
         delete ptr;         

         os << printTime(t,"   %3j   %5.0s   %02m/%02d/%04Y   %02H:%02M:%02S %P");

         return os.str();
      }
      catch(Exception& e) { GPSTK_RETHROW(e);
      }
   }

   // Dump the overhead information to the given output stream.
   // throw Invalid Request if the required data has not been stored.
   void OrbitEph::dumpHeader(ostream& os) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      os << "****************************************************************"
        << "************" << endl
        << "Broadcast Orbit Ephemeris of class " << getName() << endl;
      os << "Satellite: " << convertSatelliteSystemToString(satID.system)
         << " " << setfill('0') << setw(2) << satID.id << setfill(' ') << endl;
   }

   // Dump the orbit, etc information to the given output stream.
   // throw Invalid Request if the required data has not been stored.
   void OrbitEph::dumpBody(ostream& os) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));

      os << "           TIMES OF INTEREST" << endl;
      os << "              " << timeDisplay(beginValid,true) << endl;
      os << "Begin Valid:  " << timeDisplay(beginValid) << endl;
      os << "Clock Epoch:  " << timeDisplay(ctToc) << endl;
      os << "Eph Epoch:    " << timeDisplay(ctToe) << endl;
      os << "End Valid:    " << timeDisplay(endValid) << endl;

      os << scientific << setprecision(8)
         << "           CLOCK PARAMETERS\n"
         << "Bias T0:     " << setw(16) << af0 << " sec" << endl
         << "Drift:       " << setw(16) << af1 << " sec/sec" << endl
         << "Drift rate:  " << setw(16) << af2 << " sec/(sec**2)" << endl;

      os << "           ORBIT PARAMETERS\n"
         << "Semi-major axis:       " << setw(16) <<  A  << " m" << endl
         << "Motion correction:     " << setw(16) <<  dn << " rad/sec" << endl
         << "Eccentricity:          " << setw(16) << ecc << endl
         << "Arg of perigee:        " << setw(16) << w << " rad" << endl
         << "Mean anomaly at epoch: " << setw(16) << M0 << " rad" << endl
         << "Right ascension:       " << setw(16) << OMEGA0 << " rad    "
         << setw(16) << OMEGAdot << " rad/sec" << endl
         << "Inclination:           " << setw(16) << i0 << " rad    "
         << setw(16) << idot << " rad/sec" << endl;

      os << "           HARMONIC CORRECTIONS\n"
         << "Radial        Sine: " << setw(16) << Crs << " m    Cosine: "
         << setw(16) << Crc << " m" << endl
         << "Inclination   Sine: " << setw(16) << Cis << " rad  Cosine: "
         << setw(16) << Cic << " rad" << endl
         << "In-track      Sine: " << setw(16) << Cus << " rad  Cosine: "
         << setw(16) << Cuc << " rad" << endl;
   }

   // Output object to stream
   ostream& operator<<(ostream& os, const OrbitEph& eph)
   {
      eph.dump(os);
      return os;
   }

}  // end namespace
