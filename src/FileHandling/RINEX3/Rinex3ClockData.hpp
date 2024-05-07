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

/**
 * @file Rinex3ClockData.hpp
 * Encapsulate Rinex3Clock file data, including I/O
 */

#ifndef RINEX3CLOCKDATA_HPP
#define RINEX3CLOCKDATA_HPP

#include "RinexSatID.hpp"
#include "Rinex3ClockBase.hpp"
#include "CommonTime.hpp"
#include <iomanip>

namespace gpstk
{
      /// @ingroup FileHandling
      //@{

      /**
       * This class encapsulates data for satellite clocks as found in
       * RINEX Clock format files, and is used in conjuction with
       * class Rinex3ClockStream, which handles the I/O, and
       * Rinex3ClockHeader, which holds information from the
       * Rinex3Clock file header.
       *
       *
       * @sa gpstk::Rinex3ClockHeader and gpstk::Rinex3ClockStream for
       * more information.
       */
   class Rinex3ClockData : public Rinex3ClockBase
   {
   public:
         /// Constructor.
      Rinex3ClockData();
     
         /// Destructor
      virtual ~Rinex3ClockData() {}
     
         // The next four lines is our common interface
         /// Rinex3ClockData is "data" so this function always returns true.
      virtual bool isData() const {return true;}

         /// Debug output function.
      virtual void dump(std::ostream& s=std::cout) const throw();

      std::string datatype;   ///< Data type : AR, AS, etc
      RinexSatID sat;         ///< Satellite ID        (if AS)
      std::string site;       ///< Site label (4-char) (if AR)
      CommonTime time;        ///< Time of epoch for this record
      RCDouble bias;          ///< Clock bias in seconds
      RCDouble sig_bias;      ///< Clock bias sigma in seconds
      RCDouble drift;         ///< Clock drift in sec/sec
      RCDouble sig_drift;     ///< Clock drift sigma in sec/sec
      RCDouble accel;         ///< Clock acceleration in 1/sec
      RCDouble sig_accel;     ///< Clock acceleration sigma in 1/sec
      
   protected:

      void clear() throw()
      {
         datatype = std::string();
         sat = RinexSatID(-1,SatelliteSystem::GPS);
         time = CommonTime::BEGINNING_OF_TIME;
         bias = sig_bias = drift = sig_drift = accel = sig_accel = 0.0;
      }

         /** Writes the formatted record to the FFStream \a s.
          * @warning This function is currently unimplemented
          * @throw std::exception
          * @throw FFStreamError
          * @throw StringUtils::StringException
          */
      virtual void reallyPutRecord(FFStream& s) const;

         /**
          * This function reads a record from the given FFStream.
          * If an error is encountered in retrieving the record, the 
          * stream is reset to its original position and its fail-bit is set.
          * @throw std::exception
          * @throw StringException when a StringUtils function fails
          * @throw FFStreamError when exceptions(failbit) is set and
          *  a read or formatting error occurs.  This also resets the
          *  stream to its pre-read position.
          */
      virtual void reallyGetRecord(FFStream& s);
   };

      //@}

}  // namespace

#endif // RINEX3CLOCKDATA_HPP
