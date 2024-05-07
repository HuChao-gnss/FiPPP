/**
* @file LeapSecStore.cpp
* Class to handle interpolatable time serial data
*/

#include "StringUtils.hpp"
#include "LeapSecStore.hpp"
#include "MiscMath.hpp"
#include "CommonTime.hpp"
#include "MJD.hpp"
#include <fstream>

using namespace std;
using namespace gpstk::StringUtils;

#define debug 0

namespace gpstk
{

   void LeapSecStore::loadFile(std::string file)
      noexcept(false)
   {

      ifstream inpf(file.c_str());
      if(!inpf)
      {
         FileMissingException fme("Could not open leap second file " + file);
         GPSTK_THROW(fme);
      }

      clear();


         // Now, read the leap second data
      bool ok (true);
      while(!inpf.eof() && inpf.good())
      {
         string line;
         getline(inpf,line);
         StringUtils::stripTrailing(line,'\r');

         if( inpf.eof() ) break;
         if( inpf.bad() ) { ok = false; break; }

         if(debug)
         {
            cout << "line:" << line << endl;
         }

            // jump the header
         if(line[0] == '#') continue;

            // Define a string stream to read 'line'
         istringstream istrm(line);
         if(debug)
         {
            cout << line.size() << endl;
         }

         double mjd(0.0);
         double year(0.0), month(0.0), day(0.0);
         double leapSec(0.0);

            // Get the variable values from stream
         istrm >> mjd
               >> day >> month >> year
               >> leapSec;

         if(debug)
         {
            cout << "time: "
                 << CommonTime( MJD(mjd).convertToCommonTime() )
                 << " TAI - UTC " << leapSec << endl;
         }


            // UTC time corresponding to the leap second
         CommonTime time( MJD(mjd).convertToCommonTime() );
         time.setTimeSystem(TimeSystem::UTC);

            // Add leap second into 'leapSecData'
         leapSecData[time] = leapSec;

            // Modify the initialTime and finalTime for 'leapSecData'
         if( time < initialTime )
         {
            initialTime = time;
         }
         else if( time > finalTime)
         {
            finalTime = time;
         }

//       cout << "time:"  << time << endl;

      }

      inpf.close();

      if(!ok)
      {
         FileMissingException fme("LeapSec File " + file
                                  + " is corrupted or wrong format");
         GPSTK_THROW(fme);
      }
   }


   double LeapSecStore::getLeapSec(const CommonTime& utc) const
      noexcept(false)
   {

      double leapSec;

      for( LeapSecData::const_iterator it = leapSecData.begin();
           it != leapSecData.end();
           ++it)
      {
         if( (it->first) <= utc )
         {
               // Leap second = "TAI2UTC"
            leapSec = (it->second) ;
         }
      }

      return leapSec;

   }  // End of method 'LeapSecStore::getLeapSec()'


}  // End of namespace gpstk
