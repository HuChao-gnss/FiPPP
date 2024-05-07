/**
 * @file ComputeSPP.hpp
 * Class to compute the Least Mean Squares Solution
 *
 */

#ifndef ComputeSPP_HPP
#define ComputeSPP_HPP

#include "SolverBase.hpp"
#include "TypeID.hpp"
#include "gnssData.hpp"

#include "ComputeDerivative.hpp"
#include "IonoModel.hpp"
#include "ComputeTropModel.hpp"
#include "ComputeIonoDelay.hpp"
#include "LinearCombinations.hpp"
#include "ComputeCombination.hpp"
#include "EquSys.hpp"
#include "AboutOptions.hpp"

using namespace fippp;

namespace gpstk
{

   NEW_EXCEPTION_CLASS(IterationException, gpstk::Exception);

   /* Class to compute the standard point positioning solution. By default,
    * this class only use GPS observables, if you want to use multi-gnss
    * observables, you should set them by using the method 'setSystems()'.
    * 
    * for ppp, only gps system is used to accelerate the computation speed.
    * but if want to estimate spp solution with different systems, you MUST
    * set the systems by yourself.
    *
    */

   class ComputeSPP                                           
   {
   public:


         /** Explicit constructor. Sets the default equation definition
          *  to be used when fed with GNSS data structures.
          */
        ComputeSPP()
            : firstTime(true), pOutStream(NULL)
        {
            // linear combination 
            defineLinearList();

            // outType vector list
            typeVec.clear();
            typeVec.push_back("X");
            typeVec.push_back("Y");
            typeVec.push_back("Z");
        }; 

        /// define multiple gnss linear list.
        virtual void defineLinearList(void) ;


        /// Returns an index identifying this object.
        virtual void defineEquations(void) ;


        /// Return a reference to a gnssRinex object after solving
        /// the previously defined equation system.
        ///
        ///@param gData    Data object holding the data.
        ///
        virtual gnssRinex& Process(gnssRinex& rxData) noexcept(false);


        /// create equations and solve equations
        Position Solve(gnssRinex& rxData);
        void setOpts(const Options::options_t& opt)
        {
            opts = opt;
        }

        ///
        /// set ionosphere model 
        ///
        void setIonoModel(IonoModel& ionoModel)
        {
            computeIono.setIonoModel(ionoModel);
        };

        // set troposphere model 
        void setTropModel(TropModel& tropModel)
        {
            computeTrop.setTropModel(tropModel);
        };

        void setOutStream(std::fstream& outStrm)
        {
            pOutStream = &outStrm;
        };


        double getSolution( const TypeID& type, const Vector<double>& stateVec ) const
          noexcept(false);

        /// Return a string identifying this object.
        virtual std::string getClassName(void) const;

        /// Destructor.
        virtual ~ComputeSPP() {};

   private:
       //
        bool firstTime;
        Options::options_t opts;

        RandomWalkModel ionoStoModel;

        TypeID defaultObsType;
        SatelliteSystem defaultSys;
        
        ComputeDerivative  computeDerivative;
        ComputeTropModel   computeTrop;
        ComputeIonoDelay   computeIono;
        ComputeCombination computeCombs;

        LinearCombList linearListGPS;
        LinearCombList linearListBDS;
        LinearCombList linearListGAL;
        LinearCombList linearListGLO;
        LinearCombList linearListQZS;

        std::map<SatelliteSystem, LinearCombList> sysCombs;

        EquSys equSys;

        VariableSet currentUnkSet;

        vector<string> typeVec;

        Vector<double> solution;
        Matrix<double> covMatrix;

        std::fstream* pOutStream;

   }; // End of class 'ComputeSPP'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_ComputeSPP_HPP
