#pragma ident "$id: chaohu, ragnwang* $"
//
// chaohu:  asut,cumt	<chaohu@cumt.edu.cn>
// ragnwang: cumt	<ragwang@cumt.edu.cn>
//

#ifndef AboutOptions_HPP
#define AboutOptions_HPP

#include "GNSSconstants.hpp"
#include "ConfigReader.hpp"
#include "gnssData.hpp"

using namespace gpstk;
using namespace std;
using namespace gpstk::StringUtils;

namespace fippp
{
	class Options
	{
	public:

		struct options_t
		{
			string system;
			double begin_sod;
			double end_sod;
			double elev;
			int    mode;
			int    PPPModel;
			int    ionoopt;
			int    bds2;
			int    bdsfopt;
			int    bdsComb;
			int    codeOnly;
			std::string exPrns;
			int    sppDcbCorr;
			int	   obsCorr;
			int    ifcbCorr;
			int    EstimateBias;
			int    ambFixMode;
			int    ambProduct;
			int    fixNLMethod;
			int    nFixUnk;
			int    fixFreq;
			int    floatFix;
			int    frqs;
			std::map<SatelliteSystem, double> errL;
			std::map<SatelliteSystem, double> errratio;
			double thresAmb;
			double thresBS;
			double thresLAMDA;
			int sampleIntrval;
			int isDir;
			string dir;
			/////////// output//////
			string aimOutPath;
			string outdir;
			int	   outPos;
			int	   outENU;
			int	   outState;
			int	   outEle;
			int	   outTrop;
			int	   outAmb;
			int	   outResP;
			int	   outResL;
			int	   outPdop;
			int	   trace;
			int	   outAmbF;

			/////// files //////////
			string leapSecFileName;
			string jplFileName;
			string atxFileName;
			string blqFileName;

			string obsFile;
			string ephFile;
			string biasFile;
			string xyzFile;
			
			string ifcbFile;
			string sp3File;
			string clkFile;
			string erpFile;
			string ewlUPD;
			string wlUPD;
			string nlUPD;
		};

		

		Options() {};

		/// <summary>
		/// iono options
		/// </summary>
		enum Ionoopts
		{
			IonUnKnown = 0,
			UC1,
			UC12,
			IF12,
			UC123,
			IF1213,
			IF123,
			UC1234,
			IF121314,
			IF1234,
			UC12345,
			IF12131415,
			IF12345,

			ionCount
		};
		enum ObsCorr
		{
			NONE = 0,
			DCB,
			OSB,

			obsCorrCount
		};
		/// <summary>
		/// bds 3 frequency combination
		/// </summary>
		enum BdsCombs
		{
			OFF	= 0,
			CCI,
			CI,

			bcomCount
		};
		/// <summary>
		/// position modes
		/// </summary>
		enum ProcessModes
		{
			Unknown = 0,
			SPP,
			PPP_STA,
			PPP_KIN,

			modeCount
		};
		enum PPPModel
		{
			None = 0,
			IFCB_IFB,
			IFCB,
			IFB,

			modelCount
		};
		/// <summary>
		/// ambFixMode opts
		/// </summary>
		enum AmbFixModes
		{
			Float = 0,
			SDIFILS,
			SDIFROUND,
			SDUCILS,
			SDUCROUND,

			fixCount
		};
		enum AmbProducts
		{
			ProOFF=0,
			IRC=1,
			UPD,
			ambPCount
		};
		enum AmbFixFrqs
		{
			ZeroFrq=0,
			DFrqs,
			TFrqs,

			frqCount
		};

		enum OutputModes
		{
			DXYZ=0,
			DENU,
			DNEU,

			outCount
		};

		string ambFixModeAsStr() const
		{
			return ambFixModeStrs[ambFixMode];
		};

		string ambFixFrqAsStr() const
		{ return ambFixFreqStrs[ambFixFrq]; }

		string ionAsString() const
		{
			return ionoptStrs[ionoopt];
		}

		string bCobAsString() const
		{
			return bdsCombStrs[bdsComb];
		}

		string modeAsString() const
		{
			return procModeStrs[processMode];
		}

		Options& setIonoopt(const Ionoopts& opt);

		Options& setBdsComb(const BdsCombs& opt);

		Options& setProcMode(const ProcessModes& opt);

		Options& ionFromStr(const string& str);

		Options& bCobFromStr(const string& str);

		Options& modeFromStr(const string& str);

		Options& ambFixFromStr(const string& str);

		Options& ambFixFrqFromStr(const string& str);

		Options& ionFromInt(const int& num);

		Options& bComFromInt(const int& num);

		Options& modeFromInt(const int& num);

		Options& ambFixFromInt(const int& num);

		Options& ambFixFrqFromInt(const int& num);

		Ionoopts getIonoopts() const
		{return ionoopt;}

		BdsCombs getBdsCombs() const
		{return bdsComb;}

		ProcessModes getProcessMode() const
		{return processMode;}

		AmbFixModes getAmbFixMode() const
		{ return ambFixMode; }

		AmbFixFrqs getAmbFixFrq() const
		{ return ambFixFrq; }

		std::string cfgfile;

		void open(const string& file)
		{
			cfgfile = file;
			confReader.open(file);
		}

		void getCfgValues(options_t& opts)
			noexcept(false);


		//check obs tpye
		bool checkObsType(const Rinex3ObsHeader& rHeader,
			const options_t& opts);

		virtual ~Options() {};

	private:

		Ionoopts ionoopt;
		BdsCombs bdsComb;
		ProcessModes processMode;
		AmbFixModes ambFixMode;
		AmbFixFrqs ambFixFrq;

		static const string ionoptStrs[];
		static const string bdsCombStrs[];
		static const string procModeStrs[];
		static const string ambFixModeStrs[];
		static const string ambFixFreqStrs[];

		ConfigReader confReader;
	};
}

#endif // !AboutOptions_HPP
