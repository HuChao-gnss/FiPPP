#pragma ident "id: chaohu, ragnwang*"
/**
* This file is used to solve PPPAR with
* single-difference between satellites.
*/
#include "SolverPPPAR.hpp"

#define LOG_PI		1.14472988584940017 /* log(pi) */
#define debug		0

using namespace gpstk;

namespace fippp
{
	void PPPAR::Process()
		noexcept(false)
	{
		arData.init();
		//// float x y z
		double dx(getSolution(TypeID::dX)), dy(getSolution(TypeID::dY)), dz(getSolution(TypeID::dZ));
		arData.dxyzFloat = Triple(dx, dy, dz);

		// Construct MW observations and use UPD wide-lane correction
		smoothMW();
		// Get a multimap of satellite numbers sorted by altitude angle
		setSatElev();
		// single-difference process
		singleDifferenceARProcess();
		// print results
		if(pSolStream != NULL)
			printSols();
	}
	void PPPAR::printNewLine()
	{
		if (pSolStream != NULL)
			*pSolStream << std::endl;
	}
	void PPPAR::printSols()
	{
		CommonTime epoch(gRin.header.epoch);
		YDSTime time(epoch);
		CivilTime cvT(epoch);
		int state(6);

		// print time
		*pSolStream << fixed
			<< setprecision(2)
			<< setw(4) << time.year
			<< setw(3) << cvT.month
			<< setw(3) << cvT.day
			<< setw(3) << cvT.hour
			<< setw(3) << cvT.minute
			<< setw(6) << cvT.second
			<< setw(4) << time.doy
			<< setw(9) << time.sod;

		double dx, dy, dz;
		double x(0.), y(0.), z(0.), e(0.), n(0.), u(0.), rmsENU(0.), rmsXYZ(0.);
		double dx_, dy_, dz_;
		Vector<double> dXYZ(3, 0.0);
		Vector<double> enu(3, 0.);

		dx = getSolution(TypeID::dX);
		dy = getSolution(TypeID::dY);
		dz = getSolution(TypeID::dZ);
		/// Fixed sols
		if (arData.dxyz.mag() != 0.)
		{
			dx += arData.dxyz.X();
			dy += arData.dxyz.Y();
			dz += arData.dxyz.Z();
			state = 7;
		}

		// out esx, esy, esz
		x = rcvPos.X() + dx;
		y = rcvPos.Y() + dy;
		z = rcvPos.Z() + dz;

		if (debug)
		{
			cout << "printFixedSols" << endl;
			std::cout << x << "  " << y << "  " << z << std::endl;
		}

		dx_ = x - refPos.X();
		dy_ = y - refPos.Y();
		dz_ = z - refPos.Z();

		if (refPos.mag() != 0.0)
		{
			dXYZ[0] = dx_; dXYZ[1] = dy_; dXYZ[2] = dz_;
			enu = dXYZ * gRin.header.enuRotateMatrix;
			e = enu[0]; n = enu[1]; u = enu[2];
			rmsENU = std::sqrt(e * e + n * n + u * u);
			rmsXYZ = std::sqrt(SQR(dx) + SQR(dy) + SQR(dz));
		}

		*pSolStream << fixed
			<< setprecision(4)
			<< setw(15) << x
			<< setw(15) << y
			<< setw(15) << z;

		if (opts.outENU)
		{
			// out enu value
			*pSolStream << fixed
				<< setprecision(4)
				<< setw(9) << e
				<< setw(9) << n
				<< setw(9) << u
				<< setw(9) << rmsENU
				<< setw(3) << state
				;
		}
		else
		{
			// out dx dy dz value
			*pSolStream << fixed
				<< setprecision(4)
				<< setw(9) << dx
				<< setw(9) << dy
				<< setw(9) << dz
				<< setw(9) << rmsXYZ
				<< setw(3) << state
				;
		}
		*pSolStream << endl;
	}
	void PPPAR::checkLinear()
		noexcept(false)
	{
		std::vector<SatFixValueMap::iterator> removeDuplicate;
		satSatSet satSats;
		for(auto itWL = arData.FixedNw.begin();
			itWL != arData.FixedNw.end();
			++itWL)
		{
			SatID sat(itWL->second.sat), satRef(itWL->second.satRef);
			SatSat satSat(sat, satRef);
			auto itRes = std::find_if(satSats.begin(), satSats.end(), [&](const SatSat& element) {
				return element == satSat; });
			if (itRes != satSats.end())
				removeDuplicate.push_back(itWL);
			else
				satSats.insert(satSat);
		}
		// remove data
		for(auto it = removeDuplicate.begin();
			it != removeDuplicate.end();
			++it)
		{
			arData.FixedNw.erase(*it);
			arData.NFixedNw--;
		}
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="sat"></param>
	/// <returns></returns>
	double PPPAR::getSatResiduals(const SatID& sat)
		noexcept(false)
	{
		try
		{
			TypeID resType;
			if (sat.system == SatelliteSystem::GPS)
				resType = TypeID::prefitLC12G;
			else if (sat.system == SatelliteSystem::Galileo)
				resType = TypeID::prefitLC15E;
			else if (sat.system == SatelliteSystem::BeiDou)
				resType = TypeID::prefitLC26C;
			else
				return 99.9;
			// get postfit residuals
			int i(0);
			for (auto itequ = equations.begin();
				itequ != equations.end();
				++itequ)
			{
				SatID satEqu(itequ->getEquationSat());
				TypeID typeEqu(itequ->getIndepType());
				if (satEqu == sat && typeEqu == resType)
					return postResiduals(i);
				++i;
			}
			// check postfit residuals delta value
			return 99.9;
		}
		catch (...)
		{
			return 99.9;
		}
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="sat"></param>
	/// <param name="satRef"></param>
	/// <returns></returns>
	bool PPPAR::checkResiduals(const SatID& sat, const SatID& satRef)
		noexcept(false)
	{
		try
		{
			TypeID resType;
			double satres, satrefres;
			if (sat.system == SatelliteSystem::GPS)
				resType = TypeID::prefitLC12G;
			else if (sat.system == SatelliteSystem::Galileo)
				resType = TypeID::prefitLC15E;
			else if (sat.system == SatelliteSystem::BeiDou)
				resType = TypeID::prefitLC26C;
			else
				return false;
			// get postfit residuals
			int i(0);
			for(auto itequ = equations.begin();
				itequ != equations.end();
				++itequ)
			{
				SatID satEqu(itequ->getEquationSat());
				TypeID typeEqu(itequ->getIndepType());
				if (satEqu == sat && typeEqu == resType)
					satres = postResiduals(i);
				if (satEqu == satRef && typeEqu == resType)
					satrefres = postResiduals(i);
				++i;
			}
			// check postfit residuals delta value
			if (::abs(satres - satrefres) > 0.005)
				return false;
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
	/// <summary>
	/// 
	/// </summary>
	void PPPAR::removeFixedWL(const SatID& sat)
		noexcept(false)
	{
		std::vector<SatFixValueMap::iterator> removeDuplicate;
		for (auto itWL = arData.FixedNw.begin();
			itWL != arData.FixedNw.end();
			++itWL)
		{
			if (itWL->first == sat)
				removeDuplicate.push_back(itWL);
		}
		// remove data
		for (auto itData = removeDuplicate.begin();
			itData != removeDuplicate.end();
			++itData)
		{
			arData.FixedNw.erase(*itData);
			arData.NFixedNw--;
		}
	}
	/// <summary>
	/// Construct the intersatellite single 
	/// difference observation equation and 
	/// get the value, then process PPPAR
	/// </summary>
	bool PPPAR::singleDifferenceARProcess()
		noexcept(false)
	{
		// single difference between satellites
		// Perform ambiguity fixation and verify the result
		// If the fixation is successful, directly exit the
		// altitude angle selection cycle; otherwise, select
		// the next largest altitude angle value for fixation
		try
		{
			int nx(pppUnkSet.size());
			// init the map value
			//arData.satSat.clear();
			arData.FixedNn.clear();
			arData.FixedNw.clear();
			arData.NFixedNn = arData.NFixedNw = 0;
			arData.dxyz = Triple(0., 0., 0.);
			arData.ratio = 0.;
			// Verify the accuracy of the floating-point 
			// solution, and the narrow-lane ambiguity is 
			// fixed when the variance of the floating-
			// point solution reaches a certain level.
			Variable dx(getVariable(TypeID::dX, pppUnkSet));
			Variable dy(getVariable(TypeID::dY, pppUnkSet));
			Variable dz(getVariable(TypeID::dZ, pppUnkSet));
			double Pdxyz[3];
			Triple pxyz(P(dx.getNowIndex(), dx.getNowIndex()),
				P(dy.getNowIndex(), dy.getNowIndex()),
				P(dz.getNowIndex(), dz.getNowIndex()));
			/*skip AR if position variance too high to avoid false fix*/
			if (pxyz.mag() > 0.5)
				return false;
			
			if (arData.satMWLCValue.size() < 4)
				return false;
			std::set<SatelliteSystem> satSystems;
			// loop through the higher elevation sat
			for(auto itRefSat = ElvSatMap.rbegin();
				itRefSat != ElvSatMap.rend();
				++itRefSat)
			{
				int it(0), n(0);
				SatID satRef(itRefSat->second);
				if (satSystems.find(satRef.system) != satSystems.end())
					continue;
				if (::abs(getSatResiduals(satRef)) > 0.01)
					continue;
				for(auto itSat = ElvSatMap.rbegin();
					itSat != ElvSatMap.rend();
					++itSat)
				{
					SatID sat(itSat->second);
					if (satRef.id == itSat->second.id)
						continue;
					if (satRef.system != itSat->second.system)
						continue;

					SatValue_t NWL;
					// wide-lane ambiguity fix
					if (FixWLAmb(sat, satRef, NWL, it))
					{
						// store fixed wlAmb value and the two sats
						arData.FixedNw.insert(make_pair(satRef, NWL));
						arData.NFixedNw++;
						n++;
					}
					// Check if UPD cannot be obtained it == 1: cannot
					if (it)
						break;
				}
				// If a certain star can be used as a reference sat to fix three 
				// single-difference wide lanes, then exit
				if (n > 2)
					satSystems.insert(satRef.system);
				else
					removeFixedWL(satRef);
			}
			// check linear
			//checkLinear();
			if (arData.FixedNw.size() != arData.NFixedNw)
			{
				InvalidParameter e("Fixed WL Ambiguity size Wrong!");
				cerr << e << endl;
				cerr << e.getLocation() << endl;
				GPSTK_THROW(e);
			}
			////////////fix narrow-lane ambiguity/////////////////
			// 
			//// If the fixed number is less than 3, the equation cannot be solved.
			if (arData.NFixedNw < 3)
			{
				arData.init();
				return false;
			}
			
			if (opts.ambFixMode==Options::SDIFILS||
				opts.ambFixMode==Options::SDIFROUND)
			{
				if (FixIFNLAmb())
					return ILSIFFixSol();
			}
			return false;

		}
		catch (Exception& e)
		{
			cerr << e << endl;
			GPSTK_THROW(e);
			return false;
		}
		
	}
	/// <summary>
	/// 
	/// </summary>
	void PPPAR::resetSols()	noexcept(false)
	{
		try
		{
			int idx(getVariable(TypeID::dX, pppUnkSet).getNowIndex());
			int idy(getVariable(TypeID::dY, pppUnkSet).getNowIndex());
			int idz(getVariable(TypeID::dZ, pppUnkSet).getNowIndex());
			xhatminus(idx) += arData.dxyz.X();
			xhatminus(idy) += arData.dxyz.Y();
			xhatminus(idz) += arData.dxyz.Z();

			P(idx, idx) += arData.Pa(0, 0);
			P(idx, idy) += arData.Pa(0, 1);
			P(idx, idz) += arData.Pa(0, 2);
			P(idy, idx) += arData.Pa(1, 0);
			P(idy, idy) += arData.Pa(1, 1);
			P(idy, idz) += arData.Pa(1, 2);
			P(idz, idx) += arData.Pa(2, 0);
			P(idz, idy) += arData.Pa(2, 1);
			P(idz, idz) += arData.Pa(2, 2);
			// reset amb
			auto itData = arData.sdIFFix.begin();
			for (itData;
				itData != arData.sdIFFix.end();
				++itData)
			{
				SatID sat(itData->second.sat), satRef(itData->second.satRef);
				int iSat(itData->second.index[0]), iSatRef(itData->second.index[1]);
				xhatminus(iSat) = xhatminus(iSatRef) - itData->second.value[0];
			}
			traceVector(pTraceStream, "after xhatminus: ", xhatminus);

		}
		catch (Exception& e)
		{
			e.addText("Unable to reset fixed solution \
to float solution!");
			std::cerr << e.getLocation() 
				<< ": " 
				<< e.getText() 
				<< std::endl;
		}
	}
	/// <summary>
	/// get fix solutions
	/// </summary>
	/// <returns></returns>
	bool PPPAR::ILSIFFixSol()
		noexcept(false)
	{
		int i, j, na(opts.nFixUnk), nb(sdNLAmbValue.size()), ny(na+nb), nx(pppUnkSet.size());
		double BcFix(0.), BcFloat(0.);
		Vector<double> yb(nb), dx(na), Bc(nb), y(ny), yf(na), dya;
		Matrix<double> Qab(na, nb), QQ(na, nb), Qb(nb, nb), dQ(na, nb);
		Matrix<double> DD(ny, nx, 0.), EE(ny, nx), Pa(na,na), Pf(3, 3);
		// dx, dy, dz coeff
		for (i = 0; i < na; i++) DD(ny-1-i, nx-1-i) = 1.0;
		int idx(getVariable(TypeID::dX, pppUnkSet).getNowIndex());
		int idy(getVariable(TypeID::dY, pppUnkSet).getNowIndex());
		int idz(getVariable(TypeID::dZ, pppUnkSet).getNowIndex());

		Pf(0, 0) = P(idx, idx);	Pf(0, 1) = P(idx, idy); Pf(0, 2) = P(idx, idz);
		Pf(1, 0) = P(idy, idx);	Pf(1, 1) = P(idy, idy); Pf(1, 2) = P(idy, idz);
		Pf(2, 0) = P(idz, idx);	Pf(2, 1) = P(idz, idy); Pf(2, 2) = P(idz, idz);

		////  nl to iono-free ambiguity
		i = 0;
		int index(sdNLAmbValue.size());
		auto itData = sdNLAmbValue.begin();
		for (index; index != 0; index--)
		{
			if (itData == sdNLAmbValue.end())
				break;

			SatValue_t sdIFValue;
			SatID sat(itData->second.sat), satRef(itData->second.satRef);
			double lamNL(itData->second.coeff[0]), lam1(itData->second.coeff[1]), lam2(itData->second.coeff[2]);
			double alpa(itData->second.coeff[3]), beta(itData->second.coeff[4]);
			double Bw(getFixedWLAmbValue(sat, satRef, 1));
			int ind1(itData->second.index[0]), ind2(itData->second.index[1]);

			DD(i, ind2) = -1.0;
			DD(i, ind1) = 1.0;
			BcFloat = itData->second.sdFloatAmb[0];
			std::cout << BcFloat << std::endl;

			if (opts.ambFixMode == Options::SDIFILS)
			{
				BcFix = lamNL * NLAmbF(i, 0) - beta * lam2 * Bw;
			}
			else if (opts.ambFixMode == Options::SDIFROUND)
			{
				double upd1(pUPDData->getSatUPD(sat, TypeID::NL, gRin.header.epoch));
				double upd2(pUPDData->getSatUPD(satRef, TypeID::NL, gRin.header.epoch));
				BcFix = lamNL*(itData->second.value[0]-upd1+upd2)-beta*lam2*Bw;
			}
			yb(i) = BcFloat - BcFix;
			sdIFValue.sat = sat;
			sdIFValue.satRef = satRef;
			sdIFValue.value[0] = BcFix;
			sdIFValue.index[0] = itData->second.index[0];
			sdIFValue.index[1] = itData->second.index[1];
			// record data
			arData.sdIFFix.insert(make_pair(sat, sdIFValue));

			/// index + 1
			++i;

			++itData;
		}
		y = DD * solution;
		for (i = 0; i < na; i++)	yf(i) = y(nb + i);
		// record data
		arData.ya = y;
		//// check out


		/*get result*/
		try
		{
			Matrix<double> DDT(transpose(DD));
			QQ = DD * P * DDT;
			
			for (i = 0; i < nb; i++) for (j = 0; j < nb; j++)	Qb(i, j) = QQ(i, j);
			for (i = 0; i < na; i++) for (j = 0; j < nb; j++)	Qab(i, j) = QQ(nb+i, j);
			if (debug)
			{
				std::cout << "QQ" << std::endl;
				std::cout << QQ << std::endl;
				std::cout << "Qb" << std::endl;
				std::cout << Qb << std::endl;
				std::cout << "Qab" << std::endl;
				std::cout << Qab << std::endl;
			}
			
			Matrix<double> invQb(inverse(Qb));
			Matrix<double> QabT(transpose(Qab));

			dx = -Qab * invQb * yb;
			dya = yf -Qab * invQb * yb;
			Pa = -Qab * invQb * QabT;

			if (debug)
			{
				std::cout << "dx dy dz fixed" << std::endl;
				std::cout << dx << std::endl;
				std::cout << "dx dy dz fixed" << std::endl;
				std::cout << dya << std::endl;
				std::cout << "P (dx dy dz)" << std::endl;
				std::cout << Pa << std::endl;
			}

			arData.dxyz = Triple(dx(0), dx(1), dx(2));
			arData.Pa = Pa;
			arData.ya = dya;
			//resetSols();

			return true;

		}
		catch (...)
		{
			InvalidParameter e("Cannot compute Matrix");
			GPSTK_THROW(e);
		}

		return false;
	}
	bool PPPAR::LAMBDAFixIFNLAmb()
		noexcept(false)
	{
		int ns(sdNLAmbValue.size()), nx(pppUnkSet.size()), n, info, i(0);

		Matrix<double> Qnl(ns, ns), hNLT, H_NL(ns, nx, 0.), Qnl_(ns, ns);
		Vector<double> s, Nl(ns), VAmb;
		//
		// Sorting and selecting values according to 
		// the variance value of the narrow lane ambiguity
		//
		i = 0;
		auto itData = sdNLAmbValue.begin();
		for(itData;
			itData != sdNLAmbValue.end();
			++itData)
		{
			int indSat(itData->second.index[0]), indSatRef(itData->second.index[1]);
			double lamNL(itData->second.coeff[0]);
			H_NL(i, indSat) = 1.0 / lamNL;
			H_NL(i, indSatRef) = -1.0 / lamNL;
			Nl(i) = itData->second.floatVal[0];
			// index
			++i;
		}	// End of for sdNLAmbValue
		// End of coefficient matrix assignment.
		// Calculate the Q matrix
		try
		{
			hNLT = transpose(H_NL);
			Qnl = H_NL * P * hNLT;

			if (debug)
			{
				std::cout << "H_NL" << std::endl;
				std::cout << H_NL << std::endl;
				std::cout << "Qnl" << std::endl;
				std::cout << Qnl << std::endl;
			}
		}
		catch (...)
		{
			MatrixException e("Cannot calculate Q matrix");
			std::cerr << e.getText() << ": "
				<< e.getLocation() << std::endl;
			GPSTK_THROW(e);
		}
		// partial ambiguity search
		for (i = ns; i > 2; i--)
		{
			VAmb.resize(i);
			Qnl_.resize(i, i);
			for (int m = 0; m < i; m++)
			{
				VAmb(m) = Nl(m);
				for (int n = 0; n < i; n++)
					Qnl_(m, n) = Qnl(m, n);
			}
			NLAmbF.resize(i, 2);
			// calculate the ADOP value and check
			double ADOP(det(Qnl_));
			if (ADOP > ::pow(0.14, 2 * i))
			{
				sdNLAmbValue.erase(std::prev(sdNLAmbValue.rbegin().base()));
				continue;
			}
			if (lambda(VAmb, Qnl_, NLAmbF, s, 2))
			{
				sdNLAmbValue.erase(std::prev(sdNLAmbValue.rbegin().base()));
				continue;
			}

			if (debug)
			{
				std::cout << "Qnl_" << std::endl;
				std::cout << Qnl_ << std::endl;
				std::cout << "NL Ambiguity Fixed: " << std::endl;
				std::cout << NLAmbF << std::endl;
				std::cout << "s" << std::endl;
				std::cout << s << std::endl;
				std::cout << "s1/s0" << std::endl;
				std::cout << s(1)/s(0) << std::endl;
			}
			if (s(0) <= 0.) 
			{
				sdNLAmbValue.erase(std::prev(sdNLAmbValue.rbegin().base()));
				continue;
			}
			arData.ratio = (double)(std::min(s(1) / s(0), 999.9));
			if (arData.ratio > opts.thresLAMDA)
			{
				arData.NFixed = i;
				return true;
			}
			//remove last one
			sdNLAmbValue.erase(std::prev(sdNLAmbValue.rbegin().base()));
		}	// End of for(i=ns; i>n; i--)
		if (i <= 2)
		{
			sdNLAmbValue.clear();
			return false;
		}
		return false;
	}
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool PPPAR::FixIFNLAmb()
		noexcept(false)
	{
		try
		{
			arData.NFixedNn = 0;
			arData.FixedNn.clear();
			SatValue_t nlValue;
			sdNLAmbValue.clear();
			double alpa12, beta12, lamNL, ambSat, ambSatRef, lam1, lam2;
			double nlupd1, nlupd2, floatNL, resNL, var;
			int iAmbSat, iAmbSatRef, roundNL, fixWL;
			TypeID ambType, ambType1, ambType2;
			for(auto itData = arData.FixedNw.begin();
				itData != arData.FixedNw.end();
				++itData)
			{
				SatID sat(itData->second.sat), satRef(itData->second.satRef);
				if (sat.system==SatelliteSystem::GPS)
				{
					lamNL = LC12_WAVELENGTH_GPS;
					lam1 = L1_WAVELENGTH_GPS;
					lam2 = L2_WAVELENGTH_GPS;
					alpa12 = APHA_GPS_L1L2;
					beta12 = BETA_GPS_L1L2;
					ambType = TypeID::BLC12G;
					ambType1 = TypeID::BL1G;
					ambType2 = TypeID::BL2G;
				}
				else if (sat.system == SatelliteSystem::Galileo)
				{
					lamNL = LC15_WAVELENGTH_GAL;
					lam1 = WAVELENGTH_GALILEO_E1;
					lam2 = WAVELENGTH_GALILEO_E5a;
					alpa12 = APHA_GAL_L1L5;
					beta12 = BETA_GAL_L1L5;
					ambType = TypeID::BLC15E;
					ambType1 = TypeID::BL1E;
					ambType2 = TypeID::BL5E;
				}
				else if (sat.system == SatelliteSystem::BeiDou)
				{
					lamNL = LC26_WAVELENGTH_BDS;
					lam1 = WAVELENGTH_BEIDOU_B1I;
					lam2 = WAVELENGTH_BEIDOU_B3I;
					alpa12 = APHA_BDS_L2L6;
					beta12 = BETA_BDS_L2L6;
					ambType = TypeID::BLC26C;
					ambType1 = TypeID::BL2C;
					ambType2 = TypeID::BL6C;
				}
				else
					return false;

				iAmbSat = getVariable(ambType, sat, pppUnkSet).getNowIndex();
				iAmbSatRef = getVariable(ambType, satRef, pppUnkSet).getNowIndex();
				ambSat = getSolution(ambType, sat);
				ambSatRef = getSolution(ambType, satRef);

				fixWL = getFixedWLAmbValue(sat, satRef, 1.0);
				if (opts.ambFixMode == Options::SDIFROUND)
				{
					nlupd1 = pUPDData->getSatUPD(sat, TypeID::NL, gRin.header.epoch);
					nlupd2 = pUPDData->getSatUPD(satRef, TypeID::NL, gRin.header.epoch);
					/// cod and wum ac narrow lane upd products, NL = NL0 + upd_NL
					floatNL = (ambSat-ambSatRef+beta12*lam2*fixWL)/lamNL+nlupd1-nlupd2;
				}
				else if (opts.ambFixMode == Options::SDIFILS)
				{
					floatNL = (ambSat-ambSatRef+beta12*lam2*fixWL)/lamNL;
				}
				roundNL = ROUND(floatNL);
				resNL = floatNL - roundNL;
				var = P(iAmbSat, iAmbSat)+P(iAmbSatRef, iAmbSatRef)-2.0*P(iAmbSat, iAmbSatRef);
				if (fabs(resNL) > 0.25)
					continue;
				if (!checkResiduals(sat, satRef))
					continue;
				nlValue.sat = sat;
				nlValue.satRef = satRef;
				nlValue.value[0] = roundNL;
				nlValue.floatVal[0] = floatNL;
				nlValue.residual[0] = resNL;
				nlValue.var[0] = var;
				nlValue.index[0] = iAmbSat;
				nlValue.index[1] = iAmbSatRef;
				nlValue.coeff[0] = lamNL;
				nlValue.coeff[1] = lam1;
				nlValue.coeff[2] = lam2;
				nlValue.coeff[3] = alpa12;
				nlValue.coeff[4] = beta12;
				nlValue.sdFloatAmb[0] = ambSat - ambSatRef;
				// record data
				arData.FixedNn.insert(make_pair(satRef, nlValue));
				arData.NFixedNn += 1;
				sdNLAmbValue.insert(make_pair(var, nlValue));
			}	// End of for itData = arData.FixWL.begin()
			// check
			if (sdNLAmbValue.size() < 3)
				return false;
			else
			{
				if (opts.ambFixMode == Options::SDIFILS)
					return LAMBDAFixIFNLAmb();
				else if (opts.ambFixMode == Options::SDIFROUND)
				{
					arData.NFixed = arData.NFixedNn;
					return true;
				}
			}
		}
		catch (...)
		{
			return false;
		}
		return false;
	}

	/// <summary>
	/// get Fixed WL Amb value
	/// </summary>
	/// <param name="sat1"></param>
	/// <param name="sat2"></param>
	/// <param name="flag">0: float 1:fixed</param>
	/// <returns></returns>
	double PPPAR::getFixedWLAmbValue(const SatID& sat1, const SatID& satRef, int flag)
		noexcept(false)
	{
		auto itData = arData.FixedNw.begin();
		for (itData;
			itData != arData.FixedNw.end();
			++itData)
		{
			if (itData->second.sat == sat1 
				&& itData->second.satRef == satRef)
			{
				if (flag)
					return itData->second.value[0];
				else
					return itData->second.floatVal[0];
			}
			if (itData == arData.FixedNw.end())
			{
				InvalidParameter e("Cannot get the " + asString(sat1) + "-" + \
				asString(satRef) + "Fixed WL ambiguity value!");
				std::cerr << e.getLocation() 
					<< ": "
					<< e.getText()
					<< std::endl;
				GPSTK_THROW(e);
			}
		}
        return -0.;
	} // End of getFixedWLAmbValue

	/// <summary>
	/// Fixed wide-lane ambiguity with UPD products
	/// </summary>
	/// <param name="sat1"></param>
	/// <param name="sat2"></param>
	/// <param name="flag"></param>
	/// <returns></returns>
	bool PPPAR::FixWLAmb(const SatID& sat1, const SatID& sat2, SatValue_t& NW, int& it)
		noexcept(false)
	{
		mwlc_t mw1(arData.satMWLCValue[sat1]), mw2(arData.satMWLCValue[sat2]);
		double sdAmbWL, VarWL, wlAmb1, wlAmb2, lamWL(mw1.lamWL);
		int sdFixWL;
		if (!mw1.n || !mw2.n)	return false;
		// wide-lane amb
		try
		{
			if(opts.ambProduct)
			{
				if (pUPDData->arprod == "IRC" && pUPDData->acname == "grg")
				{
					wlAmb1 = mw1.mw / lamWL + pUPDData->getSatUPD(sat1, TypeID::WL, gRin.header.epoch);
					wlAmb2 = mw2.mw / lamWL + pUPDData->getSatUPD(sat2, TypeID::WL, gRin.header.epoch);
				}
				else
				{
					wlAmb1 = mw1.mw / lamWL - pUPDData->getSatUPD(sat1, TypeID::WL, gRin.header.epoch);
					wlAmb2 = mw2.mw / lamWL - pUPDData->getSatUPD(sat2, TypeID::WL, gRin.header.epoch);
				}
			}
			else
			{
				wlAmb1 = mw1.mw / lamWL;
				wlAmb2 = mw2.mw / lamWL;
			}
			
			VarWL = (mw1.var / mw1.n + mw2.var / mw2.n) / SQR(lamWL);
		}
		catch (Exception& e)
		{
			if (e.getText().find(asString(sat2)) != std::string::npos)
				it = 1;
			return false;
		}
		sdAmbWL = wlAmb1 - wlAmb2;
		sdFixWL = ROUND(sdAmbWL);

		// store data
		NW.sat = sat1;
		NW.satRef = sat2;
		NW.floatVal[0] = sdAmbWL;
		NW.value[0] = sdFixWL;
		NW.residual[0] = sdFixWL - sdAmbWL;
		NW.var[0] = VarWL;

		bool flagFix1 = ::fabs(sdFixWL - sdAmbWL) <= opts.thresAmb;
		bool flagFix2 = conffunc(sdFixWL, sdAmbWL,SQRT(VarWL))>=opts.thresBS;

		return (flagFix1 && flagFix2);
           
	}
	/// <summary>
	/// creat mw linear combination value
	/// </summary>
	void PPPAR::smoothMW()
		noexcept(false)
	{
		// loop through all the gData
		for (auto itData = gRin.body.begin();
			itData != gRin.body.end();
			++itData)
		{
			TypeID L1Type, L2Type, P1Type, P2Type;
			double mw, lc, pc, f1, f2, errratio,errL, var, elev, lamWL;
			SatID sat(itData->first);
			if (sat.system == SatelliteSystem::GPS)
			{
				P1Type = TypeID::C1G; P2Type = TypeID::C2G;
				L1Type = TypeID::L1G; L2Type = TypeID::L2G;
				f1 = L1_FREQ_GPS; f2 = L2_FREQ_GPS;
				lamWL = WL12_WAVELENGTH_GPS;
				errratio = opts.errratio[SatelliteSystem::GPS];
				errL = opts.errL[SatelliteSystem::GPS];
			}
			else if (sat.system == SatelliteSystem::Galileo)
			{
				P1Type = TypeID::C1E; P2Type = TypeID::C5E;
				L1Type = TypeID::L1E; L2Type = TypeID::L5E;
				f1 = L1_FREQ_GAL; f2 = L5_FREQ_GAL;
				lamWL = WL15_WAVELENGTH_GAL;
				errratio = opts.errratio[SatelliteSystem::Galileo];
				errL = opts.errL[SatelliteSystem::Galileo];
			}
			else if (sat.system == SatelliteSystem::BeiDou)
			{
				P1Type = TypeID::C2C; P2Type = TypeID::C6C;
				L1Type = TypeID::L2C; L2Type = TypeID::L6C;
				f1 = L2_FREQ_BDS; f2 = L6_FREQ_BDS;
				lamWL = WL26_WAVELENGTH_BDS;
				errratio = opts.errratio[SatelliteSystem::BeiDou];
				errL = opts.errL[SatelliteSystem::BeiDou];
			}
			else
				continue;
			try
			{
				// compute the mw combination value
				lc = (f1 * itData->second[L1Type] - f2 * itData->second[L2Type]) / (f1 - f2);
				pc = (f1 * itData->second[P1Type] + f2 * itData->second[P2Type]) / (f1 + f2);
				mw = lc - pc;
				// satellite elevation
				elev = itData->second[TypeID::elevation];
			}
			catch (Exception& e)
			{
				std::cerr << "Cannot get the value" << std::endl;
				GPSTK_THROW(e);
			}
			double sinel = ::sin(elev * DEG_TO_RAD);
			double sig;
			// compute the variance of the mw combination value
			sig = ::sqrt( SQR(errL) + SQR(errL / sinel))*errratio;
			var = (SQR(f1)+SQR(f2))/SQR(f1+f2) * SQR(sig);

			// if first time or detect slip reset the mwLCValue
			if (gRin.header.epoch == gRin.header.beginEpoch ||
				itData->second[TypeID::CSFlag] ||
				::fabs(gRin.header.epoch - 
					arData.satMWLCValue[sat].epoch)> gRin.header.interval||
				arData.satMWLCValue.find(sat)==arData.satMWLCValue.end()
				)
			{
				arData.satMWLCValue[sat].epoch = gRin.header.epoch;
				arData.satMWLCValue[sat].n = 0;
				arData.satMWLCValue[sat].lamWL = 0.;
				arData.satMWLCValue[sat].mw = 0.;
				arData.satMWLCValue[sat].var = 0.;
			}

			arData.satMWLCValue[sat].epoch = gRin.header.epoch;
			arData.satMWLCValue[sat].lamWL = lamWL;
			arData.satMWLCValue[sat].n += 1;
			arData.satMWLCValue[sat].mw+=(mw-arData.satMWLCValue[sat].mw)/arData.satMWLCValue[sat].n;
			arData.satMWLCValue[sat].var+=(var-arData.satMWLCValue[sat].var)/arData.satMWLCValue[sat].n;

		}	// end of for loop all the gRin
	}	// end of smoothMW
	/// <summary>
	/// get float solution
	/// </summary>
	/// <param name="type"></param>
	/// <returns></returns>
	double PPPAR::getSolution(const TypeID& type) const
		noexcept(false)
	{
		auto varIt = pppUnkSet.begin();
		for (varIt;
			varIt != pppUnkSet.end();
			++varIt)
		{
			if (varIt->getType() == type)
				return solution((*varIt).getNowIndex());
		}
		InvalidRequest e(type.asString() + "not found in solution vector.");
		GPSTK_THROW(e);
	}
	double PPPAR::getSolution(const TypeID& type, const SatID& sat) const
		noexcept(false)
	{
		auto varIt = pppUnkSet.begin();
		for(varIt;
			varIt != pppUnkSet.end();
			++varIt)
		{
			if(varIt->getSatellite()==sat&&varIt->getType()==type)
				return solution((*varIt).getNowIndex());
		}
		InvalidRequest e(type.asString() + "not found in solution vector.");
		GPSTK_THROW(e);
	}
	/// <summary>
	/// set the satellite elevation
	/// </summary>
	void PPPAR::setSatElev()
		noexcept(false)
	{
		ElvSatMap.clear();
		SatIDSet satIDs;

		// loop through the arData sat
		for(auto itSat = arData.satMWLCValue.begin();
			itSat != arData.satMWLCValue.end();
			++itSat)
		{
			SatID satID(itSat->first);
			// check the system, it cannot fixed the glonass
			if (satID.system != SatelliteSystem::GPS &&
				satID.system != SatelliteSystem::Galileo &&
				satID.system != SatelliteSystem::BeiDou)
				continue;
			try
			{
				double ele(gRin.body[satID][TypeID::elevation]);
				if (ele < opts.elev)	continue;
				if (satID.system == SatelliteSystem::BeiDou)
				{
					int prn = satID.id;
					if (opts.ambProduct == Options::IRC)
					{
						if ((prn >= 1 && prn <= 5) || prn == 15 || prn == 17 || prn == 18 || prn == 31 || prn > 60)
							continue;
						if (pUPDData->acname == "grg")
							continue;
					}
				}
				ElvSatMap.insert(make_pair(ele, satID));
				satIDs.insert(satID);
			}
			catch (Exception& e)
			{
				e.addText("Cannot get the elevation of sat: " + asString<RinexSatID>(satID));
				GPSTK_THROW(e);
			}
		}	//End of loop through all the data
		// check the data size:
		SatIDSet rejectSat;
		for(auto itSat = arData.satMWLCValue.begin();
			itSat != arData.satMWLCValue.end();
			++itSat)
		{
			if (satIDs.find(itSat->first) == satIDs.end())
				rejectSat.insert(itSat->first);
		}
		// remove rejectSat
		for (auto itSat = rejectSat.begin();
			itSat != rejectSat.end();
			++itSat)
		{
			arData.satMWLCValue.erase(*itSat);
			if(debug)
			{
				std::cout << "arData MWLC remove: " << std::endl;
				std::cout << *itSat << std::endl;
			}
		}
	}	// end of setSatElev

	Variable PPPAR::getVariable(const TypeID& type, const VariableSet& varSet)
		noexcept(false)
	{
		// Declare an iterator for 'stateMap' and go to the first element
		VariableSet::const_iterator it = varSet.begin();

		// Look for a variable with the same type
		while ((*it).getType() != type &&
			it != varSet.end())
		{
			++it;

			// If the same type is not found, throw an exception
			if (it == varSet.end())
			{
				InvalidRequest e("Type not found in varSet.");
				GPSTK_THROW(e);
			}
		}

		// Else, return the corresponding value
		return (*it);
	}

	Variable PPPAR::getVariable(const TypeID& type, const SatID& sat, const VariableSet& varSet)
		noexcept(false)
	{
		// Declare an iterator for 'stateMap' and go to the first element
		VariableSet::const_iterator it = varSet.begin();

		// Look for a variable with the same type
		while (true)
		{
			if ((*it).getType() == type && (*it).getSatellite() == sat)
				break;
			++it;

			// If the same type is not found, throw an exception
			if (it == varSet.end())
			{
				InvalidRequest e("Type not found in varSet.");
				GPSTK_THROW(e);
			}
		}

		// Else, return the corresponding value
		return (*it);
	}

	////////////////////// confidence function of integer ambiguity///////////////////////
	double PPPAR::conffunc(int N, double B, double sig)
	{
		double x, p = 1.0;
		int i;

		x = ::fabs(B - N);
		for (i = 1; i < 8; i++) {
			p -= f_erfc((i - x) / (SQRT(2) * sig)) - f_erfc((i + x) / (SQRT(2) * sig));
		}
		return p;
	}

	double PPPAR::f_erfc(double x)
	{
		return x>=0.0?q_gamma(0.5,x*x,LOG_PI/2.0):1.0+p_gamma(0.5,x*x,LOG_PI/2.0);
	}

	double PPPAR::p_gamma(double a, double x, double log_gamma_a)
	{
		double y, w;
		int i;

		if (x == 0.0) return 0.0;
		if (x >= a + 1.0) return 1.0 - q_gamma(a, x, log_gamma_a);

		y = w = ::exp(a * ::log(x) - x - log_gamma_a) / a;

		for (i = 1; i < 100; i++) {
			w *= x / (a + i);
			y += w;
			if (fabs(w) < 1E-15) break;
		}
		return y;
	}

	double PPPAR::q_gamma(double a, double x, double log_gamma_a)
	{
		double y,w,la=1.0,lb=x+1.0-a,lc;
		int i;

		if (x<a+1.0) return 1.0-p_gamma(a,x,log_gamma_a);
		w=::exp(-x+a*::log(x)-log_gamma_a);
		y=w/lb;
		for (i=2;i<100;i++) 
		{
			lc=((i-1.0-a)*(lb-la)+(i+x)*lb)/i;
			la=lb; lb=lc;
			w*=(i-1.0-a)/i;
			y+=w/la/lb;
			if (fabs(w/la/lb)<1E-15) break;
		}
		return y;
	}
	//////////////End of integer ambiguity functions////////////////////////


}