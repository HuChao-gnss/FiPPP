#pragma ident "$Id$"

/**
 * @file BiasDataReader.cpp
 * Class to read Bias data.
 */

#include "BiasDataReader.hpp"
#include "StringUtils.hpp"
#include "RinexSatID.hpp"
#include "YDSTime.hpp"

using namespace std;
using namespace gpstk::StringUtils;

#define debug 0

namespace gpstk
{


    // Method to store load Bias data in this class' data map
    void BiasDataReader::loadData()
        noexcept(false)
    {

        try
        {
            satBiasData.clear();

            // a buffer
            string line;

            // read
            while( true )
            {
                getline(strm, line);
                if(strm.eof())
                {
                    break;
                }

                if(line.substr(1,9) == "BIAS_MODE")
                {
                    mode = line.substr(41,8);
                }

                if(line.substr(1,4) == "DSB ")
                {
                    mode = "RELATIVE";
                    string sys( line.substr(11,1) );
                    string sat( line.substr(11, 3) );
                    string sta( line.substr(15, 9) );

                    // skip station bias
                    if( sta != string(9,' ') ) continue;

                    string type1( line.substr(25, 4) );
                    string type2( line.substr(30, 4) );

                    string time1( line.substr(35,14) );
                    string time2( line.substr(50,14) );

                    YDSTime ct1( asInt(time1.substr(0,4)),
                                 asInt(time1.substr(5,3)),
                                 asDouble(time1.substr(9,5)),
                                 TimeSystem::GPS );

                    YDSTime ct2( asInt(time2.substr(0,4)),
                                 asInt(time2.substr(5,3)),
                                 asDouble(time2.substr(9,5)),
                                 TimeSystem::GPS );

                    CommonTime start( ct1.convertToCommonTime() );

                    CommonTime end;

                    if(time2 == "0000:000:00000")
                    {
                        end = CommonTime::END_OF_TIME;
                    }
                    else
                    {
                        end = ct2.convertToCommonTime();
                    }

                    string index( sat + " " + type1 + type2 );

                    double value( asDouble(line.substr(70,21)) );
                    double std  ( asDouble(line.substr(92,11)) );

//                  cout << setw(15) << index
//                       << setw(30) << time1
//                       << setw(30) << time2
//                       << setw(10) << value
//                       << setw(10) << std
//                       << endl;

                    BiasData biasData;
                    biasData.start = start;
                    biasData.end   = end;
                    biasData.value = value;
                    biasData.std   = std;

                    satBiasData.insert( make_pair(index, biasData) );
                }
                else if(line.substr(1,4) == "OSB ")
                {
                    mode = "ABSOLUTE";
                    string sys( line.substr(11,1) );
                    string sat( line.substr(11, 3) );
                    string sta( line.substr(15, 9) );

                    // skip station bias
                    if( sta != string(9,' ') ) continue;

                    string type1( line.substr(25, 4) );
                    string type2( line.substr(30, 4) );

                    string time1( line.substr(35,14) );
                    string time2( line.substr(50,14) );

                    YDSTime ct1( asInt(time1.substr(0,4)),
                                 asInt(time1.substr(5,3)),
                                 asDouble(time1.substr(9,5)),
                                 TimeSystem::GPS );

                    YDSTime ct2( asInt(time2.substr(0,4)),
                                 asInt(time2.substr(5,3)),
                                 asDouble(time2.substr(9,5)),
                                 TimeSystem::GPS );

                    CommonTime start( ct1.convertToCommonTime() );

                    CommonTime end;

                    if(time2 == "0000:000:00000")
                    {
                        end = CommonTime::END_OF_TIME;
                    }
                    else
                    {
                        end = ct2.convertToCommonTime();
                    }

                    string index( sat + " " + type1 + type2 );

					double value = 0.0;
					double std = 0.0;
                    if (line.length() >= 91)
                    {
						value = asDouble(line.substr(70, 21));
                    }
                    if (line.length() >= 103)
                    {
                        std = asDouble(line.substr(92, 11));
                    }

                    if(debug)
                    {
                        cout << setw(15) << index
                             << setw(30) << time1
                             << setw(30) << time2
                             << setw(10) << value
                             << setw(10) << std
                             << endl;
                    }

                    BiasData biasData;
                    biasData.start = start;
                    biasData.end   = end;
                    biasData.value = value;
                    biasData.std   = std;

                    satBiasData.insert( make_pair(index, biasData) );
                }

            } // End of "while( true )"

            strm.close();

        }  // End of try block
        catch (...)
        {
            // We should close this data stream before returning
            strm.close();

            return;
        }

    }  // End of 'BiasDataReader::loadData()'


    // Method to open AND load Bias data file.
    void BiasDataReader::open(const char* fn)
    {
        // We need to be sure current data stream is closed
        strm.close();

        // Open data stream
        strm.open(fn, std::ios::in);
        if( !strm.is_open() )
        {
            std::cerr << "The file " << fn << " doesn't exist!" << std::endl;
            exit(-1);
        }

        loadData();

        return;

    }  // End of method 'BiasDataReader::open()'


    // Method to open AND load Bias data file. It doesn't
    // clear data previously loaded.
    void BiasDataReader::open(const string& fn)
    {

        // We need to be sure current data stream is closed
        strm.close();

        // Open data stream
        strm.open(fn.c_str(), std::ios::in);
        if( !strm.is_open() )
        {
            std::cerr << "The file " << fn << "doesn't exist!" << std::endl;
            exit(-1);
        }

        loadData();

        return;

    }  // End of method 'BiasDataReader::open()'


    /* Get satellite Bias.
     *
     * sat      SatID
     * type1    Type of Bias, e.g., "C1C" for DSB or OSB
     * type2    Type of Bias, e.g., "C1W" for DSB, " " for OSB
     * time     Time
     *
     */
    double BiasDataReader::getSatBias( const RinexSatID/*SatID*/& sat,
                                       const std::string& type1,
                                       const std::string& type2,
                                       const CommonTime& time )
    {
        double ret(0.0);

        if( mode == "RELATIVE" )
        {
            // index of multimap
            string index;

            index += sat.toString();
            index += string(1,' ');
            index += type1;
            index += string(1,' ');
            index += type2;
            index += string(1,' ');

            multimap<string,BiasData>::iterator it=satBiasData.find(index);
            if(it==satBiasData.end())
            {
                /*Exception e("type not found in satBiasData!");*/
				//GPS     C1C - C1W, C2C - C2W, C2W - C2S, C2W - C2L, C2W - C2X
				//GPS     C1C - C2W, C1C - C5Q, C1C - C5X, C1W - C2W
            }

            multimap<string,BiasData>::iterator beg( satBiasData.lower_bound(index) );
            multimap<string,BiasData>::iterator end( satBiasData.upper_bound(index) );

            for(multimap<string,BiasData>::iterator it = beg;
                it != end;
                ++it)
            {
                CommonTime start( it->second.start );
                CommonTime end  ( it->second.end + 86400.0  );

                if( time >= start && time < end )
                {
                    ret = it->second.value;
                }
                else
                {
                    cerr << "time is not in the data span!" << endl;
                    exit(-1);
                }
            }
        }
        else if( mode == "ABSOLUTE" )
        {
            // index of multimap
            string index1;

            index1 += sat.toString();
            index1 += string(1,' ');
            index1 += type1;
            index1 += string(1,' ');
            index1 += string(3,' ');
            index1 += string(1,' ');

            multimap<string,BiasData>::iterator it=satBiasData.find(index1);
            if(it==satBiasData.end())
            {
                Exception e("type not found in satBiasData!");
            }

            multimap<string,BiasData>::iterator beg1( satBiasData.lower_bound(index1) );
            multimap<string,BiasData>::iterator end1( satBiasData.upper_bound(index1) );

            for(multimap<string,BiasData>::iterator it = beg1;
                it != end1;
                ++it)
            {
                CommonTime start( it->second.start );

                // end time span for for current day of CAS data is the beginning of the day.
                // so plus 86400s to extend the valid span
                CommonTime end  ( it->second.end + 86400.0  );

                if(debug)
                {
                    cout << "BiasDataReader" << endl;
                    cout << "start:" << YDSTime(start) << endl;
                    cout << "end:" << YDSTime(end) << endl;
                    cout << "time:" << YDSTime(time) << endl;
                }

                if( time >= start && time < end )
                {
                    ret = it->second.value;
                }
            }
        }

        return ret;

    } // End of 'double BiasDataReader::getSatBias(...)'


	/// <summary>
	/// the first and second frq dcb must be need.
	/// </summary>
	/// <param name="sat"></param>
	/// <param name="typeVec"></param>
	/// <param name="time">current epoch</param>
	/// <returns></returns>
	std::map<std::string, double> BiasDataReader::getSatDCB(const RinexSatID sat,
		const std::vector<std::string>& typeVec, const CommonTime& time, const bool bdsfopt)
    {
        // dcbData
        std::map<std::string, double> dcbDataMap;
        //
        std::string index("");
        string channel(""), type("");
        SatelliteSystem sys(sat.system);
        std::string satStr = sat.toString();

        multimap<string, BiasData>::iterator it;
        try
        {
			if (sys == SatelliteSystem::GPS)
			{
				index = satStr + " C1C C1W ";
				// search the index value
				it = satBiasData.find(index);
				if (it == satBiasData.end())
				{
					Exception e("type not found in satBiasData!");
					cerr << e << endl;
				}
				CommonTime start(it->second.start);
				CommonTime end(it->second.end + 86400);

				if (time < start || time >= end)
				{
					cerr << "time is not in the data span!" << endl;
					exit(-1);
				}
                double c1w1 = it->second.value;

				index = satStr + " C1W C2W ";
				it = satBiasData.find(index);
				if (it == satBiasData.end())
				{
					Exception u;
					u.setErrorId(1);
					u.addText("Cannot get " + index + " basic DCB value!");
					GPSTK_THROW(u);
				}
                double w1w2 = it->second.value;
				dcbDataMap["G1W2W"] = w1w2;

				std::string L1(""), L2(""), L5("");
				for (auto it_type = typeVec.begin();
					it_type != typeVec.end();
					it_type++)
				{
					std::string frq((*it_type).substr(1, 1));
					if (frq == "1")  L1 = *it_type;
					else if (frq == "2") L2 = *it_type;
					else if (frq == "5") L5 = *it_type;
				}
				if (L2 != "")
				{   // 1w - 2?
					if (L2.substr(2, 1) != "W")
					{
						channel = L2.substr(2, 1);
						index = satStr + " C2W " + L2 + " ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						
                        double w2x2 = it->second.value;
                        type = "G1W2" + channel;
						dcbDataMap[type] = w1w2 + w2x2;
					}
				}   // end of if "L2 !="""

				if (L5 != "")
				{   // 1w-5?
					channel = L5.substr(2, 1);
					index = satStr + " C1C " + L5 + " ";
					it = satBiasData.find(index);
					if (it == satBiasData.end())
					{
						Exception e("type not found in satBiasData!");
						cerr << e << endl;
					}
					double c1x5 = it->second.value;
					type = "G1W5" + channel;
					dcbDataMap[type] = c1x5 - c1w1;
				}

			}   // end of if gps
			if (sys == SatelliteSystem::Galileo)
			{
				string L1(""), L5a(""), L5b(""), L5(""), L6("");
				for (auto it_type = typeVec.begin();
					it_type != typeVec.end();
					it_type++)
				{
					std::string frq((*it_type).substr(1, 1));
					if (frq == "1")  L1 = *it_type;
					else if (frq == "5") L5a = *it_type;
					else if (frq == "7") L5b = *it_type;
					else if (frq == "8") L5 = *it_type;
					else if (frq == "6") L6 = *it_type;
				}

				index = satStr + " C1X C5X ";
				it = satBiasData.find(index);
				if (it == satBiasData.end())
				{
					Exception u;
					u.setErrorId(1);
					u.addText("Cannot get " + index + " basic DCB value!");
					GPSTK_THROW(u);
				}
				dcbDataMap["E1X5X"] = it->second.value;
				CommonTime start(it->second.start);
				CommonTime end(it->second.end + 86400);
				if (time < start || time >= end)
				{
					cerr << "time is not in the data span!" << endl;
					exit(-1);
				}
				index = satStr + " C1C C5Q ";
				it = satBiasData.find(index);
				if (it == satBiasData.end())
				{
					Exception u;
					u.setErrorId(1);
					u.addText("Cannot get " + index + " basic DCB value!");
					GPSTK_THROW(u);
				}
				dcbDataMap["E1C5Q"] = it->second.value;

				if (L5a != "")
				{
					channel = L5a.substr(2, 1);
					if (channel == "X")
					{
						index = satStr + " C1X C5X ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						dcbDataMap["E1X5X"] = it->second.value;
					}
					else
					{
						index = satStr + " C1C C5Q ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						dcbDataMap["E1C5Q"] = it->second.value;
					}
				}
				if (L5b != "")
				{
					if (L5b.substr(2, 1) == "X")
					{
						index = satStr + " C1X C7X ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						dcbDataMap["E1X7X"] = it->second.value;
					}
					else
					{
						index = satStr + " C1C C7Q ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						dcbDataMap["E1C7Q"] = it->second.value;
					}
				}
				if (L5 != "")
				{
					if (L5.substr(2, 1) == "X")
					{
						index = satStr + " C1X C8X ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						dcbDataMap["E1X8X"] = it->second.value;
					}
					else
					{
						index = satStr + " C1C C8Q ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						dcbDataMap["E1C8Q"] = it->second.value;
					}
				}
				if (L6 != "")
				{
					index = satStr + " C1C C6C ";
					it = satBiasData.find(index);
					if (it == satBiasData.end())
					{
						Exception e("type not found in satBiasData!");
						cerr << e << endl;
					}
					dcbDataMap["E1C6C"] = it->second.value;
				}
			}   // end of if gal
			if (sys == SatelliteSystem::BeiDou)
			{   // 2671587 2i/6i/7i/1c/2a/2/2b
				string L2(""), L6(""), L7(""), L1(""), L5(""), L8("");
				// for loop type
				for (auto it_type = typeVec.begin();
					it_type != typeVec.end();
					it_type++)
				{
					std::string frq((*it_type).substr(1, 1));
					if (frq == "2")  L2 = *it_type;
					else if (frq == "6") L6 = *it_type;
					else if (frq == "7") L7 = *it_type;
					else if (frq == "1") L1 = *it_type;
					else if (frq == "5") L5 = *it_type;
					else if (frq == "8") L8 = *it_type;
				}   // end for loop typeVec
				index = satStr + " C2I C6I ";
				it = satBiasData.find(index);
				if (it == satBiasData.end())
				{
					Exception u;
					u.setErrorId(1);
					u.addText("Cannot get " + index + " basic DCB value!");
					GPSTK_THROW(u);
				}
				CommonTime start(it->second.start);
				CommonTime end(it->second.end + 86400);
				if (time < start || time >= end)
				{
					cerr << "time is not in the data span!" << endl;
					exit(-1);
				}
				dcbDataMap["C2I6I"] = it->second.value;
				double i2i6 = it->second.value;
				if (bdsfopt)
				{   // B1c/B2a/B1I/B3I/B2
					index = satStr + " C1X C6I ";
					it = satBiasData.find(index);
					if (it == satBiasData.end())
					{
						Exception e("type not found in satBiasData!");
						cerr << e << endl;
					}
                    double x1i6 = it->second.value;
					dcbDataMap["C1X6I"] = x1i6;
                    dcbDataMap["C1X2I"] = x1i6 - i2i6;
					index = satStr + " C1P C6I ";
					it = satBiasData.find(index);
					if (it == satBiasData.end())
					{
						Exception e("type not found in satBiasData!");
						cerr << e << endl;
					}
                    double p1i6 = it->second.value;
					dcbDataMap["C1P6I"] = p1i6;
                    dcbDataMap["C1P2I"] = p1i6 - i2i6;

					//index = satStr + " C1D C6I ";
					//dcbDataMap["C1D6I"] = satBiasData.find(index)->second.value;
					if (L5 != "")
					{   // get dcb12 1?-5?
						channel = L5.substr(2, 1);
						index = satStr + " C1" + channel + " C5" + channel + " ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						type = "C1" + channel + "5" + channel;
						dcbDataMap[type] = it->second.value;
					}
					if (L8 != "")
					{
						index = satStr + " C1X C8X ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						dcbDataMap["C1X8X"] = it->second.value;
					}
				} // if(bdsfopt)
				else
				{
					if (L7 != "" && L7.substr(2, 1) == "I")
					{
						// B2I/ not use B2b
						index = satStr + " C2I C7I ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						dcbDataMap["C2I7I"] = it->second.value;
					}
					if (L1 != "")
					{   // get 2I-1?
						channel = L1.substr(2, 1);
						index = satStr + " " + L1 + " C6I ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						double x1i6 = it->second.value;
						type = "C2I1" + channel;
						dcbDataMap[type] = i2i6 - x1i6;
					}
					if (L5 != "")
					{   // get 2I-5?
						channel = L5.substr(2, 1);
						type = "C2I5" + channel;
						index = satStr + " C1" + channel + " C5" + channel + " ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						double x1x5 = it->second.value; // C1?-C5?
						index = satStr + " C1" + channel + " C6I ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						double x1i6 = it->second.value; // C1?-C6I
						dcbDataMap[type] = i2i6 + x1x5 - x1i6;
					}
					if (L8 != "")
					{
						index = satStr + " C1X C8X ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						double x1x8 = it->second.value;
						index = satStr + " C1X C6I ";
						it = satBiasData.find(index);
						if (it == satBiasData.end())
						{
							Exception e("type not found in satBiasData!");
							cerr << e << endl;
						}
						double x1i6 = it->second.value;
						dcbDataMap["C2I8X"] = i2i6 - x1i6 + x1x8;
					}
				}   // end of if(bdsfopt)
			} // end of if bds

        }
        catch (Exception& e)
        {
            cerr << e.getText() << endl;
			//return dcbDataMap;
            GPSTK_THROW(e);
        }
        // calculate DCB value
        return dcbDataMap;
    }


}  // End of namespace gpstk
