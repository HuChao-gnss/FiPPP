#pragma ident "$Id$"

/**
 * @file DecimateData.hpp
 * This class is used to eliminate data that is not at the sampling interval point,
 * a tolerance, and a starting epoch.
 */
#ifndef DecimateData_HPP
#define DecimateData_HPP

#include "Exception.hpp"
#include "CommonTime.hpp"
#include "gnssData.hpp"

namespace gpstk
{

      /// Thrown when some epoch data must be decimated
      /// @ingroup exceptiongroup
    NEW_EXCEPTION_CLASS(EndOfValidInterval, gpstk::Exception);
    NEW_EXCEPTION_CLASS(DecimateException, gpstk::Exception);


   class DecimateData 
   {
   public:

         /// Default constructor
      DecimateData()
         : sampling(30.0),tolerance(0.5),
           initialTime(CommonTime::BEGINNING_OF_TIME),
           endTime(CommonTime::END_OF_TIME) 
      {};


         /** Common constructor.
          *
          * @param sampleInterval      Sampling interval, in seconds.
          * @param tol                 Tolerance, in seconds.
          * @param initialEpoch        Epoch to start decimation.
          */
      DecimateData( const double sampleInterval,
                 const double tol,
                 const CommonTime& initialEpoch,
                 const CommonTime& endEpoch)
         : sampling(sampleInterval), tolerance(tol), 
           initialTime(initialEpoch), endTime(endEpoch) 
      {};


         /** Return a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gnssRinex     Data object holding the data.
          */
      virtual satTypeValueMap& Process( const CommonTime& time,
                                        satTypeValueMap& gnssRinex )
         noexcept(false);


      virtual gnssRinex& Process(gnssRinex& gnssRinex)
         noexcept(false)
      { 
          Process(gnssRinex.header.epoch, gnssRinex.body); 
          return gnssRinex;
      };

         /** Sets sampling interval.
          *
          * @param sampleInterval      Sampling interval, in seconds.
          */
      virtual DecimateData& setSampleInterval(const double sampleInterval);


         /** Sets tolerance, in seconds.
          *
          * @param tol                 Tolerance, in seconds.
          */
      virtual DecimateData& setTolerance(const double tol);


         /** Sets epoch to start decimation.
          *
          * @param initialEpoch        Epoch to start decimation.
          */
      virtual DecimateData& setInitialEpoch(const CommonTime& initialEpoch)
      { initialTime = initialEpoch; return (*this); };

         /** Sets epoch to end decimation.
          *
          * @param initialEpoch        Epoch to start decimation.
          */
      virtual DecimateData& setEndEpoch(const CommonTime& endEpoch)
      { endTime = endEpoch; return (*this); };


         /// Return a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor
      virtual ~DecimateData() {};


   private:


         /// Sampling interval, in seconds
      double sampling;

         /// Tolerance, in seconds
      double tolerance;

         /// Last processed epoch
      CommonTime initialTime;
      CommonTime endTime;

   }; // End of class 'DecimateData'

      //@}

}  // End of namespace gpstk

#endif   // DecimateData_HPP
