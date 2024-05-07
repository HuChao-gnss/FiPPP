#pragma ident "$Id$"



#include "SourceID.hpp"
#include "Exception.hpp"


namespace gpstk
{

   std::map< SourceID::SourceType, std::string > SourceID::stStrings;


   SourceID::Initializer SourceIDsingleton;


   SourceID::Initializer::Initializer()
   {
      stStrings[Unknown]      = "UnknownSource";
      stStrings[GPS]          = "GPS";
      stStrings[GLONASS]      = "GLONASS";
      stStrings[Galileo]      = "Galileo";
      stStrings[SBAS]         = "SBAS";
      stStrings[QZSS]         = "QZSS";
      stStrings[BDS]          = "BDS";
      stStrings[IRNSS]        = "IRNSS";
      stStrings[Mixed]        = "Mixed";
      stStrings[UserDefined]  = "UserDefined";
      stStrings[DGPS]         = "DGPS";
      stStrings[RTK]          = "RTK";
      stStrings[INS]          = "INS";
   }



      // Assignment operator
   SourceID& SourceID::operator=(const SourceID& right)
   {

      if ( this == &right )
      {
         return (*this);
      }

      type = right.type;
      sourceName = right.sourceName;
      sourceNumber = right.sourceNumber;

      return *this;

   }  // End of 'SourceID::operator=()'



      // Convenience output method
   std::ostream& SourceID::dump(std::ostream& s) const
   {

      s << sourceName ;

      return s;

   }  // End of method 'SourceID::dump()'



      // Return true if this is a valid SourceID. Basically just
      // checks that none of the fields are undefined.
   bool SourceID::isValid() const
   {

      return !(type==Unknown || sourceName=="" || sourceNumber=="");

   }  // End of method 'SourceID::isValid()'



      // Method to create a new source type.
   SourceID::SourceType SourceID::newSourceType(const std::string& s)
   {

      SourceType newId =
         static_cast<SourceType>(SourceID::stStrings.rbegin()->first + 1);

      SourceID::stStrings[newId] = s;

      return newId;

   }  // End of method 'SourceID::newSourceType()'

      // Method to create a new source type.
   SourceID::SourceType SourceID::getTypeFromString(const std::string& typeStr)
   {

      SourceType srcType = Unknown;
      for(auto it=stStrings.begin(); it!=stStrings.end(); it++) 
      {
         if((*it).second== typeStr) 
         {
            srcType = (*it).first;
            break;
         }
      }

      return srcType;

   }                                              

   SourceID::SourceType SourceID::getTypeFromChar(const char& ch)
   {
      SourceType type = Unknown;
      switch(ch)
      {
         case ' ': case 'G': case 'g':
            type = GPS;
            break;
         case 'R': case 'r':
            type = GLONASS;
            break;
         case 'E': case 'e':
            type = Galileo;
            break;
         case 'C': case 'c':
            type = BDS;
            break;
         case 'S': case 's':
            type = SBAS;
            break;
         case 'J': case 'j':
            type = QZSS;
            break;
         case 'I': case 'i':
            type = IRNSS;
            break;
         case 'M': case 'm':
            type = Mixed;
            break;
         default:                                              
            Exception e(std::string("Invalid type character \"")
                        + ch + std::string("\""));
            GPSTK_THROW(e);
      }

      return type;
   }




      // Equality operator requires all fields to be the same.
   bool SourceID::operator==(const SourceID& right) const
   {

      return (type==right.type &&
              sourceName==right.sourceName &&
              sourceNumber==right.sourceNumber);

   }  // End of 'SourceID::operator==()'



      // Ordering is arbitrary but required to be able to use a SourceID
      // as an index to a std::map. If an application needs
      // some other ordering, inherit and override this function.
   bool SourceID::operator<(const SourceID& right) const
   {

      if (type == right.type)
      {
          if( sourceName == right.sourceName )
          {
              return sourceNumber < right.sourceNumber;
          }
          else
          {
              return sourceName < right.sourceName;
          }
      }
      else
      {
         return type < right.type;
      }

   }  // End of 'SourceID::operator<()'



   namespace StringUtils
   {

         // convert this object to a string representation
      std::string asString(const SourceID& p)
      {

         std::ostringstream oss;
         p.dump(oss);

         return oss.str();

      }  // End of function 'asString()'

   }  // End of namespace StringUtils



      // Stream output for SourceID
   std::ostream& operator<<( std::ostream& s,
                             const SourceID& p )
   {

      p.dump(s);

      return s;

   }  // End of 'operator<<'


}  // End of namespace gpstk
