#define _CRT_SECURE_NO_DEPRECATE
#include<stdio.h>

#ifdef GNU
#include <stdlib.h>
#include <fileapi.h>
#endif // GNU

#include "EstimateBias.h"
#include"f2c.h"
#include"clapack.h"

/// private the functions

void trimSpace(char* strsrc);
int read_data(prcopt_t* prcopt, obsd_t obs[], nav_t nav[], dcb_t* DCB);
void read_sp3(char* filepath, nav_t nav[], int i);
double str2num(const char* s, int i, int n);
gtime_t epoch2time(const double* ep);
int read_obs(prcopt_t* prcopt, char* filepath, obsd_t obs[], int f);
int str2time(const char* s, int i, int n, gtime_t* t);
void setstr(char* dst, const char* src, int n);
void interplotation(nav_t nav[]);
double* interp_lag(double* x, double* y, double* t, int n);
void read_sp3(char* filepath, nav_t nav[], int i);
void read_casdcb(prcopt_t* prcopt, char* filepath, dcb_t* DCB);
void XYZ2BLH(double X, double Y, double Z, obsd_t obs[]);
void get_EA(double xx, double yy, double zz, obsd_t obs[]);
void Get_IPP(obsd_t obs[], double IPPZ, double t_r);
int get_P4(obsd_t obs[], nav_t nav[], P12_t P12[], prcopt_t* prcopt,
	double* nav_f, lsq_t* lsq, int F1, int F2, int type);
void linspace(double start, double end, int n, double* result);
int get_Matrix(P12_t P4[], obsd_t obs[], nav_t nav[], prcopt_t* prcopt,
	lsq_t* lsq, int ith, Matrix_t* Matrix, int type, double* KK);
double* legendre(int n, double x);
void lsq_(double* BBT, double* A, double* L, double* r, int Matrix_Brows, int col);
int baohan(int j, int* d_sat, int n_r, int count);
void matmul(const char* tr, int n, int k, int m, double alpha,
	const double* A, const double* B, double beta, double* D);
int matinv(double* A, int n);
void get_equation(lsq_t* lsq, dcb_t* DCB, result_t* result, prcopt_t* prcopt, int s, int type);
void LSQ(result_t* result, P12_t* P12, obsd_t obs[],
	nav_t nav[], prcopt_t* prcopt, lsq_t* lsq, int o_num, double* KK, int type);
void get_result(result_t* result, prcopt_t* prcopt, lsq_t* lsq, double* alpha, double* beta);
void outresult(prcopt_t* prcopt, result_t result, dcb_t* DCB);

// time
long convertCalendarToJD(int yy,
	int mm,
	int dd)
{
	if (yy == 0)
		--yy;         // there is no year 0

	if (yy < 0)
		++yy;

	long jd;
	double y = (double)(yy);
	double m = (double)(mm);
	// In the conversion from the Julian Calendar to the Gregorian
	// Calendar the day after October 4, 1582 was October 15, 1582.
	//
	// if the date is before October 15, 1582
	if (yy < 1582 || (yy == 1582 && (mm < 10 || (mm == 10 && dd < 15))))
	{
		jd = 1729777 + dd + 367 * yy
			- (long)(7 * (y + 5001 +
				(long)((m - 9) / 7)) / 4)
			+ (long)(275 * m / 9);
	}
	else   // after Oct 4, 1582
	{
		jd = 1721029 + dd + 367 * yy
			- (long)(7 * (y + (long)((m + 9) / 12)) / 4)
			- (long)(3 * ((long)((y + (m - 9) / 7) / 100)
				+ 1) / 4)
			+ (long)(275 * m / 9);

		// catch century/non-400 non-leap years
		if ((!(yy % 100) &&
			(yy % 400) &&
			mm > 2 &&
			mm < 9) ||
			(!((yy - 1) % 100) &&
				((yy - 1) % 400) &&
				mm == 1))
		{
			--jd;
		}
	}
	return jd;
}

int covertCalendarToDoY(int year, int month, int day)
{
	long jd1 = convertCalendarToJD(year, month, day);
	long jd2 = convertCalendarToJD(year, 1, 1);
	return jd1 - jd2 + 1;
}

// function part
void trimSpace(char* strsrc)
{
	int i = 0, j = 0, ps, pe;
	int len = (int)strlen(strsrc);
	char str[MAXCHARS + 1];

	if (len <= 0) return;

	str[0] = '\0';
	strcpy(str, strsrc);

	ps = 0;
	for (i = 0; i < len; i++) {
		if (*(str + i) != ' ' && *(str + i) != '\t') {
			ps = i;
			break;
		}
	}

	pe = ps;
	for (j = len - 1; j >= 0; j--) {
		if (*(str + j) != ' ' && *(str + j) != '\t' && *(str + j) != '\n') {
			pe = j;
			break;
		}
	}

	if (pe == ps)
		*(str + pe) = '\0';
	else
		*(str + pe + 1) = '\0';

	strcpy(strsrc, str + ps);
}
double str2num(const char* s, int i, int n)
{
	double value;
	char str[256], * p = str;
	if (i < 0 || (int)strlen(s) < i || (int)sizeof(str) - 1 < n) return 0.0;
	for (s += i; *s && --n >= 0; s++)*p++ = *s == 'd' || *s == 'D' ? 'E' : *s; *p = '\0';
	return sscanf(str, "%lf", &value) == 1 ? value : 0.0;
}
gtime_t epoch2time(const double* ep)
{
	const int doy[] = { 1,32,60,91,121,152,182,213,244,274,305,335 };
	gtime_t time = { 0 };
	int days, sec, year = (int)ep[0], mon = (int)ep[1], day = (int)ep[2];

	if (year < 1970 || 2099 < year || mon < 1 || 12 < mon) return time;

	/* leap year if year%4==0 in 1901-2099 */
	days = (year - 1970) * 365 + (year - 1969) / 4 + doy[mon - 1] + day - 2 + (year % 4 == 0 && mon >= 3 ? 1 : 0);
	sec = (int)floor(ep[5]);
	time.time = (time_t)days * 86400 + (int)ep[3] * 3600 + (int)ep[4] * 60 + sec;
	time.sec = ep[5] - sec;
	return time;
}
int str2time(const char* s, int i, int n, gtime_t* t)
{
	double ep[6];
	char str[256], * p = str;

	if (i < 0 || (int)strlen(s) < i || (int)sizeof(str) - 1 < i) return -1;
	for (s += i; *s && --n >= 0;) *p++ = *s++; *p = '\0';
	if (sscanf(str, "%lf %lf %lf %lf %lf %lf", ep, ep + 1, ep + 2, ep + 3, ep + 4, ep + 5) < 6)
		return -1;
	if (ep[0] < 100.0) ep[0] += ep[0] < 80.0 ? 2000.0 : 1900.0;
	*t = epoch2time(ep);
	return 0;
}
void setstr(char* dst, const char* src, int n) {
	char* p = dst;
	const char* q = src;
	while (*q && q < src + n)*p++ = *q++;
	*p-- = '\0';
	while (p >= dst && *p == ' ')*p-- = '\0';
}
double* interp_lag(double* x, double* y, double* t, int n) {
	double* y_interp = (double*)malloc(sizeof(double) * n);
	for (int i = 0; i < n; i++) {
		double yi = 0.0;
		for (int j = 0; j < n; j++) {
			double Lj = 1.0;
			for (int k = 0; k < n; k++) {
				if (k != j) {
					Lj *= (t[i] - x[k]) / (x[j] - x[k]);
				}
			}
			yi += y[j] * Lj;
		}
		y_interp[i] = yi;
	}
	return y_interp;
}
void interplotation(nav_t nav[]) {
	int n;
	double m_t[297];
	double tt[10];
	//three sp3
	double new_Xmatrix[297][MAXPRN], new_Ymatrix[297][MAXPRN], new_Zmatrix[297][MAXPRN];
	for (n = 0; n < 3; n++) {
		memset(new_Xmatrix, 0, sizeof(new_Xmatrix));
		memset(new_Ymatrix, 0, sizeof(new_Ymatrix));
		memset(new_Zmatrix, 0, sizeof(new_Zmatrix));
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < MAXPRN; j++) {
				new_Xmatrix[i][j] = nav[0].X[n][i + 284][j];
				new_Ymatrix[i][j] = nav[0].Y[n][i + 284][j];
				new_Zmatrix[i][j] = nav[0].Z[n][i + 284][j];
			}
		}
		for (int i = 0; i < 288; i++) {
			for (int j = 0; j < MAXPRN; j++) {
				new_Xmatrix[i + 4][j] = nav[1].X[n][i][j];
				new_Ymatrix[i + 4][j] = nav[1].Y[n][i][j];
				new_Zmatrix[i + 4][j] = nav[1].Z[n][i][j];
			}
		}
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < MAXPRN; j++) {
				new_Xmatrix[i + 292][j] = nav[2].X[n][i][j];
				new_Ymatrix[i + 292][j] = nav[2].Y[n][i][j];
				new_Zmatrix[i + 292][j] = nav[2].Z[n][i][j];
			}
		}
		//
		for (int i = 0; i < 297; i++) {
			m_t[i] = -40.0 + 10.0 * i;
		}
		for (int i = 0; i < MAXPRN; i++) {
			for (int j = 0; j < MAXNAVEOP; j++) {
				for (int k = 0; k < 10; k++) {
					tt[k] = m_t[j + k];
				}
				double x[10], y[10], z[10];
				for (int k = 0; k < 10; k++) {
					x[k] = new_Xmatrix[j + k][i];
					y[k] = new_Ymatrix[j + k][i];
					z[k] = new_Zmatrix[j + k][i];
				}
				double t0[10];
				for (int k = 0; k < 10; k++) {
					t0[k] = m_t[j + 4] + (m_t[j + 5] - m_t[j + 4] - 1) / 9 * k;
				}
				double* interp_x = interp_lag(tt, x, t0, 10);
				double* interp_y = interp_lag(tt, y, t0, 10);
				double* interp_z = interp_lag(tt, z, t0, 10);
				for (int k = 0; k < 10; k++) {
					nav[1].interp_x[n][j * 10 + k][i] = interp_x[k];
					nav[1].interp_y[n][j * 10 + k][i] = interp_y[k];
					nav[1].interp_z[n][j * 10 + k][i] = interp_z[k];
				}
			}

		}
	}
}
void XYZ2BLH(double X, double Y, double Z, obsd_t obs[]) {
	int a = 6378137;
	double e2 = 0.0066943799013, BO, N, H, B;
	double L = atan(fabs(Y / X));
	if (Y > 0) {
		if (X < 0) {
			L = PI - L;
		}
	}
	else if (Y < 0) {
		if (X > 0) {
			L = 2 * PI - L;
		}
		else {
			L = PI + L;
		}
	}
	BO = atan(Z / sqrt(X * X + Y * Y));
	while (1) {
		N = a / sqrt(1 - e2 * sin(BO) * sin(BO));
		H = Z / sin(BO) - N * (1 - e2);
		B = atan(Z * (N + H) / (sqrt(X * X + Y * Y) * (N * (1 - e2) + H)));
		if (fabs(B - BO) < 1e-6) {
			break;
		}
		BO = B;
	}
	N = a / sqrt(1 - e2 * sin(B) * sin(B));
	obs->BB = B;
	obs->LL = L;
	obs->HH = H;
}
void get_EA(double xx, double yy, double zz, obsd_t obs[]) {
	double X, Y, Z;
	int i, j, k;
	X = obs->crdTrue[0];
	Y = obs->crdTrue[1];
	Z = obs->crdTrue[2];
	obs->E = 0; obs->A = 0;
	XYZ2BLH(X, Y, Z, obs);
	double T[3][3] = { {-sin(obs->BB) * cos(obs->LL), -sin(obs->BB) * sin(obs->LL), cos(obs->BB)},
			{-sin(obs->LL), cos(obs->LL), 0},
			{cos(obs->BB) * cos(obs->LL), cos(obs->BB) * sin(obs->LL), sin(obs->BB)} };
	double deta_xyz[3][1] = { xx - X, yy - Y, zz - Z };
	double NEU[3][1] = { 0,0,0 };
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 1; j++) {
			for (k = 0; k < 3; k++) {
				NEU[i][j] += T[i][k] * deta_xyz[k][j];
			}
		}
	}
	obs->E = atan(NEU[2][0] / sqrt(NEU[0][0] * NEU[0][0] + NEU[1][0] * NEU[1][0]));
	obs->A = atan(fabs(NEU[1][0] / NEU[0][0]));
	if (NEU[0][0] > 0) {
		if (NEU[1][0] < 0) {
			obs->A = 2 * PI - obs->A;
		}
	}
	else if (NEU[0][0] < 0) {
		if (NEU[1][0] > 0) {
			obs->A = PI - obs->A;
		}
		else if (NEU[1][0] < 0) {
			obs->A = PI + obs->A;
		}
	}

}

void Get_IPP(obsd_t obs[], double IPPZ, double t_r) {
	double t;
	t = PI / 2 - obs->E - IPPZ;
	obs->b = asin(sin(obs->BB) * cos(t) + cos(obs->BB) * sin(t) * cos(obs->A));
	obs->s = obs->LL + asin(sin(t) * sin(obs->A) / cos(obs->b));
	obs->s = obs->s + t_r - PI;
}
void multiply(double* vec, double* x, int n) {
	double scale = 1.0;
	for (int i = 0; i < n; i++) {
		scale *= vec[i];
	}
	x[0] = x[0] * scale;
}

void linspace(double start, double end, int n, double* result) {
	double step = (end - start) / (n - 1);
	for (int i = 0; i < n; i++) {
		result[i] = start + i * step;
	}
}

double* legendre(int n, double x) {
	double* y = (double*)malloc(sizeof(double) * (n + 1));
	double* rootn = (double*)malloc(sizeof(double) * (n * 2 + 1));
	double* P = (double*)malloc(sizeof(double) * (n + 3));
	double* d = (double*)malloc(sizeof(double) * (n));
	memset(P, 0, sizeof(double) * (n + 3));
	double s = 0, twocot = 0, sn = 1, tol = 0, c = 1;
	int i, nind = 0, m, k;
	if (n == 0) {
		y[n] = 1;
		return y;
	}
	for (i = 0; i < 2 * n + 1; i++) {
		rootn[i] = sqrt(i);
	}
	s = sqrt(1 - x * x);
	twocot = -2 * x / s;
	for (i = 0; i < n; i++) {
		sn *= (-s);
	}
	tol = sqrt(DBL_MIN);

	if (x != 1 && fabs(sn) >= tol)nind = 1; {
		for (i = 0; i < n; i++) {
			d[i] = 2 * (i + 1);
		}
		for (i = 0; i < n; i++) {
			c *= 1 - 1 / d[i];
		}
		P[n] = sqrt(c) * sn;
		P[n - 1] = P[n] * twocot * n / rootn[n * 2];
		for (m = n - 2; m >= 0; m--) {
			P[m] = (P[m + 1] * twocot * (m + 1) - P[m + 2] * rootn[n + m + 2] * rootn[n - m - 1]) / (rootn[n + m + 1] * rootn[n - m]);
		}
	}
	for (i = 0; i < n + 1; i++) {
		y[i] = P[i];
	}
	if (s == 0) {
		for (i = 0; i < n; i++) {
			y[0] = pow(x, n);
		}
	}
	for (m = 1; m <= n - 1; m++) {
		double* vec = (double*)malloc(sizeof(double) * (2 * m));
		for (k = 0, i = n - m + 1; k < 2 * m, i < n + m + 1; i++, k++) {
			vec[k] = rootn[i];
		}
		multiply(vec, y + m, 2 * m);
		free(vec);
	}
	double* vec = (double*)malloc(sizeof(double) * (2 * n));
	for (k = 0, i = 1; k < 2 * n, i < 2 * n + 1; k++, i++) {
		vec[k] = rootn[i];
	}
	multiply(vec, y + n, 2 * n);
	free(vec);
	free(rootn);
	free(P);
	free(d);

	return y;
}
// A*X=L
//[U,S,V]=svd(A);
//X=V*S^(-2)*VT*AT*L;


void lsq_(double* BBT, double* A, double* L, double* r, int Matrix_Brows1, int col) {
	int m = Matrix_Brows1, n = col, i;
	double* S = (double*)malloc(n * sizeof(double));
	double* SS = (double*)malloc(n * n * sizeof(double));
	memset(SS, 0, n * n * sizeof(double));
	double* SS2 = (double*)malloc(n * n * sizeof(double));
	double* VT = (double*)malloc((n * n) * sizeof(double));
	double* VTSS2 = (double*)malloc((n * n) * sizeof(double));
	double* VTSS2V = (double*)malloc((n * n) * sizeof(double));
	double* VTSS2VAT = (double*)malloc((n * m) * sizeof(double));
	int info, lwork = -1, lda = Matrix_Brows1, ldu = Matrix_Brows1, ldvt = col;
	double wkopt;
	double* work;
	char JOBU;
	char JOBVT;
	JOBU = 'N';
	JOBVT = 'A';
	dgesvd_(&JOBU, &JOBVT, &m, &n, A, &lda, S, NULL, &ldu, VT, &ldvt, &wkopt, &lwork, &info);
	lwork = (int)wkopt;
	work = (double*)malloc(lwork * sizeof(double));
	dgesvd_(&JOBU, &JOBVT, &m, &n, A, &lda, S, NULL, &ldu, VT, &ldvt, work, &lwork, &info);

	for (i = 0; i < n; i++) {
		SS[i + i * n] = S[i];
	}
	matmul("NN", col, col, col, 1.0, SS, SS, 0.0, SS2);//SS*SS
	matinv(SS2, col);
	matmul("NN", col, col, col, 1.0, VT, SS2, 0.0, VTSS2);//V*SS2
	matmul("NT", col, col, col, 1.0, VTSS2, VT, 0.0, VTSS2V);//V*SS2*VT
	matmul("NT", col, Matrix_Brows1, col, 1.0, VTSS2V, BBT, 0.0, VTSS2VAT);//V*SS2*VT*AT
	matmul("NN", col, 1, Matrix_Brows1, 1.0, VTSS2VAT, L, 0.0, r);//V*SS2*VT*AT*L

	free(SS2);
	free(SS);
	free(work);
	free(S);
	free(VT);
	free(VTSS2);
	free(VTSS2V);
	free(VTSS2VAT);
}

int baohan(int j, int* d_sat, int n_r, int count) {
	for (int i = 0; i < count; i++) {
		if (j == d_sat[i] + n_r)return 1;
	}
	return 0;
}

void matmul(const char* tr, int a, int b, int c, double alpha,
	const double* A, const double* B, double beta, double* D)
{
	double d;
	int i, j, x, f = tr[0] == 'N' ? (tr[1] == 'N' ? 1 : 2) : (tr[1] == 'N' ? 3 : 4);

	for (i = 0; i < a; i++) for (j = 0; j < b; j++) {
		d = 0.0;
		switch (f) {
		case 1: for (x = 0; x < c; x++) d += A[i * c + x] * B[x * b + j]; break;//NN}
		case 2: for (x = 0; x < c; x++) d += A[i * c + x] * B[j * c + x]; break;//NT
		case 3: for (x = 0; x < c; x++) d += A[x * a + i] * B[x * b + j]; break;//TN
		case 4: for (x = 0; x < c; x++) d += A[x * a + i] * B[j * c + x]; break;//TT
		}
		if (beta == 0.0) D[i * b + j] = alpha * d; else D[i * b + j] = alpha * d + beta * D[i * b + j];
	}
}



int matinv(double* A, int n)
{
	double* work;
	int info, lwork = n * 16, * ipiv;
	ipiv = (int*)malloc(n * (sizeof(int)));
	work = (double*)malloc(lwork * (sizeof(double)));
	dgetrf_(&n, &n, A, &n, ipiv, &info);
	if (!info) dgetri_(&n, A, &n, ipiv, work, &lwork, &info);
	free(ipiv); free(work);
	return info;
}
/// <summary>
/// 
/// </summary>
/// <param name="lsq"></param>
/// <param name="DCB"></param>
/// <param name="result"></param>
/// <param name="prcopt"></param>
/// <param name="s"></param>
/// <param name="type"></param>
void get_equation(lsq_t* lsq, dcb_t* DCB, result_t* result, prcopt_t* prcopt, int s, int type) {
	int i, j;

	for (j = 0; j < lsq->maxsys_sat[s]; j++) {
		if (lsq->PRN[prcopt->ISYS][type][j] == 0) {
			lsq->sat[prcopt->ISYS][type][lsq->count[prcopt->ISYS][type]] = j;
			lsq->count[prcopt->ISYS][type] ++;
		}
	}
	lsq->ns[prcopt->ISYS][type] = lsq->maxsys_sat[s] - lsq->count[prcopt->ISYS][type];
	lsq->equ[prcopt->ISYS][type] = (int*)malloc(((OOrder + 1) * (OOrder + 1) * 12 + lsq->ns[prcopt->ISYS][type] + lsq->nr[prcopt->ISYS][type]) * sizeof(int));
	memset(lsq->equ[prcopt->ISYS][type], 0, sizeof(int) * ((OOrder + 1) * (OOrder + 1) * 12 + lsq->ns[prcopt->ISYS][type] + lsq->nr[prcopt->ISYS][type]));
	for (i = lsq->nr[prcopt->ISYS][type]; i < lsq->ns[prcopt->ISYS][type] + lsq->nr[prcopt->ISYS][type]; i++) {
		lsq->equ[prcopt->ISYS][type][i] = 1;
	}
	if (lsq->ns[prcopt->ISYS][type] == lsq->maxsys_sat[s]) {
		lsq->Wx[prcopt->ISYS][type] = 0;
	}
	else {
		for (j = 0; j < lsq->maxsys_sat[s]; j++) {
			if (lsq->sat[prcopt->ISYS][type][j] != -1) {
				lsq->Wx[prcopt->ISYS][type] = lsq->Wx[prcopt->ISYS][type] + DCB->value[prcopt->ISYS][type][lsq->sat[prcopt->ISYS][type][j]];
			}

		}
		lsq->Wx[prcopt->ISYS][type] = -lsq->Wx[prcopt->ISYS][type];
		for (j = 0; j < lsq->count[prcopt->ISYS][type]; j++) {
			result->DCB[prcopt->ISYS][type][lsq->sat[prcopt->ISYS][type][j]] = DCB->value[prcopt->ISYS][type][lsq->sat[prcopt->ISYS][type][j]];
		}

	}
}	// End of get_equations
/// <summary>
/// 
/// </summary>
/// <param name="result"></param>
/// <param name="P12"></param>
/// <param name="obs"></param>
/// <param name="nav"></param>
/// <param name="prcopt"></param>
/// <param name="lsq"></param>
/// <param name="o_num"></param>
/// <param name="KK"></param>
/// <param name="type"></param>
void LSQ(result_t* result, P12_t* P12, obsd_t obs[],
	nav_t nav[], prcopt_t* prcopt, lsq_t* lsq, int o_num, double* KK, int type) {
	int i, j, k, m, ith = 0, Matrix_Brows = 0;
	double* l = NULL, ** B = NULL, * L = NULL, * r = NULL;
	double** BB = NULL, * BB_row = NULL, * BB_col = NULL;
	Matrix_t Matrix;
	for (i = 0; i < o_num; i++) {
		int Matrix_newBrows = 0;
		if (P12[i].flag[prcopt->ISYS][type] == 0)continue;
		else {
			Matrix_newBrows = get_Matrix(P12 + i, obs + i, nav, prcopt, lsq, ith++, &Matrix, type, KK);
			if (B == NULL) {
				B = Matrix.M;
			}
			else {
				double** new_B = (double**)malloc((Matrix_Brows + Matrix_newBrows) * sizeof(double*));
				for (j = 0; j < Matrix_Brows; j++) {
					new_B[j] = B[j];
				}
				for (j = 0; j < Matrix_newBrows; j++) {
					new_B[Matrix_Brows + j] = Matrix.M[j];
				}
				B = new_B;
			}
			//l=[l;new_l];
			if (l == NULL) {
				l = Matrix.L;
			}
			else {
				double* new_l = (double*)malloc((Matrix_Brows + Matrix_newBrows) * sizeof(double));
				for (j = 0; j < Matrix_Brows; j++) {
					new_l[j] = l[j];
				}
				for (j = 0; j < Matrix_newBrows; j++) {
					new_l[Matrix_Brows + j] = Matrix.L[j];
				}
				l = new_l;
			}
			Matrix_Brows += Matrix_newBrows;
		}
	}
	//BB(:,d_sat+n_r)=[];
	int row = Matrix_Brows, col = (OOrder + 1) * (OOrder + 1) * 12 + lsq->nr[prcopt->ISYS][type] + lsq->ns[prcopt->ISYS][type];
	BB = (double**)malloc(((row + 1)) * sizeof(double*));
	for (i = 0; i < row; i++) {
		k = 0;
		BB[i] = (double*)malloc(col * sizeof(double));
		for (j = 0, m = 0; m < lsq->count[prcopt->ISYS][type], j < (OOrder + 1) * (OOrder + 1) * 12 +
			lsq->maxsys_sat[prcopt->ISYS] + lsq->nr[prcopt->ISYS][type]; m++, j++) {
			if (!baohan(j, lsq->sat[prcopt->ISYS][type], lsq->nr[prcopt->ISYS][type], lsq->count[prcopt->ISYS][type])) {
				BB[i][k] = B[i][j];
				k++;
			}
		}
	}
	//BB=[B;c]
	BB[row] = (double*)malloc(col * sizeof(double));
	for (i = 0; i < col; i++) {
		BB[row][i] = lsq->equ[prcopt->ISYS][type][i];
	}
	//L[1;Wx]
	BB_col = (double*)malloc((Matrix_Brows + 1) * col * sizeof(double));
	if (BB_col == NULL)printf("matrix memory allocation error:Too many stations");
	BB_row = (double*)malloc(col * (Matrix_Brows + 1) * sizeof(double));
	if (BB_row == NULL)printf("matrix memory allocation error:Too many stations");
	L = (double*)malloc((Matrix_Brows + 1) * sizeof(double));
	r = (double*)malloc(col * sizeof(double));
	memset(r, 0, col * sizeof(double));
	for (i = 0; i < Matrix_Brows; i++) {
		L[i] = l[i];
	}
	L[Matrix_Brows] = lsq->Wx[prcopt->ISYS][type];

	//
	for (i = 0; i < col; i++) {
		for (j = 0; j < (Matrix_Brows + 1); j++) {
			BB_col[i * (Matrix_Brows + 1) + j] = BB[j][i];
		}
	}

	//
	for (i = 0; i < (Matrix_Brows + 1); i++) {
		for (j = 0; j < col; j++) {
			BB_row[i * col + j] = BB[i][j];
		}
	}

	lsq_(BB_row, BB_col, L, r, (Matrix_Brows + 1), col);

	free(BB_col);
	free(BB_row);
	for (int i = 0; i < Matrix_Brows; i++) {
		free(BB[i]);
	}
	free(BB);
	free(L);
	free(B);
	free(l);
	result->R[prcopt->ISYS][type] = r;

}	// End of LSQ



static  double factorial(int k) {
	double result = 1;
	for (int i = 1; i <= k; i++) {
		result *= i;
	}
	return result;
}
static double norm(int n, int m) {
	double N;
	if (m == 0) {
		N = sqrt(factorial(n - m) * (2 * n + 1) / factorial(n + m));
	}
	else {
		N = sqrt(factorial(n - m) * (4 * n + 2) / factorial(n + m));

	}
	return N;


}

static void Get_coef(obsd_t obs[], double* cof_P) {
	double COF_P[(OOrder + 1) * (OOrder + 1)];  //(order+1)*(order+1)
	memset(COF_P, 0, sizeof(double) * (OOrder + 1) * (OOrder + 1));
	double ms[OOrder], x, * p;
	int n, m, i = 0;
	linspace(obs->s, OOrder * obs->s, OOrder, ms);
	x = sin(obs->b);
	for (n = 0; n <= OOrder; n++) {
		p = legendre(n, x);
		for (m = 0; m <= n; m++) {
			if (m == 0) {
				COF_P[i] = p[m] * norm(n, m);
			}
			else {
				COF_P[i] = p[m] * norm(n, m) * cos(ms[m - 1]);
				i++;
				COF_P[i] = p[m] * norm(n, m) * sin(ms[m - 1]);
			}
			i++;
		}
	}
	for (i = 0; i < (OOrder + 1) * (OOrder + 1); i++) {
		cof_P[i] = COF_P[i];
	}

	free(p);



}

int get_Matrix(P12_t P4[], obsd_t obs[], nav_t nav[], prcopt_t* prcopt,
	lsq_t* lsq, int ith, Matrix_t* Matrix, int type, double* KK) {
	double x[MAXEOP][MAXPRN], y[MAXEOP][MAXPRN], z[MAXEOP][MAXPRN], sx, sy, sz;
	double IPPZ = 0, t_r, * cof_P, * M_col = NULL;
	int i, j, k, n, m, p = 0, q, st, ed;
	Matrix->M = NULL; Matrix->L = NULL;
	memcpy(x, nav[1].interp_x[prcopt->ISYS], sizeof(nav[1].interp_x[prcopt->ISYS]));
	memcpy(y, nav[1].interp_y[prcopt->ISYS], sizeof(nav[1].interp_y[prcopt->ISYS]));
	memcpy(z, nav[1].interp_z[prcopt->ISYS], sizeof(nav[1].interp_z[prcopt->ISYS]));
	sx = obs->crdTrue[0];
	sy = obs->crdTrue[1];
	sz = obs->crdTrue[2];
	XYZ2BLH(sx, sy, sz, obs);
	for (i = 1; i <= 12; i++) {
		for (j = 0; j < lsq->maxsys_sat[prcopt->ISYS]; j++) {
			for (k = 240 * (i - 1); k < i * 240; k++) {
				if (P4->p4[prcopt->ISYS][type][k][j] == 0) continue;
				else {
					Matrix->M = (double**)realloc(Matrix->M, (p + 1) * sizeof(double*));
					Matrix->L = (double*)realloc(Matrix->L, (p + 1) * sizeof(double));
					Matrix->M[p] = (double*)malloc(((OOrder + 1) * (OOrder + 1) * 12 + lsq->maxsys_sat[prcopt->ISYS] + lsq->nr[prcopt->ISYS][type]) * sizeof(double));
					M_col = (double*)malloc(sizeof(double) * ((OOrder + 1) * (OOrder + 1) * 12 + lsq->maxsys_sat[prcopt->ISYS] + lsq->nr[prcopt->ISYS][type]));
					cof_P = (double*)malloc(sizeof(double) * (OOrder + 1) * (OOrder + 1));
					memset(M_col, 0, sizeof(double) * ((OOrder + 1) * (OOrder + 1) * 12 + lsq->maxsys_sat[prcopt->ISYS] + lsq->nr[prcopt->ISYS][type]));
					get_EA(x[k][j] * 1000, y[k][j] * 1000, z[k][j] * 1000, obs);
					IPPZ = asin(RR * sin(0.9782 * (PI / 2 - obs->E)) / (RR + IONH));
					t_r = 30 * k * PI / 43200;
					Get_IPP(obs, IPPZ, t_r);
					M_col[ith] = KK[type] * cos(IPPZ);
					M_col[lsq->nr[prcopt->ISYS][type] + j] = KK[type] * cos(IPPZ);
					st = (OOrder + 1) * (OOrder + 1) * (i - 1) + lsq->nr[prcopt->ISYS][type] + lsq->maxsys_sat[prcopt->ISYS] + 1;
					ed = (OOrder + 1) * (OOrder + 1) * i + lsq->nr[prcopt->ISYS][type] + lsq->maxsys_sat[prcopt->ISYS];
					Get_coef(obs, cof_P);
					for (n = st - 1, m = 0; n < ed, m < (OOrder + 1) * (OOrder + 1); n++, m++) {
						M_col[n] = cof_P[m];
					}
					for (q = 0; q < (OOrder + 1) * (OOrder + 1) * 12 + lsq->maxsys_sat[prcopt->ISYS] + lsq->nr[prcopt->ISYS][type]; q++) {
						Matrix->M[p][q] = M_col[q];
					}
					Matrix->L[p] = P4->p4[prcopt->ISYS][type][k][j] * (KK[type]) * cos(IPPZ);
					p++;
					free(cof_P);
					free(M_col);

				}
			}
		}
	}
	return p;
}


static void prepro(obsd_t obs[], prcopt_t* prcopt, P12_t P12[], double* nav_f, int F1, int F2, int type) {
	double lamda_w, L6[MAXEOP][MAXPRN], Li[MAXEOP][MAXPRN], NW[MAXEOP][MAXPRN], f1, f2;
	double fir, sec, thi, fir1, sec1, thi1, sub, sub2, sub1, sub12, T, I1, I2, sum, ave;
	int i, j, k, e, n_epoch, count, l, t, n;

	f1 = nav_f[F1]; f2 = nav_f[F2];
	lamda_w = Clight / (f1 - f2);
	for (i = 0; i < MAXEOP; i++) {
		for (j = 0; j < MAXPRN; j++) {
			L6[i][j] = lamda_w * (obs->L[prcopt->ISYS][F1][i][j] - obs->L[prcopt->ISYS][F2][i][j])
				- (f1 * obs->P[prcopt->ISYS][F1][i][j] + f2 * obs->P[prcopt->ISYS][F2][i][j]) / (f1 + f2);
			Li[i][j] = obs->L[prcopt->ISYS][F1][i][j] - f1 * obs->L[prcopt->ISYS][F2][i][j] / f2;
			NW[i][j] = L6[i][j] / lamda_w;
		}
	}
	for (j = 0; j < MAXPRN; j++) {
		
		int arc[MAXEOP] = { 0 };
		int arc_len = 0, i;
		for (i = 0; i < MAXEOP; i++) {
			if (i == MAXEOP - 1) {
				if (L6[i][j] != 0) {
					arc[arc_len] = i;
					arc_len++;
				}
				continue;
			}
			if (i == 0 && L6[i][j] != 0) {
				arc[arc_len] = i;
				arc_len++;
			}
			if (L6[i][j] == 0 && L6[i + 1][j] != 0) {
				arc[arc_len] = i + 1;
				arc_len++;
				continue;
			}
			if (L6[i][j] != 0 && L6[i + 1][j] == 0) {
				arc[arc_len] = i;
				arc_len++;
			}

		}
		for (i = 0; i < arc_len / 2; i++) {
			n_epoch = arc[i * 2 + 1] - arc[i * 2];
			if (n_epoch < 10) {
				for (k = arc[i * 2]; k <= arc[i * 2 + 1]; k++) {
					obs->P[prcopt->ISYS][F1][k][j] = 0; obs->P[prcopt->ISYS][F2][k][j] = 0;
					obs->L[prcopt->ISYS][F1][k][j] = 0; obs->L[prcopt->ISYS][F2][k][j] = 0;
					L6[k][j] = 0; NW[k][j] = 0; Li[k][j] = 0;
				}
				arc[i * 2 + 1] = -1; arc[i * 2] = -1; //
			}
		}
		//MW detect cycle slip
		for (i = 0; i < arc_len / 2; i++) {
			e = arc[i * 2];
			if (arc[i * 2] < 0) continue;  //
			while (1) {
				if (e + 1 == arc[i * 2 + 1] || e == arc[i * 2 + 1])break;
				else {
					fir = NW[e][j]; sec = NW[e + 1][j]; thi = NW[e + 2][j];
					fir1 = Li[e][j]; sec1 = Li[e + 1][j]; thi1 = Li[e + 2][j];
					sub = fabs(fir - sec); sub2 = fabs(sec - thi);
					sub1 = fabs(fir1 - sec1); sub12 = fabs(sec1 - thi1);
					if (sub > 1 || sub2 > 1 || sub1 > 1 || sub12 > 1) {
						L6[e][j] = 0; obs->L[prcopt->ISYS][F1][e][j] = 0; obs->L[prcopt->ISYS][F2][e][j] = 0;
						obs->P[prcopt->ISYS][F1][e][j] = 0; obs->P[prcopt->ISYS][F2][e][j] = 0;
						NW[e][j] = 0; Li[e][j] = 0;
						e = e + 1;
						arc[i * 2] = e;
					}
					else {
						arc[i * 2] = e;
						break;
					}
				}
			}
			//MW detect//
			if (arc[i * 2 + 1] - arc[i * 2] < 10) {
				for (k = arc[i * 2]; k <= arc[i * 2 + 1]; k++) {
					obs->P[prcopt->ISYS][F1][k][j] = 0; obs->P[prcopt->ISYS][F2][k][j] = 0;
					obs->L[prcopt->ISYS][F1][k][j] = 0; obs->L[prcopt->ISYS][F2][k][j] = 0;
					L6[k][j] = 0; NW[k][j] = 0; Li[k][j] = 0;
				}
				arc[i * 2 + 1] = -1; arc[i * 2] = -1;
			}
			if (arc[i * 2] < 0) continue;
			double* ave_N = (double*)malloc((arc[i * 2 + 1] - arc[i * 2] + 1) * sizeof(double));
			double* sigma2 = (double*)malloc((arc[i * 2 + 1] - arc[i * 2] + 1) * sizeof(double));
			double* sigma = (double*)malloc((arc[i * 2 + 1] - arc[i * 2] + 1) * sizeof(double));
			memset(ave_N, 0, (arc[i * 2 + 1] - arc[i * 2] + 1) * sizeof(double));
			memset(sigma2, 0, (arc[i * 2 + 1] - arc[i * 2] + 1) * sizeof(double));
			memset(sigma, 0, (arc[i * 2 + 1] - arc[i * 2] + 1) * sizeof(double));
			ave_N[0] = NW[arc[i * 2]][j];
			sigma2[0] = 0;
			sigma[0] = 0;
			count = 2;
			for (k = arc[i * 2] + 1; k <= arc[i * 2 + 1] - 1; k++) {
				ave_N[count - 1] = ave_N[count - 2] + (NW[k][j] - ave_N[count - 2]) / count;
				sigma2[count - 1] = sigma2[count - 2] + ((NW[k][j] - ave_N[count - 2]) * (NW[k][j] - ave_N[count - 2]) - sigma2[count - 2]) / count;
				sigma[count - 1] = sqrt(sigma2[count - 1]);
				T = fabs(NW[k + 1][j] - ave_N[count - 1]);
				I1 = fabs(Li[k + 1][j] - Li[k][j]);
				if (T < 4 * sigma[count - 1] && I1 < 0.28) {
					count = count + 1;
					continue;
				}
				else if (k + 1 == arc[i * 2 + 1]) {
					if (k + 1 - arc[i * 2] > 10) {
						L6[k + 1][j] = 0;
						obs->P[prcopt->ISYS][F1][k + 1][j] = 0; obs->P[prcopt->ISYS][F2][k + 1][j] = 0;
						obs->L[prcopt->ISYS][F1][k + 1][j] = 0; obs->L[prcopt->ISYS][F2][k + 1][j] = 0;
						NW[k + 1][j] = 0; Li[k][j] = 0; arc[i * 2 + 1] = k;
					}
					else {
						for (l = arc[i * 2]; l <= k + 1; l++) {
							L6[l][j] = 0;
							obs->P[prcopt->ISYS][F1][l][j] = 0; obs->P[prcopt->ISYS][F2][l][j] = 0;
							obs->L[prcopt->ISYS][F1][l][j] = 0; obs->L[prcopt->ISYS][F2][l][j] = 0;
							NW[l][j] = 0; Li[k][j] = 0;
						}
						arc[i * 2 + 1] = -1; arc[i * 2] = -1;
					}
					break;
				}
				I2 = fabs(Li[k + 2][j] - Li[k + 1][j]);
				if (fabs(NW[k + 2][j] - NW[k + 1][j]) < 1 && I2 < 1) {
					if (k + 1 - arc[i * 2] > 10) {

						for (l = arc_len + 1; l - 2 >= i * 2 + 1; l--) {
							arc[l] = arc[l - 2];
						}
						arc[i * 2 + 1] = k;
						arc[(i + 1) * 2] = k + 1;
						arc_len = arc_len + 2;
					}
					else {
						for (l = arc[i * 2]; l <= k; l++) {
							L6[l][j] = 0;
							obs->P[prcopt->ISYS][F1][l][j] = 0; obs->P[prcopt->ISYS][F2][l][j] = 0;
							obs->L[prcopt->ISYS][F1][l][j] = 0; obs->L[prcopt->ISYS][F2][l][j] = 0;
							NW[l][j] = 0; Li[k][j] = 0;
						}
						arc[i * 2] = k + 1;
						i = i - 1;
					}
				}
				else {
					if (k + 1 - arc[i * 2] > 10) {
						L6[k + 1][j] = 0;
						obs->P[prcopt->ISYS][F1][k + 1][j] = 0; obs->P[prcopt->ISYS][F2][k + 1][j] = 0;
						obs->L[prcopt->ISYS][F1][k + 1][j] = 0; obs->L[prcopt->ISYS][F2][k + 1][j] = 0;
						NW[k + 1][j] = 0; Li[k][j] = 0;

						for (l = arc_len + 1; l - 2 >= i * 2 + 1; l--) {
							arc[l] = arc[l - 2];
						}
						arc[i * 2 + 1] = k;
						arc[(i + 1) * 2] = k + 2;
						arc_len = arc_len + 2;
					}
					else {
						for (l = arc[i * 2]; l <= k + 1; l++) {
							L6[l][j] = 0;
							obs->P[prcopt->ISYS][F1][l][j] = 0; obs->P[prcopt->ISYS][F2][l][j] = 0;
							obs->L[prcopt->ISYS][F1][l][j] = 0; obs->L[prcopt->ISYS][F2][l][j] = 0;
							NW[l][j] = 0; Li[k][j] = 0;
						}
						arc[i * 2] = k + 2;
						i = i - 1;
					}
				}
				break;
			}

			free(ave_N);
			free(sigma2);
			free(sigma);
		}

		for (k = 0; k < MAXEOP; k++) {
			P12->p4[prcopt->ISYS][type][k][j] = obs->P[prcopt->ISYS][F1][k][j] - obs->P[prcopt->ISYS][F2][k][j];
			P12->l4[prcopt->ISYS][type][k][j] = (Clight / f1) * obs->L[prcopt->ISYS][F1][k][j] - (Clight / f2) * obs->L[prcopt->ISYS][F2][k][j];
		}
		//smoothing//
		for (i = 0; i < arc_len / 2; i++) {
			if (arc[i * 2] < 0) continue;
			t = 2;
			for (k = arc[i * 2] + 1; k <= arc[i * 2 + 1]; k++) {
				P12->p4[prcopt->ISYS][type][k][j] = P12->p4[prcopt->ISYS][type][k][j] / t + (P12->p4[prcopt->ISYS][type][k - 1][j] + P12->l4[prcopt->ISYS][type][k - 1][j] - P12->l4[prcopt->ISYS][type][k][j]) * (t - 1) / t;
				t = t + 1;
			}
			for (k = arc[i * 2]; k <= arc[i * 2] + 4; k++) {
				P12->p4[prcopt->ISYS][type][k][j] = 0;
			}
		}
		//remove bad P4
		arc_len = 0;
		memset(arc, 0, sizeof(arc));
		for (i = 0; i < MAXEOP; i++) {
			if (i == MAXEOP - 1) {
				if (P12->p4[prcopt->ISYS][type][i][j] != 0) {
					arc[arc_len] = i;
					arc_len++;
				}
				continue;
			}
			else if (i == 0 && P12->p4[prcopt->ISYS][type][i][j] != 0) {
				arc[arc_len] = i;
				arc_len++;
			}
			else if (P12->p4[prcopt->ISYS][type][i][j] == 0 && P12->p4[prcopt->ISYS][type][i + 1][j] != 0) {

				arc[arc_len] = i + 1;
				arc_len++;
				continue;
			}
			else if (P12->p4[prcopt->ISYS][type][i][j] != 0 && P12->p4[prcopt->ISYS][type][i + 1][j] == 0) {

				arc[arc_len] = i;
				arc_len++;
			}

		}
		for (k = 0; k < arc_len / 2; k++) {
			sum = 0;
			for (n = arc[k * 2]; n <= arc[k * 2 + 1]; n++) {
				sum += P12->p4[prcopt->ISYS][type][n][j];
			}
			ave = sum / (arc[k * 2 + 1] - arc[k * 2] + 1);
			if (fabs(ave) > 30) {
				for (n = arc[k * 2]; n <= arc[k * 2 + 1]; n++) {
					P12->p4[prcopt->ISYS][type][n][j] = 0;
				}
			}

		}


	}

}




int get_P4(obsd_t obs[], nav_t nav[], P12_t P12[], prcopt_t* prcopt, double* nav_f, lsq_t* lsq, int F1, int F2, int type) {
	double lim = prcopt->LIM * PI / 180;
	int i, j, k, state = 0;
	double x[MAXEOP][MAXPRN], y[MAXEOP][MAXPRN], z[MAXEOP][MAXPRN];
	memcpy(x, nav[1].interp_x[prcopt->ISYS], sizeof(nav[1].interp_x[prcopt->ISYS]));
	memcpy(y, nav[1].interp_y[prcopt->ISYS], sizeof(nav[1].interp_y[prcopt->ISYS]));
	memcpy(z, nav[1].interp_z[prcopt->ISYS], sizeof(nav[1].interp_z[prcopt->ISYS]));
	//
	for (i = 0; i < MAXPRN; i++) {
		for (j = 0; j < MAXEOP; j++) {
			if (obs->P[prcopt->ISYS][F2][j][i] == 0 || obs->P[prcopt->ISYS][F1][j][i] == 0 ||
				obs->L[prcopt->ISYS][F2][j][i] == 0 || obs->L[prcopt->ISYS][F1][j][i] == 0)
			{
				obs->P[prcopt->ISYS][F2][j][i] = 0; obs->P[prcopt->ISYS][F1][j][i] = 0;
				obs->L[prcopt->ISYS][F2][j][i] = 0; obs->L[prcopt->ISYS][F1][j][i] = 0;
			}
			get_EA(x[j][i] * 1000, y[j][i] * 1000, z[j][i] * 1000, obs);
			if (fabs(obs->E) < lim) {
				obs->P[prcopt->ISYS][F2][j][i] = 0; obs->P[prcopt->ISYS][F1][j][i] = 0;
				obs->L[prcopt->ISYS][F2][j][i] = 0; obs->L[prcopt->ISYS][F1][j][i] = 0;
				continue;
			}
		}
	}
	//
	for (i = 0; i < MAXPRN; i++) {
		for (j = 0; j < MAXEOP; j++) {
			if (obs->P[prcopt->ISYS][F2][j][i] != 0 && obs->P[prcopt->ISYS][F1][j][i] != 0 &&
				obs->L[prcopt->ISYS][F2][j][i] != 0 && obs->L[prcopt->ISYS][F1][j][i] != 0) {
				P12->flag[prcopt->ISYS][type] = 1;
				break;
			}
		}

	}

	if (P12->flag[prcopt->ISYS][type] == 1) {
		prepro(obs, prcopt, P12, nav_f, F1, F2, type);
		for (j = 0; j < MAXPRN; j++) {
			for (k = 0; k < MAXEOP; k++) {
				if (P12->p4[prcopt->ISYS][type][k][j] != 0) {
					lsq->PRN[prcopt->ISYS][type][j] = lsq->PRN[prcopt->ISYS][type][j] + 1;
					state++;
					break;
				}
			}
		}
		if (state != 0)lsq->nr[prcopt->ISYS][type]++;
		return 1;
	}
	else return 0;
}


void get_result(result_t* result, prcopt_t* prcopt, lsq_t* lsq, double* alpha, double* beta) {
	int i, j, k, n;
	for (k = 0; k < 3; k++) {
		for (i = 0; i < 6; i++) {
			linspace(0, 45, 46, result->temp[k][i]);
		}
	}
	for (k = 0; k < 3; k++) {
		for (i = 0; i < 6; i++) {
			for (j = 0; j < lsq->count[k][i]; j++) {
				result->temp[k][i][lsq->sat[k][i][j]] = -1;
			}
		}
	}
	for (n = 0; n < 3; n++) {
		switch (prcopt->sys[n]) {
		case 'C':
			for (i = 0; i < 6; i++) {
				if (result->DCBflag[0][i] == 0)continue;
				for (j = 0, k = 0; j < MAXPRNCMP; j++) {
					if (result->temp[0][i][j] != -1) {
						result->DCB[0][i][j] = result->R[0][i][lsq->nr[0][i] + k] * 1e9 / Clight;
						k++;
					}
				}
			}
			for (i = 0; i < 7; i++) {
				if (result->OSBflag[0][i] == 0)continue;
				if (i == 1) {
					for (j = 0; j < MAXPRNCMP; j++) {
						result->OSB[0][i][j] = -alpha[0] * result->DCB[0][0][j]; //C6I
					}
				}
				if (i == 0) {
					for (j = 0; j < MAXPRNCMP; j++) {
						result->OSB[0][i][j] = beta[0] * result->DCB[0][0][j]; //C2I
					}
				}
				if (i == 2) {
					for (j = 0; j < 19; j++) {
						result->OSB[0][i][j] = -alpha[0] * result->DCB[0][0][j] - result->DCB[0][i - 1][j];//C7I 
					}
				}
				if (i > 2) {
					for (j = 18; j < MAXPRNCMP; j++) {
						result->OSB[0][i][j] = -alpha[0] * result->DCB[0][0][j] - result->DCB[0][i - 1][j];
					}
				}
			}break;

		case 'G':
			for (i = 0; i < 4; i++) {
				if (result->DCBflag[1][i] == 0)continue;
				for (j = 0, k = 0; j < MAXPRNGPS; j++) {
					if (result->temp[1][i][j] != -1) {
						result->DCB[1][i][j] = result->R[1][i][lsq->nr[1][i] + k] * 1e9 / Clight;
						k++;
					}
				}
			}
			for (i = 0; i < 5; i++) {
				if (result->OSBflag[1][i] == 0)continue;
				if (i == 2) {
					for (j = 0; j < MAXPRNGPS; j++) {
						result->OSB[1][i][j] = -alpha[1] * result->DCB[1][0][j]; //C2W
					}
				}
				if (i == 1) {
					for (j = 0; j < MAXPRNGPS; j++) {
						result->OSB[1][i][j] = beta[1] * result->DCB[1][1][j]; //C1W
					}
				}
				if (i == 0) {
					for (j = 0; j < MAXPRNGPS; j++) {
						result->OSB[1][i][j] = -alpha[1] * result->DCB[1][1][j] + result->DCB[1][0][j];//C1C
					}
				}
				if (i == 3) {
					for (j = 0; j < MAXPRNGPS; j++) {
						result->OSB[1][i][j] = -alpha[1] * result->DCB[1][1][j] + result->DCB[1][2][j];//C5Q
					}
				}
				if (i == 4) {
					for (j = 0; j < MAXPRNGPS; j++) {
						result->OSB[1][i][j] = -alpha[1] * result->DCB[1][1][j] + result->DCB[1][3][j];//C5X
					}
				}
			}break;
		case 'E':
			for (i = 0; i < 4; i++) {
				if (result->DCBflag[2][i] == 0)continue;
				for (j = 0, k = 0; j < MAXPRNGAL; j++) {
					if (result->temp[2][i][j] != -1) {
						result->DCB[2][i][j] = result->R[2][i][lsq->nr[2][i] + k] * 1e9 / Clight;
						k++;
					}
				}
			}
			for (i = 0; i < 5; i++) {
				if (result->OSBflag[2][i] == 0)continue;
				if (i == 1) {
					for (j = 0; j < MAXPRNGAL; j++) {
						result->OSB[2][i][j] = -alpha[2] * result->DCB[2][0][j]; //C1X
					}
				}
				if (i == 0) {
					for (j = 0; j < MAXPRNGAL; j++) {
						result->OSB[2][i][j] = beta[2] * result->DCB[2][0][j]; //C5X
					}
				}
				if (i == 2) {
					for (j = 0; j < MAXPRNGAL; j++) {
						result->OSB[2][i][j] = beta[2] * result->DCB[2][0][j] - result->DCB[2][2][j];//C7X
					}
				}
				if (i == 3) {
					for (j = 0; j < MAXPRNGAL; j++) {
						result->OSB[2][i][j] = beta[2] * result->DCB[2][0][j] - result->DCB[2][3][j];//C8X
					}
				}
				if (i == 4) {
					for (j = 0; j < MAXPRNGAL; j++) {
						result->OSB[2][i][j] = beta[2] * result->DCB[2][0][j] - result->DCB[2][4][j];//C6C
					}
				}
			}break;
		}
	}
}

//////////////////// out result

static int outDCB(char* buff, result_t result, dcb_t* DCB, int i, int j, int s) {
	char* p = buff;
	char P_code[3][7][4] = {
								  {"C2I","C6I","C7I","C1X","C5X","C7Z","C8X"},//BDS
								  {"C1C","C1W","C2W","C5Q","C5X",  "" , ""  },//GPS
								  {"C1X","C5X","C7X","C8X","C6C",  "" , ""  }//GAL
	};

	if (s == 0) {
		if (j == 0) {
			p += sprintf(p, " DCB        C%02d      %s        %s        %.4f          %.4f", i + 1, P_code[0][j], P_code[0][1], result.DCB[0][j][i], result.DCB[0][j][i] - DCB->value[0][j][i]);
		}
		else p += sprintf(p, " DCB        C%02d      %s        %s        %.4f          %.4f", i + 1, P_code[0][1], P_code[0][j + 1], result.DCB[0][j][i], result.DCB[0][j][i] - DCB->value[0][j][i]);
	}
	if (s == 1) {
		if (j >= 2)p += sprintf(p, " DCB        G%02d      %s        %s        %.4f          %.4f", i + 1, P_code[1][j + 1], P_code[1][2], result.DCB[1][j][i], result.DCB[1][j][i] - DCB->value[1][j][i]);
		else p += sprintf(p, " DCB        G%02d      %s        %s        %.4f          %.4f", i + 1, P_code[1][j], P_code[1][2], result.DCB[1][j][i], result.DCB[1][j][i] - DCB->value[1][j][i]);
	}
	if (s == 2) {
		p += sprintf(p, " DCB        E%02d      %s        %s        %.4f          %.4f", i + 1, P_code[2][0], P_code[2][j + 1], result.DCB[2][j][i], result.DCB[2][j][i] - DCB->value[2][j][i]);
	}
	p += sprintf(p, "\n");
	return  (int)(p - buff);
}

static int outOSB(char* buff, result_t result, int i, int j, int s) {
	char* p = buff;
	char P_code[3][7][4] = {
								  {"C2I","C6I","C7I","C1X","C5X","C7Z","C8X"},//BDS
								  {"C1C","C1W","C2W","C5Q","C5X",  "" , ""  },//GPS
								  {"C1X","C5X","C7X","C8X","C6C",  "" , ""  }//GAL
	};
	if (s == 0) {
		p += sprintf(p, " OSB        C%02d      %s         %.4f ", i + 1, P_code[0][j], result.OSB[0][j][i]);
	}
	if (s == 1) {
		p += sprintf(p, " OSB        G%02d      %s         %.4f ", i + 1, P_code[1][j], result.OSB[1][j][i]);
	}
	if (s == 2) {
		p += sprintf(p, " OSB        E%02d      %s         %.4f ", i + 1, P_code[2][j], result.OSB[2][j][i]);
	}
	p += sprintf(p, "\n");
	return (int)(p - buff);
}

void outresult(prcopt_t* prcopt, result_t result, dcb_t* DCB) {
	FILE* fp1, * fp2;
	int i, j, n, m, doy;
	char temp = (char)FILEPATHSEP;
	char  DCBfile[1024] = { '\0' }, OSBfile[1024] = { '\0' };
	CreateDirectory(prcopt->outpath, NULL);
	doy = covertCalendarToDoY(prcopt->time.year, prcopt->time.month, prcopt->time.day);
	sprintf(DCBfile, "%s%cFiPPP_%d0%d_%s.DCB\0", prcopt->outpath, temp,
		(int)prcopt->time.year, doy, prcopt->sys);
	sprintf(OSBfile, "%s%cFiPPP_%d0%d_%s.OSB\0", prcopt->outpath, temp, 
		(int)prcopt->time.year, doy, prcopt->sys);

	// OUTPUT DCBfile
	if (!(fp1 = fopen(DCBfile, "w"))) {
		printf("error:open output file %s", DCBfile);
	}
	//
	char buff1[1024] = { "*BIAS       PRN      OBS1      OBS2        VALUE_(ns)       res(ns)\n" };
	fwrite(buff1, strlen(buff1), 1, fp1);
	for (m = 0; m < 3; m++) {
		switch (prcopt->sys[m]) {
		case 'C':
			for (i = 0; i < 46; i++) {     //BDS
				for (j = 0; j < 6; j++) {
					if (result.DCB[0][j][i] == 0)
						continue;
					n = outDCB(buff1, result, DCB, i, j, 0);
					fwrite(buff1, n, 1, fp1);

				}
			}
			break;
		case 'G':
			for (i = 0; i < 32; i++) {
				for (j = 0; j < 4; j++) {
					if (result.DCB[1][j][i] == 0)
						continue;
					n = outDCB(buff1, result, DCB, i, j, 1);
					fwrite(buff1, n, 1, fp1);
				}
			}
			break;
		case'E':
			for (i = 0; i < 36; i++) {
				for (j = 0; j < 4; j++) {
					if (result.DCB[2][j][i] == 0)
						continue;
					n = outDCB(buff1, result, DCB, i, j, 2);
					fwrite(buff1, n, 1, fp1);
				}
			}
			break;
		}
	}
	fclose(fp1);

	// output osbfile
	if (!(fp2 = fopen(OSBfile, "w"))) {
		printf("error:open output file %s", OSBfile);
	}
	char buff2[1024] = { "*BIAS       PRN      OBS1       VALUE_(ns)\n" };
	fwrite(buff2, strlen(buff2), 1, fp2);
	for (m = 0; m < 3; m++) {
		switch (prcopt->sys[m]) 
		{
		case 'C':
			for (i = 0; i < 46; i++) {     //BDS
				for (j = 0; j < 7; j++) {
					if (result.OSB[0][j][i] == 0)
						continue;
					n = outOSB(buff2, result, i, j, 0);
					fwrite(buff2, n, 1, fp2);
				}
			}
			break;	// End of BDS
		case'G':
			for (i = 0; i < 32; i++) {
				for (j = 0; j < 5; j++) {
					if (result.OSB[1][j][i] == 0)
						continue;
					n = outOSB(buff2, result, i, j, 1);
					fwrite(buff2, n, 1, fp2);
				}
			}
			break;	// End of GPS
		case'E':
			for (i = 0; i < 36; i++) {
				for (j = 0; j < 5; j++) {
					if (result.OSB[2][j][i] == 0)
						continue;
					n = outOSB(buff2, result, i, j, 2);
					fwrite(buff2, n, 1, fp2);
				}
			}
			break;	// End of GAL
		}
	}
	fclose(fp2);
}	// End of outResult

////// read data
void read_casdcb(prcopt_t* prcopt, char* filepath, dcb_t* DCB) {
	int i;
	FILE* fp = NULL;
	char dcb[MAXCHARS], sit[MAXCHARS];
	char oneline[MAXCHARS];
	double C2I_C6I[46] = { 0 }, C2I_C7I[46] = { 0 }, C1X_C6I[46] = { 0 }, C1X_C7Z[46] = { 0 }, C1X_C8X[46] = { 0 }, C1X_C5X[46] = { 0 };
	double C1C_C2W[32] = { 0 }, C1W_C2W[32] = { 0 }, C1C_C5Q[32] = { 0 }, C1C_C5X[32] = { 0 };
	double E_C1X_C5X[36] = { 0 }, C1X_C7X[36] = { 0 }, E_C1X_C8X[36] = { 0 };
	memset(DCB->value, 0, sizeof(DCB->value));
	if ((fp = fopen(filepath, "r")) == NULL) {
		perror("fopen");
	}
	while (fgets(oneline, MAXCHARS, fp)) {
		if (strlen(oneline) > 85 && strstr(oneline, "DSB") && (oneline[11] == 'C'))
		{
			setstr(dcb, oneline + 25, 8);
			setstr(sit, oneline + 15, 4);
			DCB->sat = (int)str2num(oneline, 12, 2) - 1;
			if (DCB->sat > 45)continue;
			if (!strncmp(dcb, "C2I  C6I", 8) && strlen(sit) == 0) C2I_C6I[DCB->sat] = str2num(oneline, 82, 13);
			if (!strncmp(dcb, "C2I  C7I", 8) && strlen(sit) == 0) C2I_C7I[DCB->sat] = str2num(oneline, 82, 13);
			if (!strncmp(dcb, "C1X  C6I", 8) && strlen(sit) == 0) C1X_C6I[DCB->sat] = str2num(oneline, 82, 13);
			if (!strncmp(dcb, "C1X  C7Z", 8) && strlen(sit) == 0) C1X_C7Z[DCB->sat] = str2num(oneline, 82, 13);
			if (!strncmp(dcb, "C1X  C8X", 8) && strlen(sit) == 0) C1X_C8X[DCB->sat] = str2num(oneline, 82, 13);
			if (!strncmp(dcb, "C1X  C5X", 8) && strlen(sit) == 0) C1X_C5X[DCB->sat] = str2num(oneline, 82, 13);
		}
		if (strlen(oneline) > 85 && strstr(oneline, "DSB") && (oneline[11] == 'G')) {
			setstr(dcb, oneline + 25, 8);
			setstr(sit, oneline + 15, 4);
			DCB->sat = (int)str2num(oneline, 12, 2) - 1;
			if (!strncmp(dcb, "C1C  C2W", 8) && strlen(sit) == 0) C1C_C2W[DCB->sat] = str2num(oneline, 82, 13);
			if (!strncmp(dcb, "C1W  C2W", 8) && strlen(sit) == 0) C1W_C2W[DCB->sat] = str2num(oneline, 82, 13);
			if (!strncmp(dcb, "C1C  C5Q", 8) && strlen(sit) == 0) C1C_C5Q[DCB->sat] = str2num(oneline, 82, 13);
			if (!strncmp(dcb, "C1C  C5X", 8) && strlen(sit) == 0) C1C_C5X[DCB->sat] = str2num(oneline, 82, 13);
		}
		if (strlen(oneline) > 85 && strstr(oneline, "DSB") && (oneline[11] == 'E')) {
			setstr(dcb, oneline + 25, 8);
			setstr(sit, oneline + 15, 4);
			DCB->sat = (int)str2num(oneline, 12, 2) - 1;
			if (!strncmp(dcb, "C1X  C5X", 8) && strlen(sit) == 0) E_C1X_C5X[DCB->sat] = str2num(oneline, 82, 13);
			if (!strncmp(dcb, "C1X  C7X", 8) && strlen(sit) == 0) C1X_C7X[DCB->sat] = str2num(oneline, 82, 13);
			if (!strncmp(dcb, "C1X  C8X", 8) && strlen(sit) == 0) E_C1X_C8X[DCB->sat] = str2num(oneline, 82, 13);
		}
	}

	for (i = 0; i < 46; i++) {
		if (C2I_C6I[i] != 0)DCB->value[0][0][i] = C2I_C6I[i];
		if (C2I_C7I[i] != 0 && C2I_C6I[i] != 0)DCB->value[0][1][i] = C2I_C7I[i] - C2I_C6I[i];
		if (C1X_C6I != 0)DCB->value[0][2][i] = -C1X_C6I[i];
		if (C1X_C6I[i] != 0 && C1X_C5X[i] != 0)DCB->value[0][3][i] = -C1X_C6I[i] + C1X_C5X[i];
		if (C1X_C6I[i] != 0 && C1X_C7Z[i] != 0)DCB->value[0][4][i] = -C1X_C6I[i] + C1X_C7Z[i];
		if (C1X_C6I[i] != 0 && C1X_C8X[i] != 0)DCB->value[0][5][i] = -C1X_C6I[i] + C1X_C8X[i];
	}
	for (i = 0; i < 32; i++) {
		if (C1C_C2W[i] != 0)DCB->value[1][0][i] = C1C_C2W[i];
		if (C1W_C2W[i] != 0)DCB->value[1][1][i] = C1W_C2W[i];
		if (C1C_C5Q[i] != 0)DCB->value[1][2][i] = C1C_C5Q[i];
		if (C1C_C5X[i] != 0 && C1C_C2W[i] != 0)DCB->value[1][3][i] = -C1C_C5X[i] + C1C_C2W[i];
	}
	for (i = 0; i < 36; i++) {
		if (E_C1X_C5X[i] != 0)DCB->value[2][0][i] = E_C1X_C5X[i];
		if (C1X_C7X[i] != 0)DCB->value[2][1][i] = C1X_C7X[i];
		if (E_C1X_C8X[i] != 0)DCB->value[2][2][i] = E_C1X_C8X[i];
	}
	fclose(fp);
}

//
static  void  decode_tobs(prcopt_t* prcopt, char* oneline, FILE* fp, sigind_t* sig, int* loc, int s) {
	int i, j, k;
	char P_code[3][7][4] = {
							  {"C2I","C6I","C7I","C1X","C5X","C7Z","C8X"},//BDS
							  {"C1C","C1W","C2W","C5Q","C5X",  "" , ""  },//GPS
							  {"C1X","C5X","C7X","C8X","C6C",  "" , ""  }//GAL
	};
	char L_code[3][7][4] = {
							  {"L2I","L6I","L7I","L1X","L5X","L7Z","L8X"},
							  {"L1C","L1W","L2W","L5Q","L5X",  "" ,  "" },
							  {"L1X","L5X","L7X","L8X","L6C",  "" ,  "" }
	};


	sig->num_singal = (int)str2num(oneline, 4, 2);
	if (sig->num_singal <= 13)
	{
		for (i = 0, k = 7; i <= 13, k <= 58; i++, k += 4)setstr(sig->code[i], oneline + k, 3);
	}
	else if (13 < sig->num_singal && sig->num_singal <= 26)
	{
		for (i = 0, k = 7; i <= 13, k <= 58; i++, k += 4)
		{
			setstr(sig->code[i], oneline + k, 3);
		}
		fgets(oneline, MAXRNXLEN, fp);
		for (i = 0, k = 7; i < (sig->num_singal - 13); i++, k += 4)
		{
			setstr(sig->code[i + 13], oneline + k, 3);
		}
	}
	else if (26 < sig->num_singal && sig->num_singal <= 39)
	{
		for (i = 0, k = 7; i <= 13, k <= 58; i++, k += 4)
		{
			setstr(sig->code[i], oneline + k, 3);
		}
		fgets(oneline, MAXRNXLEN, fp);
		for (i = 0, k = 7; i <= 13, k <= 58; i++, k += 4)
		{
			setstr(sig->code[i + 13], oneline + k, 3);
		}
		fgets(oneline, MAXRNXLEN, fp);
		for (i = 0, k = 7; i < (sig->num_singal - 26); i++, k += 4)
		{
			setstr(sig->code[i + 26], oneline + k, 3);
		}
	}
	//
	if (s == 0) {
		prcopt->ISYS = 0;
		for (j = 0; j < 7; j++) {
			for (i = 0; i <= sig->num_singal; i++)
			{
				switch (prcopt->f[0][j]) {
				case '0':if (strstr(sig->code[i], P_code[0][0]))   loc[0] = i;//C2I
					if (strstr(sig->code[i], L_code[0][0]))   loc[7] = i; break;
				case '1':if (strstr(sig->code[i], P_code[0][1]))   loc[1] = i;//C6I
					if (strstr(sig->code[i], L_code[0][1]))   loc[8] = i; break;
				case '2':if (strstr(sig->code[i], P_code[0][2]))   loc[2] = i;//C7I
					if (strstr(sig->code[i], L_code[0][2]))   loc[9] = i; break;
				case '3':if (strstr(sig->code[i], P_code[0][3]))   loc[3] = i;//C1X
					if (strstr(sig->code[i], L_code[0][3]))   loc[10] = i; break;
				case '4':if (strstr(sig->code[i], P_code[0][4]))   loc[4] = i;//C5X
					if (strstr(sig->code[i], L_code[0][4]))   loc[11] = i; break;
				case '5':if (strstr(sig->code[i], P_code[0][5]))   loc[5] = i;//C7Z
					if (strstr(sig->code[i], L_code[0][5]))   loc[12] = i; break;
				case '6':if (strstr(sig->code[i], P_code[0][6]))   loc[6] = i;//C8X
					if (strstr(sig->code[i], L_code[0][6]))   loc[13] = i; break;
				}
			}
		}
	}
	if (s == 1) {
		for (j = 0; j < 7; j++) {
			for (i = 0; i <= sig->num_singal; i++)
			{
				switch (prcopt->f[1][j]) {
				case '0':if (strstr(sig->code[i], P_code[1][0]))   loc[0] = i;//L1 C1C
					if (strstr(sig->code[i], L_code[1][0]))   loc[7] = i; break;
				case '1':if (strstr(sig->code[i], P_code[1][1]))   loc[1] = i;//L1 C1W
					if (strstr(sig->code[i], L_code[1][1]))   loc[8] = i; break;
				case '2':if (strstr(sig->code[i], P_code[1][2]))   loc[2] = i;//L2 C2W
					if (strstr(sig->code[i], L_code[1][2]))   loc[9] = i; break;
				case '3':if (strstr(sig->code[i], P_code[1][3]))   loc[3] = i;//L5 C5Q
					if (strstr(sig->code[i], L_code[1][3]))   loc[10] = i; break;
				case '4':if (strstr(sig->code[i], P_code[1][4]))   loc[4] = i;//L5 C5X
					if (strstr(sig->code[i], L_code[1][4]))   loc[11] = i; break;
				}
			}
		}
	}
	if (s == 2) {
		for (j = 0; j < 7; j++) {
			for (i = 0; i <= sig->num_singal; i++)
			{
				switch (prcopt->f[2][j]) {
				case '0':if (strstr(sig->code[i], P_code[2][0]))   loc[0] = i;//E1 C1X
					if (strstr(sig->code[i], L_code[2][0]))   loc[7] = i; break;
				case '1':if (strstr(sig->code[i], P_code[2][1]))   loc[1] = i;//E5a C5X
					if (strstr(sig->code[i], L_code[2][1]))   loc[8] = i; break;
				case '2':if (strstr(sig->code[i], P_code[2][2]))   loc[2] = i;//E5b C7X
					if (strstr(sig->code[i], L_code[2][2]))   loc[9] = i; break;
				case '3':if (strstr(sig->code[i], P_code[2][3]))   loc[3] = i;//E5 C8X
					if (strstr(sig->code[i], L_code[2][3]))   loc[10] = i; break;
				case '4':if (strstr(sig->code[i], P_code[2][4]))   loc[4] = i;//E6 C6C
					if (strstr(sig->code[i], L_code[2][4]))   loc[11] = i; break;
				}
			}
		}
	}


}
static int decode_obsepoch(FILE* fp, char* oneline, gtime_t* time) {
	int n;
	if (oneline[0] != '>' || str2time(oneline, 1, 28, time)) {
		printf("rinex obs invalid epoch:epoch=%29.29s\n", oneline);
		return 0;
	}
	n = (int)str2num(oneline, 32, 3); return n;
}

int read_obs(prcopt_t* prcopt, char* filepath, obsd_t obs[], int f) {
	FILE* fp = NULL;
	char oneline[MAXCHARS], * label = oneline + 60;
	double  ver;
	int i = 0, nsat = 0, ep, j, k, m, n, loc[3][14];
	gtime_t time = { 0 }, date = {0};
	sigind_t signal;
	char tobs[NUMSYS][MAXOBSTYPE][4] = { {""} };
	memset(loc, -1, sizeof(loc));
	memset(obs[f].P, 0, sizeof(obs[f].P));
	memset(obs[f].L, 0, sizeof(obs[f].L));
	obs[f].crdTrue[0] = obs[f].crdTrue[1] = obs[f].crdTrue[2] = 0.0;
	if ((fp = fopen(filepath, "r")) == NULL) {
		perror("fopen");
	}
	while (fgets(oneline, MAXCHARS, fp))
	{
		if (strlen(oneline) <= 30)continue;
		else if (strstr(label, "RINEX VERSION / TYPE")) {
			ver = str2num(oneline, 0, 9);
			if (ver < 2.99) {
				printf("***Please select a file with version 3.0 or higher");
				break;
			}
			continue;
		}
		else if (strstr(label, "APPROX POSITION XYZ")) {
			obs[f].crdTrue[0] = str2num(oneline, 0, 14);  //X
			obs[f].crdTrue[1] = str2num(oneline, 15, 15); //Y  
			obs[f].crdTrue[2] = str2num(oneline, 29, 15);  //Z
			continue;
		}
		else if (strstr(label, "SYS / # / OBS TYPES")) {
			if (!strchr(prcopt->sys, oneline[0]))continue;
			{
				switch (oneline[0]) {
				case 'C':decode_tobs(prcopt, oneline, fp, &signal, loc[0], 0); break;
				case 'G':decode_tobs(prcopt, oneline, fp, &signal, loc[1], 1); break;
				case 'E':decode_tobs(prcopt, oneline, fp, &signal, loc[2], 2); break;
				}
			}
		}
		else if (strstr(label, "TIME OF FIRST OBS")&&
			prcopt->time.year != str2num(oneline, 0, 6))
		{
			prcopt->time.year = str2num(oneline, 0, 6);
			prcopt->time.month = str2num(oneline, 6, 6);
			prcopt->time.day = str2num(oneline, 12, 6);
			prcopt->time.h = str2num(oneline, 18, 6);
			prcopt->time.min = str2num(oneline, 24, 6);
			prcopt->time.sec = str2num(oneline, 30, 7);
		}
		else if (strstr(oneline, "END OF HEADER")) {
			while (fgets(oneline, MAXRNXLEN, fp))
			{
				if (strlen(oneline) == 0) break;
				else if ((oneline[0] == '>')) {
					nsat = decode_obsepoch(fp, oneline, &time);
					time.h = str2num(oneline, 13, 2);
					time.min = str2num(oneline, 16, 2);
					time.s = str2num(oneline, 19, 9);
					ep = (int)(time.h * 120 + time.min * 2 + time.s / 30 + 1);
				}
				else continue;
				for (i = 0; i <= nsat - 1; i++) {
					double val[MAXOBSTYPE] = { 0 };
					fgets(oneline, MAXRNXLEN, fp);
					if (!strchr(prcopt->sys, oneline[0]))
						continue;
					else {
						obs[f].sat = (int)str2num(oneline, 1, 2);
						for (k = 0, j = 3; k <= signal.num_singal; k++, j += 16) {
							val[k] = str2num(oneline, j, 14);
						}
						switch (oneline[0]) {
						case 'C':
							for (m = 0; m < 7; m++) {
								if (loc[0][m] == -1)
									continue;
								obs[f].P[0][m][ep - 1][obs[f].sat - 1] = val[loc[0][m]];
							}
							for (n = 7; n < 14; n++) {
								if (loc[0][n] == -1)
									continue;
								obs[f].L[0][n - 7][ep - 1][obs[f].sat - 1] = val[loc[0][n]];
							}
							break;
						case 'G':
							for (m = 0; m < 7; m++) {
								if (loc[1][m] == -1)
									continue;
								obs[f].P[1][m][ep - 1][obs[f].sat - 1] = val[loc[1][m]];
							}
							for (n = 7; n < 14; n++) {
								if (loc[1][n] == -1)
									continue;
								obs[f].L[1][n - 7][ep - 1][obs[f].sat - 1] = val[loc[1][n]];
							}
							break;
						case 'E':
							for (m = 0; m < 7; m++) {
								if (loc[2][m] == -1)
									continue;
								obs[f].P[2][m][ep - 1][obs[f].sat - 1] = val[loc[2][m]];
							}
							for (n = 7; n < 14; n++) {
								if (loc[2][n] == -1)
									continue;
								obs[f].L[2][n - 7][ep - 1][obs[f].sat - 1] = val[loc[2][n]];
							}
							break;
						}
					}
				}
			}
		}
	}
	return 1;
	fclose(fp);
}

void read_sp3(char* filepath, nav_t nav[], int i) {
	FILE* fp = NULL;
	char oneline[MAXCHARS];
	int ep;
	gtime_t time = { 0 };
	memset(nav[i].X, 0, sizeof(nav[i].X));
	memset(nav[i].Y, 0, sizeof(nav[i].Y));
	memset(nav[i].Z, 0, sizeof(nav[i].Z));
	if ((fp = fopen(filepath, "r")) == NULL) {
		perror("fopen");
	}
	while (fgets(oneline, MAXCHARS, fp)) {
		if (strlen(oneline) <= 30)continue;
		if ((oneline[0] == '*')) {
			time.h = str2num(oneline, 14, 2);
			time.min = str2num(oneline, 17, 2);
			time.s = str2num(oneline, 21, 9);
			ep = (int)(time.h * 12 + time.min / navinter) + 1;//
			continue;
		}
		else if (strlen(oneline) > 1 && strstr(oneline, "PC") && (nav[i].sat = (int)str2num(oneline, 2, 2)) > 0) {
			nav[i].X[0][ep - 1][nav[i].sat - 1] = str2num(oneline, 5, 14);
			nav[i].Y[0][ep - 1][nav[i].sat - 1] = str2num(oneline, 19, 14);
			nav[i].Z[0][ep - 1][nav[i].sat - 1] = str2num(oneline, 33, 14);
			continue;
		}
		else if (strlen(oneline) > 1 && strstr(oneline, "PG") && (nav[i].sat = (int)str2num(oneline, 2, 2)) > 0) {
			nav[i].X[1][ep - 1][nav[i].sat - 1] = str2num(oneline, 5, 14);
			nav[i].Y[1][ep - 1][nav[i].sat - 1] = str2num(oneline, 19, 14);
			nav[i].Z[1][ep - 1][nav[i].sat - 1] = str2num(oneline, 33, 14);
			continue;
		}
		else if (strlen(oneline) > 1 && strstr(oneline, "PE") && (nav[i].sat = (int)str2num(oneline, 2, 2)) > 0) {
			nav[i].X[2][ep - 1][nav[i].sat - 1] = str2num(oneline, 5, 14);
			nav[i].Y[2][ep - 1][nav[i].sat - 1] = str2num(oneline, 19, 14);
			nav[i].Z[2][ep - 1][nav[i].sat - 1] = str2num(oneline, 33, 14);
			continue;
		}
	}
	fclose(fp);
}

int read_data(prcopt_t* prcopt, obsd_t obs[], nav_t nav[], dcb_t* DCB)
{
	char* folder;
	DIR* dir;
	char sep = (char)FILEPATHSEP;
	double crdTrue[3] = { 0 };
	int o_file_count = 0, sp3_file_count = 0, dcb_file_count = 0, ion_file_count = 0, i;
	char* o_file_name[MAX_FILES];
	char* sp3_file_name[MAX_sp3FILES], * dcb_file_name[MAX_dcbFILES], * ion_file_name[MAX_dcbFILES], * ext, * filepath;
	struct dirent* file;
	folder = prcopt->inputpath;
	if (!(dir = opendir(folder)))
	{
		printf("Unable to open directory");
		exit(-1);
	}
	while ((file = readdir(dir)) != NULL) {
		if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
			ext = strrchr(file->d_name, '.');
			if (ext != NULL) 
			{
				// obs file
				if (strchr(ext + strlen(ext) - 1, 'o')||
					strchr(ext + strlen(ext) - 1, 'O')) {
					o_file_name[o_file_count] = _strdup(file->d_name);
					o_file_count++;
				}
				else if (strstr(ext, "sp3")||
					strstr(ext, "SP3")) {
					sp3_file_name[sp3_file_count] = _strdup(file->d_name);
					sp3_file_count++;
				}
				else if (strstr(ext, ".BSX")) {
					dcb_file_name[dcb_file_count] = _strdup(file->d_name);
					dcb_file_count++;
				}
				else if (strstr(ext, "I")) {
					ion_file_name[ion_file_count] = _strdup(file->d_name);
					ion_file_count++;
				}
			}	// End of if ext != NULL
		}	// End of if (file->d_type == DT_REG)
	}	// End of while
	//check files
	if (o_file_count == 0)
		printf("	there is no obs files");
	if (sp3_file_count == 0)
		printf("	there is no sp3 files");
	if (dcb_file_count == 0)
		printf("	there is no dcb files");
	if (ion_file_count == 0)
		printf("	there is no dcb files");
	closedir(dir);
	//read sp3
	if (sp3_file_count < 3) 
	{
		printf("need at least three SP3 files");
	}
	else 
	{
		for (int i = 0; i < sp3_file_count; i++) {
			filepath = (char*)malloc(strlen(folder) + strlen(sp3_file_name[i]) + 2);
			sprintf(filepath, "%s%c%s", folder, sep, sp3_file_name[i]);
			read_sp3(filepath, nav, i);
			free(filepath);
		}
		interplotation(nav);
	}
	//read  CAS DCB
	for (i = 0; i < dcb_file_count; i++) {
		filepath = (char*)malloc(strlen(folder) + strlen(dcb_file_name[i]) + 2);
		sprintf(filepath, "%s%c%s", folder, sep, dcb_file_name[i]);
		read_casdcb(prcopt, filepath, DCB);
		free(filepath);
	}

	//read obs
	for (i = 0; i < o_file_count; i++) {
		filepath = (char*)malloc(strlen(folder) + strlen(o_file_name[i]) + 2);
		sprintf(filepath, "%s%c%s", folder, sep, o_file_name[i]);
		read_obs(prcopt, filepath, obs, i);

		free(filepath);
	}
	return o_file_count;
}

/////////////////////////////////////////// process ///////////////////
static int proccfgfile(const char cfgfile[], prcopt_t* prcopt) {
	long t1;
	t1 = clock();
	FILE* fp = NULL;
	//obsd_t obs[MAX_FILES];
	obsd_t* obs = malloc(sizeof(obsd_t) * MAX_FILES);
	nav_t nav[3];
	dcb_t DCB;
	lsq_t lsq;
	result_t result;
	P12_t* P12 = calloc(MAX_FILES, sizeof(P12_t));
	double nav_f[3][7] = {
						  {1561.098e6, 1268.52e6, 1207.14e6, 1575.42e6,  1176.45e6, 1207.14e6, 1191.795e6}, //BDS
						  {1575.42e6,  1575.42e6, 1227.6e6,  1176.45e6,  1176.45e6,     0,         0     }, //GPS
						  {1575.42e6,  1176.45e6, 1207.14e6, 1191.795e6, 1278.75e6,     0,         0     }  //GAL 
	};
	//f1^2*f2^2/(40.3*(f1^2-f2^2))
	double KK[3][6] = {
						  {1.1754,    -3.829,  1.1354 ,  -2.455 , -3.829, -3.0172},
						  {-9.5196,  -9.5196, 4.2089 ,  4.2089 ,    0 ,     0  },
						  {-7.7637,  -8.7575, -8.2403, -1.1893 ,     0,     0  }
	};
	char line[MAXSTRPATH];
	char* p = NULL;
	int o_num, i, j, m;
	obsd_t* OBS = malloc(sizeof(obsd_t));
	//............................initialization..................................................

	double alpha[3] = {
						 (nav_f[0][0] * nav_f[0][0]) / (nav_f[0][0] * nav_f[0][0] - nav_f[0][1] * nav_f[0][1]) ,
						 (nav_f[1][1] * nav_f[1][1]) / (nav_f[1][1] * nav_f[1][1] - nav_f[1][2] * nav_f[1][2]) ,
						 (nav_f[2][0] * nav_f[2][0]) / (nav_f[2][0] * nav_f[2][0] - nav_f[2][1] * nav_f[2][1])
	};
	double	beta[3] = {
						(-nav_f[0][1] * nav_f[0][1]) / (nav_f[0][0] * nav_f[0][0] - nav_f[0][1] * nav_f[0][1]),
						(-nav_f[1][2] * nav_f[1][2]) / (nav_f[1][1] * nav_f[1][1] - nav_f[1][2] * nav_f[1][2]),
						(-nav_f[2][1] * nav_f[2][1]) / (nav_f[2][0] * nav_f[2][0] - nav_f[2][1] * nav_f[2][1])
	};
	lsq.maxsys_sat[0] = MAXPRNCMP; lsq.maxsys_sat[1] = MAXPRNGPS; lsq.maxsys_sat[2] = MAXPRNGAL;
	memset(lsq.count, 0, sizeof(lsq.count));
	memset(lsq.nr, 0, sizeof(lsq.nr));
	memset(lsq.ns, 0, sizeof(lsq.ns));
	memset(lsq.PRN, 0, sizeof(lsq.PRN));
	memset(lsq.sat, -1, sizeof(lsq.sat));
	memset(lsq.Wx, 0, sizeof(lsq.Wx));
	memset(result.DCBflag, 0, sizeof(result.DCBflag));
	memset(result.OSBflag, 0, sizeof(result.OSBflag));
	memset(result.DCB, 0, sizeof(result.DCB));
	memset(result.OSB, 0, sizeof(result.OSB));
	prcopt->inputpath[0] = '\0';

	//....................................Read cfgfile................................................
	if ((fp = fopen(cfgfile, "r")) == NULL) {
		perror("fopen");
		return 0;
	}
	while (!feof(fp))
	{
		line[0] = '\0';
		fgets(line, MAXSTRPATH, fp);
		trimSpace(line);
		if ((line != NULL) && (line[0] == '#'))continue;
		p = strrchr(line, '=');
		if (strstr(line, "folder"))
		{
			sscanf(p + 1, "%[^,]", prcopt->inputpath);
			trimSpace(prcopt->inputpath);
		}
		else if (strstr(line, "BDSType")) {
			sscanf(p + 1, "%s", &prcopt->f[0]);
		}
		else if (strstr(line, "GPSType")) {
			sscanf(p + 1, "%s", &prcopt->f[1]);
		}
		else if (strstr(line, "GALType")) {
			sscanf(p + 1, "%s", &prcopt->f[2]);
		}
		else if (strstr(line, "elevation")) {
			sscanf(p + 1, "%d", &prcopt->LIM);
		}
		else if (strstr(line, "system")) {
			sscanf(p + 1, "%s", &prcopt->sys);
		}
		else if (strstr(line, "OutBiasDir")) {
			sscanf(p + 1, "%[^,]", &prcopt->outpath);
			trimSpace(prcopt->outpath);
		}

	}
	fclose(fp);

	//............................................Read data.........................................
	o_num = read_data(prcopt, obs, nav, &DCB);
	printf("read data completing!\n");

	//............................................Get  P4............................................
	for (i = 0; i < o_num; i++)
	{
		for (m = 0; m < 3; m++)
		{
			switch (prcopt->sys[m])
			{
			case 'C':
				for (j = 0; j < 7; j++) {
					prcopt->ISYS = 0;
					memcpy(OBS, obs + i, sizeof(obsd_t));
					switch (prcopt->f[0][j])
					{
					case '0':get_P4(OBS, nav, P12 + i, prcopt, nav_f[0], &lsq, 0, 1, 0); break;//C2I_C6I
					case '2':get_P4(OBS, nav, P12 + i, prcopt, nav_f[0], &lsq, 1, 2, 1); break;//C6I-C7I
					case '3':get_P4(OBS, nav, P12 + i, prcopt, nav_f[0], &lsq, 1, 3, 2); break;//C6I-C1X
					case '4':get_P4(OBS, nav, P12 + i, prcopt, nav_f[0], &lsq, 1, 4, 3); break;//C6I-C5X
					case '5':get_P4(OBS, nav, P12 + i, prcopt, nav_f[0], &lsq, 1, 5, 4); break;//C6I-C7Z
					case '6':get_P4(OBS, nav, P12 + i, prcopt, nav_f[0], &lsq, 1, 6, 5); break;//C6I-C8X
					}
				}
				break;
			case 'G':
				for (j = 0; j < 7; j++) {
					prcopt->ISYS = 1;
					memcpy(OBS, obs + i, sizeof(obsd_t));
					switch (prcopt->f[1][j]) {
					case '0':get_P4(OBS, nav, P12 + i, prcopt, nav_f[1], &lsq, 0, 2, 0); break;//C1C-C2W
					case '1':get_P4(OBS, nav, P12 + i, prcopt, nav_f[1], &lsq, 1, 2, 1); break;//C1W-C2W
					case '3':get_P4(OBS, nav, P12 + i, prcopt, nav_f[1], &lsq, 3, 2, 2); break;//C5Q-C2W
					case '4':get_P4(OBS, nav, P12 + i, prcopt, nav_f[1], &lsq, 4, 2, 3); break;//C5X-C2W
					}
				}
				break;
			case 'E':
				for (j = 0; j < 7; j++) {
					prcopt->ISYS = 2;
					memcpy(OBS, obs + i, sizeof(obsd_t));
					switch (prcopt->f[2][j]) {
					case '1':get_P4(OBS, nav, P12 + i, prcopt, nav_f[2], &lsq, 0, 1, 0); break;//C1X-C5X
					case '2':get_P4(OBS, nav, P12 + i, prcopt, nav_f[2], &lsq, 0, 2, 1); break;//C1X-C7X
					case '3':get_P4(OBS, nav, P12 + i, prcopt, nav_f[2], &lsq, 0, 3, 2); break;//C1X-C8X
					case '4':get_P4(OBS, nav, P12 + i, prcopt, nav_f[2], &lsq, 0, 4, 3); break;//C1X-C6C
					}
				}
					break;
			}
		}
	}
	printf("Get P4 completing !\n");
	printf("Start Least Squares!\n");



	//.........................................Least Square................................................
	for (m = 0; m < 3; m++) {
		switch (prcopt->sys[m]) {
		case 'C':
			for (j = 0; j < 7; j++)
			{
				prcopt->ISYS = 0;
				switch (prcopt->f[0][j]) {
				case '0':
					printf("Starting to estimate  BDS C2I-C6I\n");
					get_equation(&lsq, &DCB, &result, prcopt, 0, 0);
					LSQ(&result, P12, obs, nav, prcopt, &lsq, o_num, KK[0], 0);
					result.DCBflag[prcopt->ISYS][0] = 1;
					result.OSBflag[prcopt->ISYS][0] = 1;
					result.OSBflag[prcopt->ISYS][1] = 1;
					printf("BDS C2I-C6I completed\n");
					break;//C2I_C6I
				case '2':
					printf("Starting to estimate  BDS C6I-C7I\n");
					get_equation(&lsq, &DCB, &result, prcopt, 0, 1);
					LSQ(&result, P12, obs, nav, prcopt, &lsq, o_num, KK[0], 1);
					result.DCBflag[prcopt->ISYS][1] = 1;
					result.OSBflag[prcopt->ISYS][2] = 1;
					printf("BDS C6I-C7I completed\n");
					break;//C6I_C7I
				case '3':
					printf("Starting to estimate  BDS C6I-C1X\n");
					get_equation(&lsq, &DCB, &result, prcopt, 0, 2);
					LSQ(&result, P12, obs, nav, prcopt, &lsq, o_num, KK[0], 2);
					result.DCBflag[prcopt->ISYS][2] = 1;
					result.OSBflag[prcopt->ISYS][3] = 1;
					printf("BDS C6I-C1X completed\n");
					break;//C6I_C1X
				case '4':
					printf("Starting to estimate  BDS C6I-C5X\n");
					get_equation(&lsq, &DCB, &result, prcopt, 0, 3);
					LSQ(&result, P12, obs, nav, prcopt, &lsq, o_num, KK[0], 3);
					result.DCBflag[prcopt->ISYS][3] = 1;
					result.OSBflag[prcopt->ISYS][4] = 1;
					printf("BDS C6I-C5X completed\n");
					break;//C6I_C5X
				case '5':
					printf("Starting to estimate  BDS C6I-C7Z\n");
					get_equation(&lsq, &DCB, &result, prcopt, 0, 4);
					LSQ(&result, P12, obs, nav, prcopt, &lsq, o_num, KK[0], 4);
					result.DCBflag[prcopt->ISYS][4] = 1;
					result.OSBflag[prcopt->ISYS][5] = 1;
					printf("BDS C6I-C7Z completed\n");
					break;//C6I_C7Z
				case '6':
					printf("Starting to estimate  BDS C6I-C8X\n");
					get_equation(&lsq, &DCB, &result, prcopt, 0, 5);
					LSQ(&result, P12, obs, nav, prcopt, &lsq, o_num, KK[0], 5);
					result.DCBflag[prcopt->ISYS][5] = 1;
					result.OSBflag[prcopt->ISYS][6] = 1;
					printf("BDS C6I-C8X completed\n");
					break;//C6I_C8X

				}
			}
				break;
		case 'G':
			for (j = 0; j < 7; j++)
			{
				prcopt->ISYS = 1;
				switch (prcopt->f[1][j]) {
				case '0':
					printf("Starting to estimate  GPS C1C-C2W\n");
					get_equation(&lsq, &DCB, &result, prcopt, 1, 0);
					LSQ(&result, P12, obs, nav, prcopt, &lsq, o_num, KK[1], 0);
					result.DCBflag[prcopt->ISYS][0] = 1;
					result.OSBflag[prcopt->ISYS][0] = 1;
					result.OSBflag[prcopt->ISYS][2] = 1;
					printf("GPS C1C-C2W completed\n");
					break;//C1C-C2W
				case '1':printf("Starting to estimate  GPS C1W-C2W\n");
					get_equation(&lsq, &DCB, &result, prcopt, 1, 1);
					LSQ(&result, P12, obs, nav, prcopt, &lsq, o_num, KK[1], 1);
					result.DCBflag[prcopt->ISYS][1] = 1;
					result.OSBflag[prcopt->ISYS][2] = 1;
					result.OSBflag[prcopt->ISYS][1] = 1;
					printf("GPS C1W-C2W completed\n");
					break;//C1W-C2W
				case '3':printf("Starting to estimate  GPS C5Q-C2W\n");
					get_equation(&lsq, &DCB, &result, prcopt, 1, 2);
					LSQ(&result, P12, obs, nav, prcopt, &lsq, o_num, KK[1], 2);
					result.DCBflag[prcopt->ISYS][2] = 1;
					result.OSBflag[prcopt->ISYS][3] = 1;
					printf("GPS C5Q-C2W completed\n");
					break;//C5Q-C2W
				case '4':printf("Starting to estimate  GPS C5X-C2W\n");
					get_equation(&lsq, &DCB, &result, prcopt, 1, 3);
					LSQ(&result, P12, obs, nav, prcopt, &lsq, o_num, KK[1], 3);
					result.DCBflag[prcopt->ISYS][3] = 1;
					result.OSBflag[prcopt->ISYS][4] = 1;
					printf("GPS C5X-C2W completed\n");
					break;//C5X-C2W
				}

			}
			break;
		case 'E':
			for (j = 0; j < 7; j++)
			{
				prcopt->ISYS = 2;
				switch (prcopt->f[2][j]) {
				case '1':printf("Starting to estimate  GAL C1X-C5X\n");
					get_equation(&lsq, &DCB, &result, prcopt, 2, 0);
					LSQ(&result, P12, obs, nav, prcopt, &lsq, o_num, KK[2], 0);
					result.DCBflag[prcopt->ISYS][0] = 1;
					result.OSBflag[prcopt->ISYS][0] = 1;
					result.OSBflag[prcopt->ISYS][1] = 1;
					printf("GAL C1X-C5X completed\n");
					break;//C1X-C5X
				case '2':printf("Starting to estimate  GAL C1X-C7X\n");
					get_equation(&lsq, &DCB, &result, prcopt, 2, 1);
					LSQ(&result, P12, obs, nav, prcopt, &lsq, o_num, KK[2], 1);
					result.DCBflag[prcopt->ISYS][1] = 1;
					result.OSBflag[prcopt->ISYS][2] = 1;
					printf("GAL C1X-C7X completed\n");
					break;//C1X-C7X
				case '3':printf("Starting to estimate  GAL C1X-C8X\n");
					get_equation(&lsq, &DCB, &result, prcopt, 2, 2);
					LSQ(&result, P12, obs, nav, prcopt, &lsq, o_num, KK[2], 2);
					result.DCBflag[prcopt->ISYS][2] = 1;
					result.OSBflag[prcopt->ISYS][3] = 1;
					printf("GAL C1X-C8X completed\n");
					break;//C1X-C8X
				case '4':printf("Starting to estimate  GAL C1X-C6C\n");
					get_equation(&lsq, &DCB, &result, prcopt, 2, 3);
					LSQ(&result, P12, obs, nav, prcopt, &lsq, o_num, KK[2], 3);
					result.DCBflag[prcopt->ISYS][3] = 1;
					result.OSBflag[prcopt->ISYS][4] = 1;
					printf("GAL C1X-C6C completed\n");
					break;//C1X-C6C
				}
			}
			break;
		}
	}
	//....................................................Format Output.............................................
	get_result(&result, prcopt, &lsq, alpha, beta);
	outresult(prcopt, result, &DCB);

	long t2 = clock();
	printf("\n *The total time for running the program :%6.3f seconds\n%c", (double)(t2 - t1) / CLOCKS_PER_SEC, '\0');
	
	free(obs);
	free(OBS);
	free(P12);
	return 1;
}

void EstimateBias(const char* cfg)
{
	prcopt_t prcopt;
	proccfgfile(cfg, &prcopt);
}