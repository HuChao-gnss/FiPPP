#pragma ident "$ chaohu, ragnwang* $"
#include "FiPPPSolver.hpp"
#include "StringProcess.hpp"
#include "CorrectPhaseBiases.hpp"
#include "Counter.hpp"

using namespace std;
using namespace gpstk::StringUtils;

// Let's implement constructor details
positioning_algo::positioning_algo(char *arg0)
    : BasicFramework(arg0, ""),
      // Option initialization. "true" means a mandatory option
	  _configFile(CommandOption::stdType, 'c', "config",
		       " [-c|--config]          Name of config file.", true)
{
    _configFile.setMaxCount(1);
} // End of constructor details

// Method that will be executed AFTER initialization but BEFORE processing
void positioning_algo::spinUp()
{
	try
	{
        //config file
        const std::string& configFile = _configFile.getValue()[0]; 
		opts.open(configFile);
	}
	catch (Exception& e)
	{
		std::cerr << "please put FiPPP.conf in current directory!" << endl;
		std::exit(-1);
	}

}

#define debug 1

void positioning_algo::process()
{
	/*---------------------------- part of reading file------------------------------------ */
	// now, let's read configuation data into variables
	Options::options_t cfgOpts;
	opts.getCfgValues(cfgOpts);
    // estimate bias, code osb and DCB
	if (cfgOpts.EstimateBias)
    {
        std::cout<<"Ready to estimate bias!"<<std::endl;
        EstimateBias(("./"+opts.cfgfile).c_str());
    }
	if (cfgOpts.mode == Options::OFF)
	{
		std::cout << "Please choose Process Mode!" << std::endl;
		exit(0);
	}
	if (cfgOpts.mode!=Options::SPP&&cfgOpts.PPPModel==Options::IFCB&&
		(cfgOpts.ionoopt==Options::IF123||
			cfgOpts.ionoopt==Options::IF1234||
			cfgOpts.ionoopt==Options::IF12345))
	{
		std::cout << "Cannot process PPP, because \
equation redundancy is negative" << std::endl;
		exit(0);
	}

	if (!cfgOpts.bdsfopt)
		cfgOpts.bdsComb = 0;
	if (cfgOpts.bdsfopt)
		cfgOpts.bds2 = 0;

	if (cfgOpts.mode>Options::SPP&&cfgOpts.ambFixMode)
	{
		if(cfgOpts.ambFixMode==Options::SDIFILS||cfgOpts.ambFixMode==Options::SDIFROUND)
			cfgOpts.ionoopt = Options::IF12;
		cfgOpts.ifcbCorr = 0;
		cfgOpts.bdsComb = 0;
		cfgOpts.codeOnly = 0;
		cfgOpts.bdsfopt = 0;
		cfgOpts.fixFreq = Options::DFrqs;
		if (cfgOpts.ambProduct == Options::ProOFF)
			cfgOpts.obsCorr = Options::OSB;
		if (
			(cfgOpts.ambFixMode == Options::SDIFROUND || 
				cfgOpts.ambFixMode == Options::SDUCROUND) &&
			cfgOpts.ambProduct == Options::IRC)
		{
			std::cout << "Unable to fix ambiguity with \
integer clock product combined with ROUND method!" << std::endl;
			exit(0);
		}
	}
	/// set the frqs
	if (cfgOpts.ionoopt == Options::UC1 || cfgOpts.ionoopt == Options::IF12 ||
		cfgOpts.ionoopt == Options::IF123 || cfgOpts.ionoopt == Options::IF1234 ||
		cfgOpts.ionoopt == Options::IF12345)
		cfgOpts.frqs = 1;
	if (cfgOpts.ionoopt == Options::UC12 || cfgOpts.ionoopt == Options::IF1213 ||
		(cfgOpts.system == "C" && cfgOpts.bdsComb == Options::CI))
		cfgOpts.frqs = 2;
	if (cfgOpts.ionoopt == Options::UC123 || cfgOpts.ionoopt == Options::IF121314 ||
		(cfgOpts.system == "C" && cfgOpts.bdsComb == Options::CCI))
		cfgOpts.frqs = 3;
	if (cfgOpts.ionoopt == Options::UC1234 || cfgOpts.ionoopt == Options::IF12131415)
		cfgOpts.frqs = 4;
	if (cfgOpts.ionoopt == Options::UC12345)
		cfgOpts.frqs = 5;

	if (debug)
	{
		std::cout << "system: " << cfgOpts.system << endl;
		std::cout << "begin_sod: " << cfgOpts.begin_sod << endl;
		std::cout << "end_sod: " << cfgOpts.end_sod << endl;
		std::cout << "elev: " << cfgOpts.elev << endl;
		std::cout << "mode: " << opts.modeFromInt(cfgOpts.mode).modeAsString() << endl;
		std::cout << "sppDCBCorr: " << cfgOpts.sppDcbCorr << endl;
		std::cout << "outENU: " << cfgOpts.outENU << endl;
		std::cout << "ionoopt: " << opts.ionFromInt(cfgOpts.ionoopt).ionAsString() << endl;
		std::cout << "BDS-2: " << cfgOpts.bds2 << endl;
		std::cout << "BDS-3Freq: " << cfgOpts.bdsfopt << endl;
		std::cout << "BDS-3Comb: " << opts.bComFromInt(cfgOpts.bdsComb).bCobAsString() << endl;
	}

	// ==>2nd, Parse required options
	/* get leap second file name from config file and read it stored to leapSecStore! */
	LeapSecStore    leapSecStore;
	try
	{
		std::string& leapSecFileName = cfgOpts.leapSecFileName;
		leapSecStore.loadFile(leapSecFileName);

		if (debug)
		{
			std::cout << "leapSecStore:" << leapSecFileName << endl;
		}
	}
	catch (Exception& e)
	{
		std::cerr << "read leap second file error!" << endl;
		std::cerr << e << endl;
		std::exit(-1);
	}

	//get de405 file name from config file and read it stored to solarSys!
	SolarSystem     solarSys;
	try
	{
		std::string& jplFileName = cfgOpts.jplFileName;
		solarSys.initializeWithBinaryFile(jplFileName);
		if (debug)
		{
			std::cout << "solarSys:" << jplFileName << endl;
		}
	}
	catch (Exception& e)
	{
		std::cerr << "read de405 file error!" << endl;
		std::cerr << e << endl;
		std::exit(-1);
	}

	//get atx file name from config file and read it!
	AntexReader     antexReader;
	try
	{
		std::string& atxFileName = cfgOpts.atxFileName;
		antexReader.open(atxFileName);
		if (debug)
		{
			std::cout << "antexReader:" << atxFileName << endl;
		}
	}
	catch (Exception& e)
	{
		std::cerr << "read atx file error!" << endl;
		std::cerr << e << endl;
		std::exit(-1);
	}

	//get ocean load file name from config file and read it!
	BLQDataReader   blqReader;
	try
	{
		std::string& blqFileName = cfgOpts.blqFileName;
		blqReader.open(blqFileName);
		if (debug)
		{
			std::cout << "blqReader:" << blqFileName << endl;
		}
	}
	catch (Exception& e)
	{
		std::cerr << "read ocean load file error!" << endl;
		std::cerr << e << endl;
		std::exit(-1);
	}

	FindFile ff;
	double clkStart, clkEnd;
	do
	{
		clkStart = Counter::now();
		// traverse the folder to get the files.
		if (cfgOpts.isDir&&cfgOpts.dir!="")
		{
			ff.setPath(cfgOpts.dir);
			ff.traverseDir();
		}
		// files
		string obsFile;
		string ephFile;
		string biasFile;

		string xyzFile;
		vector<string> ifcbFileVec;
		vector<string> sp3FileVec;
		vector<string> clkFileVec;
		vector<string> erpFileVec;

		map<TypeID, string> updFileMap;

		// class
		Rinex3EphemerisStore brdcEph;
		brdcEph.SearchNear();
		SP3EphemerisStore sp3Store;
		sp3Store.rejectBadPositions(true);
		sp3Store.rejectBadClocks(true);
		EOPDataStore erpStore;
		BiasDataReader biasReader;
		IFCBDataReader ifcbReader;
		XYZStore xyzStore;
		UPDDataReader updReader;

		Triple refPos;
		Triple rcvPos;
		

		// get file that process
		if (!cfgOpts.isDir)
		{
			try
			{
				obsFile = cfgOpts.obsFile;
				ephFile = cfgOpts.ephFile;
				xyzFile = cfgOpts.xyzFile;
				biasFile = cfgOpts.biasFile;
				if (cfgOpts.ifcbCorr)
				{
					string& ifcbFile = cfgOpts.ifcbFile;
					stringSplit(ifcbFile, ifcbFileVec, ' ');
				}
				string& sp3File = cfgOpts.sp3File;
				stringSplit(sp3File, sp3FileVec, ' ');

				string& clkFile = cfgOpts.clkFile;
				stringSplit(clkFile, clkFileVec, ' ');

				string erpFile = cfgOpts.erpFile;
				stringSplit(erpFile, erpFileVec, ' ');
				if (cfgOpts.ambFixMode)
				{
					updFileMap[TypeID::EWL] = cfgOpts.ewlUPD;
					updFileMap[TypeID::WL] = cfgOpts.wlUPD;
					updFileMap[TypeID::NL] = cfgOpts.nlUPD;
				}
			}
			catch (Exception e)
			{
				std::cerr << e << endl;
				std::cerr << "ERROR: get file from config file error!" << endl;
				std::exit(-1);
			}
		}
		else
		{
			try
			{
				if (!ff.findObsFile(obsFile))
				{
					std::cerr << "Cannot get obs file!" << std::endl;
					return;
				}
				ff.addPObs(obsFile);
			}
			catch (Exception e)
			{
				std::cout << e.getText() << endl;
				return;
			}
			
		}	// End of get files

		Rinex3ObsHeader rxHeader;
		Rinex3ObsData rxData;
		Rinex3ObsStream rxStream(obsFile.c_str(), ios::in);   // Object to read Rinex observation data files
		if (!rxStream)
		{
			std::cerr << "can't open file:" << obsFile.c_str() << endl;
		}
		else
		{
			std::cout << "obsFile: " << obsFile << endl;
		}
		// now, let's read data for current satiation
		rxStream >> rxHeader;

		// first time
		CommonTime firstEpoch, lastEpoch, headEpoch;
		headEpoch = rxHeader.firstObs.convertToCommonTime();
		double interval = rxHeader.interval;
		firstEpoch = rxHeader.firstObs.convertToCommonTime() + int(cfgOpts.begin_sod / interval) * interval;
		lastEpoch = rxHeader.firstObs.convertToCommonTime() + int(cfgOpts.end_sod / interval) * interval;
		// Check whether the obstype meets the requirements
		if(!opts.checkObsType(rxHeader, cfgOpts)) 
			continue;
		// added by ragn wang.
		rxHeader.beginEpoch = firstEpoch;
		rxHeader.endEpoch = lastEpoch;
		if (cfgOpts.isDir)
		{
			if (!ff.findEphFile(headEpoch, ephFile))
			{
				std::cout << "Cannot find EPH File in: " << cfgOpts.dir << std::endl;
				return;
			}
			if (!ff.findSnxFile(headEpoch, xyzFile)) 
			{
				std::cout << "Cannot find Snx File in: " << cfgOpts.dir << std::endl;
				return;
			}
			if (cfgOpts.obsCorr ? 
				(cfgOpts.obsCorr==Options::DCB?
				(!ff.findDCBFile(headEpoch, biasFile)) :
				(!ff.findBiaFile(headEpoch, biasFile))
					):0
				)
			{
				std::cout << "Cannot find Bias File in: " << cfgOpts.dir << std::endl;
				return;
			}
			if (cfgOpts.ifcbCorr && (!ff.findSGGIfcb(headEpoch, ifcbFileVec)))
			{
				std::cout << "Cannot find IFCB File in: " << cfgOpts.dir << std::endl;
				return;
			}

			if (!ff.findClkFile(headEpoch, clkFileVec))
			{
				std::cout << "Cannot find CLK File in: " << cfgOpts.dir << std::endl;
				return;
			}
			if (!ff.findErpFile(headEpoch, erpFileVec))
			{
				std::cout << "Cannot find ERP File in: " << cfgOpts.dir << std::endl;
				return;
			}
			if (!ff.findSp3File(headEpoch, sp3FileVec))
			{
				std::cout << "Cannot find SP3 File in: " << cfgOpts.dir << std::endl;
				return;
			}
			if (cfgOpts.ambFixMode && (!ff.findUPDFile(headEpoch, updFileMap)))
			{
				std::cout << "Cannot find upd files in: " << cfgOpts.dir << std::endl;
				return;
			}
		}

		// load file data
		// read ephFile
		try
		{
			if (debug)
			{
				std::cout << "Broadcast ephemeris file: " << ephFile << endl;
			}
			brdcEph.loadFile(ephFile);
		}
		catch (Exception& e)
		{
			cerr << e.getLocation();
			cerr << " cannot read: " << ephFile << endl;
			GPSTK_THROW(e);
		}
		// read bias file
		try
		{
            if(cfgOpts.obsCorr)
                biasReader.open(biasFile);
			if (debug)
				std::cout << "biasReader:" << biasFile << endl;
		}
		catch (Exception e)
		{
			std::cerr << e << endl;
			std::cerr << "unknown error in read erp file" << endl;
			std::exit(-1);
		}
		// read xyzFile
		try
		{
			if (debug)
				std::cout << "station reference coord file: " << xyzFile << std::endl;
			if (std::string::npos == xyzFile.find(".snx"))
			{
				xyzStore.loadFile(xyzFile);
				CommonTime posTi;
				refPos = xyzStore.getPos(rxHeader.markerName.substr(0, 4), posTi);
			}
			else
				xyzStore.loadFile(xyzFile, upperCase(rxHeader.markerName.substr(0, 4)), refPos);
		}   //End of read xyzFile
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
		// read precise files.
		if (cfgOpts.mode != Options::SPP)
		{
			try
			{
				// read sp3file
				for (auto f : sp3FileVec)
				{
					if (debug)
						std::cout << "sp3 file:" << f << endl;
					try
					{
						sp3Store.loadSP3File(f);
					}
					catch (Exception& e)
					{
						std::cerr << e << endl;
						std::cerr << "unknown error in read sp3 data" << endl;
						std::exit(-1);
					}
				}
				// read clk file
				for (auto f : clkFileVec)
				{
					if (debug)
						std::cout << "clk file:" << f << endl;
					try
					{
						sp3Store.loadRinexClockFile(f);
						if (cfgOpts.ambFixMode &&
							cfgOpts.ambProduct == Options::IRC)
						{
							updReader.loadIRCWL(f, headEpoch);
						}
					}
					catch (Exception& e)
					{
						std::cerr << e << endl;
						std::cerr << "unknown error in read clk file" << endl;
						std::exit(-1);
					}
				}
				// check IRC
				if (cfgOpts.ambFixMode &&
					cfgOpts.ambProduct == Options::IRC &&
					(!updReader.checkWLData()))
				{
					std::cout << "unknown error in loading IRC Product WL data!" << std::endl;
					exit(-1);
				}

				// read erp file
				for (auto f : erpFileVec)
				{
					try
					{
						if (debug)
						{
							std::cout << "erp file:" << f << endl;
						}
						erpStore.loadIGSFile(f);
					}
					catch (Exception e)
					{
						std::cerr << e << endl;
						std::cerr << "unknown error in read erp file" << endl;
						std::exit(-1);
					}
				}
				// read ifcb file
				if (cfgOpts.ifcbCorr)
				{
					try
					{
						for (auto f : ifcbFileVec)
						{
							if (debug)
								std::cout << "ifcb file:" << f << endl;
							ifcbReader.open(f);
						}
					}
					catch (Exception e)
					{
						std::cerr << e << endl;
						std::cerr << "unknown error in read ifcb file" << endl;
						std::exit(-1);
					}
				}
				//read upd
				if (cfgOpts.ambFixMode && cfgOpts.ambProduct == Options::UPD)
				{
					try
					{
						updReader.loadFile(updFileMap);
					}
					catch (Exception& e)
					{
						cerr << e.what() << endl;
						GPSTK_THROW(e);
					}

				}	// End of read upd files

			}
			catch (Exception e)
			{
				std::cerr << e << std::endl;
			}
			// end of load file data
		}	// End of if !SPP

		// DecimateData data 
		//filter observation according the start and end epoch and process interval in seted in the config file
		DecimateData decimateData;
		decimateData.setInitialEpoch(firstEpoch);
		decimateData.setEndEpoch(lastEpoch);
		decimateData.setSampleInterval(cfgOpts.sampleIntrval);

		// ReferenceSystem
		//used for calculate the transformation matrix between two coordination reference system
		//such as:CIS2ECEF and so on
		ReferenceSystem refSys;
		refSys.setEOPDataStore(erpStore);
		refSys.setLeapSecStore(leapSecStore);

		// keep satellite system for positioning
		//filter observation according the input system information in config file
		KeepSystems keepSystems(cfgOpts.system);

		// filter out bad code/phase observables
		//use for filter observation according to the range of pseudo range observation
		//default:[15000000.0,45000000.0]
		//if the observation is zero,that must be removed from the observation list
		FilterCode filterCode;

		// optimal observations
		//choose one optimal observation if there are more than one observation in this frequency band.
		ChooseOptimalObs chooseObs;

		//  code biases
		//used for correct the C1 pseudo range observation to P1 observation
		CorrectCodeBiases codeBias;
		codeBias.setDCBStore(biasReader);
		codeBias.setDCBCorr(cfgOpts.obsCorr);
		codeBias.setBDSFf(cfgOpts.bdsfopt);
		CorrectPhaseBiases phaseBias;
		if (cfgOpts.obsCorr==Options::OSB)
			phaseBias.setBiasStore(biasReader);
		// convert 4-char TypeID to 2-char TypeID for easy use in following classes
		//store short types of satellite observations
		//such as: L1CG=> L1G; C1WG=>C1G
		ConvertObs convertObs;
		// used for correct the L5G phase ifcb value.
		CorrectIFCB cifcb;
		cifcb.setIFCBStore(ifcbReader);
		
		//used for filter observation type according default type.
		RequiredObs reqObs;
		reqObs.setOpts(cfgOpts);
		// detect cycle slips, ragn wang
		DetectCSGF detectGFCS;
		DetectCSMW detectMWCS;
		// now, let's compute mw12 for gps/galileo/bds
		ComputeCombination computeMW;
		ComputeCombination computeGF;
		// the combination is consistent with the CODE's orbit/clock products
		ComputeCombination computeLC;
		// undifference-prefit
		ComputeCombination computePrefit;
		// compute residual include ambiguity, clk
		ComputeResidual computeResidual;
		// add combination options
		LinearCombinations linear;
		CombinationOptions combOpts(linear);
		combOpts.setOpts(cfgOpts);
		combOpts.addReqObsType(reqObs);
		combOpts.addComputeCSType(computeMW, computeGF);
		combOpts.addCSType(detectGFCS, detectMWCS);
		combOpts.addLCType(computeLC);
		combOpts.addPrefitType(computePrefit, computeResidual);
		// mark satellite-arc
		//update the epoch time of cycle slip and the num of cycle slip when it appears.
		MarkArc markArc;
		EraseSat eraseSat(cfgOpts);
		// compute satellite-positions according to sp3
		ComputeSatPos computeSatPos(sp3Store);

		// now, geometry-based models
		ComputeDerivative computeDerivative;
		// tropospheric model
		SaasTropModel saasTM;
		ComputeTropModel computeTrop;
		computeTrop.setTropModel(saasTM);
		// grav
		GravitationalDelay gravDelay;
		// attitude
		ComputeSatAttitude satAttitude;
		satAttitude.setReferenceSystem(refSys);
		satAttitude.setSolarSystem(solarSys);
		satAttitude.setAntexReader(antexReader);

		// eclipsed filter 
		EclipsedSatFilter eclipsedSV;
		eclipsedSV.setAntexReader(antexReader);

		// pc center
		ComputeSatPCenter satPCenter;
		satPCenter.setAntexReader(antexReader);
		satPCenter.setEclipse(eclipsedSV);

		ComputeWindUp windUp;
		windUp.setEclipse(eclipsedSV);

		// solid/ocean/polar tides
		ComputeStaTides staTides;
		staTides.setReferenceSystem(refSys);
		staTides.setSolarSystem(solarSys);
		staTides.setBLQDataReader(blqReader);

		// receiver bias
		//get pco and tide etc. for correction reveiver position
		CorrectRecBias recBias;
		recBias.setTidePointer(staTides);
		recBias.setAntexReader(antexReader);
		recBias.setUseAzimuth(false);
		// the reference station position
		XYZ2NEU xyz2neu;
		xyz2neu.setRefPos(refPos);
		// out put pos file path
		vector<string> res;
		string obsName("");
		string fileRootPath("");
		char splitType('\\');

		pathSplitType(obsFile, res, splitType);
		if (res.size() == 0)
		{
			Exception e("Cannot get file root path");
			GPSTK_THROW(e);
		}
		obsName = res[res.size() - 1];
		auto itName(find(res.begin(), res.end(), obsName));
		res.erase(itName);
		stringStitch(res, fileRootPath, splitType);
		
		string bds3Type, codeOnly;
		if(cfgOpts.system.find("C")!=string::npos)
			bds3Type = opts.bComFromInt(cfgOpts.bdsComb).bCobAsString() + "_";
		if (cfgOpts.codeOnly)
			codeOnly = "P_";

		std::string proMode(opts.modeFromInt(cfgOpts.mode).modeAsString());
		std::string ionMode(opts.ionFromInt(cfgOpts.ionoopt).ionAsString());
		std::string aimDir(cfgOpts.outdir);
		
		// creat output dir path
		if (aimDir == "")
		{
			fileRootPath += proMode + "_" + ionMode + "_" + cfgOpts.system 
				+ "_" + bds3Type + splitType;
			_mkdir(fileRootPath.c_str());
		}
		else
		{
			fileRootPath += aimDir + splitType;
			_mkdir(fileRootPath.c_str());
		}

		std::string fwdOutFile = fileRootPath + codeOnly +
			proMode + "_" + ionMode + "_" + bds3Type +
			cfgOpts.system + "_" + upperCase(obsName);
		// Obtaining the output Obs name is 
		// convenient for outputting other files.
		cfgOpts.aimOutPath=fwdOutFile;
		fwdOutFile += ".pos";
		if (cfgOpts.mode == Options::SPP)
		{
			fwdOutFile = fileRootPath + proMode + "_" +
				cfgOpts.system + "_" + upperCase(obsName) + ".pos";
		}
		std::ofstream floatOutStream;
		if (cfgOpts.ambFixMode)
		{
			if (cfgOpts.floatFix)
			{
				floatOutStream.open(fwdOutFile.c_str(), ios::out);
				if (!floatOutStream.is_open())
				{
					std::cerr << "can't open fwdOutFile!" << endl;
					std::exit(-1);
				}
			}

			fwdOutFile = fileRootPath +
				proMode + "_" +
				ionMode + "_" +
				opts.ambFixFromInt(cfgOpts.ambFixMode).ambFixModeAsStr() + "_" +
				opts.ambFixFrqFromInt(cfgOpts.fixFreq).ambFixFrqAsStr() + "_" +
				cfgOpts.system + "_" + upperCase(obsName) + ".pos";
		}

		std::ofstream fwdOutStream;
		if (cfgOpts.outPos)
		{
			fwdOutStream.open(fwdOutFile.c_str(), ios::out);
			if (!fwdOutStream.is_open())
			{
				std::cerr << "can't open fwdOutFile!" << endl;
				std::exit(-1);
			}
		}

		// solver SPP
		SPP spp(brdcEph, cfgOpts);
		bool isSPP(false);
		spp.setRefPos(refPos);
		if (cfgOpts.mode == Options::SPP&&cfgOpts.outPos)
		{
			spp.setFwdOutStream(&fwdOutStream);
		}
		// SolverPPPAll object
		SolverPPPAll pppFilter;
		pppFilter.setSysLinear(computePrefit.getLinear());
		pppFilter.setOpts(cfgOpts);
		pppFilter.setUpEquations();			// add variable into equation
		pppFilter.setRefPos(refPos);

		// PPPAR
		PPPAR pppAR;
		pppAR.setOpts(cfgOpts);
		pppAR.setRefPos(refPos);
		if (cfgOpts.outPos)
		{
			if (cfgOpts.ambFixMode)
			{
				pppAR.setStream(&fwdOutStream);
				if (cfgOpts.floatFix)
					pppFilter.setFwdOutStream(&floatOutStream);
			}
			else
				pppFilter.setFwdOutStream(&fwdOutStream);
		}
		// set trace
		std::ofstream traceOutStream;
		if (cfgOpts.trace)
		{
			traceOutStream.open((cfgOpts.aimOutPath + ".trace").c_str(), ios::out);
			if (!traceOutStream.is_open())
				exit(-1);
			spp.setTraceStream(&traceOutStream);
			pppFilter.setTraceStream(&traceOutStream);
			pppAR.setTraceStream(&traceOutStream);
		}
		
		
		// now, let's process gnss data for curret station
		std::cout << "now, start processing file:" << obsFile << endl;
		while (true)
		{
			try
			{
				// read data (epoch by epoch)
				try
				{
					rxStream >> rxData;
					rxData.auxHeader = rxHeader;
					if (rxStream.eof())	break;
				}
				catch (FFStreamError& e)
				{
					std::cerr << "Cannot get obs epoch data!" << std::endl;
					continue;
				}
				catch (EndOfFile& e)
				{
					break;
				}

				// now, convert to gnssRinex
				// Add the corresponding data header to the epoch observation data
				// And match the observation type with the observation value as a key-value pair
				gnssRinex gRin(rxData);
				gRin.header.refPos = refPos;
				gRin.header.system = cfgOpts.system;
				try
				{
					// Determine whether the current epoch is within 
					// the input start and end time
					decimateData.Process(gRin);
				}
				//jump this epoch
				catch (DecimateException& e)
				{
					std::cerr << e << std::endl;
					continue;
				}
				if (gRin.header.epochFlag > 1)
				{
					std::cerr << "current epoch data cannot be used!" << std::endl;
					continue;
				}

				// Added enu/neu rotate Matrix
				// to the observation map
				xyz2neu.Process(gRin);

				// keep only given system
				keepSystems.Process(gRin);
				// remove exclude sats.
				eraseSat.Process(gRin);
				// Filter out the set of observations with 
				// pseudorange values of 0 and out of range (CLSD)
				filterCode.Process(gRin);
				// Keep CL Observations
				chooseObs.Process(gRin);
				// spp
				if (cfgOpts.sppDcbCorr)
					codeBias.Process(gRin);

				if (cfgOpts.mode == Options::SPP)
				{
					spp.Process(gRin);
					continue;
				}
				// initialize rcvPos for first epoch
				if (gRin.header.epoch == gRin.header.beginEpoch||
					!isSPP)
				{
					try {
						spp.Process(gRin);
						rcvPos = gRin.header.rcvPos;
						isSPP = true;
					}
					catch(...)
					{
						if (gRin.header.antennaPosition.mag() != 0)
						{ rcvPos = gRin.header.antennaPosition; isSPP=true; }
						else if (gRin.header.refPos.mag() != 0)
						{ rcvPos = gRin.header.refPos; isSPP = true; }
						else
						{
							isSPP = false;
							pppFilter.printNewLine();
							if (cfgOpts.ambFixMode)
								pppAR.printNewLine();
							continue;
						}
					}
					if (cfgOpts.mode == Options::PPP_KIN)
						isSPP = false;
				}
				traceT(&traceOutStream, "receiver pos: ", rcvPos);
				traceT(&traceOutStream, "receiver refrence pos: ", refPos);
				// set rcvPos
				computeTrop.setAllParameters(gRin.header.epoch, rcvPos, 1);
				computeDerivative.setCoordinates(rcvPos);
				gravDelay.setNominalPosition(rcvPos);
				satPCenter.setNominalPosition(rcvPos);
				windUp.setNominalPosition(rcvPos);
				staTides.setNominalPosition(rcvPos);
				recBias.setNominalPosition(rcvPos);
				pppFilter.setRcvPos(rcvPos);
				pppAR.setRcvPos(rcvPos);
				//code bias correction
				if (cfgOpts.obsCorr > Options::NONE) 
				{
					if (!cfgOpts.sppDcbCorr)
						codeBias.Process(gRin);
					if (cfgOpts.obsCorr == Options::OSB)
						phaseBias.Process(gRin);
				}
				// C1WG->C1G, L1WG->L1G
				convertObs.Process(gRin);
				if (cfgOpts.ifcbCorr)
					cifcb.Process(gRin);
				// required obs
				reqObs.Process(gRin);
				// if all satellites are removed, then continue
				if (gRin.numSats() == 0)
				{
					std::cout << "The number of sat is 0!"
						<< std::endl;
					pppFilter.printNewLine();
					if (cfgOpts.ambFixMode)
						pppAR.printNewLine();
					continue;
				}
				// compute satellite position and velocity
				computeSatPos.Process(gRin);
				//// detect cs
				computeMW.Process(gRin);
				detectMWCS.Process(gRin);
				computeGF.Process(gRin);
				detectGFCS.Process(gRin);
				markArc.Process(gRin);
				// Compute and obtain multi-frequency combined observations
				computeLC.Process(gRin);
				// compute dops{GDOP, PDOP, HDOP, VDOP}
				computeDerivative.Process(gRin);
				//observation correction relative with satellite attitude
				eclipsedSV.Process(gRin);
				gravDelay.Process(gRin);
				satPCenter.Process(gRin);
				windUp.Process(gRin);
				//observation correction of tropospheric delay
				computeTrop.Process(gRin);
				// Observation station tide correction
				staTides.Process(gRin);
				// Receiver Antenna Correction
				recBias.Process(gRin);
				// filter-related class
				// Add weights based on satellite elevation weighting
				// elevation-weight for solver
				ComputeElevWeights elevWeights;
				elevWeights.Process(gRin);
				// Compute residuals for individual and combined observation equations
				computePrefit.Process(gRin);
				if (gRin.numSats() < 4)
				{
					std::cout <<
						static_cast<YDSTime>(gRin.header.epoch)
						<< "sat number: "
						<< gRin.numSats()
						<< std::endl;
					pppFilter.printNewLine();
					if (cfgOpts.ambFixMode)
						pppAR.printNewLine();
					if (rxData.time == lastEpoch)	break;
					continue;
				}
				// ppp Filter float solution
				pppFilter.Process(gRin);
				// ppp AR solution
				if (cfgOpts.ambFixMode)
				{
					pppAR.setUPD(updReader);
					pppAR.setCurrentEqus(pppFilter.getCurrentEqus());
					pppAR.setPostResiduals(pppFilter.getPostResi());
					pppAR.setSolutions(pppFilter.getSols());
					pppAR.setPPPUnkSet(pppFilter.getUnkSet());
					pppAR.setP(pppFilter.getP());
					pppAR.Process(gRin);
				}
				// end of while
				if (rxData.time == lastEpoch)	break;
			}
			catch (Exception& e)
			{
				std::cerr << e << endl;
				break;
			}

		} // end of "while()" to get epoch data
		rxStream.close();
		clkEnd = Counter::now();
		// close streams 
		std::cout << "/*--------------------------------------------------------------------*/" << endl;
		std::cout << "End of processing file:" << endl
			<< "  " << obsFile << endl;
		std::cout << "  Take: " << fixed
			<< setprecision(3)
			<< (clkEnd - clkStart) << "s" << endl;
		std::cout << "/*--------------------------------------------------------------------*/" << endl;

	} while (cfgOpts.isDir);
}   // End of process()

int main(int argc, char *argv[])
{
    positioning_algo program(argv[0]);
    if (!program.initialize(argc, argv))
        return 0;

    if (!program.run())
        return 1;

    return 0;
}
