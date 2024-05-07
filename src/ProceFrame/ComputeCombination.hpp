
/**
 * @file ComputeCombination.hpp
 * This class computes linear combinations of GDS data.
 *
 */

#include "gnssData.hpp"

#ifndef ComputeCombination_HPP
#define ComputeCombination_HPP

namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /** This class computes linear combinations of GDS data.
       *
       * This class is meant to be used with the GNSS data structures (GDS)
       * objects found in "DataStructures" class, and it is intended to be
       * used as a more flexible replacement for classes such as ComputePC.hpp.
       *
       * A typical way to use this class follows:
       *
       * @code
       *
       *      // Define a linear combination to compute PC using C1
       *   gnssLinearCombination pcComb;
       *   pcComb.header = TypeID::PC;
       *   pcComb.body[TypeID::C1] = +GAMMA_GPS/0.646944444;
       *   pcComb.body[TypeID::P2] = -1.0/0.646944444;
       *
       *
       *      // Define another linear combination to compute PC prefit
       *      // residual
       *   gnssLinearCombination prefitComb;
       *   prefitComb.header = TypeID::prefitC;
       *   prefitComb.body[TypeID::PC12G] = +1.0;
       *   prefitComb.body[TypeID::rho] = -1.0;
       *   prefitComb.body[TypeID::dtSat] = +1.0;
       *   prefitComb.body[TypeID::relativity] = -1.0;
       *   prefitComb.body[TypeID::tropoSlant] = -1.0;
       *
       *
       *      // Object to compute linear combinations of data
       *      // Linear combinations will be computed in a FIFO basis
       *   ComputeCombination linear;
       *   linear.addLinear(pcComb);
       *   linear.addLinear(prefitComb);
       *
       *      // Load observation data
       *   RinexObsStream rin("ebre0300.02o");
       *
       *      // Loads precise ephemeris object with file data
       *   SP3EphemerisStore SP3EphList;
       *   SP3EphList.loadFile("igs11513.sp3");
       *
       *      // Sets nominal position of receiver
       *   Position nominalPos(4833520.3800, 41536.8300, 4147461.2800);
       *
       *      // Declare a MOPSTropModel object, setting the defaults
       *   MOPSTropModel mopsTM( nominalPos.getAltitude(),
       *                         nominalPos.getGeodeticLatitude(), 30);
       *
       *      // Object to compute the tropospheric data
       *   ComputeTropModel computeTropo(mopsTM);
       *
       *      // Declare a basic modeler
       *   BasicModel basic(nominalPos, SP3EphList);
       *
       *   gnssRinex gRin;
       *
       *   while(rin >> gRin)
       *   {
       *
       *      gRin >> basic >> computeTropo >> linear;
       *
       *         // Dump results
       *      gRin.body.dump(cout,1);
       *   }
       *
       * @endcode
       *
       * The "ComputeCombination" object will visit every satellite in the GNSS
       * data structure that is "gRin" and will compute the previously defined
       * linear combination (or combinations).
       *
       * When used with the ">>" operator, this class returns the same
       * incoming data structure with the results inserted in it. Be warned
       * that if a given satellite does not have the observations or data
       * required by the linear combination definition, such data will be
       * taken as zero.
       *
       * \warning If the "ComputeCombination" object has more than one linear
       * combination definition, they will be applied in the same order they
       * were added to the object, i.e. in a FIFO (First Input - First Output)
       * basis. Therefore, you must be mindful of combination order.
       *
       * @sa ComputeCombination.hpp, ComputePC.hpp, ModelObsFixedStation.hpp
       * and ModelObs.hpp, among others, for related classes.
       */
    class ComputeCombination                         
    {
    public:

        /// Default constructor
        ComputeCombination()
        { clearAll(); };

        /// add other methods here

         /** Return a satTypeValueMap object, adding the new data generated
          *  when calling this object.
          *
          * @param time      Epoch corresponding to the data.
          * @param gData     Data object holding the data.
          */
        virtual satTypeValueMap& Process( const CommonTime& time,
                                          satTypeValueMap& gData )
            noexcept(false);


        virtual void Process(gnssRinex& gRin)
            noexcept(false)
        { 
           Process(gRin.header.epoch, gRin.body); 
        };


        void setSysCombs(std::map<SatelliteSystem, LinearCombList>& sysCombs)
        {
            systemCombs = sysCombs;
        };

        void addLinear(const SatelliteSystem& sys, LinearCombList& list)
        {
            for(auto it=list.begin();it!=list.end(); it++)
            {
                systemCombs[sys].push_back( (*it) );
            }
        };

        void addLinear(const SatelliteSystem& sys, gnssLinearCombination& comb)
        {
            systemCombs[sys].push_back(comb);
        };

        LinearCombList getLinear(const SatelliteSystem& sys)
        {
            return systemCombs[sys];
        }

        std::map<SatelliteSystem, LinearCombList> getLinear()
        {
            return systemCombs;
        }
        

         /// Clear all linear combinations.
        virtual ComputeCombination& clearAll(void)
        {
            systemCombs.clear();
            return (*this);
        };


         /// Return a string identifying this object.
        virtual std::string getClassName(void) const;

		void setBdsFopt(int opt)
		{
			bdsfopt = opt;
		}

        void addTypeIonCoeff(const SatelliteSystem& sys, const TypeID& type,
            const double value)
        {
            sysTypeIonCoeff[sys].insert(make_pair(type, value));
        }

         /// Destructor
        virtual ~ComputeCombination() {};


    private:

        int bdsfopt;
         /// List of linear combinations to compute
        std::map<SatelliteSystem, LinearCombList> systemCombs;
        // sys-<prefitType-coeffient>
        map<SatelliteSystem, map<TypeID, double>> sysTypeIonCoeff;
    }; // End class ComputeCombination

    //@}

}  // End of namespace gpstk

#endif   // ComputeCombination_HPP
