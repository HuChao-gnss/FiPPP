import os

from load_data import *
from plot import *
from plot_conf import *
from count_data import *


def countENU_():
    try:
        pos_dir = countSettings.get('posdir')
        poss = countSettings.get('pos')
        save_csv = countSettings.get('savecsv')
        save_path = countSettings.get('savepath')
        if pos_dir != '':
            poss = get_files(pos_dir, 'pos')
            pass

        # get res
        res_dic = {}
        if save_csv:
            if save_path == '':
                save_path = '..\\' + poss[0].split(SPLIT)[-2] + '.csv'
                pass
            if os.path.exists(save_path):
                os.remove(save_path)
                pass  # End of if os.path.exists
            with open(save_path, 'a', encoding='utf-8') as of:
                of.write('# posname, E/cm, N/cm, U/cm, E/min, N/min, U/min, FixRate, TTFF/min')
                of.write('\n')
                for pos in poss:
                    print(pos.split(SPLIT)[-1], end='')
                    print(count_fixed_rate(pos))
                    fixed = count_fixed_rate(pos)
                    line = ''
                    res_dic = count_acc_time(pos, countSettings)
                    ttff = count_ttff(pos)
                    pos_name = res_dic['pos']
                    acc = res_dic['acc']
                    time = res_dic['time']
                    if acc[0] == NAN:
                        line += '#'
                        pass
                    line += pos_name + ','
                    line += '{:.4f}'.format(acc[0] * 100) + ',' + '{:.4f}'.format(acc[1] * 100) + ',' + '{:.4f}'.format(
                        acc[2] * 100) + ','
                    line += '{:.4f}'.format(time[0]) + ',' + '{:.4f}'.format(time[1]) + ',' + '{:.4f}'.format(time[2]) + ','
                    line += '{:.4f}'.format(fixed) + ',' + '{:.4f}'.format(ttff)
                    line += '\n'
                    of.write(line)
                    print(line)
                    pass  # End of for pos in poss
                of.close()
                pass  # End of with open
            pass  # End of if save_csv
        else:
            for pos in poss:
                print(pos, end='')
                print(count_fixed_rate(pos))
                print(count_acc_time(pos, countSettings))
                pass
            pass

    except ...:
        print('Cannot get the params!')
        exit(-1)
        pass
    pass


if __name__ == '__main__':

    # count_acc_time()
    if countENU:
        countENU_()
        pass  # End of if countENU

    if plotStations:
        obsFilesDir = stationSettings.get('obsFilesDir')
        stations = stationSettings.get('stations')
        sta_list = get_obs_stations(obsFilesDir)
        if not sta_list:
            sta_list = stations
            pass
        if not sta_list:
            print('Please input station names!')
            exit(-1)
        plot_stations(sta_list, stationSettings)
        pass

    if plotPDOP:
        try:
            pdop = load_satNumValue(PDOPFile, pdopSettings)
            if pdop != {}:
                plot_satNumValue(pdop, pdopSettings, 'PDOP')
        except ... as e:
            print(e)
        pass

    if plotTropo:
        try:
            tropo = load_satNumValue(tropoFile, tropoSettings)
            if tropo != {}:
                plot_satNumValue(tropo, tropoSettings, 'WT')
        except ... as e:
            print(e)
        pass  # End of if plotTropo

    if plotENU:
        try:
            posFiles = enuSettings.get('posFiles')
            if len(posFiles) != 0:
                pos_dic = {}
                print(posFiles)
                for pos in posFiles:
                    posName = pos.split(SPLIT)[-1]
                    enu = load_pos(pos, enuSettings)
                    pos_dic[posName] = enu
                    pass  # End of for pos in posFiles

                plot_enu(pos_dic, enuSettings)
                pass
        except ... as e:
            print(e)
            pass
        pass  # End of if plotENU

    if plotResidual:
        value_dic = load_satTypeValue(residualFile, resSettings)
        if value_dic != {}:
            plot_satTypeValue(value_dic, resSettings)
        pass  # End of if plotResidual

    if plotAmbiguity:
        value_dic = load_satTypeValue(residualFile, ambSettings)
        if value_dic != {}:
            plot_satTypeValue(value_dic, ambSettings)
        pass  # End of if plotResidual

    if plotEleAzi:
        value_dic = load_eleAzi(elevationFile, eleaziSettings)
        if value_dic != {}:
            plot_satZenithMap(value_dic, eleaziSettings)

    pass  # End of main
