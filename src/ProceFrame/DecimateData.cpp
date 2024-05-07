#pragma ident "$Id$"

/**
 * @file DecimateData.cpp
 * This class decimates GNSS Data Structures data given a sampling interval,
 * a tolerance, and a starting epoch.
 */


#include "DecimateData.hpp"

using namespace std;

namespace gpstk
{

      // Return a string identifying this object.
    std::string DecimateData::getClassName() const
    { return "DecimateData"; }



      /* Sets sampling interval.
       *
       * @param sampleInterval      Sampling interval, in seconds.
       */
    DecimateData& DecimateData::setSampleInterval(const double sampleInterval)
    {

         // Make sure that sample interval is positive
        if( sampleInterval >= 0.0 )
        {
            sampling = sampleInterval;
        }

        return (*this);

    }  // End of method 'DecimateData::setSampleInterval()'



      /* Sets tolerance, in seconds.
       *
       * @param tol                 Tolerance, in seconds.
       */
    DecimateData& DecimateData::setTolerance(const double tol)
    {

         // Make sure that tolerance is positive
        if( tol >= 0.0 )
        {
            tolerance = tol;
        }

        return (*this);

    }  // End of method 'DecimateData::setTolerance()'


      /* Return a satTypeValueMap object, adding the new data generated when
       * calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       * @warning         The modified decimate avoids the wrong decimate when
       *                  there is a large data gap more than threshold, which
       *                  is not considered in the old version.
       */
    satTypeValueMap& DecimateData::Process( const CommonTime& time,
                                        satTypeValueMap& gData )
        noexcept(false)
    {


        // Time offset relative to the intial epoch time
        double offset( (time - initialTime) );

        // Check if current epoch is whether or not in the integer sampling
        // position with tolerance error.
        // if not, it will be decimated

        // DecimateData the data with time earlier than the given initial time
        if ( offset < 0.0)
        {
            // If epoch must be decimated, we issue an Exception
            DecimateException e("This epoch must be decimated.");
            GPSTK_THROW(e);
        }


        // According to the definition of the modulo(%) operator in C++,
        // for the positive integers, the remainder will also be positive
        // for example, 301 % 30 = 1, and 299 % 30 = 29.
        // So, given the tolerence, if the the remainder of the time
        // is greater than (sampling - tolerance), or less than (tolerance),
        // the time is near the integer sampling position. Or else,
        // we will decimate the data.
        if ( (int)(offset) % (int)(sampling) != 0  )
        {
            // If epoch must be decimated, we issue an Exception
            DecimateException e("This epoch must be decimated.");
            GPSTK_THROW(e);
        }

        double offset2 = time - endTime;

        // if greater than endTime, decimate
        if(offset2 > 0.0)
        {
            // If epoch must be decimated, we issue an Exception
            DecimateException e("time is greater than endTime");
            GPSTK_THROW(e);
        }

        return gData;

    }  // End of method 'DecimateData::Process()'


}  // End of namespace gpstk
