#pragma ident "$Id$"

/**
 * @file Variable.hpp
 * Class to define and handle 'descriptions' of GNSS variables.
 */

#ifndef GPSTK_VARIABLE_HPP
#define GPSTK_VARIABLE_HPP


#include "DataStructures.hpp"
#include "StochasticModel.hpp"



namespace gpstk
{

      /** @addtogroup DataStructures */
      //@{


      /// Class to define and handle 'descriptions' of GNSS variables.
   class Variable
   {
   public:

         /// Default constructor for Variable
      Variable()
          :satArc(0.0)
      {
          TypeID type;   // Unknown/undefined variable type
          Init(type);
      };

         /** Common constructor for Variable.
          *  By default, it is indexed by SourceID.
          *
          * @param type             TypeID of variable.
          * @param sourceIndexed    Whether this variable is SourceID-indexed
          *                         or not. By default, it IS SourceID-indexed.
          * @param satIndexed       Whether this variable is SatID-indexed
          *                         or not. By default, it is NOT.
          * @param variance         Initial variance assigned to this variable.
          * @param coef             Default coefficient assigned.
          * @param forceCoef        Always use default coefficient.
          */
      Variable( const TypeID& type,
                bool sourceIndexed        = true,
                bool satIndexed           = false,
                bool arcIndexed           = false,
                double variance           = 1.0e6,
                double arc                = 0.0,
                int  type_order           = -1,
				int  now_index            = -1, 
                int  pre_index            = -1 );


         /** Common constructor for Variable.
          *  By default, it is indexed by SourceID.
          *
          * @param type             TypeID of variable.
          * @param pModel           Pointer to StochasticModel associated with
          *                         this variable. By default, it is a white
          *                         noise model.
          * @param sourceIndexed    Whether this variable is SourceID-indexed
          *                         or not. By default, it IS SourceID-indexed.
          * @param satIndexed       Whether this variable is SatID-indexed
          *                         or not. By default, it is NOT.
          * @param variance         Initial variance assigned to this variable.
          * @param coef             Default coefficient assigned.
          * @param forceCoef        Always use default coefficient.
          */
      Variable( const TypeID& type,
                StochasticModel* pModel,
                bool sourceIndexed     = true,
                bool satIndexed        = false,
                bool arcIndexed        = false,
                double variance        = 1.0e6,
                double arc             = 0.0,
                int  type_order        = -1,
				int  now_index         = -1, 
                int  pre_index         = -1);


      Variable( const SourceID& source, 
                const SatID& sat,
                const TypeID& type,
                const double& arc,
                int type_order = -1,
                int  now_index = -1,
                int  pre_index = -1
                )
          : varSource(source), varSat(sat), varType(type), 
            satArc(arc), m_type_order(type_order),
            m_now_index(now_index), m_pre_index(pre_index)
      {
      };


         /// Get variable type
      TypeID getType() const
      { return varType; };


         /** Set variable type
          *
          * @param type        New TypeID of variable.
          */
      Variable& setType(const TypeID& type)
      { varType = type; return (*this); };


         /// Get variable model pointer
      StochasticModel* getModel() const
      { return pVarModel; };


         /** Set variable model
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    this variable. By default, it is a white
          *                    noise model.
          */
      Variable& setModel(StochasticModel* pModel)
      { pVarModel = pModel; return (*this); };


         /// Get if this variable is SourceID-indexed
      bool getSourceIndexed() const
      { return isSourceIndexed; };


         /** Set if this variable is SourceID-indexed.
          *
          * @param sourceIndexed    Whether this variable is SourceID-indexed
          *                         or not. By default, it IS SourceID-indexed.
          */
      Variable& setSourceIndexed(bool sourceIndexed)
      { isSourceIndexed = sourceIndexed; return (*this); };


         /// Get if this variable is SatID-indexed.
      bool getSatIndexed() const
      { return isSatIndexed; };


         /** Set if this variable is SatID-indexed.
          *
          * @param satIndexed       Whether this variable is SatID-indexed
          *                         or not. By default, it is NOT.
          */
      Variable& setSatIndexed(bool satIndexed)
      { isSatIndexed = satIndexed; return (*this); };


      	/// Get if this variable is Arc-indexed.
	  bool getArcIndexed() const
	  { return isArcIndexed; };

	     /** Set if this variable is Arc-indexed.
          * @param arcIndexed       Whether this variable is Arc-indexed
          *                         or not. By default, it is.
          */
      Variable& setArcIndexed(bool arcIndexed)
      { isArcIndexed = arcIndexed; return (*this); };


         /// Get value of initial variance assigned to this variable.
      double getInitialVariance() const
      { return initialVariance; };


         /** Set value of initial variance assigned to this variable.
          *
          * @param variance      Initial variance assigned to this variable.
          */
      Variable& setInitialVariance(double variance)
      { initialVariance = variance; return (*this); };


         /// Get internal source this variable is assigned to (if any).
      SourceID getSource() const
      { return varSource; };


         /** Set internal source this variable is assigned to.
          *
          * @param source     Internal, specific SourceID of variable.
          */
      Variable& setSource(const SourceID& source)
      { varSource = source; return (*this); };



         /// Get internal satellite this variable is assigned to (if any).
      SatID getSatellite() const
      { return varSat; };


         /** Set internal satellite this variable is assigned to.
          *
          * @param satellite  Internal, specific SatID of variable.
          */
      Variable& setSatellite(const SatID& satellite)
      { varSat = satellite; return (*this); };


         /// Get internal satellite this variable is assigned to (if any).
      double getArc() const
      { return satArc; };


         /** Set internal satellite this variable is assigned to.
          *
          * @param satellite  Internal, specific SatID of variable.
          */
      Variable& setArc(const double& arcNum)
      { satArc = arcNum; return (*this); };


         // return satellite system for current variable
      SatelliteSystem getSatSys() const
      {
          return varSat.system; 
      };


      void dump(std::ostream& s) const
      {
          s << dec << "("
            << " m_type_order:" << m_type_order
            << " Source:" << varSource 
            << " sat:" << varSat
            << " Type:" << varType
            << " satArc:" << satArc
            << " nowIndex:" << m_now_index
            << " preIndex:" << m_pre_index 
            << ")";
      }

      void setNowIndex(const int index) 
      {
         m_now_index = index;
      }
     
      void setPreIndex(const int index)
      {
         m_pre_index = index;
      }

      int getNowIndex() const 
      {
         return m_now_index;
      }

      int getPreIndex() const
      {
         return m_pre_index;
      }

      void setTypeOrder(const int index) 
      {
         m_type_order = index;
      }

      int getTypeOrder() const
      {
          return m_type_order;
      }

         /// Equality operator
      virtual bool operator==(const Variable& right) const;


         /// This ordering is somewhat arbitrary, but is required to be able
         /// to use a Variable as an index to a std::map, or as part of a
         /// std::set.
      virtual bool operator<(const Variable& right) const;


         /// Inequality operator
      bool operator!=(const Variable& right) const
      { return !(operator==(right)); }


         /// Assignment operator.
      virtual Variable& operator=(const Variable& right);


         /// SourceID object representing all sources : type(Unknown),
         /// sourceName("").
      static SourceID allSources;


         /// SourceID object representing "some" sources : type(Mixed),
         /// sourceName("").
      static SourceID someSources;


         /// SatID object representing no satellites:
         /// system(systemUnknown), id(-1).
      static SatID noSats;


         /// SatID object representing all satellites:
         /// system(systemMixed), id(-1).
      static SatID allSats;


         /// SatID object representing all satellites of GPS System:
         /// system(systemGPS), id(-1).
      static SatID allGPSSats;


         /// SatID object representing all satellites of Galileo System:
         /// system(systemGalileo), id(-1).
      static SatID allGalileoSats;


         /// SatID object representing all satellites of Glonass System:
         /// system(systemGlonass), id(-1).
      static SatID allGlonassSats;


         /// SatID object representing all satellites of BSD System:
         /// system(systemBDS), id(-1).
      static SatID allBDSSats;


         /// Default stochastic model to be assigned to variables.
      static WhiteNoiseModel defaultModel;


         /// Destructor
      virtual ~Variable() {};


   private:


         /// Type of the variable
      TypeID varType;


         /// Pointer stochastic model applying to variable
      StochasticModel* pVarModel;


         /** Whether this variable is or not source-specific. By default
          *  it is true, which means that this variable is different for
          *  each source (p.e., the TypeID::dx position variable of a given
          *  station).
          */
      bool isSourceIndexed;


         /** Whether this variable is or not satellite-specific. By default
          *  it is false, which means that this variable is the same
          *  independently from the satellites involved (p.e., the TypeID::dx
          *  position variable of a given station).
          *
          *  On the other hand, if this variable is satellite-specific (i.e.,
          *  this field is true), this means that this variable is different
          *  for each satellite on view; for instance, the TypeID::dtSat
          *  satellite clock offset variable.
          */
      bool isSatIndexed;

      
         /** Whether this variable is or not satellite-specific. By default
          *  it is true, which means that this variable is the same
          *  independently from the observation type (p.e., the TypeID::BL1 and TypeID::BL2
          *  variable of L1 and L2).
          *
          *  On the other hand, if this variable is not type-specific (i.e.,
          *  this field is false), this means that this variable is shared among all
          *  the observation type; for instance, the TypeID::ionL1 and the TypeID::ionL2
          */
      bool isTypeIndexed;        
      

      
         /** Whether this varialbe is or not arc-spefici. By default
           * it is false, which means this variable is not ambiguity.
           * On the other hand, this varialbe is ambiguity.
           */
      bool isArcIndexed;


         /// Value of initial variance assigned to this variable.
      double initialVariance;


         /// Value of default coefficient assigned to this variable.
      double defaultCoefficient;


         /// Boolean indicating if default coefficient is always used
      bool forceDefault;


         /// Internal source this variable is assigned to (if any).
      SourceID varSource;


         /// Internal satellite this variable is assigned to (if any).
      SatID varSat;


      /// arc number
      double satArc;


      /// the index about this Variable in variableSet now. 
      int m_now_index;

      /// the index about this Variable in VariableSet previous
      int m_pre_index;

      //  index to sort the variable according this new user-defined order
      int m_type_order;

         /** Initializing function
          *
          * @param type        TypeID of variable.
          * @param pModel      Pointer to StochasticModel associated with
          *                    this variable. By default, it is a white
          *                    noise model.
          * @param variance    Initial variance assigned to this variable.
          * @param coef        Default coefficient assigned.
          * @param forceCoef   Always use default coefficient.
          */
      void Init( const TypeID& type,
                 StochasticModel* pModel,
                 double variance = 4.0e14,
                 double arc      = 0.0,
                 int type_order  = -1,
				 int now_index   = -1,
                 int pre_index   = -1);

      void Init( const TypeID& type,
                 double variance = 4.0e14,
                 double arc      = 0.0,
                 int type_order  = -1,
                 int now_index   = -1,
                 int pre_index   = -1);


   }; // End of class 'Variable'


      /// A structure used to store the coefficent information for a Variable.
   struct Coefficient
   {
         /// Default constructor
      Coefficient()  
          : forceDefault(false), defaultCoefficient(1.0)
      {};

         /// Default constructor
      Coefficient( bool forceCoef,
                   double coef )  
          : forceDefault(forceCoef), defaultCoefficient(coef)
      {};


      Coefficient( TypeID type )  
          : forceDefault(false), coeffType(type)
      {};


      void setForcedCoeff(bool forceCoef, double coef)
      {
          forceDefault = forceCoef;
          defaultCoefficient = coef;
      };

      void setType(TypeID type)
      {
          forceDefault = false;
          coeffType = type;
      };

         /// Boolean indicating if default coefficient is always used
      bool forceDefault;

         /// Value of default coefficient assigned to this variable.
      double defaultCoefficient;

      /// type which stores the coefficient in gnssRinex
      TypeID coeffType;

         /// Equality operator
      bool operator==(const Coefficient& right) const
      {
         return ( ( defaultCoefficient == right.defaultCoefficient )  &&
                  ( forceDefault == right.forceDefault ) &&
                  ( coeffType == right.coeffType ) 
                  );            
      } 

      void dump(std::ostream& s) const
      {
          s << "forceDefault:" << forceDefault 
            << "defaultCoefficient:" << defaultCoefficient
            << "coeffType:" << coeffType
            ;
      }

         /// Destructor
      virtual ~Coefficient() {};

   };

   /// Handy type definition

   typedef std::set<Variable> VariableSet;
   typedef std::list<Variable> VariableList;
   typedef std::vector<Variable> VariableVector;
   typedef std::map<Variable, double> VariableDataMap;
   typedef std::map<Variable, CommonTime> VariableTimeMap;
   typedef std::map<Variable, EstData> VariableEstDataMap;
   typedef std::map<SourceID, VariableEstDataMap> sourceVarEstDataMap;
   typedef std::map<SatID, VariableEstDataMap> satVarEstDataMap;
   typedef std::map<Variable, VariableDataMap> CovarianceDataMap;
   typedef std::map<Variable, Coefficient> VarCoeffMap;
   typedef std::map<CommonTime, VariableSet> epochVarsMap;
   typedef std::map<Variable, std::vector<double> > VariableVectorMap;;

   typedef std::map<CommonTime, VariableEstDataMap> epochVarEstDataMap;

   typedef std::map<TypeID, Variable> typeVarMap;
   typedef std::map<TypeID, VariableSet> typeVarSetMap;
   typedef std::map<SatelliteSystem, typeVarMap> systemTypeVarMap;
   typedef std::map<SatelliteSystem, typeVarSetMap> systemTypeVarsMap;

   typedef std::map<TypeID, VariableDataMap> typeVarDataMap;
   typedef std::map<TypeID, CovarianceDataMap> typeCovDataMap;

   /// stream output for Variable
   inline std::ostream& operator<<(std::ostream& s, const Variable& v)
   {
       v.dump(s);
       return s;
   }

   /// stream output for Variable
   inline std::ostream& operator<<(std::ostream& s, const Coefficient& c)
   {
       c.dump(s);
       return s;
   }

   namespace StringUtils
   {
      inline std::string asString(const Variable& v)
      {
         std::ostringstream oss;
         oss << v.getType() << "   "
             << v.getSource() << "   "
             << v.getSatellite() << "   "
             << v.getArc() << " "
             << v.getSourceIndexed() << " "
             << v.getSatIndexed() << " "
             << v.getArcIndexed() << " "
             ;

         return oss.str();
      }
   }
      //@}

}  // End of namespace gpstk
#endif   // GPSTK_VARIABLE_HPP
