# #################################################################
# Copyright (c) 2023. All rights reserved.
# Author: chao hu, ragn wang
# Contact: chaohu@cumt.edu.cn, ragwang@cumt.edu.cn
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
# CONTRIBUTORS ʺAS ISʺ AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
# OF SUCH DAMAGE.
# #################################################################

# This configuration file is convenient for counting and plotting

countENU = 0  # (0: off   1: on)
countSettings = dict(
    pos=[
    ],  # pos file, if is null, choose posdir
    posdir='',  # pos file dir
    elimit=0.1,  # limitation of e direction, unit: m
    nlimit=0.1,  # limitation of n direction
    ulimit=0.1,  # limitation of u direction
    savecsv=1,  # save result csv file, 0: off, 1: on
    savepath=''  # save path, If the csv is
    # saved, but the parameter is empty,
    # the default is the name of the folder where the pos file is located
)

SPLIT = '\\'
imagePath = '..' + SPLIT + 'pics'

# # -------- plot IGS Observation station Locations -------------# #
plotStations = 0  # (0:off  1:on)
stationSettings = dict(
    plotdpi=600,  # plot dpi
    marker='o',  # '.': 'point', ',': 'pixel', 'o': 'circle' more:
    # https://matplotlib.org/stable/api/_as_gen/matplotlib.markers.MarkerStyle.html#matplotlib.markers.MarkerStyle
    markersize=30.0,
    obsFilesDir='..\obs',  # To get the name of the obs stations
    stations=[
    ],
    showname=0,
    fonttype='Helvetica',
    fontsize=10,
    lablesize=9,
    savefig=1,
    path=imagePath + "\\site",  # save image path
    savename='site',  # pic name
    savetype='tiff',  # png/jpg/tiff
)

# # -------- plot positioning results ENU -------------# #
plotENU = 1  # (0: off 1:on)
enuSettings = dict(
    posFiles=[
        

    ],
    begin_sod=0,  # seconds of day    # If you want to draw according to the actual pos file length,
    # set begin_sod=0, end_sod=86400
    end_sod=86400,  # seconds of day
    path=imagePath + "\\enu",  # save image path
    plotorscatter=1,  # plot line, or           scatter point
    plotdpi=600,  # plot dpi
    figsize=(6, 4),  # figure size ratio
    fonttype='Helvetica',
    ylim=1,  # unit: m
    xinterval=4,  # unit: H
    xunit='H',  # H: hours, M: minutes, S: seconds, Y: year, m: month, d: day
    linewidth=0.8,  # line width in plot pic
    fontsize=10,  # font size of x,y axis and legend
    labelsize=8,  # label size of x,y axis
    ncol=3,  # numbers of legend coloum
    saveplot=1,  # 0: off, 1:on
    savename='',  # pic name
    savetype='tiff',  # png/jpg/tiff
)

# # -------- plot satellite zenith map -------------# #
# plot ele
plotEleAzi = 0  # (0: off  1: on)
elevationFile = ''
eleaziSettings = dict(
    path=imagePath,  # save image path
    plotdpi=600,  # plot dpi
    fonttype='Helvetica',
    showid=1,
    fontsize=10,  # satID font size
    sat='E',  # 0 or others, 0: all sats,
    # others: specific satellite, e.g. G01/G02 (multiple satellites); G(all sats in GPS system) G/E/C(multiple system)
    linewidth=0.8,  # line width in plot pic
    saveplot=1,  # 0: off, 1:on
    savename='satZenithMap',  # pic name
    savetype='tiff',  # png/jpg/tiff
)
# # -------- plot wet troposphere -------------# #
plotTropo = 0  # (0:off 1:on)
tropoFile = ''
tropoSettings = dict(
    path=imagePath,  # save image path
    plotdpi=600,  # plot dpi
    figsize=(4, 3),
    fonttype='Helvetica',
    xinterval=4,  # unit: xunit
    xunit='H',  # H: hours, M: minutes, S: seconds, Y: year, m: month, d: day
    linewidth=0.8,  # line width in plot pic
    fontsize=10,  # font size of x,y axis and legend
    labelsize=8,  # label size of x,y axis
    ylabel='Wet Troposphere [m]',
    plotnsat=0,  # plot numbers of satellite 0: off, 1: on
    nsatlabel='Numbers of Satellites',
    saveplot=0,  # 0: off, 1:on
    savename='tropo',  # pic name
    savetype='tiff',  # png/jpg/tiff
)
# plot pdop
plotPDOP = 0  # (0: off  1: PDOP)
PDOPFile = ''
pdopSettings = dict(
    path=imagePath,  # save image path
    plotdpi=600,  # plot dpi
    figsize=(4, 3),  # figure size ratio
    fonttype='Helvetica',
    xinterval=4,  # unit: xunit
    xunit='H',  # H: hours, M: minutes, S: seconds, Y: year, m: month, d: day
    linewidth=0.8,  # line width in plot pic
    fontsize=10,  # font size of x,y axis and legend
    labelsize=8,  # label size of x,y axis
    ylabel='PDOP',
    plotnsat=1,  # plot numbers of satellite 0: off, 1: on
    nsatlabel='Numbers of Satellites',
    saveplot=1,  # 0: off, 1:on
    savename='pdop',  # pic name
    savetype='tiff',  # png/jpg/tiff
)

# # -------- plot wet troposphere -------------# #
plotAmbiguity = 0  #
ambFile = ''
ambSettings = dict(
    path=imagePath,  # save image path
    plotdpi=600,  # plot dpi
    figsize=(4, 3),  # figure size ratio
    fonttype='Helvetica',  # font type in figure
    sat='G01',  # 0 or others, 0: all sats,
    # others: specific satellite, e.g. G01/G02 (multiple satellites); G(all sats in GPS system) G/E/C(multiple system)
    frequency=0,
    # 1：first freq 2: second freq 4: third freq    8: fourth freq  16: fifth-freq,
    # If there are multiple frequencies, select the corresponding frequency digital addition
    xinterval=4,  # unit: xunit
    xunit='H',  # H: hours, M: minutes, S: seconds, Y: year, m: month, d: day
    ylim=2,  # unit: m
    linewidth=0.8,  # line width in plot pic
    fontsize=10,  # font size of x,y axis and legend
    labelsize=8,  # label size of x,y axis
    saveplot=0,  # 0: off, 1:on
    savename='amb',  # pic name
    savetype='tiff',  # png/jpg/tiff
)

# plot the residual
plotResidual = 0  # (0: off  1: on)
residualFile = ''
resSettings = dict(
    path=imagePath,  # save image path
    plotdpi=600,  # plot dpi
    figsize=(5, 3),  # figure size ratio, length and width
    ylable='Phase Residual [m]',
    fonttype='Helvetica',
    frequency=1,  # 1:first freq 2: second freq 4: third freq    8: fourth freq  16: fifth-freq
    LP=0,  # 0: carrier phase residual, 1: pseudorange residual
    sat='E30/E36',  # 0 or others, 0: all sats,
    # others: specific satellite, e.g. G01/G02 (multiple satellites); G(all sats in GPS system) G/E/C(multiple system)
    xinterval=4,  # unit: xunit
    xunit='H',  # H: hours, M: minutes, S: seconds, Y: year, m: month, d: day
    ylim=0.05,  # unit: m
    linewidth=1.0,  # line width in plot pic
    fontsize=10,  # font size of x,y axis and legend
    labelsize=8,  # label size of x,y axis
    saveplot=0,  # 0: off, 1:on
    savename='resp',  # pic name
    savetype='tiff',  # png/jpg/tiff
)
