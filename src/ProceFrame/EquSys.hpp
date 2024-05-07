#pragma ident "$Id$"

/**
 * @file EquSys.hpp
 * Class to define and handle complex equation systems for solvers.
 */

#ifndef EquSys_HPP
#define EquSys_HPP

//============================================================================
//
//  2015/07/01  
//  design a new 'equationSystem' for the generation of the equations
//  for 'SolverGeneral'
//
//  2020/09/03
//  add dump for descriptionEquation output 
//
//============================================================================


#include <algorithm>

#include "DataStructures.hpp"
#include "StochasticModel.hpp"
#include "Equation.hpp"
#include "gnssData.hpp"


namespace gpstk
{

      /// Thrown when attempting to use an invalid EquSys
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(InvalidEquSys, gpstk::Exception);


      /** @addtogroup DataStructures */
      //@{


      /** This class defines and handles complex equation systems for solvers,
       *  generating the Vectors and Matrices they will need, including:
       *
       *    \li Measurements Vector (prefit residuals).
       *    \li Geometry matrix.
       *    \li Equation weights matrix.
       *    \li State Transition Matrix (PhiMatrix).
       *    \li Process noise covariance matrix (QMatrix).
       *
       * In addition, this class will provide important/interesting information
       * such as number of variables, satellites and data sources being
       * processed.
       *
       * In order to achieve this, the 'EquSys' class will start from a
       * set of 'Equation descriptions' and the currently available GNSS data.
       * From there, it will deduce the unknowns corresponding to each data
       * source as well as the specific equations.
       *
       * A typical way to use this class follows, showing how to set up an
       * object to perform "Precise Point Positioning" (PPP):
       *
       * @code
       *
       *      // DEFINE VARIABLES
       *
       *      // Declare stochastic models to be used
       *   StochasticModel coordinatesModel;
       *   TropoRandomWalkModel tropoModel;
       *   PhaseAmbiguityModel ambiModel;
       *
       *      // These variables are, by default, SourceID-indexed
       *   Variable dx( TypeID::dx, &coordinatesModel, true, false, 100.0 );
       *
       *   Variable dy( TypeID::dy, &coordinatesModel );
       *   dy.setInitialVariance( 100.0 );     // Equivalent to 'dx' setup
       *
       *   Variable dz( TypeID::dz, &coordinatesModel );
       *   dz.setInitialVariance( 100.0 );
       *
       *   Variable cdt( TypeID::cdt );
       *   cdt.setDefaultForced(true);   // Force default coefficient (1.0)
       *
       *   Variable tropo( TypeID::wetMap, &tropoModel );
       *   tropo.setInitialVariance( 25.0 );
       *
       *
       *      // The following variable is, SourceID and SatID-indexed
       *   Variable ambi( TypeID::BLC, &ambiModel, true, true );
       *   ambi.setDefaultForced(true);   // Force default coefficient (1.0)
       *
       *      // This will be the independent term for code equation
       *   Variable prefitC( TypeID::prefitC );
       *
       *      // This will be the independent term for phase equation
       *   Variable prefitL( TypeID::prefitL );
       *
       *
       *      // DESCRIBE EQUATIONS
       *
       *      // Define Equation object for code equations, and add variables
       *   Equation equPC( prefitC );
       *
       *   equPC.addVariable(dx);
       *   equPC.addVariable(dy);
       *   equPC.addVariable(dz);
       *   equPC.addVariable(cdt);
       *   equPC.addVariable(tropo);
       *
       *      // Define Equation object for phase equations, and add variables
       *   Equation equLC( prefitL );
       *
       *   equLC.addVariable(dx);
       *   equLC.addVariable(dy);
       *   equLC.addVariable(dz);
       *   equLC.addVariable(cdt);
       *   equLC.addVariable(tropo);
       *   equLC.addVariable(ambi);       // This variable is for phase only
       *
       *      // Phase equations should have higher relative weight
       *   equLC.setWeight(10000.0);     // 100.0 * 100.0
       *
       *
       *      // SETUP EQUATION SYSTEM
       *
       *      // Create 'EquSys' object
       *   EquSys eqSystem;
       *
       *      // Add equation descriptions
       *   eqSystem.addEquation(equPC);
       *   eqSystem.addEquation(equLC);
       *
       * @endcode
       *
       * In this way, rather complex processing strategies may be set up in a
       * handy and flexible way.
       *
       * \warning Please be aware that this class requires a significant amount
       * of overhead. Therefore, if your priority is execution speed you should
       * either use the already provided 'purpose-specific' solvers (like
       * 'SolverPPP' or hand-code your own class. For new processing strategies
       * you should balance the importance of machine time (extra overhead)
       * versus researcher time (writing a new solver).
       *
       * @sa Variable.hpp, Equation.hpp.
       *
       */
   class EquSys
   {
   public:

         /// Default constructor
      EquSys()
         : isPrepared(false)
      {};


         /** Add a new equation to be managed.
          *
          * @param equation   Equation object to be added.
          */
      virtual EquSys& addEquation( const Equation& equation );


         /** Remove an Equation being managed. In this case the equation is
          *  identified by its independent term.
          *
          * @param indterm  Variable object of the equation independent term
          *                 (measurement type).
          *
          * \warning All Equations with the same independent term will be
          *          erased.
          */
      virtual EquSys& removeEquation( const Variable& indterm );


         /// Remove all Equation objects from this EquSys.
      virtual EquSys& clearEquations();


         /** Prepare this object to carry out its work.
          *
          * @param gData   Map of GNSS data structures (GDS), indexed
          *                by SourceID.
          */
      virtual EquSys& Prepare( sourceRinex& gData);


      // setUp index for current variables
      virtual void setUpIndex();


         /** Return the TOTAL number of variables being processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquSys exception.
          */
      virtual int getTotalNumVariables() const
         noexcept(false);


         /** Return the set containing all variables being processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquSys exception.
          */
      virtual VariableSet getVarUnknowns() 
         noexcept(false);



         /** Return the CURRENT number of sources, given the current equation
          *  system definition and the GDS's involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquSys exception.
          */
      virtual int getCurrentNumSources() const
         noexcept(false);


         /** Return the set containing sources being currently processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquSys exception.
          */
      virtual SourceIDSet getCurrentSources() const
         noexcept(false);


         /** Return the CURRENT number of satellites, given the current equation
          *  system definition and the GDS's involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquSys exception.
          */
      virtual int getCurrentNumSats() const
         noexcept(false);


         /** Return the set containing satellites being currently processed.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquSys exception.
          */
      virtual SatIDSet getCurrentSats() const
         noexcept(false);


         /** Get prefit residuals GPSTk Vector, given the current equation
          *  system definition and the GDS' involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquSys exception.
          */
      virtual Vector<double> getPrefitsVector() const
         noexcept(false);


         /** Get geometry matrix, given the current equation system definition
          *  and the GDS' involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquSys exception.
          */
      virtual Matrix<double> getGeometryMatrix() const
         noexcept(false);


         /** Get weights matrix, given the current equation system definition
          *  and the GDS' involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquSys exception.
          */
      virtual Matrix<double> getWeightsMatrix() const
         noexcept(false);


         /** Get the State Transition Matrix (PhiMatrix), given the current
          *  equation system definition and the GDS' involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquSys exception.
          */
      virtual Matrix<double> getPhiMatrix() const
         noexcept(false);


         /** Get the Process Noise Covariance Matrix (QMatrix), given the
          *  current equation system definition and the GDS' involved.
          *
          * \warning You must call method Prepare() first, otherwise this
          * method will throw an InvalidEquSys exception.
          */
      virtual Matrix<double> getQMatrix() const
         noexcept(false);


         /// Get the number of equation descriptions being currently processed.
      virtual int getEquationDefinitionNumber() const
      { return equDescripSet.size(); };

         /// Get the set of current equations.
      virtual int getEquNumber() const
      { return currentEquSet.size(); };

         /// Get the set of current equations.
      virtual std::set<Equation> getDescripEqus() const
      { return equDescripSet; };

         /// Get the set of current equations.
      virtual std::set<Equation> getCurrentEquationsSet() const
      { return currentEquSet; };

      void dumpDescripEquations(std::ostream& os) 
      {
          for(auto it=equDescripSet.begin(); it!=equDescripSet.end(); it++)
          {
              cout << (*it) << endl;
          }
      }

      void dumpEquations(std::ostream& os) 
      {
          for(auto it=currentEquSet.begin(); it!=currentEquSet.end(); it++)
          {
              cout << (*it) << endl;
          }
      }
         /// Destructor
      virtual ~EquSys() {};


   private:
      /// Set containing the DESCRIPTIONS of Equation objects.
      std::set<Equation> equDescripSet;

      /// set of current equations
      std::set<Equation> currentEquSet;

      // Current set of unknowns
      VariableSet currentUnkSet;

         /// Old set of unknowns
      VariableSet oldUnkSet;

         /// Set of all unknowns(std::set is used to keep order)
      std::set<Variable> allUnknowns;

         /// Set of reject unknowns
      VariableSet rejectUnknowns;

         /// Whether or not this EquSys is ready to be used
      bool isPrepared;

         /// Set containing all sources being currently processed
      SourceIDSet currentSourceSet;

         /// Set containing satellites being currently processed
      SatIDSet currentSatSet;

         /// State Transition Matrix (PhiMatrix)
      Matrix<double> phiMatrix;

         /// Process noise covariance matrix (QMatrix)
      Matrix<double> qMatrix;

         /// Geometry matrix
      Matrix<double> hMatrix;

         /// Weights matrix
      Matrix<double> rMatrix;

         /// Measurements vector (Prefit-residuals)
      Vector<double> measVector;

         /// General white noise stochastic model
      static WhiteNoiseModel whiteNoiseModel;

         /// Prepare set of current unknowns and set of current equations
      VariableSet prepareEquations( sourceRinex& gData );

         /// Get current sources (SourceID's) and satellites (SatID's)
      void prepareCurrentSourceSat( sourceRinex& gData );

         /// Compute phiMatrix and qMatrix
      void getPhiQ( const sourceRinex& gData );

         /// Compute prefit residuals vector
      void getPrefitGeometryWeights( sourceRinex& gData );


   }; // End of class 'EquSys'

      //@}

}  // End of namespace gpstk

#endif   // equSys_HPP
