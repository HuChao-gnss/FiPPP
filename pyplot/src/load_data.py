import glob
import datetime
from Constants import *
import os


def get_files(dir_, suffix_):
    aim_files = dir_ + "\\*.*" + suffix_
    return glob.glob(aim_files)


def read_file(file_):
    with open(file_, 'r', encoding='utf-8') as rf:
        lines = rf.readlines()
        rf.close()
        pass
    return lines
    pass


def load_pos(pos, settings={}):
    lines = read_file(pos)
    if len(lines) == 0:
        return {}
    dates = []
    es = []
    ns = []
    us = []
    flags = []
    try:
        begin_sod = settings.get('begin_sod')
        end_sod = settings.get('end_sod')
    except ...:
        begin_sod = 0
        end_sod = 86400
        pass

    if begin_sod is None:
        begin_sod = 0
        end_sod = 86400
        pass
    for line in lines:
        temp_line = line.split()
        if len(temp_line) < 5:
            continue
            pass  # End of if len
        try:
            y = int(temp_line[0])
            m = int(temp_line[1])
            d = int(temp_line[2])
            h = int(temp_line[3])
            mm = int(temp_line[4])
            s = int(float(temp_line[5]))
            try:
                date = datetime(y, m, d, h, mm, s)
                sod = (date - datetime(y, m, d, 0, 0, 0)).seconds
                pass
            except ...:
                print("Cannot get date in line: " \
                      + line)
                continue
                pass

            e = float(line[80:89])
            n = float(line[89:98])
            u = float(line[98:107])
            flag = int(float(line[116:119]))
        except ...:
            raise ValueError("Cannot get data!")
            pass
        if begin_sod != 0 or end_sod != 86400:
            if sod < begin_sod or sod > end_sod:
                continue
                pass
            pass

        dates.append(date)
        es.append(e)
        ns.append(n)
        us.append(u)
        flags.append(flag)
        pass  # End of for line in lines

    return {DATE_TIME: dates, E: es, N: ns, U: us, FLAG: flags}
    pass  # End of load_pos(pos)


def load_satNumValue(infile, settings):

    try:
        lines = read_file(infile)
    except ...:
        print("Cannot open the file: " + infile)
        return
    pass

    numSats = []
    values = []
    dates = []
    for line in lines:
        temp_line = line.split()
        if len(temp_line) < 9:
            continue
            pass  # End of if len
        try:
            y = int(temp_line[1])
            m = int(temp_line[2])
            d = int(temp_line[3])
            h = int(temp_line[4])
            mm = int(temp_line[5])
            s = int(float(temp_line[6]))
            date = datetime(y, m, d, h, mm, s)
            num = int(temp_line[9])
            value = float(temp_line[11])
        except ...:
            raise ValueError("Cannot get data!")
            pass
        dates.append(date)
        numSats.append(num)
        values.append(value)
        pass  # End of for line in lines
    return {DATE_TIME: dates, NSAT: numSats, VALUE: values}
    pass  # End of load_pdop


def load_satTypeValue(infile, settings):
    sat = ''
    freq = 0
    LP = 0
    try:
        freq = settings.get('frequency')
        try:
            LP = settings.get('LP')
        except ...:
            LP = 0
            pass
        sat = settings.get('sat')
    except... as e_:
        print(e_)
        pass

    # check sat:
    syses = []
    sats = []
    temp_sat = sat.split('/')
    if temp_sat[0] in ['G', 'E', 'C']:  # system
        syses = temp_sat
        pass
    else:  # satellites
        sats = temp_sat
        pass
    if not (len(syses) + len(sats)):
        print('Please check the "sat" values!')
        exit(-1)
        pass
    aimTypes = []
    if freq & FIRST:
        aimTypes.extend(Types[FIRST])
        pass
    if freq & SECOND:
        aimTypes.extend(Types[SECOND])
        pass
    if freq & THIRD:
        aimTypes.extend(Types[THIRD])
        pass
    if freq & FOURTH:
        aimTypes.extend(Types[FOURTH])
        pass
    if freq & FIFTH:
        aimTypes.extend(Types[FIFTH])
        pass

    aimTypesL = []
    aimTypesP = []
    aimTypesBL = []
    if not LP:
        aimTypesL = ['L' + v for v in aimTypes]
        aimTypesBL = ['BL' + v for v in aimTypes]
        pass
    else:
        aimTypesP = ['P' + v for v in aimTypes]
        pass

    res_dic = {}
    date = datetime(2023, 4, 25, 9, 50, 0)
    lines = read_file(infile)
    for line in lines:
        #
        try:
            # time stamp
            temp_line = line.split()
            if line[0] == "#":
                y = int(temp_line[1])
                m = int(temp_line[2])
                d = int(temp_line[3])
                h = int(temp_line[4])
                mm = int(temp_line[5])
                s = int(float(temp_line[6]))
                date = datetime(y, m, d, h, mm, s)
                continue
                pass
            sat = temp_line[0]
            if (sat not in sats) and (sat[0] not in syses):
                continue
                pass
            if sat not in res_dic.keys():
                res_dic[sat] = {}
                pass
            if date not in res_dic[sat].keys():
                res_dic[sat][date] = {}
                res_type = ""
            for i in range(len(temp_line)):
                if i == 0:
                    continue  # sat
                if i % 2:  # the type
                    res_type = temp_line[i]
                    pass
                else:
                    if (res_type not in aimTypesL) and \
                            (res_type not in aimTypesP) and \
                            (res_type not in aimTypesBL):
                        continue
                        pass
                    res_dic[sat][date][res_type] = \
                        float(temp_line[i])
                    pass
                pass  # End of for i in range(len(temp_line))

        except ...:
            raise ValueError("Cannot get data from: " + infile)

            pass  # End of get data

        pass  # End of for line in lines

    return res_dic
    pass  # End of load_ambiguity(res)


def load_eleAzi(ele, settings):
    sat = ''
    try:
        sat = settings.get('sat')
    except...:
        print('Cannot get sat infos!')
        exit(-1)
    # check sat:
    syses = []
    sats = []
    temp_sat = sat.split('/')
    if temp_sat[0] in ['G', 'E', 'C']:  # system
        syses = temp_sat
        pass
    else:  # satellites
        sats = temp_sat
        pass

    res_dic = {}
    date = datetime(2023, 4, 25, 9, 50, 0)
    lines = read_file(ele)
    for line in lines:
        temp_line = line.split()
        if line[0] == "#":
            y = int(temp_line[1])
            m = int(temp_line[2])
            d = int(temp_line[3])
            h = int(temp_line[4])
            mm = int(temp_line[5])
            s = int(float(temp_line[6]))
            date = datetime(y, m, d, h, mm, s)
            continue
            pass
        sat = temp_line[0]
        if (sat not in sats) and (sat[0] not in syses):
            continue
        if sat not in res_dic.keys():
            res_dic[sat] = {}
            pass
        if date not in res_dic[sat].keys():
            res_dic[sat][date] = {}
            pass
        res_dic[sat][date][ELE] = float(temp_line[1])
        res_dic[sat][date][AZI] = float(temp_line[2])

        pass  # End of for line in lines

    return res_dic
    pass  # End of load_eleAzi


def get_obs_stations(obs_dir):
    """

    :param obs_dir:
    :return:
    """
    sta_list = []
    obs_files = get_files(obs_dir, 'o')
    for obs in obs_files:
        temp_obs = obs.split(SPLIT)[-1][0:4].upper()
        sta_list.append(temp_obs)
        pass
    return sta_list
    pass  # End of get obs stations


def check_enu_value(dic_={}):
    """

    :param dic_:
    :return:
    """
    keys = dic_.keys()
    nums = []
    posValue = {}
    obsNames = []
    for k in keys:
        obsName = str(k).split("_")[-1].split(".")[0][0:4].upper()
        obsNames.append(obsName)
        num = len(dic_[k][E])
        nums.append(num)
        posValue[k] = num
        pass  # End of for k in keys

    if len(set(obsNames)) != 1:
        raise ValueError("Please input the same Station")

    if len(set(nums)) != 1:
        for k in posValue.keys():
            print(k + " : " + posValue[k].__str__() + " epochs!")
            pass
        raise ValueError("Please input the pos file \
        with the same number of observation epochs")
        pass

    pass


def check_inputType(str_):
    """

    :param str_:
    :return:
    """
    temp_type = str_.split('/')
    lennum = []
    for v in temp_type:
        lennum.append(len(v))
        pass
    if len(set(lennum)) != 1:
        raise TypeError("Please enter satellite or system!")

    pass
