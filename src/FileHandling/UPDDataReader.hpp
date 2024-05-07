#pragma ident "$Id$"

/**
 * @file UPDDataReader.hpp
 * Class to read upd data.
 */

#ifndef UPDDataReader_HPP
#define UPDDataReader_HPP


#include <string>
#include <map>
#include <fstream>

#include "DataStructures.hpp"
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "SatID.hpp"
#include "TypeID.hpp"
#include "Epoch.hpp"
#include "RinexSatID.hpp"

using namespace gpstk;

namespace fippp
{

    class UPDDataReader 
    {


        struct updData_t
        {
            updData_t() :
                value(0.),
                std(0.)
            {};
            double value;
            double std;
        };

        typedef std::map<SatID, updData_t> satUPDDataMap;

    public:

         /// Default constructor
        UPDDataReader() {
            updEWLData.clear();
            updWLData.clear();
            updNLData.clear();
        };

         /** Common constructor. It will always open file for read and will
          *  load Bias data in one pass.
          *
          * @param fn   Bias data file to read
          *
          */
        UPDDataReader(const char* fn)
        {
            open(fn);
        }


         /** Common constructor. It will always open file for read and will
          *  load Bias data in one pass.
          *
          * @param fn   Bias data file to read
          *
          */
        UPDDataReader(const std::string& fn)
        {
            open(fn);
        }

        UPDDataReader(const std::map<TypeID, std::string>& UPDFile)
        {
            std::map<TypeID, std::string>::const_iterator itUpd;
            itUpd = UPDFile.find(TypeID::EWL);
            if ( itUpd != UPDFile.end())
                openEWL( itUpd->second );
            itUpd = UPDFile.find(TypeID::WL);
            if (itUpd != UPDFile.end())
                openWL(itUpd->second);
            itUpd = UPDFile.find(TypeID::NL);
            if (itUpd != UPDFile.end())
                openNL(itUpd->second);
        }

        void loadFile(const std::map<TypeID, std::string>& UPDFile)
        {
            std::map<TypeID, std::string>::const_iterator itUpd;
            itUpd = UPDFile.find(TypeID::EWL);
            if (itUpd != UPDFile.end())
            {
                std::cout << "EWL UPD: " << itUpd->second << std::endl;
                openEWL(itUpd->second);
            }
            itUpd = UPDFile.find(TypeID::WL);
            if (itUpd != UPDFile.end())
            {
                std::cout << "WL UPD: " << itUpd->second << std::endl;
                openWL(itUpd->second);
            }
            itUpd = UPDFile.find(TypeID::NL);
            if (itUpd != UPDFile.end())
            {
                std::cout << "NL UPD: " << itUpd->second << std::endl;
                openNL(itUpd->second);
            }
        }
        /// <summary>
        /// for IRC product
        /// </summary>
        /// <param name="f"></param>
        void loadIRCWL(const std::string& fn,
            const CommonTime& today)
            noexcept(false);

        double getSatUPD(const SatID& sat,
            const TypeID& type,
            const CommonTime& time)
            noexcept(false);

        double getValue(epochValueMap& epochValueData,
            const CommonTime& ttag) {
            return 0.;
        };

        bool checkWLData()
        {
            return updWLData.size();
        }

        std::string acname;     // clk or upd acs
        std::string arprod;

        /// Destructor
        virtual ~UPDDataReader() {};


    protected:
        satUPDDataMap updEWLData;
        satUPDDataMap updWLData;
        std::map<CommonTime, satUPDDataMap> updNLData;

    private:
        std::fstream strm;
        virtual void loadEWL()
            noexcept(false);
        virtual void loadWL()
            noexcept(false);
        virtual void loadNL()
            noexcept(false);

        /// Method to open AND load Bias data file.
        virtual void open(const char* fn) {};

        virtual void openEWL(const char* fn);
        virtual void openWL(const char* fn);
        virtual void openNL(const char* fn);
        virtual void openEWL(const string& fn);
        virtual void openWL(const string& fn);
        virtual void openNL(const string& fn);

        /// Method to open AND load Bias data file. It doesn't
        /// clear data previously loaded.
        virtual void open(const std::string& fn) {};

        virtual void lodaData() {};

    };  // End of class 'UPDDataReader'

}  // End of namespace gpstk

#endif  // BIAS_DATA_READER_HPP
