#pragma ident "$id: 2023-03-20 ragnwang$"

#ifndef FiPPP_SolverPPPAll_HPP
#define FiPPP_SolverPPPAll_HPP



#include "SolverBase.hpp"
#include "TypeID.hpp"
#include "EquSys.hpp"
#include "StochasticModel.hpp"
#include "gnssData.hpp"
#include "AboutOptions.hpp"
#include "SolverPPPAR.hpp"
#include "XYZ2ENU.hpp"

#include "Trace.hpp"

using namespace gpstk;

namespace fippp
{

	class SolverPPPAll: public SolverBase
	{

		typedef map<SatelliteSystem, LinearCombList> sysLinearCombList;
		typedef map<SatID, map<string, double>> satTypeStrValue;

	public:
		// default constructor
		SolverPPPAll() :firstTime(true),
			forwardProcessing(true),
			firstKalman(true),
			pFloatSolStream(NULL),
			pTraceStream(NULL),
			pFloatStaStream(NULL),
			pFloatAmbStream(NULL),
			pEleAziStream(NULL),
			pResLStream(NULL),
			pPDOPStream(NULL),
			pResPStream(NULL),
			pTropStream(NULL)
		{
			gpsCombTypeSet.clear();
			galCombTypeSet.clear();
			bdsCombTypeSet.clear();
		}
		// Set ambiguity parameter settings according to ionoopt
		virtual SolverPPPAll& setAmbSet();

		virtual SolverPPPAll& setRecClk();

		TypeIDSet getAmbSet() { return ambTypeSet; }
		TypeIDSet getRecClk() { return reclkTypeSet; }

		virtual gnssRinex& Process(gnssRinex& gRin)
		{
			rotationMatrix = gRin.header.enuRotateMatrix;
			preCompute(gRin);
			Compute(gRin);
			postCompute(gRin);
			return gRin;
		}

		virtual void varrPWLC(Matrix<double>& R)
			noexcept(false);

		virtual gnssRinex& preCompute(gnssRinex& gRin)
			noexcept(false);

		/** Returns a reference to a gnssRinex object after computing .
		 *
		 * @param gData    Data object holding the data.
		 */
		virtual gnssRinex& Compute(gnssRinex& gRin)
			noexcept(false);

		virtual gnssRinex& postCompute(gnssRinex& gRin)
			noexcept(false);



		/** Time Update of the kalman filter.
		 *
		 * @param gData    Data object holding the data.
		 */
		virtual int TimeUpdate(const Matrix<double>& phiMatrix,
			const Matrix<double>& processNoiseCovariance)
			noexcept(false);

		/** Measurement Update of the kalman filter.
		 *
		 * @param gData    Data object holding the data.
		 */
		virtual int MeasUpdate(const Vector<double>& prefitResiduals,
			const Matrix<double>& designMatrix,
			const Matrix<double>& weightMatrix)
			noexcept(false);

		/// This method resets the filter, setting all variance values in
		/// covariance matrix to a very high level.
		virtual SolverPPPAll& reset(void)
		{
			firstTime = true;
			return (*this);
		};

		// set receiver position
		virtual SolverPPPAll& setRefPos(const Triple& pos)
		{
			refPos = pos;
			return (*this);
		}
		virtual SolverPPPAll& setRcvPos(const Triple& pos)
		{
			rcvPos = pos;
			return (*this);
		}
		Matrix<double> getP() const
		{ return P; }

		Vector<double> getSols() const
		{ return solution; }

		VariableSet getUnkSet() const
		{ return currentUnkSet; }

		Vector<double> getPostResi() const
		{ return postfitResiduals; }

		set<Equation> getCurrentEqus() const
		{ return equSystem.getCurrentEquationsSet(); }

		VariableSet getVarSet(const TypeID& type, const VariableSet& varSet) const
			noexcept(false);

		/// Returns an index identifying this object.
		virtual void setUpEquations(void);

		void setFwdOutStream(std::ofstream* pStream)
		{
			pFloatSolStream = pStream;
		};

		void setTraceStream(std::ofstream* pStream)
		{ pTraceStream = pStream; }

		void printSols(std::ofstream* outStr,
			const CommonTime& epoch,
			const Vector<double>& stateVec,
			const Matrix<double>& covMatrix,
			int numSat);

		void printNewLine();

		void printOptState(const CommonTime& epoch,
			const gnssRinex& gRin);

		void printTime(std::ofstream* outStr,
			const CommonTime& epoch);

		virtual Variable getVariable(const TypeID& type,
			const VariableSet& varSet) const
			noexcept(false);

		/** Returns the solution associated to a given TypeID.
		 *
		 * @param type    TypeID of the solution we are looking for.
		 *
		 * \warning In the case the solution contains more than one variable
		 * of this type, only the first one will be returned.
		 */
		virtual double getSolution(const TypeID& type,
			const Vector<double>& stateVec) const
			noexcept(false);


		/// Returns an index identifying this object.
		virtual int getIndex(void) const;

		/// Returns a string identifying this object.
		virtual std::string getClassName(void) const;

		void setOpts(const Options::options_t& opt)
		{
			opts = opt;
			codeOnly = opts.codeOnly;
			ionoopt = opts.ionoopt;
			bdsfopt = opts.bdsfopt;
			bds2 = opts.bds2;
			bdsComb = opts.bdsComb;
			kinematic = (opts.mode == Options::PPP_KIN ? true : false);
			outENU = opts.outENU;
			errmeasLG = opts.errL[SatelliteSystem::GPS];
			errmeasLE = opts.errL[SatelliteSystem::Galileo];
			errmeasLC = opts.errL[SatelliteSystem::BeiDou];
			errmeasLR = opts.errL[SatelliteSystem::Glonass];
			errmeasLJ = opts.errL[SatelliteSystem::QZSS];
			errratioC = opts.errratio[SatelliteSystem::BeiDou];
			errratioE = opts.errratio[SatelliteSystem::Galileo];
			errratioG = opts.errratio[SatelliteSystem::GPS];
			errratioR = opts.errratio[SatelliteSystem::Glonass];
			errratioJ = opts.errratio[SatelliteSystem::QZSS];
			
		}	// End of setOpts
		
		void setSysLinear(const sysLinearCombList& sysList)
		{
			sysLinear = sysList;
		};

		void resetMinus(const Vector<double> sols_, const Matrix<double> P_)
		{
			xhatminus = sols_;
			Pminus = P_;
		}

		// default destructor
		virtual ~SolverPPPAll() {
			// free pointer
			pFloatSolStream = NULL;
			delete pFloatSolStream;
			pFloatStaStream = NULL;
			delete pFloatStaStream;
			pEleAziStream = NULL;
			delete pEleAziStream;
			pFloatAmbStream = NULL;
			delete pFloatAmbStream;
			pPDOPStream = NULL;
			delete pPDOPStream;
			pTropStream = NULL;
			delete pTropStream;
			pResLStream = NULL;
			delete pResLStream;
			pResPStream = NULL;
			delete pResPStream;
		};

	private:

		//
		Options::options_t opts;

		// ecef 2 enu rotate matrix
		Matrix<double> rotationMatrix;
		// linear coefficent for modify rMatrix
		sysLinearCombList sysLinear;

		// weight
		double errmeasLG;
		double errmeasLE;
		double errmeasLC;
		double errmeasLR;
		double errmeasLJ;
		
		double errratioG;
		double errratioE;
		double errratioC;
		double errratioR;
		double errratioJ;

		// ionophere options
		int ionoopt;
		// bds3 frequency options
		int bdsfopt;
		int bdsComb;
		bool bds2;
		// receiver position
		Position refPos;
		Position rcvPos;

		// first time filter
		bool firstTime, firstKalman;

		// kinematic
		bool kinematic;
		// forward filter
		bool forwardProcessing;

		//use neu
		bool outENU;
		bool codeOnly;

		double sigH, sigLat, sigLon;
		double sigX, sigY, sigZ;

		bool usingGPS, usingGAL, usingBDS, usingGLO;

		TypeIDSet gpsCombTypeSet, galCombTypeSet, bdsCombTypeSet;
		
		VariableSet currentUnkSet;
		VariableSet oldUnkSet;

		CommonTime firtEpoch;

		std::ofstream* pFloatSolStream;
		std::ofstream* pTraceStream;

		std::ofstream* pFloatAmbStream;
		std::ofstream* pFloatStaStream;
		std::ofstream* pEleAziStream;	// elevation, azimuth, trop and CS
		std::ofstream* pResPStream;
		std::ofstream* pResLStream;
		std::ofstream* pPDOPStream;
		std::ofstream* pTropStream;
		/// matrix for solution

		 /// State Transition Matrix (PhiMatrix)
		Matrix<double> phiMatrix;

		/// Noise covariance matrix (QMatrix)
		Matrix<double> qMatrix;

		/// Geometry matrix
		Matrix<double> hMatrix;

		/// Weights matrix
		Matrix<double> rMatrix;

		/// Measurements vector (Prefit-residuals)
		Vector<double> measVector;

		/// Map holding state information
		VariableDataMap stateMap;

		/// Map holding covariance information
		CovarianceDataMap covMap;

		// Predicted state
		Vector<double> xhatminus;

		// Predicted covariance.
		Matrix<double> Pminus;

		// A bwderiori state
		Vector<double> xhat;

		// A bwderiori covariance.
		Matrix<double> P;

		epochRinexMap forwardRinexData;
		epochVectorMap forwardStateData;
		epochMatrixMap forwardCovData;
		epochPosMap  nominalPosData;

		bool print;

		// Declare coordinate model

		// kinematic
		// xyz
		WhiteNoiseModel whiteNoiseDXModel;
		WhiteNoiseModel whiteNoiseDYModel;
		WhiteNoiseModel whiteNoiseDZModel;

		// static positioning
		// xyz
		StochasticModel constantDXModel;
		StochasticModel constantDYModel;
		StochasticModel constantDZModel;

		// trop
		TropoRandomWalkModel tropoModel;

		// cdt
		WhiteNoiseModel dcdtGPSModel;
		WhiteNoiseModel dcdtGALModel;
		WhiteNoiseModel dcdtBDSModel;

		// ionophere
		IonoRandomWalkModel slantIonoC1GModel;
		IonoRandomWalkModel slantIonoC1EModel;
		IonoRandomWalkModel slantIonoC1CModel;
		IonoRandomWalkModel slantIonoC2CModel;
		

		// ambiguities
			// GPS
		PhaseAmbiguityModel BL1GModel;
		PhaseAmbiguityModel BL2GModel;
		PhaseAmbiguityModel BL5GModel;

		PhaseAmbiguityModel BLC12GModel;
		PhaseAmbiguityModel BLC15GModel;
		PhaseAmbiguityModel BLC125GModel;

			// GAL E1 E5a E5b E5 E6: 15786
		PhaseAmbiguityModel BL1EModel;
		PhaseAmbiguityModel BL5EModel;
		PhaseAmbiguityModel BL7EModel;
		PhaseAmbiguityModel BL8EModel;
		PhaseAmbiguityModel BL6EModel;

		PhaseAmbiguityModel BLC15EModel;
		PhaseAmbiguityModel BLC17EModel;
		PhaseAmbiguityModel BLC18EModel;
		PhaseAmbiguityModel BLC16EModel;

		PhaseAmbiguityModel BLC157EModel;
		PhaseAmbiguityModel BLC1578EModel;
		PhaseAmbiguityModel BLC15786EModel;

			// bds: B1I/B3I/B2I/B1c/B2a/B2: 267158
		PhaseAmbiguityModel BL2CModel;
		PhaseAmbiguityModel BL6CModel;
		PhaseAmbiguityModel BL7CModel;
		PhaseAmbiguityModel BL1CModel;
		PhaseAmbiguityModel BL5CModel;
		PhaseAmbiguityModel BL8CModel;

		PhaseAmbiguityModel BLC26CModel;
		PhaseAmbiguityModel BLC27CModel;
		PhaseAmbiguityModel BLC21CModel;
		PhaseAmbiguityModel BLC61CModel;
		PhaseAmbiguityModel BLC25CModel;
		PhaseAmbiguityModel BLC28CModel;
		// ionophere free one equation
		PhaseAmbiguityModel BLC267CModel;
		PhaseAmbiguityModel BLC261CModel;
		PhaseAmbiguityModel BLC2615CModel;
		PhaseAmbiguityModel BLC26158CModel;

		// bds3: B1c/B2a/B1I/B3I/B2: 15268
		PhaseAmbiguityModel BLC15CModel;
		PhaseAmbiguityModel BLC12CModel;
		PhaseAmbiguityModel BLC52CModel;
		PhaseAmbiguityModel BLC16CModel;
		PhaseAmbiguityModel BLC18CModel;
		// another if combination
		PhaseAmbiguityModel BLC158CModel;
		PhaseAmbiguityModel BLC152CModel;
		PhaseAmbiguityModel BLC1526CModel;
		PhaseAmbiguityModel BLC15268CModel;

		// IFB random walk model
		IFBRandomWalkModel ifbC5GRWModel;
		IFBRandomWalkModel ifbC7CRWModel;
		IFBRandomWalkModel ifbC1CRWModel;
		IFBRandomWalkModel ifbC5CRWModel;
		IFBRandomWalkModel ifbC8CRWModel;
		IFBRandomWalkModel ifbC2CRWModel;
		IFBRandomWalkModel ifbC6CRWModel;
		IFBRandomWalkModel ifbC26CRWModel;
		IFBRandomWalkModel ifbC7ERWModel;
		IFBRandomWalkModel ifbC8ERWModel;
		IFBRandomWalkModel ifbC6ERWModel;
		//ifcb
		IFCBRandomWalkModel ifcbL5GModel;
		IFCBRandomWalkModel ifcbL7CModel;
		IFCBRandomWalkModel ifcbL2CModel;
		IFCBRandomWalkModel ifcbL6CModel;
		IFCBRandomWalkModel ifcbL1CModel;
		IFCBRandomWalkModel ifcbL5CModel;
		IFCBRandomWalkModel ifcbL8CModel;
		IFCBRandomWalkModel ifcbL7EModel;
		IFCBRandomWalkModel ifcbL8EModel;
		IFCBRandomWalkModel ifcbL6EModel;


		/// Equation system
		EquSys              equSystem;

		TypeIDSet           ambTypeSet;

		TypeIDSet			reclkTypeSet;

		/// Initial index assigned to this class.
		static int classIndex;

		/// Index belonging to this object.
		int index;

		/// Sets the index and increment classIndex.
		void setIndex(void)
		{
			index = classIndex++;
		};
		/// <summary>
		///  get the alpha value in the linear combination
		/// </summary>
		/// <param name="linear"></param>
		/// <param name="type"></param>
		/// <returns></returns>
		void getABCoeff(const SatID& sat, const LinearCombList& linear,
			const TypeID& type, double& a, double& b, double& c)
			noexcept(false);		

	};	// end of class 'SolverPPPAll'
}


#endif // !FiPPP_SolverPPPAll_HPP
