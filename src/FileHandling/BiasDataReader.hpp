#pragma ident "$Id$"

/**
 * @file BiasDataReader.hpp
 * Class to read Bias data.
 */

#ifndef BIAS_DATA_READER_HPP
#define BIAS_DATA_READER_HPP


#include <string>
#include <map>
#include <fstream>

#include "Exception.hpp"
#include "StringUtils.hpp"
#include "SatID.hpp"
#include "TypeID.hpp"
#include "Epoch.hpp"
#include "RinexSatID.hpp"

#include "AboutOptions.hpp"


namespace gpstk
{

      /** @addtogroup formattedfile */
      //@{

    class BiasDataReader 
    {
    public:

         /// Default constructor
        BiasDataReader() {};

         /** Common constructor. It will always open file for read and will
          *  load Bias data in one pass.
          *
          * @param fn   Bias data file to read
          *
          */
        BiasDataReader(const char* fn)
        {
            open(fn);
        }


         /** Common constructor. It will always open file for read and will
          *  load Bias data in one pass.
          *
          * @param fn   Bias data file to read
          *
          */
        BiasDataReader(const std::string& fn)
        {
            open(fn);
        }

        /// Method to open AND load Bias data file.
        virtual void open(const char* fn);


        /// Method to open AND load Bias data file. It doesn't
        /// clear data previously loaded.
        virtual void open(const std::string& fn);


        double getSatBias(const RinexSatID/*SatID*/& sat,
                           const std::string& type1,
                           const std::string& type2,
                           const CommonTime& time );

        std::map<std::string, double> getSatDCB(const RinexSatID sat,
            const std::vector<std::string>& typeVec, const CommonTime& time, const bool bdsfopt);


        /// Destructor
        virtual ~BiasDataReader() {};


    private:

        std::string mode;

        std::fstream strm;

        struct BiasData
        {
            BiasData()
                : start(CommonTime::BEGINNING_OF_TIME),
                  end(CommonTime::END_OF_TIME),
                  value(0.0), std(0.0)
            {};

            CommonTime start;
            CommonTime end;

            double value;
            double std;
        };


        // key  - PRN + OBS1 + OBS2
        // data - TIME1 + TIME2 + VALUE + STD
        std::multimap<std::string, BiasData> satBiasData;

        /// Method to store Bias data in this class' data map
        virtual void loadData()
            noexcept(false);

    };  // End of class 'BiasDataReader'

    //@}

}  // End of namespace gpstk

#endif  // BIAS_DATA_READER_HPP
