#pragma ident "$Id$"

/**
 * @file CorrectRecBias.hpp
 * This class corrects observables from effects such as antenna excentricity,
 * difference in phase centers, offsets due to tide effects, etc.
 */

#ifndef CorrectRecBias_HPP
#define CorrectRecBias_HPP


#include <string>

#include "Antenna.hpp"
#include "ComputeStaTides.hpp"
#include "XvtStore.hpp"
#include "AntexReader.hpp"
#include "DataStructures.hpp"
#include "gnssData.hpp"
#include "GNSSconstants.hpp"

namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class corrects observables from effects such as antenna
       *  excentricity, difference in phase centers, offsets due to
       *  tidal effects, etc.
       *
       * This class is meant to be used with the GNSS data structures objects
       * found in "DataStructures" class.
       *
       * A typical way to use this class follows:
       *
       * @code
       * @endcode
       *
       * The "CorrectRecBias" object will visit every satellite in the
       * GNSS data structure that is "gRin" and will correct the
       * corresponding observables from the given effects.
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the observables corrected. Be warned
       * that if a given satellite does not have the observations required,
       * it will be summarily deleted from the data structure.
       *
       */
    class CorrectRecBias                         
    {
    public:

         /// Default constructor.
        CorrectRecBias()
            : nominalPos(0.0, 0.0, 0.0), 
              useAzimuth(false),
              pAntexReader(NULL),
              pStaTides(NULL),
              L1PhaseCenter(0.0, 0.0, 0.0), L2PhaseCenter(0.0, 0.0, 0.0),
              L5PhaseCenter(0.0, 0.0, 0.0), L6PhaseCenter(0.0, 0.0, 0.0), 
              L7PhaseCenter(0.0, 0.0, 0.0), L8PhaseCenter(0.0, 0.0, 0.0), 
              L9PhaseCenter(0.0, 0.0, 0.0),
              monument(0.0, 0.0, 0.0), tide(0.0, 0.0, 0.0),
              firstTime(true)
        {};


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


         /// Return whether azimuth-dependent antenna patterns are being used.
         /// When an Antenna is set, this parameter is true by default.
        virtual bool getUseAzimuth(void) const
        { return useAzimuth; };


         /** Sets whether azimuth-dependent antenna patterns will be used.
          *
          * @param useAzimuthPattern   Whether azimuth patterns will be used.
          */
        virtual CorrectRecBias& setUseAzimuth(bool useAzimuthPattern)
        { useAzimuth = useAzimuthPattern; return (*this); };



         /// Return nominal position of receiver station.
        virtual Position getNominalPosition(void) const
        { return nominalPos; };


         /** Sets nominal position of receiver station.
          *
          * @param pos    Nominal position of receiver station.
          */
        virtual CorrectRecBias& setNominalPosition(const Position& pos)
        { nominalPos = pos; return (*this); };


        virtual CorrectRecBias& setAntexReader(AntexReader& antexObj)
        { pAntexReader = &antexObj; return (*this); };


         /** Return extra biases affecting monument, such as tidal
          *  effects ([UEN]).
          */
        virtual Triple getExtraBiases(void) const
        { return tide; };


         /** Sets extra biases affecting monument, such as tidal
          *  effects ([UEN]).
          *
          * @param extra   Extra biases affecting monument, such as tidal
          *                effects ([UEN]).
          */
        virtual CorrectRecBias& setExtraBiases(const Triple& extra)
        { tide = extra; return (*this); };


         /// Return a pointer to the sourceMonumentMap object currently
         /// in use.
        virtual std::map<SourceID,Triple> getSourceMonumentData(void) const
        { return sourceMonumentData; };


         /** Sets sourceMonumentMap object to be used.
          *
          * @param monument     sourceMonumentMap object.
          */
        virtual CorrectRecBias& setSourceMonumentData(
                                const std::map<SourceID,Triple>& monument )
        { sourceMonumentData = monument; return (*this); };


         /** Return vector from monument to ARP ([UEN]).
          */
        virtual Triple getMonument(void) const
        { return monument; };


         /** Sets vector from monument to ARP ([UEN]).
          *
          * @param mon   Vector from monument to ARP ([UEN]).
          */
        virtual CorrectRecBias& setMonument(const Triple& mon)
        { monument = mon; return (*this); };


         /// Return a pointer to the sourceAntennaMap object currently
         /// in use.
        virtual std::map<SourceID,Antenna> getSourceAntennaData(void) const
        { return sourceAntennaData; };


         /** Sets sourceAntennaMap object to be used.
          *
          * @param antenna     sourceAntennaMap object.
          */
        virtual CorrectRecBias& setSourceAntennaData(
                                const std::map<SourceID,Antenna>& antenna )
        { sourceAntennaData = antenna; return (*this); };


         /// Return the antenna object being used.
        virtual Antenna getAntenna(void) const
        { return antenna; };


         /** Sets the antenna object to be used.
          *
          * @param ant    Antenna object to be used.
          */
        virtual CorrectRecBias& setAntenna(const Antenna& ant)
        { antenna = ant; useAzimuth = true; return (*this); };


         /** Sets ComputeStaTides object to be used.
          *
          * @param tides     ComputeStaTides object.
          */
        virtual CorrectRecBias& setTidePointer(ComputeStaTides& tides)
        { pStaTides = &tides; return (*this); };


         /** Sets the tide vector to be used.
          *
          * @param tid    Tide vector to be used.
          */
        virtual CorrectRecBias& setTide(const Triple& tid)
        { tide = tid; return (*this); };


         /// Return a string identifying this object.
        virtual std::string getClassName(void) const;


         /// Destructor
        virtual ~CorrectRecBias() {};


    private:

        /// Station position.
        Position nominalPos;

        /// Station monument map.
        std::map<SourceID,Triple> sourceMonumentData;
        std::map<SourceID,Triple> sourceTideData;

        /// Station antenna map.
        std::map<SourceID,Antenna> sourceAntennaData;

         /// Correct tide effects.
        ComputeStaTides* pStaTides;
        AntexReader*  pAntexReader;

        /// Station monument.
        Triple monument;

        string antModel;

        /// Station antenna.
        Antenna antenna;
         /// Station tidal displacement.
        Triple tide;

        bool firstTime;

         /// Whether azimuth-dependent antenna patterns will be used or not
        bool useAzimuth;

         /// Position of antenna L1 phase center with respect to ARP ([UEN]).
        Triple L1PhaseCenter;
        Triple L2PhaseCenter;
        Triple L5PhaseCenter;
        Triple L6PhaseCenter;
        Triple L7PhaseCenter;
        Triple L8PhaseCenter;
        Triple L9PhaseCenter;


    }; // End of class 'CorrectRecBias'

    //@}

}  // End of namespace gpstk

#endif  // CorrectRecBias_HPP
