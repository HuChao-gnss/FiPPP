import datetime
import math
import matplotlib.pyplot as plt
import matplotlib as mpl
import matplotlib.dates as mdate
import numpy as np
import pandas as pd
from load_data import *
import cartopy.crs as ccrs
import cartopy.feature as cfeature
from cartopy.mpl.ticker import LongitudeFormatter, LatitudeFormatter

from plot_conf import *

colors = [
    'orangered', 'forestgreen', 'deepskyblue', 'orange', 'purple', 'slategrey', 'lime', 'darkblue',
    'yellow', 'rosybrown', 'lightcoral', 'teal', 'maroon', 'royalblue', 'violet', 'firebrick']


# Draw the distribution status of
# observation stations on the map projection
# Reference observation station data comes from https://network.igs.org/
# File name: igs_stations.csv
# Updated as of 2023-04-24
def plot_stations(sta_list, settings={}):
    # # get params
    global fonttype, dpi, lbsize, showname, saveflag, aimpic, marker, ms
    try:
        dpi = settings.get('plotdpi')
        marker = settings.get('marker')
        ms = settings.get('markersize')
        lbsize = settings.get('lablesize')
        fonttype = settings.get('fonttype')
        showname = settings.get('showname')
        ftsize = settings.get('fontsize')
        savepath = settings.get("path")
        os.makedirs(savepath, exist_ok=True)
        saveflag = settings.get('savefig')
        savename = settings.get('savename') + '.' + settings.get('savetype')
        aimpic = savepath + SPLIT + savename
    except ...:
        print("Cannot get the params in plot_station!")
        exit(-1)
        pass
    # load igs stations infos
    igs = np.recfromcsv('igs_stations.csv', names=True, encoding='utf-8')

    fig = plt.figure()
    fig.set_size_inches(12.9, 10)
    plt.rcParams['font.sans-serif'] = [fonttype]
    plt.rcParams['axes.unicode_minus'] = False
    plt.rc('xtick', labelsize=lbsize)
    plt.rc('ytick', labelsize=lbsize)
    plt.rcParams['xtick.direction'] = 'in'
    plt.rcParams['ytick.direction'] = 'in'
    # Set projection
    ax = plt.axes(projection=ccrs.PlateCarree(central_longitude=0))
    # Add ocean and land
    ax.add_feature(cfeature.LAND)
    ax.add_feature(cfeature.OCEAN)
    ax.add_feature(cfeature.COASTLINE, linewidth=0.1)

    lists = []
    found = False
    for i in sta_list:
        for j in igs:
            if j['site_name'][0:4] == i[0:4].upper():
                lists.append(j)
                found = True
                pass  # End of if

            pass  # End of for j in igs

        if not found:
            print("Cannot find " + i + " in igs stations!")
        pass  # End of for i in sta_list

    # check
    if len(lists) == 0:
        print("This is a null station list!")
        return 0

    plot_sites = pd.array(lists)

    # Add MGEX & IGS core sites
    ax.scatter(plot_sites['longitude'], plot_sites['latitude'],
               marker=marker,
               s=ms,
               alpha=1.0,
               color='tomato',
               transform=ccrs.Geodetic())
    if showname:
        for j in plot_sites:
            plt.text(j['longitude'], j['latitude'], j['site_name'][0:4], transform=ccrs.Geodetic())
            pass

        pass
    # Plot gridlines
    ax.grid()

    ax.set_global()
    ax.set_xticks([-180, -120, -60, 0, 60, 120, 180], crs=ccrs.PlateCarree())
    ax.set_xticks([-180, -120, -60, 0, 60, 120, 180], crs=ccrs.PlateCarree())
    ax.set_yticks([-90, -60, -30, 0, 30, 60, 90], crs=ccrs.PlateCarree())
    lon_formatter = LongitudeFormatter(zero_direction_label=True)
    lat_formatter = LatitudeFormatter()
    ax.xaxis.set_major_formatter(lon_formatter)
    ax.yaxis.set_major_formatter(lat_formatter)

    if saveflag:
        fig.savefig(aimpic, bbox_inches='tight', dpi=dpi)
    plt.show()
    pass


def plot_enu(sta_dic={}, settings={}):
    ess = []
    nss = []
    uss = []
    dates = []
    modes = []
    mode_ = ""
    sys = []
    kin = ""
    KINS = []
    sys_ = ""
    bds = ""
    bdss = []
    keys = sta_dic.keys()

    for k in keys:
        print(k)
        try:
            temp_types = str(k).split("_")
            if len(temp_types) < 6:  # BDS is not involved
                if temp_types[0] == "SPP":  # SPP pos file
                    sys_ = temp_types[1]
                    pass  # End of if SPP
                elif temp_types[0] == "PPP":  # PPP pos file
                    kin = temp_types[1]
                    sys_ = temp_types[3]
                    mode_ = temp_types[2]
                    pass  # End of if PPP
                elif temp_types[0] == "P":  # CodeOnly PPP pos file
                    kin = temp_types[2]
                    sys_ = temp_types[4]
                    mode_ = temp_types[3]
                    kin = 1 if temp_types[2].upper() == "KIN" else 0
                    pass  # End of if P PPP

                else:  # other pos file, like GAMP pos
                    sys_ = "NAN"
                    mode_ = "NAN"
                    kin = "NAN"
                    pass
                pass  # End of if len temp_types < 6

            elif len(temp_types) == 6:
                if temp_types[0] == "PPP":
                    kin = temp_types[1]
                    sys_ = temp_types[4]
                    mode_ = temp_types[2]
                    bds = temp_types[3]
                    pass
                elif temp_types[0] == "P":
                    kin = temp_types[2]
                    sys_ = temp_types[5]
                    mode_ = temp_types[3]
                    bds = temp_types[4]
                    pass
                else:
                    kin = "NAN"
                    sys_ = "NAN"
                    mode_ = "NAN"
                    pass

                pass  # End of len = 6
            elif len(temp_types) == 7:  # fixed ambiguity
                kin = temp_types[1]
                sys_ = temp_types[5]
                mode_ = temp_types[2] + temp_types[4] + "-FIX"
                pass  # End of len == 7
            else:  # other pos file
                sys_ = "NAN"
                mode_ = "NAN"
                kin = "NAN"
                pass
            if bds == "OFF":
                bds = ""
                pass
            if mode_ == "OFF":
                mode_ = ""
                pass
            bdss.append(bds)
            KINS.append(kin)
            sys.append(sys_)
            modes.append(mode_)
            dates = sta_dic[k][DATE_TIME]
            ess.append(sta_dic[k][E])
            nss.append(sta_dic[k][N])
            uss.append(sta_dic[k][U])
        except ...:
            raise ValueError("Cannot add data!")
            pass
        pass  # End of for k in keys

    # plot begin:

    try:
        dpi = settings.get('plotdpi')
        scatter = settings.get('plotorscatter')
        figsize = settings.get('figsize')
        ylim = settings.get("ylim")
        xInterval = settings.get("xinterval")
        lnwd = settings.get("linewidth")
        xunit = settings.get('xunit')
        ncol = settings.get('ncol')
        fonttype = settings.get("fonttype")
        ftsize = settings.get("fontsize")
        lbsize = settings.get("labelsize")
        savepath = settings.get("path")
        os.makedirs(savepath, exist_ok=True)
        saveflag = settings.get('saveplot')
        savename = settings.get('savename')

    except ...:
        raise ValueError("Cannot get the values, in plot_enu !")

    if not scatter:
        try:
            check_enu_value(sta_dic)
        except ... as e:
            print(e)
            return -1
        pass

    fig, axs = plt.subplots(
        3, 1,
        sharex=True,
        dpi=dpi,
        figsize=figsize,
        tight_layout=dict(
            h_pad=0
        )
    )

    # default setting for pic
    plt.rcParams['font.sans-serif'] = [fonttype]
    plt.rcParams['axes.unicode_minus'] = False
    miny = -ylim
    maxy = ylim
    minx = min(dates)
    maxx = max(dates)
    num = xInterval - maxx.hour % xInterval
    if maxx.hour + num == 24:
        maxx = datetime(maxx.year, maxx.month, maxx.day + 1, 0, 0, 0)
        pass
    else:
        maxx = datetime(maxx.year, maxx.month, maxx.day, maxx.hour + num, 0, 0)
        pass
    delta = 0.01
    gap = ylim   # the gap value of y axis gap
    ylabels = ['E (m)', 'N (m)', 'U (m)']
    enus = [ess, nss, uss]
    # set x,y tick_params
    for i in range(3):
        axs[i].set_ylabel(ylabels[i], fontsize=ftsize)
        axs[i].grid(color='gray', alpha=0.5, linewidth=lnwd / 2)
        axs[i].spines['bottom'].set_linewidth(lnwd)
        axs[i].spines['left'].set_linewidth(lnwd)
        axs[i].spines['right'].set_linewidth(lnwd)
        axs[i].spines['top'].set_linewidth(lnwd)
        axs[i].set_xlim(minx, maxx)
        # set the xaxis major formatter
        axs[i].xaxis.set_major_formatter(mdate.DateFormatter('%' + xunit))
        axs[i].xaxis.set_major_locator(mdate.HourLocator(interval=xInterval))
        axs[i].set_ylim(miny, maxy)
        axs[i].set_yticks(np.arange(miny, maxy + delta, gap))

        if i < 2:
            axs[i].tick_params('x', width=0)
            axs[i].tick_params('y', labelsize=lbsize, width=lnwd)
            pass
        else:
            axs[i].tick_params(labelsize=lbsize, width=lnwd)
            pass
        # plot
        j = 0
        for dxs in enus[i]:
            if not scatter:
                axs[i].plot_date(
                    dates, dxs, '-',
                    c=colors[j],
                    lw=lnwd,
                    label=modes[j] + bdss[j]
                )
                pass
            else:
                axs[i].scatter(
                    dates, dxs,
                    marker='.',
                    s=lnwd,
                    c=colors[j],
                    label=modes[j] + bdss[j]
                )
            j += 1
            pass
        pass  # End of for i in range(3)

    axs[0].set_title(str(k).split('_')[-1].split('.')[0])
    if savename == '':
        savename = str(k).split('_')[2] + str(k).split('_')[-1].split('.')[0]
    savename += '.' + settings.get('savetype')
    aimpic = savepath + SPLIT + savename
    axs[0].legend(loc='best', ncol=ncol, scatterpoints=10)
    plt.xlabel('Time [h]', fontsize=ftsize)
    plt.show()
    if saveflag:
        fig.savefig(aimpic, bbox_inches='tight', dpi=dpi)
    plt.clf()
    pass  # End of plot_enu(sta_dic)


def plot_satNumValue(values, settings, type_=''):
    color1 = colors[0]
    color2 = colors[13]
    date = values[DATE_TIME]
    n_sat = values[NSAT]
    pdop_value = values[VALUE]

    try:
        dpi = settings.get('plotdpi')
        figsize = settings.get('figsize')
        xInterval = settings.get("xinterval")
        lnwd = settings.get("linewidth")
        xunit = settings.get('xunit')
        fonttype = settings.get("fonttype")
        ftsize = settings.get("fontsize")
        lbsize = settings.get("labelsize")
        savepath = settings.get("path")
        os.makedirs(savepath, exist_ok=True)
        ylabel = settings.get('ylabel')
        plotNSat = settings.get('plotnsat')
        nsat_label = settings.get('nsatlabel')
        saveflag = settings.get('saveplot')
        savename = settings.get('savename') + '.' + settings.get('savetype')
        aimpic = savepath + SPLIT + savename
    except ...:
        raise ValueError("Cannot get the values!")

    fig, ax = plt.subplots(
        dpi=dpi,
        figsize=figsize
    )

    plt.rcParams['font.sans-serif'] = [fonttype]
    plt.rcParams['axes.unicode_minus'] = False

    if not plotNSat:
        color1 = 'black'

        pass

    minx = min(date)
    maxx = max(date)
    if maxx.hour + 1 == 24:
        maxx = datetime(maxx.year, maxx.month, maxx.day + 1, 0, 0, 0)
        pass
    else:
        num = 1
        if (maxx.hour + 1) % xInterval:
            num = xInterval - maxx.hour % xInterval
            pass
        maxx = datetime(maxx.year, maxx.month, maxx.day, maxx.hour + num, 0, 0)
        pass
    # set the xaxis major formatter
    ax.set_xlim(minx, maxx)
    ax.xaxis.set_major_formatter(mdate.DateFormatter('%H'))
    ax.xaxis.set_major_locator(mdate.HourLocator(interval=xInterval))
    p1, = ax.plot_date(date, pdop_value, '-', lw=0.8,
                       color=color1, label=type_)

    ax.tick_params(axis='y')
    ax.set_ylabel(ylabel)

    if plotNSat:
        ax.set_xlim(minx, maxx)
        ax.tick_params(axis='y', colors=p1.get_color())
        ax.spines['left'].set_color(color1)
        ax.set_ylabel(ylabel, fontdict=dict(
            color=p1.get_color(),
        ))
        # ax.yaxis.label.set_color(p1.get_color())
        ax1 = ax.twinx()
        ax.set_xlim(minx, maxx)
        ax1.set_xlim(minx, maxx)
        ax1.xaxis.set_major_formatter(mdate.DateFormatter('%' + xunit))
        ax1.xaxis.set_major_locator(mdate.HourLocator(interval=xInterval))
        p2, = ax1.plot_date(date, n_sat,
                            '-',
                            lw=lnwd,
                            color=color2,
                            label='NSAT')
        ax1.tick_params(axis='y', colors=p2.get_color())
        ax1.set_ylabel(nsat_label, fontdict=dict(
            color=p2.get_color(),
            fontsize=ftsize,
        ))
        ax1.spines['left'].set_color(p1.get_color())
        ax1.spines['right'].set_color(p2.get_color())
        ax.legend(handles=[p1, p2], labels=[ylabel, 'NSAT'],
                  loc='upper right', fontsize=lbsize, scatterpoints=10)
        pass  # End of plot num sat

    ax.set_xlabel('Time [h]')
    plt.tight_layout()
    plt.show()
    if saveflag:
        fig.savefig(aimpic, bbox_inches='tight', dpi=dpi)
    plt.close(fig)
    plt.clf()
    pass  # End of plot_satNumValue


def plot_satTypeValue(value_dic, settings):
    try:
        dpi = settings.get('plotdpi')
        figsize = settings.get('figsize')
        xInterval = settings.get("xinterval")
        lnwd = settings.get("linewidth")
        xunit = settings.get('xunit')
        ylim = settings.get('ylim')
        fonttype = settings.get("fonttype")
        ftsize = settings.get("fontsize")
        lbsize = settings.get("labelsize")
        savepath = settings.get("path")
        os.makedirs(savepath, exist_ok=True)
        ylable = settings.get('ylable')
        plotNSat = settings.get('plotnsat')
        nsat_label = settings.get('nsatlabel')
        saveflag = settings.get('saveplot')
        savename = settings.get('savename') + '.' + settings.get('savetype')
        aimpic = savepath + SPLIT + savename
    except ...:
        raise ValueError("Cannot get the values!")

        pass

    fig, ax = plt.subplots(
        figsize=figsize,
        dpi=dpi,
        layout='tight',
    )
    # set params
    # default setting for pic
    plt.rcParams['font.sans-serif'] = [fonttype]
    plt.rcParams['axes.unicode_minus'] = False
    miny = -ylim
    maxy = ylim
    delta = 0.01
    gap = ylim / 2  # the gap value of y axis gap

    # get all dates:
    dates = []
    for sat in value_dic.keys():
        for date in value_dic[sat].keys():
            dates.append(date)
            pass
        pass
    minx = min(dates)
    maxx = max(dates)
    num = xInterval - maxx.hour % xInterval
    if maxx.hour + num == 24:
        maxx = datetime(maxx.year, maxx.month, maxx.day + 1, 0, 0, 0)
        pass
    else:
        maxx = datetime(maxx.year, maxx.month, maxx.day, maxx.hour + num, 0, 0)
        pass
    # set the xaxis major formatter
    ax.set_xlim(minx, maxx)
    ax.xaxis.set_major_formatter(mdate.DateFormatter('%' + xunit))
    ax.xaxis.set_major_locator(mdate.HourLocator(interval=xInterval))
    ax.set_ylim(miny, maxy)
    ax.set_yticks(np.arange(miny, maxy + delta, gap))
    ax.set_ylabel(ylable, fontsize=ftsize)
    # plot
    for sat in value_dic.keys():
        dates = []
        values = {}
        for date in value_dic[sat].keys():
            dates.append(date)
            for type_ in value_dic[sat][date].keys():
                if type_ not in values.keys():
                    values[type_] = []
                    pass
                v = value_dic[sat][date][type_]
                values[type_].append(v)
                pass  # End of for type_
            pass  # End of for date
        for t in values.keys():
            ax.scatter(dates, values[t],
                       s=lnwd,
                       marker='.',
                       alpha=1.0)
        pass  # End of for sat
    plt.xlabel('Time [h]', fontsize=ftsize)
    plt.show()

    if saveflag:
        fig.savefig(aimpic, bbox_inches='tight', dpi=dpi)

    pass  # End of plot_satTypeValue(values, settings)


def plot_satZenithMap(value_dic, settings):
    """

    :param value_dic: aim data dictionary
    :param settings: settings params
    :return: result of checking
    """
    try:
        dpi = settings.get('plotdpi')
        figsize = settings.get('figsize')
        sat = settings.get('sat')
        lnwd = settings.get("linewidth")
        fontsize = settings.get('fontsize')
        fonttype = settings.get("fonttype")
        showid = settings.get("showid")
        savepath = settings.get("path")
        os.makedirs(savepath, exist_ok=True)
        saveflag = settings.get('saveplot')
        savename = settings.get('savename') + '.' + settings.get('savetype')
        aimpic = savepath + SPLIT + savename
    except ...:
        raise ValueError("Cannot get the values!")

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

    aim_data = {}
    for sat in value_dic.keys():
        sys_ = sat[0]
        if (sys_ not in syses) and (sat not in sats):
            continue
            pass  # End of if sys_ in sys
        if sat not in aim_data.keys():
            aim_data[sat] = {}
            pass  # End of if sat not in aim_data.keys()
        aim_data[sat] = value_dic[sat]  # add data
        pass  # for k in value_dic.keys()

    fig = plt.figure(
        figsize=figsize,
        dpi=dpi,
    )
    ax = plt.subplot(111, projection='polar')
    ax.set_theta_direction(-1)
    ax.set_theta_zero_location('N')
    ax.set_rticks([0, 15, 30, 45, 60, 75, 90])
    labels = []
    for sat in aim_data.keys():
        labels.append(sat)
        azis = []
        eles = []
        # get the current satellite date, contains elevation and azimuth
        for date in aim_data[sat].keys():
            ele = 90 - aim_data[sat][date][ELE]
            azi = np.deg2rad(aim_data[sat][date][AZI])
            eles.append(ele)
            azis.append(azi)
            pass  # End of for date in aim_data[sat].keys()

        ax.scatter(azis, eles,
                   s=lnwd,
                   marker='.',
                   alpha=0.75)
        if showid:
            ax.text(azis[0], eles[0], sat,
                    fontsize=fontsize,
                    fontfamily=fonttype)

        pass  # End of for sat in aim_data.keys()
    ax.yaxis.set_label_position('right')
    ax.tick_params('y', labelleft=False)
    if saveflag:
        fig.savefig(aimpic, bbox_inches='tight', dpi=dpi)
    plt.show()
    pass  # End of plot_satZenithMap
