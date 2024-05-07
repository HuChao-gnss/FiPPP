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

/// @file Rinex3EphemerisStore.cpp
/// Read and store RINEX formated navigation message (Rinex3Nav) data, following
/// the RINEX 3.02 spec. Support for GNSS GPS, GAL, GLO, BDS, QZS.

#include "Rinex3EphemerisStore.hpp"

#include "Rinex3NavStream.hpp"
#include "Rinex3NavData.hpp"
#include "GPSEphemeris.hpp"
#include "GalEphemeris.hpp"
#include "BDSEphemeris.hpp"
#include "QZSEphemeris.hpp"

using namespace std;

namespace gpstk
{
   // add a Rinex3NavData to the store
   // @param Rinex3NavData Rdata data to be added
   // @return true if data was added, false otherwise
   bool Rinex3EphemerisStore::addEphemeris(const Rinex3NavData& inRdata)
   {
      Rinex3NavData Rdata(inRdata);

      switch(Rdata.sat.system) {
         case SatelliteSystem::GPS:
         {
            Rdata.time = correctTimeSystem(Rdata.time, TimeSystem::GPS);
            gpsStore.addEphemeris(Rdata);
            //return ORBstore.addEphemeris(dynamic_cast<OrbitEph*>(&eph));
            break;
         }

         case SatelliteSystem::Galileo:
         {
            Rdata.time = correctTimeSystem(Rdata.time, TimeSystem::GAL);
            galStore.addEphemeris(Rdata);
            //return ORBstore.addEphemeris(dynamic_cast<OrbitEph*>(&eph));
            break;
         }

         case SatelliteSystem::BeiDou:
         {
            Rdata.time = correctTimeSystem(Rdata.time, TimeSystem::BDT);
            bdsStore.addEphemeris(Rdata);
            //BDSEphemeris eph(Rdata);
            //return ORBstore.addEphemeris(dynamic_cast<OrbitEph*>(&eph));
            break;
         }

         case SatelliteSystem::QZSS:
         {
            Rdata.time = correctTimeSystem(Rdata.time, TimeSystem::QZS);
            QZSEphemeris eph(Rdata);
            return ORBstore.addEphemeris(dynamic_cast<OrbitEph*>(&eph));
            break;
         }

         case SatelliteSystem::Glonass:
            Rdata.time = correctTimeSystem(Rdata.time, TimeSystem::GLO);
            return GLOstore.addEphemeris(GloEphemeris(Rdata));
            break;

         case SatelliteSystem::Geosync:
            //return GEOstore.addEphemeris(GeoEphemeris(Rdata));
            break;

         default:
            break;
      }
      return false;
   }

   // load the given Rinex navigation file
   // return -1 failed to open file,
   //        -2 failed to read header (this->Rhead),
   //        -3 failed to read data (this->Rdata),
   //       >=0 number of nav records read
   int Rinex3EphemerisStore::loadFile(const string& filename, bool dump, ostream& s)
   {
      try {
         int nread(0);
         Rinex3NavStream strm;
         what = string();
         
         strm.open(filename.c_str(), ios::in);
         if(!strm.is_open()) {
            what = string("File ") + filename + string(" could not be opened.");
            return -1;
         }
         strm.exceptions(ios::failbit);

         try { 
             strm >> Rhead;
         }
         catch(Exception& e) {
            what = string("Failed to read header of file ") + filename
               + string(" : ") + e.getText();
            return -2;
         }
         if(dump)
             Rhead.dump(s);

         // add to FileStore
         NavFiles.addFile(filename, Rhead);

         // add to mapTimeCorr
         if(Rhead.mapTimeCorr.size() > 0) {
            map<string, TimeSystemCorrection>::const_iterator it;
            for(it=Rhead.mapTimeCorr.begin(); it!=Rhead.mapTimeCorr.end(); ++it)
               addTimeCorr(it->second);
         }

         while(1) {
            // read the record
            try { 
                strm >> Rdata;
            }
            catch(Exception& e) {
               what = string("Failed to read data in file ") + filename
                  + string(" : ") + e.getText();
               return -3;
            }
            catch(std::exception& e) {
               what = string("std excep: ") + e.what();
               return -3;
            }
            catch(...) {
               what = string("Unknown exception while reading data of file ")
                  + filename;
               return -3;
            }

            if(!strm.good() || strm.eof()) 
                break;

            nread++;
            if(dump) Rdata.dump(s);

            try {
               addEphemeris(Rdata);
            }
            catch(Exception& e) {
               cout << "addEphemeris caught excp " << e.what();
               GPSTK_RETHROW(e);
            }
         }

         return nread;
      }
      catch(Exception& e) {
         GPSTK_RETHROW(e);
      }

   } // end Rinex3EphemerisStore::loadFile

   // Find the appropriate time system correction object in the collection for the
   // given time systems, and dump it to a string and return that string.
   string Rinex3EphemerisStore::dumpTimeSystemCorrection(
      const TimeSystem fromSys, const TimeSystem toSys) const
   {
      string msg;
      ostringstream oss;
      oss << "Convert from " << gpstk::StringUtils::asString(fromSys)
         << " to " << gpstk::StringUtils::asString(toSys) << " : ";

      if(toSys == fromSys) {
         oss << "time systems are the same";
         return oss.str();
      }

      // look up the TimeSystemCorr in list, and dump it
      map<string, TimeSystemCorrection>::const_iterator it;
      for(it = mapTimeCorr.begin(); it != mapTimeCorr.end(); ++it) {
         if(it->second.isConverterFor(fromSys, toSys)) {
            it->second.dump(oss);
            return oss.str();
         }
      }
      oss << "conversion not found!";
      return oss.str();
   }

   // Utility routine for getXvt and addEphemeris to convert time systems.
   // Convert ttag to the target time system, using the first appropriate correction
   // in the list, and return it. If no correction is found, ttag is unchanged and
   // an exception is thrown.
   CommonTime Rinex3EphemerisStore::correctTimeSystem(const CommonTime ttag,
                                                      const TimeSystem targetSys)
      const
   {
      CommonTime toReturn(ttag);
      TimeSystem fromSys(ttag.getTimeSystem());

      // is a conversion necessary?
      if(fromSys == targetSys)
         return toReturn;

      // first correct for leap seconds
      const CivilTime civt(ttag);
      double dt = gpstk::getTimeSystemCorrection(fromSys, targetSys,
                              civt.year, civt.month, civt.day);
      toReturn += dt;
      // the corrected timetag: now only the system, not the value, matters
      toReturn.setTimeSystem(targetSys);

      // look up the TimeSystemCorr in list, and do the conversion
      map<string, TimeSystemCorrection>::const_iterator it;
      for(it = mapTimeCorr.begin(); it != mapTimeCorr.end(); ++it) {
         if(it->second.isConverterFor(fromSys, targetSys)) {
            dt = it->second.Correction(ttag);
            toReturn += dt;
            return toReturn;
         }
      }

      return toReturn;      // never reached, satisfy some compilers
   }

   // Returns the position, velocity, and clock offset of the indicated
   // object in ECEF coordinates (meters) at the indicated time.
   // @param[in] sat the satellite of interest
   // @param[in] ttag the time to look up
   // @return the Xvt of the object at the indicated time
   // @throw InvalidRequest If the request can not be completed for any
   //    reason, this is thrown. The text may have additional
   //    information as to why the request failed.
   Xvt Rinex3EphemerisStore::getXvt(const SatID& sat, const CommonTime& inttag) const
   {
      try {
         Xvt xvt;
         CommonTime ttag;
         TimeSystem ts;

         switch(sat.system) {
            case SatelliteSystem::GPS:
                ts = TimeSystem::GPS;
                ttag = correctTimeSystem(inttag, ts);
                xvt = gpsStore.getXvt(sat, ttag);
                break;
            case SatelliteSystem::Galileo:
                ts = TimeSystem::GAL;
                ttag = correctTimeSystem(inttag, ts);
                xvt = galStore.getXvt(sat, ttag);
                break;
            case SatelliteSystem::BeiDou:
                ts = TimeSystem::BDT;
                ttag = correctTimeSystem(inttag, ts);
                xvt = bdsStore.getXvt(sat, ttag);
                break;
            case SatelliteSystem::QZSS:
               ts = TimeSystem::QZS;
               ttag = correctTimeSystem(inttag, ts);
               xvt = ORBstore.getXvt(sat,ttag);
               break;
            case SatelliteSystem::Glonass:
               ttag = correctTimeSystem(inttag, TimeSystem::GLO);
               xvt = GLOstore.getXvt(sat,ttag);
               break;
            default:
               InvalidRequest e("Unsupported satellite system");
               GPSTK_THROW(e);
               break;
         }

         return xvt;
      }
      catch(InvalidRequest& ir) { GPSTK_RETHROW(ir); }
   }


   Xvt Rinex3EphemerisStore ::
   computeXvt(const SatID& sat, const CommonTime& inttag) const throw()
   {
      Xvt rv;
      rv.health = Xvt::HealthStatus::Unavailable;
      try
      {
         CommonTime ttag;
         TimeSystem ts;

         switch(sat.system)
         {
            case SatelliteSystem::GPS:
            case SatelliteSystem::Galileo:
            case SatelliteSystem::BeiDou:
            case SatelliteSystem::QZSS:
               if(sat.system == SatelliteSystem::GPS    ) ts = TimeSystem::GPS;
               if(sat.system == SatelliteSystem::Galileo) ts = TimeSystem::GAL;
               if(sat.system == SatelliteSystem::BeiDou ) ts = TimeSystem::BDT;
               if(sat.system == SatelliteSystem::QZSS   ) ts = TimeSystem::QZS;
               ttag = correctTimeSystem(inttag, ts);
               return ORBstore.computeXvt(sat,ttag);
               break;
            case SatelliteSystem::Glonass:
               ttag = correctTimeSystem(inttag, TimeSystem::GLO);
               return GLOstore.computeXvt(sat,ttag);
               break;
            default:
               return rv;
               break;
         }
         return rv;
      }
      catch(...)
      {
      }
      return rv;
   }


   Xvt::HealthStatus Rinex3EphemerisStore ::
   getSVHealth(const SatID& sat, const CommonTime& inttag) const throw()
   {
      Xvt::HealthStatus rv = Xvt::HealthStatus::Unavailable;
      try
      {
         CommonTime ttag;
         TimeSystem ts;

         switch(sat.system)
         {
            case SatelliteSystem::GPS:
            case SatelliteSystem::Galileo:
            case SatelliteSystem::BeiDou:
            case SatelliteSystem::QZSS:
               if(sat.system == SatelliteSystem::GPS    ) ts = TimeSystem::GPS;
               if(sat.system == SatelliteSystem::Galileo) ts = TimeSystem::GAL;
               if(sat.system == SatelliteSystem::BeiDou ) ts = TimeSystem::BDT;
               if(sat.system == SatelliteSystem::QZSS   ) ts = TimeSystem::QZS;
               ttag = correctTimeSystem(inttag, ts);
               return ORBstore.getSVHealth(sat,ttag);
               break;
            case SatelliteSystem::Glonass:
               ttag = correctTimeSystem(inttag, TimeSystem::GLO);
               return GLOstore.getSVHealth(sat,ttag);
               break;
            default:
               return rv;
               break;
         }
         return rv;
      }
      catch(...)
      {
      }
      return rv;
   }

   // Dump information about the store to an ostream.
   // @param[in] os ostream to receive the output; defaults to cout
   // @param[in] detail integer level of detail to provide; allowed values are
   //    0: number of satellites, time step and time limits (default)
   //    1: above plus flags, gap and interval values, and number of data/sat
   //    2: above plus all the data tables
   void Rinex3EphemerisStore::dump(ostream& os, short detail) const
   {
      os << "Dump of Rinex3EphemerisStore:\n";
      // dump the time system corrections
      map<string,TimeSystemCorrection>::const_iterator tcit;
      for(tcit=mapTimeCorr.begin(); tcit != mapTimeCorr.end(); ++tcit) {
         tcit->second.dump(os);
         os << "\n";
      }

      NavFiles.dump(os, detail);

      if(ORBstore.size()) {
         os << "Dump of GPS/GAL/BDS/QZS ephemeris store:\n";
         ORBstore.dump(os, detail);
      }

      if(GLOstore.size()) {
         os << "Dump of GLO ephemeris store:\n";
         GLOstore.dump(os, detail);
      }


      os << "End dump of Rinex3EphemerisStore\n";
   }

   // Determine the earliest time for which this object can successfully 
   // determine the Xvt for any object.
   // @return the earliest time in the table
   // @throw InvalidRequest if the object has no data.
   CommonTime Rinex3EphemerisStore::getInitialTime(void) const
   {
      try {
         CommonTime retTime(CommonTime::END_OF_TIME),time;

         // CommonTime does not allow comparisions unless TimeSystems agree,
         // or if one is "Any"
         retTime.setTimeSystem(TimeSystem::Any);
         
         if(ORBstore.size()) {
            time = ORBstore.getInitialTime();
            if(time < retTime) {
               retTime = time;
               retTime.setTimeSystem(TimeSystem::Any);
            }
         }
         if(GLOstore.size()) {
            time = GLOstore.getInitialTime();
            if(time < retTime) {
               retTime = time;
               retTime.setTimeSystem(TimeSystem::Any);
            }
         }

         return retTime;
      }
      catch(InvalidRequest& ir) { GPSTK_RETHROW(ir); }
   }

   // Determine the latest time for which this object can successfully 
   // determine the Xvt for any object.
   // @return the latest time in the table
   // @throw InvalidRequest if the object has no data.
   CommonTime Rinex3EphemerisStore::getFinalTime(void) const
   {
      try {
         CommonTime retTime(CommonTime::BEGINNING_OF_TIME),time;

         // CommonTime does not allow comparisions unless TimeSystems agree,
         // or if one is "Any"
         retTime.setTimeSystem(TimeSystem::Any);
         
         if(ORBstore.size()) {
            time = ORBstore.getFinalTime();
            if(time > retTime) {
               retTime = time;
               retTime.setTimeSystem(TimeSystem::Any);
            }
         }
         if(GLOstore.size()) {
            time = GLOstore.getFinalTime();
            if(time > retTime) {
               retTime = time;
            retTime.setTimeSystem(TimeSystem::Any);
            }
         }

         return retTime;
      }
      catch(InvalidRequest& ir) { GPSTK_RETHROW(ir); }
   }

   // Determine the earliest time for which this object can successfully 
   // determine the Xvt for any object.
   // @param SatID sat satellite, or system if sat.id==-1
   // @return the earliest time in the table
   // @throw InvalidRequest if the object has no data.
   CommonTime Rinex3EphemerisStore::getInitialTime(const SatID& sat) const
   {
      try {
         if(sat.system == SatelliteSystem::Mixed)
            return getInitialTime();

         CommonTime retTime(CommonTime::END_OF_TIME),time;
         retTime.setTimeSystem(TimeSystem::Any);

         switch(sat.system) {
            case SatelliteSystem::GPS:
            case SatelliteSystem::Galileo:
            case SatelliteSystem::BeiDou:
            case SatelliteSystem::QZSS:
               retTime = ORBstore.getInitialTime(sat);
            case SatelliteSystem::Glonass:
               retTime = GLOstore.getInitialTime(sat);
            default:
               break;
         }

         return retTime;
      }
      catch(InvalidRequest& ir) { GPSTK_RETHROW(ir); }
   }

   // Determine the latest time for which this object can successfully 
   // determine the Xvt for any object.
   // @param SatID sat satellite, or system if sat.id==-1
   // @return the latest time in the table
   // @throw InvalidRequest if the object has no data.
   CommonTime Rinex3EphemerisStore::getFinalTime(const SatID& sat) const
   {
      try {
         if(sat.system == SatelliteSystem::Mixed)
            return getFinalTime();

         CommonTime retTime(CommonTime::BEGINNING_OF_TIME);
         retTime.setTimeSystem(TimeSystem::Any);

         switch(sat.system) {
            case SatelliteSystem::GPS:
            case SatelliteSystem::Galileo:
            case SatelliteSystem::BeiDou:
            case SatelliteSystem::QZSS:
               retTime = ORBstore.getFinalTime(sat);
            case SatelliteSystem::Glonass:
               retTime = GLOstore.getFinalTime(sat);
            default:
               break;
         }


         return retTime;
      }
      catch(InvalidRequest& ir) { GPSTK_RETHROW(ir); }
   }

   // use to access the data records in the store in bulk
   int Rinex3EphemerisStore::addToList(list<Rinex3NavData>& theList, SatID sysSat)
      const
   {
      int n(0);

      // pure fussiness
      const bool keepAll(sysSat.system == SatelliteSystem::Mixed);
      const bool keepGPS(keepAll || sysSat.system==SatelliteSystem::GPS);
      const bool keepGAL(keepAll || sysSat.system==SatelliteSystem::Galileo);
      const bool keepGLO(keepAll || sysSat.system==SatelliteSystem::Glonass);
      const bool keepBDS(keepAll || sysSat.system==SatelliteSystem::BeiDou);
      const bool keepQZS(keepAll || sysSat.system==SatelliteSystem::QZSS);
      const bool keepOrb(keepAll || keepGPS || keepGAL || keepBDS || keepQZS);

      if(keepOrb) {
         list<OrbitEph*> OElist;
         ORBstore.addToList(OElist);

         list<OrbitEph*>::const_iterator it;
         for(it=OElist.begin(); it != OElist.end(); ++it) {
            OrbitEph *ptr = *it;
            if((ptr->satID).system == SatelliteSystem::GPS && keepGPS) {
               GPSEphemeris *sysptr = dynamic_cast<GPSEphemeris*>(ptr);
               theList.push_back(Rinex3NavData(*sysptr));
               n++;
            }
            else if((ptr->satID).system == SatelliteSystem::Galileo && keepGAL) {
               GalEphemeris *sysptr = dynamic_cast<GalEphemeris*>(ptr);
               theList.push_back(Rinex3NavData(*sysptr));
               n++;
            }
            else if((ptr->satID).system == SatelliteSystem::BeiDou && keepBDS) {
               BDSEphemeris *sysptr = dynamic_cast<BDSEphemeris*>(ptr);
               theList.push_back(Rinex3NavData(*sysptr));
               n++;
            }
            else if((ptr->satID).system == SatelliteSystem::QZSS && keepQZS) {
               QZSEphemeris *sysptr = dynamic_cast<QZSEphemeris*>(ptr);
               theList.push_back(Rinex3NavData(*sysptr));
               n++;
            }
            delete ptr;
         }
      }
      if(keepGLO) {
         list<GloEphemeris> GLOlist;
         n += GLOstore.addToList(GLOlist);

         list<GloEphemeris>::const_iterator it;
         for(it=GLOlist.begin(); it != GLOlist.end(); ++it)
            theList.push_back(Rinex3NavData(*it));
      }
      return n;
   }

}  // namespace gpstk
