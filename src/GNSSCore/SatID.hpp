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

#ifndef GPSTK_SATID_HPP
#define GPSTK_SATID_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include "GNSSconstants.hpp"
#include "SatelliteSystem.hpp"

/**
 * @file SatID.hpp
 * gpstk::SatID - navigation system-independent representation of a satellite.
 */

namespace gpstk
{
      // forward declarations
   class SatID;
//   std::istream& operator<<(std::istream& s, SatID& p);

      /// @ingroup GNSSEph
      //@{

      /// Satellite identifier consisting of a satellite number (PRN, etc.)
      /// and a satellite system. For GLONASS (systemGlonass), the identifier
      /// is the slot number as per section 3.5 of the RINEX 3 spec.
      /// For SBAS (systemGeosync), the id is the PRN-100.
   class SatID
   {
   public:
         /// empty constructor, creates an invalid object
      SatID() { id=-1; system=SatelliteSystem::GPS; }

         /// explicit constructor, no defaults
         /// @note if s is given a default value here,
         /// some compilers will silently cast int to SatID.
      SatID(int p, SatelliteSystem s) { id=p; system=s; }

         /// Convenience output method.
      void dump(std::ostream& s) const
      {
         s << convertSatelliteSystemToString(system) << " " << id;
      }

      // operator=, copy constructor and destructor built by compiler
      void operator=(const SatID& right)
      {
          id = right.id;
          system = right.system;
      }

         /// operator == for SatID
      bool operator==(const SatID& right) const
      { return ((system == right.system) && (id == right.id)); }

         /// operator != for SatID
      bool operator!=(const SatID& right) const
      { return !(operator==(right)); }

         /// operator < for SatID : order by system, then number
      bool operator<(const SatID& right) const
      {
         if (system==right.system)
            return (id<right.id);
         return (system<right.system);
      }

         /// operator > for SatID
      bool operator>(const SatID& right) const
      {  return (!operator<(right) && !operator==(right)); }

         /// operator <= for SatID
      bool operator<=(const SatID& right) const
      { return (operator<(right) || operator==(right)); }

         /// operator >= for SatID
      bool operator>=(const SatID& right) const
      { return !(operator<(right)); }

         /// return true if this is a valid SatID
         /// @note assumes all id's are positive and less than 100;
         ///     plus GPS id's are less than or equal to MAX_PRN (32).
         /// @note this is not used internally in the gpstk library
      bool isValid() const;
   
      int id;                   ///< satellite identifier, e.g. PRN
      SatelliteSystem system;   ///< system for this satellite

   }; // class SatID

      /// stream output for SatID
   inline std::ostream& operator<<(std::ostream& s, const SatID& p)
   {
      p.dump(s);
      return s;
   }

      //@}

   namespace StringUtils
   {
         /// @ingroup StringUtils
         //@{

         /// SatID as a string
      inline std::string asString(const SatID& p)
      {
         std::ostringstream oss;
         p.dump(oss);
         return oss.str();
      }
   }

} // namespace gpstk

#endif
