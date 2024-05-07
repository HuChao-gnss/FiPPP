#pragma ident "$Id$"

/**
 * @file SourceID.hpp
 * gpstk::SourceID - Simple index to represent the source of data.
 */

#ifndef GPSTK_SOURCEID_HPP
#define GPSTK_SOURCEID_HPP

//============================================================================
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2006, 2007, 2008
//
//============================================================================


#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>

using namespace std;

namespace gpstk
{

      /** @addtogroup DataStructures */

   class SourceID
   {
   public:

         /// The type of source.
      enum SourceType
      {
         GPS,        ///< GPS data
         GLONASS,    ///< GLONASS data
         Galileo,    ///< Galileo data
         SBAS,       ///< SBAS data
         QZSS,       ///< QZSS data
         BDS,        ///< BDS data
         IRNSS,      ///< IRNSS data
         Mixed,      ///< Mixed data
         UserDefined,///< UserDefined
         DGPS,       ///< Differential GPS data
         RTK,        ///< Real Time Kinematic data
         INS,        ///< Inertial System data
         Unknown
      };


         /// empty constructor, creates an unknown source data object
      SourceID()
         : type(Unknown), sourceName(""), sourceNumber("")
      {};

         /// Explicit constructor
      SourceID( std::string st,
                std::string name )
         : sourceName(name), sourceNumber("")
      {
          type = getTypeFromString(st);
      };

         /// Explicit constructor
      SourceID( char st,
                std::string name )
         : sourceName(name), sourceNumber("")
      {
          type = getTypeFromChar(st);
      };


         /// Explicit constructor
      SourceID( SourceType st,
                std::string name )
         : type(st), sourceName(name), sourceNumber("")
      {};


         /// Explicit constructor
      SourceID( SourceType st,
                std::string name,
                std::string number )
         : type(st), sourceName(name), sourceNumber(number)
      {};


         /// Copy constructor
      SourceID(const SourceID& s)
         : type(s.type), sourceName(s.sourceName), sourceNumber(s.sourceNumber)
      {};


         /// Equality operator requires all fields to be the same.
      virtual bool operator==(const SourceID& right) const;


         /// Ordering is arbitrary but required to be able to use a SourceID
         /// as an index to a std::map. If an application needs
         /// some other ordering, inherit and override this function.
      virtual bool operator<(const SourceID& right) const;


         /// Inequality operator
      bool operator!=(const SourceID& right) const
      { return !(operator==(right)); }


         /// 'Greater than' operator
      bool operator>(const SourceID& right) const
      {  return (!operator<(right) && !operator==(right)); }


         /// 'Less or equal than' operator
      bool operator<=(const SourceID& right) const
      { return (operator<(right) || operator==(right)); }


         /// 'Greater or equal than' operator
      bool operator>=(const SourceID& right) const
      { return !(operator<(right)); }


         /// Assignment operator
      SourceID& operator=(const SourceID& right);


      SourceType getTypeFromString(const std::string& typeStr);
      SourceType getTypeFromChar(const char& typeChar);


         /// Convenience output method
      virtual std::ostream& dump(std::ostream& s) const;


         /// Return true if this is a valid SourceID. Basically just
         /// checks that none of the fields are undefined.
      virtual bool isValid() const;


         /// Destructor
      virtual ~SourceID() {};


         /// Method to create a new source type.
      static SourceType newSourceType(const std::string& s);


         // Fields

         /// Type of the data source (GPS receiver, Inertial system, etc)
      SourceType  type;


         /// Name of the data source
      std::string sourceName;


         /// Number of the data source
      std::string sourceNumber;


   private:


      static std::map< SourceType, std::string > stStrings;


   public:

      #ifndef SWIG
      class Initializer
      {
      public:
         Initializer();
      };

      static Initializer SourceIDsingleton;
      #endif

   }; // End of class 'SourceID'



   namespace StringUtils
   {

      /// Convert this object to a string representation
      std::string asString(const SourceID& p);

   }



   /// Stream output for SourceID
   std::ostream& operator<<( std::ostream& s,
                             const SourceID& p );


}  // End of namespace gpstk

#endif   // GPSTK_SOURCEID_HPP
