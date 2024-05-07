#pragma ident "$ID$"

/**
 * this class is used to convet 4-char TypeID to 3-char TypeID,
 * which is convenient for the systematic bias computation, 
 * such as PCO/PCV/Windup correction.
 */

#ifndef ConvertObs_HPP
#define ConvertObs_HPP

#include "gnssData.hpp"

namespace gpstk
{


    class ConvertObs                         
    {
    public :

        // Default constructr
        ConvertObs() {};


        /** Return a satTypeValueMap object, adding the new data generated
         *  when calling this object.
         *
         * @param  time     Epoch corresponding to the data.
         * @param  gData    Data object holding the data.
         */
        virtual satTypeValueMap& Process( std::map<RinexSatID/*SatID*/, TypeIDVec>& satTypes,
                                          std::map<RinexSatID/*SatID*/, TypeIDVec>& satShortTypes,
                                          satTypeValueMap& gData )
            noexcept(false);


        /** Return a gnssRinex object, adding the new data generated
         *  when calling this object.
         *
         * @param gData     Data object holding the data.
         */
        virtual void Process(gnssRinex& gRin)
            noexcept(false)
        { 
            Process(gRin.header.satTypes, 
                    gRin.header.satShortTypes, 
                    gRin.body); 
        };


        // Return a string identifying this object.
        virtual std::string getClassName () const;


        // Default deconstructr
        ~ConvertObs() {};


    private:

        SatelliteSystem satSys;

        TypeID oldType;

        TypeID newType;

    }; // End of class 'ConvertObs'

    // @}

}; // end of namespace gpstk

#endif   // ConvertObs_HPP
