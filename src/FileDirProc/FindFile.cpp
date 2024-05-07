#pragma ident "$Id: ragn wang 2023-03-14"


#include "FindFile.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>

#define debug 0

using namespace gpstk;

namespace fippp
{
	/// <summary>
	/// 
	/// </summary>
	/// <param name="path"></param>
	/// <param name="fileMap"></param>
	void FindFile::traverseDir()
	{
		fileMap.clear();
		DIR* dir = opendir(path.c_str());
		if (dir == nullptr)
		{
			std::cerr << "Failed to open directory: " << path << std::endl;
			return;
		}
		std::vector<std::string> res;
		char split;
		pathSplitType(path, res, split);
		struct dirent* entry;

		while ((entry = readdir(dir)) != nullptr)
		{
			std::string name = entry->d_name;
			if (name == "." || name == "..") continue;

			std::string full_path = path + split + name;
			struct stat stat_buf;

			if (stat(full_path.c_str(), &stat_buf) != 0)
			{
				std::cerr << "Failed to stat file: " << full_path << std::endl;
				return;
			}

			/* determine whether is a file */
			if (S_ISREG(stat_buf.st_mode))
			{
				int len = full_path.size();
				/* .o file*/
				if (lowerCase(full_path.substr(len - 1)) == "o")
				{
					fileMap[TypeID::OBSF].insert(full_path);
				}
				else if (lowerCase(full_path.substr(len - 1)) == "p" &&
					lowerCase(full_path.substr(len - 3)) != "erp")
				{	/* .p file */
					fileMap[TypeID::EPHF].insert(full_path);
				}
				else if (lowerCase(full_path.substr(len - 3)) == "sp3")
				{	/* .sp3 file */
					fileMap[TypeID::SP3F].insert(full_path);
				}
				else if (lowerCase(full_path.substr(len - 3)) == "clk")
				{	/* .clk file */
					fileMap[TypeID::CLKF].insert(full_path);
				}
				else if (lowerCase(full_path.substr(len - 3)) == "bia" ||
					lowerCase(full_path.substr(len - 3)) == "bsx")
				{	/* OSB/DCB file */
					fileMap[TypeID::BIASF].insert(full_path);
				}
				else if (lowerCase(full_path.substr(len - 3)) == "erp")
				{	/* .erp file */
					fileMap[TypeID::ERPF].insert(full_path);
				}
				else if (lowerCase(full_path.substr(len - 3)) == "snx")
				{	/* .snx file */
					fileMap[TypeID::SNXF].insert(full_path);
				}
				else if (lowerCase(name.substr(0, 4)) == "ifcb")
				{	/* .sp3 file */
					fileMap[TypeID::IFCBF].insert(full_path);
				}
				else if (lowerCase(name.substr(0,3)) == "upd")
				{
					fileMap[TypeID::UPDF].insert(full_path);
				}
			}
		} //END of while

		closedir(dir);
	}	// END of "traverseDir"

	/// <summary>
	/// find obs file in the input dir path
	/// </summary>
	/// <param name="obs"> out obs file </param>
	bool FindFile::findObsFile(std::string& obs) 
	{
		auto it = fileMap.find(TypeID::OBSF);
		try
		{
			if (it != fileMap.end())
			{
				if (pObsSet.size() != 0)
				{
					for (auto it_obs = pObsSet.begin();
						it_obs != pObsSet.end();
						it_obs++)
					{
						fileMap[TypeID::OBSF].erase(*it_obs);
					}
					if (fileMap[TypeID::OBSF].size() == 0)
					{
						Exception e("All obs files have been processed!");
						GPSTK_THROW(e);
					}
				}

				obs = *fileMap[TypeID::OBSF].begin();
				if (debug)
				{
					std::cout << "obs: " << obs << std::endl;
				}
				return true;
			}//END of if in file Map
			return false;
		}
		catch (Exception e)
		{
			std::cerr << "Cannot get obs file!" << std::endl;
			GPSTK_THROW(e);
		}

	}
	/// <summary>
	/// Match the nav file according to the observation time of the obs file
	/// </summary>
	/// <param name="epoch"> obs first Epoch </param>
	/// <param name="p"> out .p file </param>
	bool FindFile::findEphFile(const CommonTime& epoch, std::string& p)
	{
		YDSTime ydt(epoch);
		int year = ydt.year;
		int doy = ydt.doy;
		std::string fName = "brdm" + std::to_string(doy) + "0." + std::to_string(year).substr(2) + "p";
		auto it = fileMap.find(TypeID::EPHF);
		try
		{
			if (it != fileMap.end())
			{
				std::set<std::string> ephs = fileMap[TypeID::EPHF];
				auto it_ephs = ephs.begin();
				// loop through all the .p file
				for (it_ephs; it_ephs != ephs.end(); ++it_ephs)
				{
					std::vector<std::string> vPFile;
					pathSplit(*it_ephs, vPFile);
					if (upperCase(fName) == upperCase(vPFile.back()))
					{
						p = *it_ephs;
						if (debug)
						{
							std::cout << "eph: " << p << std::endl;
						}
						return true;
					}
				}
				return false;
			}//END of if in file Map
			return false;
		}
		catch (Exception e)
		{
			std::cerr << "Cannot get the nav eph file!" << std::endl;
			exit(-1);
		}
	}
	/// <summary>
	/// Match the bias file according to the observation time of the obs file
	/// </summary>
	/// <param name="epoch"></param>
	/// <param name="bia"></param>
	bool FindFile::findBiaFile(const CommonTime& epoch, std::string& bia)
	{
		YDSTime ydt(epoch);
		std::string year = std::to_string(ydt.year);
		std::string doy = std::to_string(ydt.doy);
		std::vector<std::string> acs{"GBM", "WUM", "COD","GRG","SHA","SIO","JAX"};
		std::vector<std::string> type{"FIN", "RAP"};
		// insert ac in 0 pos and then insert type in 7 pos
		std::string suffix = "OSB.BIA";
		std::string acType = "";
		std::string fName = "0MGX_" + year + doy + "0000_01D_01D_";
		auto it = fileMap.find(TypeID::BIASF);
		try
		{
			if (it != fileMap.end())
			{
				std::set<std::string> bias = fileMap[TypeID::BIASF];
				auto it_bia = bias.begin();
				for (it_bia; it_bia != bias.end(); ++it_bia)
				{
					std::vector<std::string> vBias;
					pathSplit(*it_bia, vBias);
					for (auto it_ac = acs.begin(); 
						it_ac != acs.end();
						it_ac++)
					{
						for (auto it_type = type.begin();
							it_type != type.end();
							it_type++)
						{
							std::string fName_ = fName;
							fName_.insert(0, *it_ac);
							fName_.insert(7, *it_type);
							//if (*it_ac == "GBM")
							//	suffix = "OSB.BIA";
							//else
							//	suffix = "ABS.BIA";
							fName_ += suffix;
							acType = *it_ac;
							if (upperCase(fName_) == upperCase(vBias.back()))
							{
								bia = *it_bia;
								if (debug)
								{
									std::cout << "Bia: " << bia << std::endl;
								}
								return true;
							}
						}	// END for loop type
					}	// END for loop ac
				}	// END for all bia files

			}//END of if in file Map
			// added by ragnwang
			std::string fName_ = fName;
			fName_.insert(0, "WUM");
			fName_.insert(7, "RAP");
			fName_ += "ABS.BIA";
			std::set<std::string> bias = fileMap[TypeID::BIASF];
			for(auto itBia = bias.begin();
				itBia != bias.end();
				++itBia)
			{
				std::vector<std::string> res;
				pathSplit(*itBia, res);
				if (res.back()==fName_)
				{
					bia = *itBia;
					return true;
				}
			}
			return false;
		}
		catch (...)
		{
            return false;
		}
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="epoch"></param>
	/// <param name="dcb"></param>
	/// <returns></returns>
	bool FindFile::findDCBFile(const CommonTime& epoch, std::string& dcb)
	{
		YDSTime ydt(epoch);
		std::string year = std::to_string(ydt.year);
		std::string doy = std::to_string(ydt.doy);
		std::string fName = "CAS0MGXRAP_" + year + doy + "0000_01D_01D_DCB.BSX";
		auto it = fileMap.find(TypeID::BIASF);
		try
		{
			if (it != fileMap.end())
			{
				std::set<std::string> dcbs = fileMap[TypeID::BIASF];
				auto it_dcb = dcbs.begin();
				for (it_dcb; it_dcb != dcbs.end(); ++it_dcb)
				{
					std::vector<std::string> vDcbF;
					pathSplit(*it_dcb, vDcbF);
					if (upperCase(fName) == upperCase(vDcbF.back()))
					{
						dcb = *it_dcb;
						if (debug)
						{
							std::cout << "DCB: " << std::endl;
						}
						return true;
					}
				}
				return false;
			}//END of if in file Map
			return false;
		}
		catch (const std::exception&)
		{
			std::cerr << "Cannot get the nav eph file!" << std::endl;
			exit(-1);
		}
	}
	/// <summary>
	/// Match the snx file according to the observation time of the obs file
	/// </summary>
	/// <param name="epoch"></param>
	/// <param name="snx"></param>
	bool FindFile::findSnxFile(const CommonTime& epoch, std::string& snx)
	{
		GPSWeekZcount gpsWeekT(epoch);
		int gpsW = gpsWeekT.week;
		std::string fName = "igs" + std::to_string(gpsW) + ".snx";
		auto it = fileMap.find(TypeID::SNXF);
		try
		{
			if (it != fileMap.end())
			{
				std::set<std::string> snxs = fileMap[TypeID::SNXF];
				auto it_snx = snxs.begin();
				// loop through all the .p file
				for (it_snx; it_snx != snxs.end(); ++it_snx)
				{
					std::vector<std::string> vSnxF;
					pathSplit(*it_snx, vSnxF);
					if (upperCase(fName) == upperCase(vSnxF.back()))
					{
						snx = *it_snx;
						if (debug)
						{
							std::cout << "snx: " << snx << std::endl;
						}
						return true;
					}
				} //END of for loop all snx file
				return false;
			}//END of if in file Map
			return false;
		}
		catch (Exception e)
		{
			std::cerr << "Cannot get the snx file!" << std::endl;
			exit(-1);
		}
	}
	/// <summary>
	/// Match the sgg ifcb file according to the observation time of the obs file
	/// </summary>
	/// <param name="epoch"></param>
	/// <param name="ifcb"></param>
	bool FindFile::findSGGIfcb(const CommonTime& epoch, std::vector<std::string>& ifcb)
	{
		YDSTime ydt0(epoch-86400);
		YDSTime ydt1(epoch);
		YDSTime ydt2(epoch+86400);
		std::string year0 = std::to_string(ydt0.year);
		std::string year1 = std::to_string(ydt1.year);
		std::string year2 = std::to_string(ydt2.year);
		std::string doy0 = std::to_string(ydt0.doy);
		std::string doy1 = std::to_string(ydt1.doy);
		std::string doy2 = std::to_string(ydt2.doy);
		std::string fName0 = "ifcb_" + year0 + doy0;
		std::string fName1 = "ifcb_" + year1 + doy1;
		std::string fName2 = "ifcb_" + year2 + doy2;
		std::set<std::string> fName{fName0, fName1, fName2};
		try
		{
			auto it = fileMap.find(TypeID::IFCBF);
			if (it != fileMap.end())
			{
				std::set<std::string> ifcbs = fileMap[TypeID::IFCBF];
				
				for (auto it_ifcb = ifcbs.begin();
					it_ifcb != ifcbs.end();
					it_ifcb++)
				{
					std::vector<std::string> vIfcb;
					pathSplit(*it_ifcb, vIfcb);
					for (auto it_name = fName.begin();
						it_name != fName.end();
						it_name++)
					{
						if (upperCase(*it_name) == upperCase(vIfcb.back()))
						{
							ifcb.push_back(*it_ifcb);
							if (debug)
							{
								std::cout << "ifcb: " << *it_ifcb << std::endl;
							}
						}
						if (ifcb.size() == 3)
						{
							std::sort(ifcb.begin(), ifcb.end());
							return true;
						}
					}	// end of for loop file name

				}	// EDN of for loop all ifcb
				return false;
			}// END of if it in fileMap
			return false;
		}
		catch (Exception e)
		{
			std::cerr << e << std::endl;
		}
		return false;
	}	// end of findSGGIFCB
	/// <summary>
	/// Match the sp3 file according to the observation time of the obs file
	/// </summary>
	/// <param name="epoch"></param>
	/// <param name="sp3Vec"></param>
	bool FindFile::findSp3File(const CommonTime& epoch, std::vector<std::string>& sp3Vec)
	{
		std::vector<std::string> acs{ "wum","igs","gbm","esa","cod","gfz","grg","jpl","com","grm" };
		GPSWeekZcount gpsWeekT(epoch);
		int week = gpsWeekT.week;
		int dow = gpsWeekT.getDayOfWeek();
		std::set<std::string> sp3Name;
		std::string sp3Name1;
		std::string sp3Name2;
		std::string sp3Name3;
		std::string ac("");
		// compute the sp3 file name: 
		sp3Name1 = std::to_string(week) + std::to_string(dow - 1) + ".sp3";
		sp3Name2 = std::to_string(week) + std::to_string(dow) + ".sp3";
		sp3Name3 = std::to_string(week) + std::to_string(dow + 1) + ".sp3";

		if (dow == 0)
			sp3Name1 = std::to_string(week - 1) + "6.sp3";
		else if (dow == 6)
			sp3Name3 = std::to_string(week + 1) + "0.sp3";

		// push_back into sp3Name;
		sp3Name.insert(sp3Name1);
		sp3Name.insert(sp3Name2);
		sp3Name.insert(sp3Name3);

		try
		{
			auto it = fileMap.find(TypeID::SP3F);
			if (it != fileMap.end())
			{
				std::set<std::string> sp3s = fileMap[TypeID::SP3F];
				auto it_sp3 = sp3s.begin();
				for (it_sp3;
					it_sp3 != sp3s.end();
					it_sp3++)
				{
					std::vector<std::string> vSp3;
					pathSplit(*it_sp3, vSp3);
					for (auto it_sp3N = sp3Name.begin();
						it_sp3N != sp3Name.end();
						it_sp3N++)
					{
						if (ac == "")
						{
							for (auto it_ac = acs.begin();
								it_ac != acs.end();
								it_ac++)
							{
								std::string sp3Name_full = *it_ac + (*it_sp3N);
								if (upperCase(sp3Name_full) == upperCase(vSp3.back()))
								{
									ac = *it_ac;
									sp3Vec.push_back(*it_sp3);
									sp3Name.erase(*it_sp3N);
									if (sp3Name.size() == 0 && sp3Vec.size() == 3)
									{
										sort(sp3Vec.begin(), sp3Vec.end());
										return true;
									}
									break;
								}
							}// END of for loop ac
							break;
						}
						else
						{
							std::string sp3Name_full = ac + (*it_sp3N);
							if (upperCase(sp3Name_full) == upperCase(vSp3.back()))
							{
								sp3Vec.push_back(*it_sp3);
								sp3Name.erase(*it_sp3N);
								if (sp3Name.size() == 0 && sp3Vec.size() == 3)
								{
									std::sort(sp3Vec.begin(), sp3Vec.end());
									return true;
								}
								break;
							}
						}	// END of if ac==""
					}//END of for loop sp3 name

				}//END for loop sp3 file
				return false;
			}//End if it in fileMap
			return false;
		}
		catch (const std::exception&)
		{
			exit(-1);
		}

	}
	/// <summary>
	/// Match the clk file according to the observation time of the obs file
	/// </summary>
	/// <param name="epoch"></param>
	/// <param name="clkVec"></param>
	bool FindFile::findClkFile(const CommonTime& epoch, std::vector<std::string>& clkVec)
	{
		std::vector<std::string> acs{ "wum","igs","gbm","esa","cod","gfz","grg","jpl","com","grm" };
		GPSWeekZcount gpsWeekT(epoch);
		int week = gpsWeekT.week;
		int dow = gpsWeekT.getDayOfWeek();
		std::set<std::string> clkName;
		std::string clkName1;
		std::string clkName2;
		std::string clkName3;
		std::string ac("");
		// compute the sp3 file name: 
		clkName1 = std::to_string(week) + std::to_string(dow - 1) + ".clk";
		clkName2 = std::to_string(week) + std::to_string(dow) + ".clk";
		clkName3 = std::to_string(week) + std::to_string(dow + 1) + ".clk";

		if (dow == 0)
			clkName1 = std::to_string(week - 1) + "6.clk";
		else if (dow == 6)
			clkName3 = std::to_string(week + 1) + "0.clk";

		clkName.insert(clkName1);
		clkName.insert(clkName2);
		clkName.insert(clkName3);

		try
		{
			auto it = fileMap.find(TypeID::CLKF);
			if (it != fileMap.end())
			{
				std::set<std::string> clks = fileMap[TypeID::CLKF];
				// loop clk files
				for (auto it_clk = clks.begin();
					it_clk != clks.end();
					it_clk++)
				{
					std::vector<std::string> vClk;
					pathSplit(*it_clk, vClk);
					// loop clk part name
					for (auto it_clkN = clkName.begin();
						it_clkN != clkName.end();
						it_clkN++)
					{
						if (ac=="")
						{
							for (auto it_ac = acs.begin();
								it_ac != acs.end();
								it_ac++)
							{
								std::string clkName_full = *it_ac + *it_clkN;
								if (upperCase(clkName_full) == upperCase(vClk.back()))
								{
									ac = *it_ac;
									clkVec.push_back(*it_clk);
									clkName.erase(*it_clkN);
									if (clkName.size() ==0 && clkVec.size() == 3)
									{
										std::sort(clkVec.begin(), clkVec.end());
										return true;
									}
									break;
								}
							}	//END of for loop ac
							if (ac != "") break;
						}
						else
						{
							if (upperCase(ac + *it_clkN) == upperCase(vClk.back()))
							{
								clkVec.push_back(*it_clk);
								clkName.erase(*it_clkN);
								if (clkName.size() == 0 && clkVec.size() == 3)
								{
									std::sort(clkVec.begin(), clkVec.end());
									return true;
								}
								break;
							}
						}	// END of if ac == ""
					}	//END of for loop clk name (just gpsweek and dow)
				}	//END of for loop clk file
			}	//END of if it in fileMap
			return false;
		}
		catch (const std::exception&)
		{
			exit(-1);
		}

	}
	/// <summary>
	/// Match the erp file according to the observation time of the obs file
	/// </summary>
	/// <param name="epoch"></param>
	/// <param name="erpSet"></param>
	bool FindFile::findErpFile(const CommonTime& epoch, std::vector<std::string>& erpVec)
	{
		std::vector<std::string> acs{ "WUM","GBM","COD","JAX" };
		std::vector<std::string> types{"FIN", "RAP", "ULT"};
		YDSTime ydt1(epoch - 86400);
		YDSTime ydt2(epoch);
		YDSTime ydt3(epoch + 86400);
		int year1(ydt1.year), year2(ydt2.year), year3(ydt3.year);
		int doy1(ydt1.doy), doy2(ydt2.doy), doy3(ydt3.doy);
		std::vector<std::string> erpName;
		std::string erpName1;
		std::string erpName2;
		std::string erpName3;
		std::string ac(""), type("");
		// compute the erp file name: 
		// WUM0MGXFIN_20222550000_01D_01D_ERP.ERP
		// 0 insert ac, 7 insert type
		// 0MGX_ + year + doy +0000_01D_01D_ERP.ERP
		erpName1 = "0MGX_" + std::to_string(year1) + std::to_string(doy1) + "0000_01D_01D_ERP.ERP";
		erpName2 = "0MGX_" + std::to_string(year2) + std::to_string(doy2) + "0000_01D_01D_ERP.ERP";
		erpName3 = "0MGX_" + std::to_string(year3) + std::to_string(doy3) + "0000_01D_01D_ERP.ERP";

		erpName.push_back(erpName1);
		erpName.push_back(erpName2);
		erpName.push_back(erpName3);

		try
		{
			auto it = fileMap.find(TypeID::ERPF);
			if (it != fileMap.end())
			{
				std::set<std::string> erps = fileMap[TypeID::ERPF];
				for (auto it_erp = erps.begin();
					it_erp != erps.end();
					it_erp++)
				{
					std::vector<std::string> vErp;
					pathSplit(*it_erp, vErp);
					for (auto it_erpN = erpName.begin();
						it_erpN != erpName.end();
						it_erpN++)
					{
						if (ac=="")
						{
							for (auto it_ac = acs.begin();
								it_ac != acs.end();
								it_ac++)
							{
								for(auto it_type = types.begin();
									it_type != types.end();
									it_type++)
								{
									std::string erpN = *it_erpN;
									std::string full_erpName =
										erpN.insert(0, *it_ac).insert(7, *it_type);
									if (upperCase(full_erpName) == upperCase(vErp.back()))
									{
										ac = *it_ac;
										type = *it_type;
										erpVec.push_back(*it_erp);
										erpName.erase(it_erpN);
										if (erpName.size() == 0 && erpVec.size() == 3)
										{
											std::sort(erpVec.begin(), erpVec.end());
											return true;
										}
										break;
									}
								} //End of for loop type
								if (ac != "") break;
							}	//end of for loop ac
						}
						else
						{
							std::string erpN = *it_erpN;
							std::string full_erpName =
								erpN.insert(0, ac).insert(7, type);
							if (upperCase(full_erpName) == upperCase(vErp.back()))
							{
								erpVec.push_back(*it_erp);
								erpName.erase(it_erpN);
								if (erpName.size() == 0 && erpVec.size() == 3)
								{
									std::sort(erpVec.begin(), erpVec.end());
									return true;
								}
								break;
							}
						}	// end of if ac == ""
						if (ac != "") break;
					}	//End of for loop erp name
				}	//End of for loop erp file
			}	// End of if it in fileMap
			return false;
		}
		catch (const std::exception&)
		{
			exit(-1);
		}
	}	// End of findErpFile

	bool FindFile::findUPDFile(const CommonTime& epoch, std::map<TypeID, std::string>& updMap)
	{
		// upd_ewl_####***_GEC, upd_wl_yeardoy_GREC, upd_nl_yeardoy_GREC
		YDSTime ydt(epoch);
		std::string ewl, wl, nl;
		std::string year(std::to_string(ydt.year)), doy(std::to_string(ydt.doy));

		ewl = "upd_ewl_" + year + doy;
		wl = "upd_wl_" + year + doy;
		nl = "upd_nl_" + year + doy;
		// find file
		try
		{
			auto it = fileMap.find(TypeID::UPDF);
			if (it != fileMap.end())
			{
				for (auto itUpd = it->second.begin();
					itUpd != it->second.end();
					++itUpd)
				{
					std::vector<std::string> fileVec;
					pathSplit(*itUpd, fileVec);
					std::string fileName(fileVec[fileVec.size()-1]);
					if (lowerCase(fileName.substr(0, 15)) == ewl)
					{
						updMap[TypeID::EWL] = *itUpd;
						continue;
					}
					else if (lowerCase(fileName.substr(0, 14)) == wl)
					{
						updMap[TypeID::WL] = *itUpd;
						continue;
					}
					else if (lowerCase(fileName.substr(0, 14)) == nl)
					{
						updMap[TypeID::NL] = *itUpd;
						continue;
					}
				}
				if (updMap.size() == 0)
				{
					Exception e("The correct UPD files cannot\
 be matched, please check the folder UPD files!");
					GPSTK_THROW(e);
					return false;
				}
				return true;
			}
			else
			{
				Exception e("fileMap does not contain UPD files!\
Please check if there is a UPD file in the folder!");
				GPSTK_THROW(e);
			}
			return false;
			
		}
		catch (Exception& e)
		{
			std::cerr << e.getLocation() << std::endl;
			std::cerr << e.getText() << std::endl;
			GPSTK_THROW(e);
		}
		return false;
	}	// End of findUPDFile

}