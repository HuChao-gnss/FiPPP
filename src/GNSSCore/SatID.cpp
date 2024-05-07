#pragma ident "$Id$"

#include "SatID.hpp"

/**
 * @file SatID.cpp
 * gpstk::SatID - navigation system-independent representation of a satellite
 * as defined by the SP3 specification.
 */

namespace gpstk
{
	bool SatID::isValid()const
	{
		switch (system)
		{
		case SatelliteSystem::GPS:     return (id > 0 && id <= MAX_PRN_GPS);
		case SatelliteSystem::Glonass: return (id > 0 && id <= MAX_PRN_GLO);
		case SatelliteSystem::Galileo: return (id > 0 && id <= MAX_PRN_GAL);
		case SatelliteSystem::BeiDou:  return (id > 0 && id <= MAX_PRN_BDS);
		case SatelliteSystem::Unknown: return (id > 200);
		default: return (id > 0 && id < 1000);
		}
	}
}
