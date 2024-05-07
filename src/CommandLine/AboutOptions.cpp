#pragma ident "$ chaohu, ragnwang* $"

#include "AboutOptions.hpp"

namespace fippp
{


	void Options::getCfgValues(options_t& opts)
		noexcept(false)
	{
		try
		{
			opts.system = confReader.getValue("system", "DEFAULT");
			opts.begin_sod = confReader.getValueAsDouble("begin_sod", "DEFAULT");
			opts.end_sod = confReader.getValueAsDouble("end_sod", "DEFAULT");
			opts.elev = confReader.getValueAsDouble("elevation", "DEFAULT");
			opts.mode = confReader.getValueAsInt("Mode", "DEFAULT");
			opts.sppDcbCorr = confReader.getValueAsInt("SPPDCBCorr", "DEFAULT");
			opts.PPPModel = confReader.getValueAsInt("PPPModel", "DEFAULT");
			opts.ionoopt = confReader.getValueAsInt("ionoopt", "DEFAULT");
			opts.bds2 = confReader.getValueAsInt("bds2", "DEFAULT");
			opts.bdsfopt = confReader.getValueAsInt("bdsfopt", "DEFAULT");
			opts.bdsComb = confReader.getValueAsInt("bdsComb", "DEFAULT");
			opts.codeOnly = confReader.getValueAsInt("codeOnly", "DEFAULT");
			opts.exPrns = confReader.getValue("exPrns", "DEFAULT");
			opts.obsCorr = confReader.getValueAsInt("obsCorr", "DEFAULT");
			opts.ifcbCorr = confReader.getValueAsInt("ifcbCorr", "DEFAULT");
			opts.ambFixMode = confReader.getValueAsInt("ambFixModel", "DEFAULT");/////////////////
			opts.ambProduct = confReader.getValueAsInt("ambProduct", "DEFAULT");/////////////////
			opts.nFixUnk = confReader.getValueAsInt("UnkFixNum", "DEFAULT");/////////////////
			opts.fixFreq = confReader.getValueAsInt("fixFreqs", "DEFAULT");/////////////////
			opts.floatFix = confReader.getValueAsInt("FloatFixPos", "DEFAULT");/////////////////
			opts.errratio[SatelliteSystem::GPS] = confReader.getValueAsDouble("errratioG", "DEFAULT");
			opts.errratio[SatelliteSystem::Galileo] = confReader.getValueAsDouble("errratioE", "DEFAULT");
			opts.errratio[SatelliteSystem::BeiDou] = confReader.getValueAsDouble("errratioC", "DEFAULT");
			opts.errratio[SatelliteSystem::Glonass] = confReader.getValueAsDouble("errratioR", "DEFAULT");
			opts.errratio[SatelliteSystem::QZSS] = confReader.getValueAsDouble("errratioJ", "DEFAULT");
			opts.errL[SatelliteSystem::GPS] = confReader.getValueAsDouble("errmeasLG", "DEFAULT");
			opts.errL[SatelliteSystem::Galileo] = confReader.getValueAsDouble("errmeasLE", "DEFAULT");
			opts.errL[SatelliteSystem::BeiDou] = confReader.getValueAsDouble("errmeasLC", "DEFAULT");
			opts.errL[SatelliteSystem::Glonass] = confReader.getValueAsDouble("errmeasLR", "DEFAULT");
			opts.errL[SatelliteSystem::QZSS] = confReader.getValueAsDouble("errmeasLJ", "DEFAULT");
			opts.thresAmb = confReader.getValueAsDouble("ambThreshold", "DEFAULT");
			opts.thresBS = confReader.getValueAsDouble("BootstrapThreshhold", "DEFAULT");
			opts.thresLAMDA = confReader.getValueAsDouble("lamdaThreshold", "DEFAULT");
			opts.sampleIntrval = confReader.getValueAsDouble("sampleInterval", "DEFAULT");
			opts.isDir = confReader.getValueAsInt("FileDir", "DEFAULT");
			opts.dir = confReader.getValue("folder", "DEFAULT");
			opts.EstimateBias = confReader.getValueAsInt("EstimateBias", "DEFAULT");
			///////////////////// Output /////////////////////////////
			opts.outdir = confReader.getValue("outdir", "DEFAULT");
			opts.outPos = confReader.getValueAsInt("outPos", "DEFAULT");
			opts.outENU = confReader.getValueAsInt("outENU", "DEFAULT");
			opts.outState = confReader.getValueAsInt("outState", "DEFAULT");
			opts.outEle = confReader.getValueAsInt("outEle", "DEFAULT");
			opts.outTrop = confReader.getValueAsInt("outTrop", "DEFAULT");
			opts.outAmb = confReader.getValueAsInt("outAmb", "DEFAULT");
			opts.outResP = confReader.getValueAsInt("outResP", "DEFAULT");
			opts.outResL = confReader.getValueAsInt("outResL", "DEFAULT");
			opts.outPdop = confReader.getValueAsInt("outPdop", "DEFAULT");
			opts.trace = confReader.getValueAsInt("trace", "DEFAULT");
			//opts.outAmbF = confReader.getValueAsInt("outAmbF", "DEFAULT");

			//////////////// File ////////////////////
			opts.leapSecFileName = confReader.getValue("leapSecFileName", "DEFAULT");
			opts.jplFileName = confReader.getValue("jplFileName", "DEFAULT");
			opts.atxFileName = confReader.getValue("atxFileName", "DEFAULT");
			opts.blqFileName = confReader.getValue("blqFileName", "DEFAULT");
			opts.obsFile = confReader.getValue("obsFile", "DEFAULT");
			opts.ephFile = confReader.getValue("ephFile", "DEFAULT");
			opts.xyzFile = confReader.getValue("xyzFile", "DEFAULT");
			opts.biasFile = confReader.getValue("biasFile", "DEFAULT");
			opts.ifcbFile = confReader.getValue("ifcbFile", "DEFAULT");
			opts.sp3File = confReader.getValue("sp3File", "DEFAULT");
			opts.clkFile = confReader.getValue("clkFile", "DEFAULT");
			opts.erpFile = confReader.getValue("erpFile", "DEFAULT");
			opts.ewlUPD = confReader.getValue("EWLUPDFile", "DEFAULT");
			opts.wlUPD = confReader.getValue("WLUPDFile", "DEFAULT");
			opts.nlUPD = confReader.getValue("NLUPDFile", "DEFAULT");
		}
		catch (Exception& e)
		{
			cerr << e << endl;
			exit(-1);
		}
	}

	const string Options::ionoptStrs[ionCount] =
	{
		string("OFF"),
		string("UC1"),
		string("UC12"),
		string("IF12"),
		string("UC123"),
		string("IF1213"),
		string("IF123"),
		string("UC1234"),
		string("IF121314"),
		string("IF1234"),
		string("UC12345"),
		string("IF12131415"),
		string("IF12345"),
	};

	const string Options::bdsCombStrs[bcomCount] =
	{
		string("OFF"),
		string("CCI"),
		string("CI"),
	};

	const string Options::procModeStrs[modeCount] =
	{
		string("Unknown"),
		string("SPP"),
		string("PPP_STA"),
		string("PPP_KIN"),
	};

	const string Options::ambFixModeStrs[fixCount] =
	{
		string("Float"),
		string("SDIFILS"),
		string("SDIFROUND"),
		string("SDUCILS"),
		string("SDUCROUND"),
	};

	const string Options::ambFixFreqStrs[frqCount] =
	{
		string("ZF"),
		string("DF"),
		string("TF"),
	};

	Options& Options::setIonoopt(const Ionoopts& opt)
	{
		if (opt < 0 || opt >= ionCount)
			ionoopt = IonUnKnown;
		else
			ionoopt = opt;

		return (*this);
	}

	Options& Options::setProcMode(const ProcessModes& opt)
	{
		if (opt < 0 || opt >= modeCount)
			processMode = Unknown;
		else
			processMode = opt;

		return (*this);
	}

	Options& Options::setBdsComb(const BdsCombs& opt)
	{
		if (opt < 0 || opt >= bcomCount)
			bdsComb = OFF;
		else
			bdsComb = opt;

		return (*this);
	}


	Options& Options::ionFromStr(const string& str)
	{
		ionoopt = IonUnKnown;
		for (int i = 0; i < ionCount; i++)
		{
			if (ionoptStrs[i] == str)
			{
				ionoopt = static_cast<Ionoopts>(i);
				break;
			}
		}
		return (*this);
	}

	Options& Options::bCobFromStr(const string& str)
	{
		bdsComb = OFF;
		for (int i = 0; i < bcomCount; i++)
		{
			if (bdsCombStrs[i] == str)
			{
				bdsComb = static_cast<BdsCombs>(i);
				break;
			}
		}
		return (*this);
	}

	Options& Options::modeFromStr(const string& str)
	{
		processMode = Unknown;
		for (int i = 0; i < modeCount; i++)
		{
			if (procModeStrs[i] == str)
			{
				processMode = static_cast<ProcessModes>(i);
				break;
			}
		}
		return (*this);
	}

	Options& Options::ambFixFromStr(const string& str)
	{
		ambFixMode = Float;
		for (int i = 0; i < fixCount; i++)
		{
			if (ambFixModeStrs[i] == str)
			{
				ambFixMode = static_cast<AmbFixModes>(i);
				break;
			}
		}
		return (*this);
	}

	Options& Options::ambFixFrqFromStr(const string& str)
	{
		ambFixFrq = ZeroFrq;
		for (int i = 0; i < frqCount; i++)
		{
			if (ambFixFreqStrs[i] == str)
			{
				ambFixFrq = static_cast<AmbFixFrqs>(i);
				break;
			}
		}
		return (*this);
	}

	Options& Options::ionFromInt(const int& num)
	{
		if (num < 0 || num > ionCount)
			ionoopt = IonUnKnown;
		else
			ionoopt = static_cast<Ionoopts>(num);

		return (*this);
	}

	Options& Options::bComFromInt(const int& num)
	{
		if (num < 0 || num > ionCount)
			bdsComb = OFF;
		else
			bdsComb = static_cast<BdsCombs>(num);

		return (*this);
	}

	Options& Options::modeFromInt(const int& num)
	{
		if (num < 0 || num > modeCount)
			processMode = Unknown;
		else
			processMode = static_cast<ProcessModes>(num);

		return (*this);
	}

	Options& Options::ambFixFromInt(const int& num)
	{
		if (num < 0 || num > fixCount)
			ambFixMode = Float;
		else
			ambFixMode = static_cast<AmbFixModes>(num);

		return(*this);
	}

	Options& Options::ambFixFrqFromInt(const int& num)
	{
		if (num < 0 || num > fixCount)
			ambFixFrq = ZeroFrq;
		else
			ambFixFrq = static_cast<AmbFixFrqs>(num);

		return(*this);
	}


	bool Options::checkObsType(const Rinex3ObsHeader& gHeader, 
		const options_t& opts)
	{
		std::map<std::string, TypeIDSet> sysObsType;
		// change obs type into C1PC->C1C
		auto itTypes = gHeader.mapObsTypes.begin();
		for (itTypes;
			itTypes != gHeader.mapObsTypes.end();
			++itTypes)
		{
			std::string sys(itTypes->first);
			for (auto itType = itTypes->second.begin();
				itType != itTypes->second.end();
				++itType)
			{
				std::string typeStr((*itType).asString());
				try
				{
					TypeID type(typeStr.substr(0, 2) + sys);
					sysObsType[sys].insert(type);
				}
				catch (...)
				{
					continue;
				}
			}	// End of for itType
		}	//End of for itTypes
		std::string reqSys(opts.system);
		int ionopt(opts.ionoopt), bds2(opts.bds2), bdsfopt(opts.bdsfopt);
		
		TypeIDSet bdsTypes, gpsTypes, galTypes;
		if (ionopt == Options::UC1 || ionopt == Options::IF12 ||
			ionopt == Options::IF123 || ionopt == Options::IF1234 ||
			ionopt == Options::IF12345 || opts.bdsComb
			)
		{
			if (ionopt >= Options::UC1)
			{
				gpsTypes.insert(TypeID::C1G);
				gpsTypes.insert(TypeID::L1G);
				galTypes.insert(TypeID::C1E);
				galTypes.insert(TypeID::L1E);
				//
				if (bdsfopt)
				{
					bdsTypes.insert(TypeID::C1C);
					bdsTypes.insert(TypeID::L1C);
				}
				else
				{
					bdsTypes.insert(TypeID::C2C);
					bdsTypes.insert(TypeID::L2C);
				}
			}
			if (ionopt >= Options::UC12)
			{
				gpsTypes.insert(TypeID::C2G);
				gpsTypes.insert(TypeID::L2G);
				galTypes.insert(TypeID::C5E);
				galTypes.insert(TypeID::L5E);
				//
				if (bdsfopt)
				{
					bdsTypes.insert(TypeID::C5C);
					bdsTypes.insert(TypeID::L5C);
				}
				else
				{
					bdsTypes.insert(TypeID::C6C);
					bdsTypes.insert(TypeID::L6C);
				}
			}
			if (ionopt >= Options::UC123)
			{
				gpsTypes.insert(TypeID::C5G);
				gpsTypes.insert(TypeID::L5G);
				galTypes.insert(TypeID::C7E);
				galTypes.insert(TypeID::L7E);
				//
				if (bdsfopt)
				{
					bdsTypes.insert(TypeID::C2C);
					bdsTypes.insert(TypeID::L2C);
				}
				else
				{
					bdsTypes.insert(TypeID::C1C);
					bdsTypes.insert(TypeID::L1C);
					if (bds2)
					{
						bdsTypes.insert(TypeID::C7C);
						bdsTypes.insert(TypeID::L7C);
					}
				}
			}
			if (ionopt >= Options::UC1234)
			{
				galTypes.insert(TypeID::C8E);
				galTypes.insert(TypeID::L8E);
				//
				if (bdsfopt)
				{
					bdsTypes.insert(TypeID::C6C);
					bdsTypes.insert(TypeID::L6C);
				}
				else
				{
					bdsTypes.insert(TypeID::C5C);
					bdsTypes.insert(TypeID::L5C);
				}
			}
			if (ionopt >= Options::UC12345)
			{
				galTypes.insert(TypeID::C6E);
				galTypes.insert(TypeID::L6E);
				bdsTypes.insert(TypeID::C8C);
				bdsTypes.insert(TypeID::L8C);
			}
			if (opts.bdsComb)
			{
				bdsTypes.insert(TypeID::C2C);
				bdsTypes.insert(TypeID::L2C);
				bdsTypes.insert(TypeID::C1C);
				bdsTypes.insert(TypeID::L1C);
				bdsTypes.insert(TypeID::C6C);
				bdsTypes.insert(TypeID::L6C);
				bdsTypes.insert(TypeID::C5C);
				bdsTypes.insert(TypeID::L5C);
				bdsTypes.insert(TypeID::C8C);
				bdsTypes.insert(TypeID::L8C);
			}
		}
		else
			return true;

		/// check 
		if (reqSys.find("G")!=std::string::npos)
		{
			auto itObsTypes = sysObsType["G"];
			for (auto itReqType = gpsTypes.begin();
				itReqType != gpsTypes.end();
				++itReqType)
			{
				if (itObsTypes.find(*itReqType) == itObsTypes.end())
				{
					std::cout << "This Obs File is missing ' "
						<< *itReqType << "' type."
						<< " Cannot be processed!" << std::endl;
					return false;
				}
			}
		}
		if (reqSys.find("C")!=std::string::npos)
		{
			auto itObsTypes = sysObsType["C"];
			for (auto itReqType = bdsTypes.begin();
				itReqType != bdsTypes.end();
				++itReqType)
			{
				if (itObsTypes.find(*itReqType) == itObsTypes.end())
				{
					std::cout << "This Obs File is missing ' "
						<< *itReqType << "' type."
						<< " Cannot be processed!" << std::endl;
					return false;
				}
			}
		}
		if (reqSys.find("E")!=std::string::npos)
		{
			auto itObsTypes = sysObsType["E"];
			for (auto itReqType = galTypes.begin();
				itReqType != galTypes.end();
				++itReqType)
			{
				if (itObsTypes.find(*itReqType) == itObsTypes.end())
				{
					std::cout << "This Obs File is missing ' "
						<< *itReqType << "' type."
						<< " Cannot be processed!" << std::endl;
					return false;
				}
			}
		}
		return true;
	}	// End of checkObsType


//	bool directory_exists(const char* path)
//	{
//		struct stat info;
//		if (stat(path, &info) != 0) {
//			return false;
//		}
//		return (info.st_mode & S_IFDIR) != 0;
//	}
}