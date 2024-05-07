from load_data import *
import pandas as pd
from numpy import *


def count_fixed_rate(pos):
    flags = []
    try:
        res = load_pos(pos)
        if res == {}:
            return 0
        flags = res[FLAG]
    except ... as e:
        print(e)
        return -1
        pass
    total = len(flags)
    if total == 0:
        return 0
    return flags.count(7) / total
    pass


def count_ttff(pos):
    """
    This function is used to count TTFF
    :param pos:
    """
    flags = []
    times = []
    res = load_pos(pos)
    if res == {}:
        return -1.0
    flags = res[FLAG]
    times = res[DATE_TIME]
    first_time = times[0]
    for i in range(len(flags)):
        if 6 not in flags[i: i + 10]:
            ttff_time = times[i]
            return (ttff_time - first_time).seconds / 60.0
        pass

    return -1.0
    pass


def count_acc_time(pos, settings={}):
    """
    This function is used to count the
    convenience accuracy and time.
    :param pos: gamp or mp software positioning result file.
    :param settings: the accuracy limitation.
    """
    pos_name = pos.split(SPLIT)[-1]
    try:
        value_dic = load_pos(pos)
        if value_dic == {}:
            return {
                'pos': pos_name,
                'acc': [NAN, NAN, NAN],
                'time': [NAN, NAN, NAN],
            }
    except ...:
        print('Cannot load pos file!')
        exit(-1)
        pass
    try:
        elimit = settings.get('elimit')
        nlimit = settings.get('nlimit')
        ulimit = settings.get('ulimit')

    except ...:
        print('Cannot get limit!')
        exit(-1)
        pass

    # value_dic: dates, e, n, u, flag
    dates = value_dic[DATE_TIME]
    es = value_dic[E]
    ns = value_dic[N]
    us = value_dic[U]
    first_time = dates[0]
    el = []
    nl = []
    ul = []
    e_t = 0.
    n_t = 0.
    u_t = 0.
    n_epoch = int(len(es))
    ind_e = n_epoch
    ind_n = n_epoch
    ind_u = n_epoch
    e_flag = False
    n_flag = False
    u_flag = False
    for i in range(len(dates)):
        temp_date = dates[i]
        temp_es = [abs(x) for x in es[i:i + 20]]
        temp_ns = [abs(x) for x in ns[i:i + 20]]
        temp_us = [abs(x) for x in us[i:i + 20]]
        if max(temp_es) < elimit:
            e_flag = True
            pass
        if max(temp_ns) < nlimit:
            n_flag = True
            pass
        if max(temp_us) < ulimit:
            u_flag = True
            pass

        if e_flag:
            ind_e = i
            if e_t == 0.:
                e_t = (temp_date - first_time).seconds / 60.0
            pass

        if n_flag:
            ind_n = i
            if n_t == 0.:
                n_t = (temp_date - first_time).seconds / 60.0
            pass

        if u_flag:
            ind_u = i
            if u_t == 0.:
                u_t = (temp_date - first_time).seconds / 60.0
            pass

        if e_flag and n_flag and u_flag:
            break
            pass
        pass  # End of for i in n_epoch

    el = [abs(v) for v in es[ind_e:]]
    nl = [abs(v) for v in ns[ind_n:]]
    ul = [abs(v) for v in us[ind_u:]]

    if not len(el) * len(nl) * len(ul):
        return {
            'pos': pos_name,
            'acc': [NAN, NAN, NAN],
            'time': [NAN, NAN, NAN],
            'max': [NAN, NAN, NAN],
        }
    pass  # End of if not len(el) * len(nl) * len(ul)

    mean_e = mean(el)
    mean_n = mean(nl)
    mean_u = mean(ul)

    return {
        'pos': pos_name,
        'acc': [mean_e, mean_n, mean_u],
        'time': [e_t, n_t, u_t],
    }

    pass
