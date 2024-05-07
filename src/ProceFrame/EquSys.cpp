#pragma ident "$Id$"

/**
 * @file EquSys.cpp
 * Class to define and handle complex equation systems for solvers.
 */

//============================================================================
//
//  History 
//
//  Modify of the Method "getPhiQ()". The gnssRinex is needed to 'Prepare' 
//  the stochastic model. When the variable is source-indexed, the gnssRinex 
//  can be given by finding the source in gData, but when the variable is 
//  only satellite-indexed, the gnssRinex is the first gnssRinex which "SEE"
//  this satellite.
//
//  Change 
//  "currentUnkSet.insert( currentUnknowns.begin(), currentUnknowns.end() );"
//  to 
//  "currentUnkSet = currentUnknowns;"
//
//  Re-design the whole equation system for SolverGeneral/SolverGeneral2
//
//  modify prepareEquations();
// 
//  modify Prepare
//
//============================================================================


#include "SystemTime.hpp"
#include "EquSys.hpp"
#include <iterator>
#include "Epoch.hpp"
#include "TimeString.hpp"

#define debug 0

namespace gpstk
{



      // General white noise stochastic model
   WhiteNoiseModel EquSys::whiteNoiseModel;


      /* Add a new equation to be managed.
       *
       * @param equation   Equation object to be added.
       *
       */
   EquSys& EquSys::addEquation( const Equation& equation )
   {

         // Add "equation" to "equDescripSet"
      equDescripSet.insert(equation);

         // We must "Prepare()" this EquSys
      isPrepared = false;

      return (*this);

   }  // End of method 'EquSys::addEquation()'



      /* Remove an Equation being managed. In this case the equation is
       * identified by its independent term.
       *
       * @param indterm  Variable object of the equation independent term
       *                 (measurement type).
       *
       * \warning All Equations with the same independent term will be
       *          erased.
       */
   EquSys& EquSys::removeEquation( const Variable& indterm )
   {

         // Create a backup set
      std::set<Equation> backupSet;

         // Visit each "Equation" in "equDescripSet"
      for( std::set<Equation>::const_iterator itEq =
                                                equDescripSet.begin();
           itEq != equDescripSet.end();
           ++itEq )
      {

            // If current equation has a different independent term, save it
         if ( (*itEq).getIndependentTerm() != indterm )
         {
            backupSet.insert(*itEq);
         }

      }

         // Clear the full contents of this object
      clearEquations();

         // Add each "Equation" in the backup equation set
      for( std::set<Equation>::const_iterator itEq = backupSet.begin();
           itEq != backupSet.end();
           ++itEq )
      {
         addEquation(*itEq);
      }

         // We must "Prepare()" this EquSys again
      isPrepared = false;

      return (*this);

   }  // End of method 'EquSys::removeEquation()'



      // Remove all Equation objects from this EquSys.
   EquSys& EquSys::clearEquations()
   {
         // First, clear the "equDescripSet"
      equDescripSet.clear();

      isPrepared = false;

      return (*this);

   }  // End of method 'EquSys::clearEquations()'


      /* Prepare this object to carry out its work.
       *
       * @param gData     Map of GNSS data structures (GDS), indexed
       *                   by SourceID.
       *
       */
   EquSys& EquSys::Prepare( sourceRinex& gData )
   {

         // Let's prepare current sources and satellites
      prepareCurrentSourceSat(gData); 

         // Prepare set of current unknowns and set of current equations
      currentUnkSet = prepareEquations(gData);

         // Compute phiMatrix and qMatrix
      getPhiQ(gData);


         // Build prefit residuals vector
      getPrefitGeometryWeights(gData);

         // Setup indexs for vars
      setUpIndex();


         // Let's start storing 'current' unknowns set from 'previous' epoch
      oldUnkSet = currentUnkSet;

         // Set this object as "prepared"
      isPrepared = true;

      return (*this);

   }  // End of method 'EquSys::Prepare()'



      // Get current sources (SourceID's) and satellites (SatID's)
   void EquSys::prepareCurrentSourceSat( sourceRinex& gData )
   {

         // Clear "currentSatSet" and "currentSourceSet"
      currentSatSet.clear();
      currentSourceSet.clear();

         // Insert the corresponding SatID's in "currentSatSet"
      currentSatSet = gData.getSatIDSet();

         // Insert the corresponding SourceID's in "currentSourceSet"
      currentSourceSet = gData.getSourceIDSet();

         // Let's return
      return;

   }  // End of method 'EquSys::prepareCurrentSourceSat()'



      // Prepare set of current unknowns and set of current equations
   VariableSet EquSys::prepareEquations( sourceRinex& gData )
   {

         // Let's clear the current equations set
      currentEquSet.clear();

         // Let's create 'currentUnks' set
      VariableSet currentUnks;

         // Let's retrieve the unknowns according to the 
         // equation descriptions and 'gData'

         // Then, iterate through corresponding 'sourceDataMap'
      for( sourceDataMap::const_iterator sdmIter = gData.body.begin();
           sdmIter != gData.body.end();
           ++sdmIter )
      {
          SourceID currentSource( (sdmIter->first));

             // Iterate the satellite and create the equations
          for( satTypeValueMap::const_iterator stvmIter = (*sdmIter).second.begin();
               stvmIter != (*sdmIter).second.end();
               stvmIter++ )
          {
              SatID currentSat( (stvmIter->first));

              typeValueMap tvData (stvmIter->second) ;

                // Visit each "Equation" in "equDescripSet"
              for( std::set<Equation>::const_iterator itEq = equDescripSet.begin();
                   itEq != equDescripSet.end();
                   ++itEq )
              {
                     // We need a copy of current Equation object description
                  Equation tempEquation( (*itEq) );
                  if( tempEquation.getSystem() == currentSat.system )
                  {
                      tempEquation.header.equationSource = currentSource;
                      tempEquation.header.equationSat = currentSat;
                      tempEquation.header.typeValueData = tvData;

                      Variable varIndep = tempEquation.getIndependentTerm();
                      TypeID obsType = varIndep.getType();

                      if(tvData.find(obsType)!=tvData.end())
                      {
                            // Remove all the variables from this equation
                         tempEquation.clear();
                         bool found(true);
                         for( auto vc : (*itEq).body )
                         {
                               // We will work with a copy of current Variable
                            Variable var( vc.first );
                            Coefficient coef( vc.second);
                            if( !coef.forceDefault )
                            {
                                if(tvData.find(coef.coeffType) == tvData.end())
                                {
                                    found = false;
                                }
                            }

                               // Check what type of variable we are working on
                            if( var.getSourceIndexed() )
                            {
                                var.setSource( currentSource );
                            }
                            if( var.getSatIndexed() )
                            {
                                var.setSatellite( currentSat );
                            }
                            if( var.getArcIndexed() )
                            {
                                double arcNum = tvData(TypeID::satArc);
                                var.setArc(arcNum);
                            }

                            currentUnks.insert(var);
                            tempEquation.addVariable(var, coef);

                         }  // End of 'for( VarCoeffMap::const_iterator varIt = ...'

                            // New equation is complete: Add it to 'currentEquSet'
                         if(found)
                         {
                            currentEquSet.insert( tempEquation );
                         }

                      }
                  }

              }  // End of 'for( satTypeValueMap::const_iterator ...'

          }  // End of 'for(std::set<Equation>::const_iterator)'

      }  // End of 'for( sourceDataMap::const_iterator sdmIter = ...'

      if(currentEquSet.size() ==0)
      {
          InvalidEquSys e("equation number is zero!");
          GPSTK_THROW(e);
      }

         // Return set of current unknowns
      return currentUnks;

   }  // End of method 'EquSys::prepareEquations()'



      // Compute PhiMatrix
   void EquSys::getPhiQ( const sourceRinex& gData )
   {
    
         // Set to store the variables that already prepared
      VariableSet varUnkPrepared;

      const int numVar( currentUnkSet.size() );

         // Resize phiMatrix and qMatrix
      phiMatrix.resize( numVar, numVar, 0.0);
      qMatrix.resize( numVar, numVar, 0.0);

         // Let's get current time
      CommonTime epoch(gData.header.epoch);

         // Visit each Equation in "currentEquSet"
      for( std::set<Equation>::const_iterator itEq = currentEquSet.begin();
           itEq != currentEquSet.end();
           ++itEq )
      {
             // Now, let's visit all Variables and the corresponding 
             // coefficient in this equation description
          for( auto vc: (*itEq).body)
          {
                // We will work with a copy of current Variable
             Variable var( vc.first );
             Coefficient coef( vc.second );


                // find this current varialbe in 'varUnkPrepared'
             VariableSet::const_iterator itVar=varUnkPrepared.find( var );

                // If not found, then you should 'prepare' the stochastic model
                // for this variable
             if( itVar == varUnkPrepared.end() )
             {
                SatID varSat(var.getSatellite());
                SourceID varSource(var.getSource());
                typeValueMap tData( (*itEq).header.typeValueData );

                   // Prepare variable's stochastic model
                var.getModel()->Prepare(epoch,
                                        varSource, 
                                        varSat, 
                                        tData);

                   // Now, Let's get the position of this variable in 
                   // 'currentUnkSet'
                int i(0);
                VariableSet::const_iterator itVar1=currentUnkSet.begin();
                while( (*itVar1) != var )
                {
                   i++;
                   itVar1++;
                }
                   // Now, check if this is an 'old' variable
                if( oldUnkSet.find( var ) != oldUnkSet.end() )
                {
                      // This variable is 'old'; compute its phi and q values
                   phiMatrix(i,i) = var.getModel()->getPhi();
                   qMatrix(i,i)   = var.getModel()->getQ();
                }
                else
                {
                      // This variable is 'new', so let's use its initial variance
                      // instead of its stochastic model
                   phiMatrix(i,i) = 0.0;
                   qMatrix(i,i)   = var.getInitialVariance();

                }

                   // At last, insert current variable into 'varUnkPrepared'
                varUnkPrepared.insert(var);

             }  // End of 'if(itVar==varUnkPrepared.end())'
               
          }  // End of 'for( VarCoeffMap::const_iterator varIt = ...'

      }  // End of 'for( std::set<Equation>::const_iterator itEq = ...'


      return;

   }  // End of method 'EquSys::getPhiQ()'


      // Compute prefit residuals vector
   void EquSys::getPrefitGeometryWeights( sourceRinex& gData )
   {

         // Declare temporal storage for values
      std::vector<double> tempPrefit;

         // Total number of the current equations
      int numEqu( currentEquSet.size() );
      int numVar( currentUnkSet.size() );

      if(numVar==0)
      {
         GPSTK_THROW(InvalidEquSys("currentUnkSet is empty, you must set it first!"));
      }

         // Resize hMatrix and rMatrix
      hMatrix.resize( numEqu, numVar, 0.0);
      rMatrix.resize( numEqu, numEqu, 0.0);

         // We need an equation index
      int row(0);

         // Visit each Equation in "currentEquSet"
      for( std::set<Equation>::const_iterator itEq = currentEquSet.begin();
           itEq != currentEquSet.end();
           ++itEq )
      {
            // Get the type value data from the header of the equation
         typeValueMap tData( (*itEq).header.typeValueData );

            // Get the independent type of this equation
         TypeID indepType( (*itEq).header.indTerm.getType() );
            // Temp measurement
         double tempMeas(tData(indepType));

            // First, fill weights matrix
            // Check if current 'tData' has weight info. If you don't want those
            // weights to get into equations, please don't put them in GDS
         if( tData.find(TypeID::weight) != tData.end() )
         {
               // Weights matrix = Equation weight * observation weight
            rMatrix(row,row) = (*itEq).header.constWeight * tData(TypeID::weight);
         }
         else
         {
               // Weights matrix = Equation weight
            rMatrix(row,row) = (*itEq).header.constWeight;
         }

            // Second, fill geometry matrix: Look for equation coefficients
            
            // Now, let's visit all Variables and the corresponding 
            // coefficient in this equation description
         for( auto vc: (*itEq).body )
         {
               // We will work with a copy of current Variable
            Variable var( vc.first );
            Coefficient coef( vc.second );

               // Coefficient values
            double tempCoef(0.0);
            if( coef.forceDefault )
            {
                  // Use default coefficient
               tempCoef = coef.defaultCoefficient;
            }
            else
            {
                  // Look for the coefficient in 'tdata'
/*
                  // Get type of current unknown
               TypeID type( var.getType() );
*/

               TypeID type = coef.coeffType;

                  // Check if this type has an entry in current GDS type set
               if( tData.find(type) != tData.end() )
               {
                     // If type was found, insert value into hMatrix
                  tempCoef = tData(type);
               }
               else
               {
                  cerr << "unknown coefficient for current vars" << endl;
                  InvalidEquSys e("can't find coefficient for current var");
                  exit(-1);
               }

            }  // End of 'if( (*itCol).isDefaultForced() ) ...'

               // Now, Let's get the position of this variable in 
               // 'currentUnkSet'
            int col(0);
            VariableSet::const_iterator itVar2=currentUnkSet.begin();
            while( (*itVar2) != var )
            {
               col++;
               itVar2++;
            }

               // Set the geometry matrix
            hMatrix(row, col) = tempCoef;
              
         }  // End of 'for( VarCoeffMap::const_iterator varIt = ...'

            // insert current 'measurment vector' into 'tempPrefit'
         tempPrefit.push_back(tempMeas);

            // Increment row number
         ++row;

      }  // End of 'for( std::set<Equation>::const_iterator itEq = ...'

         // Then, finally get prefit residuals into appropriate gpstk::Vector
      measVector = tempPrefit;

      return;

   }  // End of method 'EquSys::getPrefitGeometryWeights()'


      /* Return the TOTAL number of variables being processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquSys exception.
       */
   int EquSys::getTotalNumVariables() const
      noexcept(false)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquSys("EquSys is not prepared"));
      }

      return currentUnkSet.size();

   }  // End of method 'EquSys::getTotalNumVariables()'



      /* Return the set containing all variables being processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquSys exception.
       */
   VariableSet EquSys::getVarUnknowns() 
      noexcept(false)
   {
         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquSys("EquSys is not prepared"));
      }

      return currentUnkSet;

   }  // End of method 'EquSys::getVarUnknowns()'



      /* Return the CURRENT number of sources, given the current equation
       * system definition and the GDS's involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquSys exception.
       */
   int EquSys::getCurrentNumSources() const
      noexcept(false)
   {

         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquSys("EquSys is not prepared"));
      }

      return currentSourceSet.size();

   }  // End of method 'EquSys::getCurrentNumSources()'



      /* Return the set containing sources being currently processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquSys exception.
       */
   SourceIDSet EquSys::getCurrentSources() const
      noexcept(false)
   {
         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquSys("EquSys is not prepared"));
      }

      return currentSourceSet;

   }  // End of method 'EquSys::getCurrentSources()'



      /* Return the CURRENT number of satellites, given the current equation
       * system definition and the GDS's involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquSys exception.
       */
   int EquSys::getCurrentNumSats() const
      noexcept(false)
   {
         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquSys("EquSys is not prepared"));
      }
      return currentSatSet.size();

   }  // End of method 'EquSys::getCurrentNumSats()'



      /* Return the set containing satellites being currently processed.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquSys exception.
       */
   SatIDSet EquSys::getCurrentSats() const
      noexcept(false)
   {
         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquSys("EquSys is not prepared"));
      }
      return currentSatSet;

   }  // End of method 'EquSys::getCurrentSats()'



      /* Get prefit residuals GPSTk Vector, given the current equation
       *  system definition and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquSys exception.
       */
   Vector<double> EquSys::getPrefitsVector() const
      noexcept(false)
   {
         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquSys("EquSys is not prepared"));
      }
      return measVector;

   }  // End of method 'EquSys::getPrefitsVector()'



      /* Get geometry matrix, given the current equation system definition
       *  and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquSys exception.
       */
   Matrix<double> EquSys::getGeometryMatrix() const
      noexcept(false)
   {
         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquSys("EquSys is not prepared"));
      }
      return hMatrix;
   }  // End of method 'EquSys::getGeometryMatrix()'



      /* Get weights matrix, given the current equation system definition
       *  and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquSys exception.
       */
   Matrix<double> EquSys::getWeightsMatrix() const
      noexcept(false)
   {
         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquSys("EquSys is not prepared"));
      }
      return rMatrix;

   }  // End of method 'EquSys::getWeightsMatrix()'


      /* Get the State Transition Matrix (PhiMatrix), given the current
       * equation system definition and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquSys exception.
       */
   Matrix<double> EquSys::getPhiMatrix() const
      noexcept(false)
   {
         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquSys("EquSys is not prepared"));
      }
      return phiMatrix;
   }  // End of method 'EquSys::getPhiMatrix()'



      /* Get the Process Noise Covariance Matrix (QMatrix), given the
       * current equation system definition and the GDS' involved.
       *
       * \warning You must call method Prepare() first, otherwise this
       * method will throw an InvalidEquSys exception.
       */
   Matrix<double> EquSys::getQMatrix() const
      noexcept(false)
   {
         // If the object as not ready, throw an exception
      if (!isPrepared)
      {
         GPSTK_THROW(InvalidEquSys("EquSys is not prepared"));
      }
      return qMatrix;

   }  // End of method 'EquSys::getQMatrix()'


   void EquSys::setUpIndex()
   {

       // Then setup index for this epoch
       int nowIndex  = 0;
       VariableSet varSet;
       for( auto &var: currentUnkSet )
       {
           // set old index
           Variable nowVar = var; 
           auto preIter = oldUnkSet.find(nowVar);
           if( preIter != oldUnkSet.end() )
           {
               nowVar.setPreIndex( (*preIter).getNowIndex() );
           }


           /// set current index
           nowVar.setNowIndex( nowIndex );

           /// store the set
           varSet.insert(nowVar);

           /// index increment
           nowIndex++;
       }

       // copy 'varSet' to 'currentUnkSet'
       currentUnkSet = varSet;

   }  // End of method 'SolverPPPGNSS::SetupIndex()'



}  // End of namespace gpstk
