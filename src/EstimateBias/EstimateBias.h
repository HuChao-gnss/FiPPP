#define _CRT_SECURE_NO_DEPRECATE
#include <string.h>
#include <dirent.h>
#ifdef _MSC_VER // Check if using MSVC compiler
#include <corecrt_math.h>
#else
#include <math.h>
#endif
#include <float.h>
#include <time.h>

// Fippp

#define PI 3.141592653589793
#define OOrder 4
#define Clight 299792458
#define RR 6378137 //earth r
#define IONH 506700 //

#ifdef LINUX
#define FILEPATHSEP '/'
#else
#define FILEPATHSEP '\\'
#endif // linux

#define LAPACK_ROW_MAJOR 101
#define MAXSTRPATH 1024
#define MAX_FILES 100
#define MAX_dcbFILES 3
#define MAX_sp3FILES 3
#define MAXCHARS 2048
#define NUMSYS  6
#define navinter 5 ///gbmnav
#define MAXOBSTYPE 64
#define MAXOBS 64
#define MAXEOP 2880
#define MAXNAVEOP 288  
#define MAXPRN  60
#define MAXRNXLEN (16*MAXOBSTYPE+4)
#define MINPRNGPS 1
#define MAXPRNGPS 32
#define MINPRNSBS 120
#define MAXPRNSBS 142
#define MINPRNQZS 1
#define MAXPRNQZS 7
#define MAXPRNGLO 27
#define MINPRNGLO 1
#define MAXPRNGAL 36
#define MINPRNGAL 1
#define MAXPRNCMP 46
#define MINPRNCMP 1
#define MAXPRNIRN 0
#define MINPRNIRN 0
#define MAXPRNLEO 0
#define MINPRNLEO 0

//#define SYS_GPS 1
#define SYS_GLO 2
#define SYS_CMP 3




#define NSATGPS (MAXPRNGPS-MINPRNGPS+1)
#define NSATGLO (MAXPRNGLO-MINPRNGLO+1)
#define NSATGAL (MAXPRNGAL-MINPRNGAL+1)
#define NSATQZS (MAXPRNQZS-MINPRNQZS+1)
#define NSATCMP (MAXPRNCMP-MINPRNCMP+1)
#define NSATIRN (MAXPRNIRN-MINPRNIRN+1)
#define NSATSBS (MAXPRNSBS-MINPRNSBS+1)
#define NSATLEO (MAXPRNLEO-MINPRNLEO+1)
#define MAXSAT (NSATGPS+NSATGLO+NSATGAL+NSATQZS+NSATCMP+NSATIRN+NSATSBS+NSATLEO)


typedef struct {
	time_t time;
	double year, month, day;
	double sec;
	double h, min, s;
}gtime_t;

typedef struct {
	gtime_t time;
	int sat;
	double P[3][7][MAXEOP][MAXPRN];
	double L[3][7][MAXEOP][MAXPRN];
	double crdTrue[3];
	double BB, LL, HH;   //XYZ2BLH
	double E, A; //Get_EA
	double b, s;

}obsd_t;

typedef struct {
	double p4[3][6][MAXEOP][MAXPRN];
	double l4[3][6][MAXEOP][MAXPRN];
	int flag[3][6];
}P12_t;

typedef struct {        /* TEC grid */
	gtime_t time;       /* epoch time (GPST) */
	int ndata[3];       /* TEC grid data size {nlat,nlon,nhgt} */
	double rb;          /* earth radius (km) */
	double lats[3];     /* latitude start/interval (deg) */
	double lons[3];     /* longitude start/interval (deg) */
	double hgts[3];     /* heights start/interval (km) */
	double* data;       /* TEC grid data (tecu) */
	float* rms;         /* RMS values (tecu) */
} tec_t;

typedef struct {
	gtime_t time;
	int sat;
	int nt, ntmax;               /* number of tec grid data */
	double X[3][MAXNAVEOP][MAXPRN];
	double Y[3][MAXNAVEOP][MAXPRN];
	double Z[3][MAXNAVEOP][MAXPRN];
	double interp_x[3][MAXEOP][MAXPRN];
	double interp_y[3][MAXEOP][MAXPRN];
	double interp_z[3][MAXEOP][MAXPRN];
	tec_t* tec;
}nav_t;

typedef struct {
	int sat;
	double value[3][6][MAXPRN];
}dcb_t;

typedef struct {
	char sys;
	int num_singal; 
	char code[MAXOBSTYPE][4];
}sigind_t;

typedef struct {
	char inputpath[1024];
	char outpath[1024];
	char  f[3][7];
	int LIM;
	int ISYS;
	gtime_t time;
	char sys[3];
}prcopt_t;

typedef struct {
	double** M;
	double* L;
}Matrix_t;

typedef struct {
	int PRN[3][6][MAXPRN];
	int sat[3][6][MAXPRNCMP];
	int nr[3][6]; 
	int ns[3][6];
	double Wx[3][6];
	int count[3][6];
	int* equ[3][6];
	int maxsys_sat[3];
}lsq_t;

typedef struct {
	double DCB[3][6][MAXPRN];
	double OSB[3][7][MAXPRN];
	double* R[3][6];
	double temp[3][6][MAXPRNCMP];
	int DCBflag[3][6];
	int OSBflag[3][7];

}result_t;

#ifdef __cplusplus
extern "C" {
#endif // _cplusplus

void EstimateBias(const char* cfgfile);
#ifdef __cplusplus
}
#endif // _cplusplus
