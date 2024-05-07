
/**
 * @file Equation.cpp
 * GNSS Data Structure to define and handle 'descriptions' of GNSS equations.
 */

#include "Equation.hpp"


namespace gpstk
{


      // Assignment operator for equationHeader
   equationHeader& equationHeader::operator=(const equationHeader& right)
   {

      if ( this == &right )
      {
         return (*this);
      }

      equationSource = right.equationSource;
      equationSat = right.equationSat;
      equationSourceSet = right.equationSourceSet;
      equationSatSet = right.equationSatSet;
      indTerm = right.indTerm;
      satSys = right.satSys;
      orderIndex = right.orderIndex;
      constWeight = right.constWeight;

      return (*this);

   }  // End of 'equationHeader::operator='



      // Default constructor.
   Equation::Equation()
   {

      header.equationSource = Variable::allSources;
      header.equationSat = Variable::allSats;
      header.constWeight = 1.0;

   }  // End of 'Equation::Equation()'



      /* Common constructor. It defines an Equation from its independent
       * term. You must later use other methods to input the variables.
       *
       * @param indep     Variable object describing the independent term.
       */
   Equation::Equation( const Variable& indep , SatelliteSystem sys)
   {

      header.equationSource = Variable::allSources;
      header.equationSat = Variable::allSats;
      header.indTerm = indep;
      header.satSys = sys;
      header.constWeight = 1.0;

   }  // End of 'Equation::Equation()'



      /* Common constructor. It defines an Equation from its independent
       * term. You must later use other methods to input the variables.
       *
       * @param var     TypeID object describing the independent term.
       */
   Equation::Equation( const TypeID& type, 
                       SatelliteSystem sys,
                       const int& index)
   {

      header.equationSource = Variable::allSources;
      header.equationSat = Variable::allSats;
      header.indTerm.setType(type);
      header.satSys = sys;
      header.constWeight = 1.0;
      header.orderIndex = index;

   }  // End of 'Equation::Equation()'



}  // End of namespace gpstk
