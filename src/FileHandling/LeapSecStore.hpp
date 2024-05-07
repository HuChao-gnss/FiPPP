/**
* @file LeapSecStore.hpp
* Class to read and store the leapsec data.
*/

#ifndef GPSTK_LEAP_SEC_STORE_HPP
#define GPSTK_LEAP_SEC_STORE_HPP


#include <iostream>
#include <string>
#include <set>
#include <map>
#include "CommonTime.hpp"

using namespace std;

namespace gpstk
{

      /** @addtogroup General Data Tool */
      //@{

      /** This class will read the leap second data from given file and
       *  you can get the leap second by given a time.
       *
       *  The leap second file can be downloaded from the IERS ftp:
       *
       *  ftp://hpiers.obspm.fr/iers/bul/bulc/Leap_Second_History.dat
       *
       *  \warning You ONLY can get the leapsec at given "UTC" time, so
       *  the time conversion is needed if your time system is 'GPS' or
       *  some other time system.
       *
       *  \warning Different leap second file from difference organizations
       *  would be different. This class only 'distiguish' the file from
       *  the IERS 'Leap_Second_History.dat'.
       */

   class LeapSecStore
   {
   public:

      /// Handy type definition
      typedef std::map<CommonTime, double > LeapSecData;


      /// Default constructor
      LeapSecStore(CommonTime iniTime = CommonTime::END_OF_TIME,
                   CommonTime finTime = CommonTime::BEGINNING_OF_TIME)
         : initialTime(iniTime),
           finalTime(finTime)
      {}


      /// Load the leap second data from input file
      void loadFile(std::string file)
         noexcept(false);


      /** Determine the earliest time stored in the object
       *
       * @return The initial time
       *
       */
      virtual CommonTime getInitialTime()
      { return initialTime; };


      /** Determine the latest time stored in the object
       *
       * @return The final time
       *
       */
      virtual CommonTime getFinalTime()
      { return finalTime; };


      /** Get the leap second at the given UTC time
       *
       *  @param t   the time given in UTC
       *
       */
      double getLeapSec(const CommonTime& t) const
         noexcept(false);



      /// clear the all the data
      void clear()
      { leapSecData.clear(); }


      /// Default deconstructor
      virtual ~LeapSecStore()
      { leapSecData.clear(); }


   private:

      /// Object holding all the leap second data
      LeapSecData leapSecData;

      /// These give the overall span of time for which this object
      /// contains data.
      CommonTime initialTime;
      CommonTime finalTime;

   }; // End of class 'LeapSecStore'

      // @}

}  // End of namespace gpstk


#endif   // GPSTK_LEAP_SEC_STORE_HPP
