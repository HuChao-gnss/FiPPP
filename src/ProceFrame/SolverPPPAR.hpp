#pragma ident "id: chaohu ragnwang* 2023-4"

//
// This class is for PPPAR
//
#ifndef FiPPP_SolverPPPAR_HPP
#define FiPPP_SolverPPPAR_HPP

#include "AboutOptions.hpp"
#include "gnssData.hpp"
#include "TypeID.hpp"
#include "CommonTime.hpp"
#include "RinexDatum.hpp"
#include "DataStructures.hpp"
#include "RinexSatID.hpp"
#include "SatelliteSystem.hpp"
#include "SatID.hpp"
#include "Variable.hpp"
#include "ARLambda.hpp"
#include "UPDDataReader.hpp"
#include "ComputeDerivative.hpp"
#include "EquSys.hpp"
#include "Trace.hpp"

//#include "Trace.hpp"

#define NFREQ	5
using namespace gpstk;
using namespace gpstk::StringUtils;

namespace fippp
{

	
	class PPPAR : public ARLambda
	{
	public:
		// default
		PPPAR() :
			pSolStream(NULL),
			pTraceStream(NULL)
		{
			ElvSatMap.clear();
			pppUnkSet.clear();
			
		};
		PPPAR(const gnssRinex& Rin,
			const VariableSet& unkSet,
			const Matrix<double>& p) :
			gRin(Rin),
			pppUnkSet(unkSet),
			/*P(p),*/
			pUPDData(NULL)
		{};

		virtual void Process(const gnssRinex& Rin)
		{
			gRin = Rin;
			init();
			Process();
		};

		void printNewLine();

		void setPPPUnkSet(const VariableSet& set)
		{ pppUnkSet = set; }

		void setP(const Matrix<double>& p)
		{ P = p; }

		void setRefPos(const Position& pos)
		{ refPos = pos; }

		void setRcvPos(const Position& pos)
		{ rcvPos = pos; }

		void setOpts(const Options::options_t& opt)
		{
			opts = opt;
			frq = opt.frqs;
		}

		void setStream(std::ofstream* solStream)
		{ pSolStream = solStream; }

		void setTraceStream(std::ofstream* stream)
		{ pTraceStream = stream; }

		void setSolutions(const Vector<double>& sols)
		{ solution = sols; xhatminus = sols;}

		void setPostResiduals(const Vector<double>& residuals)
		{ postResiduals = residuals; }

		void setCurrentEqus(const set<Equation>& equas)
		{ equations = equas; } 

		Vector<double> getXhatminus() const
		{ return xhatminus; }

		Matrix<double> getPminus() const
		{ return P; }

		void setUPD(UPDDataReader& pUpd)
		{ pUPDData = &pUpd; }

		virtual ~PPPAR() {};
	private:
		// typedef variable
		typedef std::multimap<double, SatID> MapElvSat;
		struct SatValue_t
		{
			SatID sat;
			SatID satRef;
			/// <summary>
			/// the index of amb of sat1,2
			/// </summary>
			int index[2 * NFREQ];
			int value[NFREQ];	/// fixed amb value
			double floatVal[NFREQ];	//// float amb value befor fixed
			double var[NFREQ];
			double residual[NFREQ];	/// value-float value
			/// <summary>
			/// single difference ambiguity value between satellites
			/// </summary>
			double sdFloatAmb[NFREQ];
			/// <summary>
			/// lamLC, lam1, lam2, alpha12, beta12
			/// </summary>
			double coeff[5];
		};


		typedef std::multimap<SatID, SatValue_t> SatFixValueMap;
		typedef std::multimap<double, SatValue_t> valueSatFixMap;
		struct mwlc_t
		{
			mwlc_t()
			{
				epoch = CommonTime::BEGINNING_OF_TIME;
				n = 0;
				lamWL = 0.;
				mw = 0.;
				var = 0.;

			}
			CommonTime epoch;
			int n;
			double lamWL;
			double mw;	// Smoothed mw value of the current epoch
			double var;
		};

		struct ARData
		{
			ARData()
			{
				satMWLCValue.clear();
				FixedNn.clear();
				FixedNw.clear();
				sdIFFix.clear();
				NFixed = NFixedNn = NFixedNw = 0;
				ratio = 0.0;
				dxyz = Triple(0., 0., 0.);
			}
			void init()
			{
				FixedNn.clear();
				FixedNw.clear();
				sdIFFix.clear();
				NFixed = NFixedNn = NFixedNw = 0;
				ratio = 0.0;
				dxyz = Triple(0., 0., 0.);
			}
			// sat-mw linear combination data
			std::map<SatID, mwlc_t> satMWLCValue;
			// fixed wide-lane amb sat
			SatFixValueMap FixedNw;
			//satSatIntMap preFixNn;
			SatFixValueMap FixedNn;

			int NFixedNw;	// number of Fixed WL-amb
			int NFixedNn;	// number of Fixed NL-amb
			int NFixed;		// number of Fixed WL-NL amb
			double ratio;	// fixed NL threshold

			//std::map<SatID, SatID> satSat;	// Fixed sd sat-sat
			// fixed solution value
			Position dxyz;
			// Float xyz
			Position dxyzFloat;
			Matrix<double> Pa;
			// Fixed sols
			Vector<double> ya;
			SatFixValueMap sdIFFix;
		};

		struct SatSat{
			SatSat(SatID sat, SatID satRef):
				sat1(sat), sat2(satRef)
			{}

			SatID sat1;
			SatID sat2;

			bool operator==(const SatSat& other) const{
				bool flag1 = (sat1 == other.sat1) && (sat2 == other.sat2);
				bool flag2 = (sat2 == other.sat1) && (sat1 == other.sat2);
				return flag1 || flag2;
			}

			bool operator<(const SatSat& other) const{
				if (sat1 == other.sat1)
					return sat2 < other.sat2;
				return sat1 < other.sat1;
			}
		};
		typedef std::set<SatSat> satSatSet;
		// methods
		// 
		virtual void Process()
			noexcept(false);
		// get the value of float solution
		double getSolution(const TypeID& type) const
			noexcept(false);
		double getSolution(const TypeID& type, const SatID& sat) const
			noexcept(false);
		// process elev
		void setSatElev()
			noexcept(false);
		// MW value
		void smoothMW()
			noexcept(false);

		bool checkResiduals(const SatID& sat, const SatID& satRef)
			noexcept(false);

		double getSatResiduals(const SatID& sat)
			noexcept(false);

		void removeFixedWL(const SatID& sat)
			noexcept(false);

		void checkLinear()
			noexcept(false);

		bool singleDifferenceARProcess()
			noexcept(false);

		bool FixWLAmb(const SatID& sat1, const SatID& sat2, SatValue_t& NW, int& it)
			noexcept(false);

		bool FixIFNLAmb()
			noexcept(false);

		double getFixedWLAmbValue(const SatID& sat1, const SatID& sat2, int flag)
			noexcept(false);

		bool ILSIFFixSol()	noexcept(false);

		void printSols();

		bool LAMBDAFixIFNLAmb()	noexcept(false);

		void resetSols() noexcept(false);

		void init()
		{
			sdNLAmbValue.clear();
			ElvSatMap.clear();
		}
		Variable getVariable(const TypeID& type, const VariableSet& varSet)
			noexcept(false);
		Variable getVariable(const TypeID& type, const SatID& sat, const VariableSet& varSet)
			noexcept(false);

		double q_gamma(double a, double x, double log_gamma_a);
		double p_gamma(double a, double x, double log_gamma_a);
		double f_erfc(double x);
		double conffunc(int N, double B, double sig);


		// variables
		// elmin
		double elmin;
		// cfg options
		Options::options_t opts;
		// frqs
		int frq;
		// current epoch gnss Rinex data
		gnssRinex gRin;
		// float unkset
		VariableSet pppUnkSet;
		/// for NL amb result
		Matrix<double> NLAmbF;	//NLAmbF: nl ambi fixed res
		Vector<double> Bw;

		// satellite-elve map
		MapElvSat ElvSatMap;
		// upd data 
		UPDDataReader* pUPDData;
		// float solution reference position
		Position refPos;
		Position rcvPos;
		// out file stream
		std::ofstream* pSolStream;
		std::ofstream* pTraceStream;
		// epoch ARData
		ARData arData;
		valueSatFixMap sdNLAmbValue;
		/// <summary>
		/// PPP float res
		/// </summary>
		Vector<double> solution;
		Vector<double> postResiduals;
		set<Equation> equations;
		Vector<double> xhatminus;
		Matrix<double> P;
		Matrix<double> Pminus;
	};
}


#endif // !FiPPP_SolverPPPAR_HPP
