//
// This class is used to solve single point positioning.
// 2023-04
//

#include "SPP.hpp"

using namespace gpstk;
namespace fippp 
{
	string SPP::getClassName() const
	{return "SPP"; }

	void SPP::Process(gnssRinex& gRin) 
		noexcept(false)
	{
		try
		{
			// convert obs type
			ConvertObs converObs;
			// copy gRin
			gnssRinex bRin(gRin);
			// covert obs type C1WG=>C1G
			converObs.Process(bRin);
			ComputeSatPos computeSatPos(navEph);
			computeSatPos.Process(bRin);
			
			// ionoModel
			IonoModel ionoModel;
			if (navEph.Rhead.mapIonoCorr.find("GPSA") != navEph.Rhead.mapIonoCorr.end()&&
				navEph.Rhead.mapIonoCorr.find("GPSB") != navEph.Rhead.mapIonoCorr.end())
			{
				ionoModel.setModel(navEph.Rhead.mapIonoCorr["GPSA"].param,
					navEph.Rhead.mapIonoCorr["GPSB"].param);
			}
			// Tropospheric model
			SaasTropModel saasTM;
			ComputeSPP sppSol;
			sppSol.setIonoModel(ionoModel);
			sppSol.setTropModel(saasTM);
			sppSol.setOpts(opts);
			sppSol.Process(bRin);
			gRin.header.rcvPos = bRin.header.rcvPos;
            if (pOutStream)
            {
                try
                {
                    printSol(pOutStream, gRin.header);
                }
                catch (...)
                {
                    printNewLine(pOutStream);
                }
            }
		}
		catch (Exception& e)
		{
			cerr << e.getLocation();
			cerr << e.getText() << endl;
			GPSTK_THROW(e);
		}	
	}	// end of SPP::Process
    ///
    /// \param pOutStream
    void SPP::printNewLine(std::ofstream *pOutStream) {
        *pOutStream << std::endl;
    }
    ///
    /// \param pOutStream
    /// \param gRinHeader
	void SPP::printSol(std::ofstream* pOutStream, 
		const gnssRinexHeader& gRinHeader)
	{
		CommonTime epoch(gRinHeader.epoch);
		YDSTime time(epoch);
		CivilTime cvT(epoch);

		Position rcvPos = gRinHeader.rcvPos;

		// print time
		*pOutStream << fixed
			<< setprecision(2)
			<< setw(4) << time.year
			<< setw(3) << cvT.month
			<< setw(3) << cvT.day
			<< setw(3) << cvT.hour
			<< setw(3) << cvT.minute
			<< setw(6) << cvT.second
			<< setw(4) << time.doy
			<< setw(9) << time.sod;

		*pOutStream << fixed
			<< setprecision(4)
			<< setw(15) << rcvPos.X()
			<< setw(15) << rcvPos.Y()
			<< setw(15) << rcvPos.Z();

		Vector<double> enu(3, 0.), dxyz(3, 0.);
		double e, n, u, rmsENU, rmsXYZ;
		double dx(rcvPos.X() - refPos.X());
		double dy(rcvPos.Y() - refPos.Y());
		double dz(rcvPos.Z() - refPos.Z());
		dxyz[0] = dx; dxyz[1]= dy; dxyz[2] = dz;

		enu = dxyz * gRinHeader.enuRotateMatrix;

		rmsXYZ = std::sqrt(dx * dx + dy * dy + dz * dz);
		e = enu[0]; n = enu[1]; u = enu[2];
		rmsENU = std::sqrt(e * e + n * n + u * u);
		if (refPos.mag() == 0.0)
		{
			dx = dy = dz = rmsXYZ = 0.;
			e = n = u = rmsENU = 0.;
		}

		if (opts.outENU)
		{
			// out enu value
			*pOutStream << fixed
				<< setprecision(4)
				<< setw(9) << e
				<< setw(9) << n
				<< setw(9) << u
				<< setw(9) << rmsENU
				<< setw(3) << 0
				;
		}
		else
		{
			// out dx dy dz value
			*pOutStream << fixed
				<< setprecision(4)
				<< setw(9) << dx
				<< setw(9) << dy
				<< setw(9) << dz
				<< setw(9) << rmsXYZ
				<< setw(3) << 0
				;
		}
		*pOutStream << endl;

	}
}