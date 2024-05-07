#pragma ident "$Id$"

/**
 * @file ComputeStaTides.hpp
 * This class computes station tides.
 */

#ifndef COMPUTE_STA_TIDES_HPP
#define COMPUTE_STA_TIDES_HPP


#include <string>

#include "SolidTides.hpp"
#include "OceanLoading.hpp"
#include "PoleTides.hpp"
#include "DataStructures.hpp"
#include "gnssData.hpp"

namespace gpstk
{

    class ComputeStaTides
    {
    public:

         /// Default constructor
        ComputeStaTides() 
            : pSolSys(NULL), pRefSys(NULL), pBLQReader(NULL), correctOTL(1)
        {};

        virtual ComputeStaTides& setNominalPosition(const Position& pos)
        { nominalPos = pos; return (*this); };

        ComputeStaTides& setBLQDataReader( BLQDataReader& blqStore )
        { 
            pBLQReader = &blqStore;
            return (*this); 
        };

        ComputeStaTides& setCorrectOTL( bool correct)
        { 
            correctOTL = correct;
            return (*this); 
        };


        ComputeStaTides& setReferenceSystem( ReferenceSystem& refSys )
        {
            pRefSys = & refSys;
            return (*this);
        };


        ComputeStaTides& setSolarSystem( SolarSystem& solSys )
        {
            pSolSys = &solSys;
            return (*this);
        };

        Triple getTides( const CommonTime& time,
                         const std::string& station )
            noexcept(false);

         /** Return a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gData     Data object holding the data.
          */
        virtual satTypeValueMap& Process( const CommonTime& time,
                                          satTypeValueMap& gData )
            noexcept(false);



        virtual void Process(gnssRinex& gData)
            noexcept(false);


         /// Return a string identifying this object.
        virtual std::string getClassName(void) const;


         /// Destructor
        virtual ~ComputeStaTides() {};

    private:

        SolarSystem*           pSolSys;
        ReferenceSystem*       pRefSys;
        BLQDataReader*         pBLQReader;
        bool                   correctOTL;

        Position nominalPos;
        SourceID source;
        string station;

        Triple tideData;
        std::map<SourceID,Triple> sourceTideData;

        SolidTides solid;
        OceanLoading ocean;
        PoleTides pole;

    }; // End of class 'ComputeStaTides'

    //@}

}  // End of namespace gpstk

#endif  // COMPUTE_STA_TIDES_HPP
