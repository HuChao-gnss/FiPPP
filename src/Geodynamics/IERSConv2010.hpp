#ifndef IERS_CONV_2010_HPP
#define IERS_CONV_2010_HPP

#include "Vector.hpp"
#include "Matrix.hpp"
#include "CommonTime.hpp"

namespace gpstk
{

    /* dsign(A,B) - magnitude of A with sign of B (double) */
    double dsign(double A, double B);


    /**Rotate an r-matrix about the z-axis.
     *
     * @param   psi     angle (radians)
     * @return  r       r_matrix, rotated
     *
     */
    void iauRz(double psi, Matrix<double>& r);


    /**Rotate an r-matrix about the y-axis.
     *
     * @param   theta   angle (radians)
     * @return  r       r_matrix, rotated
     *
     */
    void iauRy(double theta, Matrix<double>& r);


    /**Rotate an r-matrix about the x-axis.
     *
     * @param   phi     angle (radians)
     * @return  r       r_matrix, rotated
     *
     */
    void iauRx(double phi, Matrix<double>& r);



    /**Normalize angle into the range 0 <= a < 2pi.
     *
     * @param   a       angle (radians)
     * @return          angle in range 0-2pi
     *
     */
    double iauAnp(double a);


    /**Normalize angle into the range -pi <= a < +pi.
     *
     * @param   a       angle (radians)
     * @return          angle in range +/-pi
     *
     */
    double iauAnpm(double a);


    /**Fundamental argument, IERS Conventions (2003):
     * mean elongation of the Moon from the Sun.
     *
     * @param   t   TDB, Julian centuries since J2000.0
     * @return      D, radians
     *
     */
    double iauFad03(double t);


    /**Fundamental argument, IERS Conventions (2003):
     * mean longitude of Earth.
     *
     * @param   t   TDB, Julian centuries since J2000.0
     * @return      mean longitude of Earth, radians
     *
     */
    double iauFae03(double t);


    /**Fundamental argument, IERS Conventions (2003):
     * mean longitude of the Moon minus mean longitude of ascending
     * node.
     *
     * @param   t   TDB, Julian centuries since J2000.0
     * @return      F, radians
     *
     */
    double iauFaf03(double t);


    /**Fundamental argument, IERS Conventions (2003):
     * mean longitude of Jupiter.
     *
     * @param   t   TDB, Julian centuries since J2000.0
     * @return      mean longitude of Jupiter, radians
     *
     */
    double iauFaju03(double t);


    /**Fundamental argument, IERS Conventions (2003):
     * mean anomaly of the Moon.
     *
     * @param   t   TDB, Julian centuries since J2000.0
     * @return      l, radians
     *
     */
    double iauFal03(double t);


    /**Fundamental argument, IERS Conventions (2003):
     * mean anomaly of the Sun.
     *
     * @param   t   TDB, Julian centuries since J2000.0
     * @return      l', radians
     *
     */
    double iauFalp03(double t);


    /**Fundamental argument, IERS Conventions (2003):
     * mean longitude of Mars.
     *
     * @param   t   TDB, Julian centuries since J2000.0
     * @return      mean longitude of Mars, radians
     *
     */
    double iauFama03(double t);


    /**Fundamental argument, IERS Conventions (2003):
     * mean longitude of Mercury.
     *
     * @param   t   TDB, Julian centuries since J2000.0
     * @return      mean longitude of Mercury, radians
     *
     */
    double iauFame03(double t);


    /**Fundamental argument, IERS Conventions (2003):
     * mean longitude of Neptune.
     *
     * @param   t   TDB, Julian centuries since J2000.0
     * @return      mean longitude of Neptune, radians
     *
     */
    double iauFane03(double t);


    /**Fundamental argument, IERS Conventions (2003):
     * mean longitude of the Moon's ascending node.
     *
     * @param   t   TDB, Julian centuries since J2000.0
     * @return      Omega, radians
     *
     */
    double iauFaom03(double t);


    /**Fundamental argument, IERS Conventions (2003):
     * general accumulated precession in longitude.
     *
     * @param   t   TDB, Julian centuries since J2000.0
     * @return      general precession in longitude, radians
     *
     */
    double iauFapa03(double t);


    /**Fundamental argument, IERS Conventions (2003):
     * mean longitude of Saturn.
     *
     * @param   t   TDB, Julian centuries since J2000.0
     * @return      mean longitude of Saturn, radians
     *
     */
    double iauFasa03(double t);


    /**Fundamental argument, IERS Conventions (2003):
     * mean longitude of Uranus.
     *
     * @param   t   TDB, Julian centuries since J2000.0
     * @return      mean longitude of Uranus, radians
     *
     */
    double iauFaur03(double t);


    /**Fundamental argument, IERS Conventions (2003):
     * mean longation of Venus.
     *
     * @param   t   TDB, Julian centuries since J2000.0
     * @return      mean longitude of Venus, radians
     *
     */
    double iauFave03(double t);


    /**X,Y coordinates of celestial Intermediate pole from series based
     * on IAU 2006 precession and IAU 2000A nutation.
     *
     * @param   date1,date2     TT as a 2-part Julian Date
     * @return  x,y             CIP X,Y coordinates
     *
     */
    void iauXy06(double date1, double date2, double *x, double *y);


    /**Form the celestial to intermediate-frame-of-date matrix given the CIP
     * X,Y and the CIO locator s.
     *
     * @param   x,y             Celestial Intermediate Pole
     *          s               the CIO locator s
     * @return  rc2i            celestial-to-intermediate matrix
     *
     */
    void iauC2ixys(double x, double y, double s, Matrix<double>& rc2i);


    /**Nutation, IAU 2000A model (MHB2000 luni-solar and planetary nutation
     * with free core nutation omitted).
     *
     * @param   date1,date2     TT as a 2-part Julian Date
     * @return  dpsi,deps       nutation, luni-solar + planetary
     *
     */
    void iauNut00a(double date1, double date2, double *dpsi, double *deps);


    /**IAU 2000A nutation model with adjustments to match the IAU 2006
     * precession.
     *
     * @param   date1,date2     TT as a 2-part Julian Date
     * @return  dpsi,deps       nutation, luni-solar + planetary
     *
     */
    void iauNut06a(double date1, double date2, double *dpsi, double *deps);


    /**Mean obliquity of the ecliptic, IAU 2006 precession model.
     *
     * @param   date1,date2     TT as a 2-part Julian Date
     * @return                  obliquity of the ecliptic
     *
     */
    double iauObl06(double date1, double date2);


    /**Precession angles, IAU 2006 (Fukushima-Williams 4-angle formulation).
     *
     * @param   date1,date2     TT as a 2-part Julian Date
     * @return  gamb            F-W angle gamma_bar (radians)
     *          phib            F-W angle phi_bar (radians)
     *          psib            F-W angle psi_bar (radians)
     *          epsa            F-W angle epsilon_A (radians)
     *
     */
    void iauPfw06(double date1, double date2,
                  double *gamb, double *phib, double *psib, double *epsa);


    /**Form rotation matrix given the Fukushima-Williams angles.
     *
     * @param   gamb            F-W angle gamma_bar (radians)
     *          phib            F-W angle phi_bar (radians)
     *          psi             F-W angle psi (radians)
     *          eps             F-W angle epsilon (radians)
     * @return  r               rotation matrix
     *
     */
    void iauFw2m(double gamb, double phib, double psi, double eps,
                 Matrix<double>& r);


    /**Precession-nutation, IAU 2006 model: a multi-purpose function,
     * supporting classical (equinox-based) use directly and CIO-based use
     * indirectly.
     *
     * @param   date1,date2     TT as a 2-part Julian Data
     *          dpsi,deps       nutation
     * @return  epsa            mean obliquity
     *          rb              frame bias matrix
     *          rp              precession matrix
     *          rbp             bias-precession matrix
     *          rn              nutation matrix
     *          rbpn            GCRS-to-true matrix
     *
     */
    void iauPn06(double date1, double date2, double dpsi, double deps,
                 double *epsa,
                 Matrix<double>& rb, Matrix<double>& rp, Matrix<double>& rbp,
                 Matrix<double>& rn, Matrix<double>& rbpn);


    /**Precession-nutation, IAU 2006/2000A models: a multi-purpose function,
     * supporting classical (equinox-based) use directly and CIO-based use
     * indirectly.
     *
     * @param   date1,date2     TT as a 2-part Julian Data
     * @return  dpsi,deps       nutation
     *          epsa            mean obliquity
     *          rb              frame bias matrix
     *          rp              precession matrix
     *          rbp             bias-precession matrix
     *          rn              nutation matrix
     *          rbpn            GCRS-to-true matrix
     *
     */
    void iauPn06a(double date1, double date2,
                  double *dpsi, double *deps, double *epsa,
                  Matrix<double>& rb, Matrix<double>& rp, Matrix<double>& rbp,
                  Matrix<double>& rn, Matrix<double>& rbpn);


    /**Fram bias components of IAU 2000 precession-nutation models (part
     * of MHB2000 with additions).
     *
     * @return  dpsibi,depsbi   longitude and obliquity corrections
     *          dra             the ICRS RA of the J2000.0 mean equinox
     *
     */
    void iauBi00(double *dpsibi, double *depsbi, double *dra);


    /**Precession angles, IAU 2006, equinox based.
     *
     * @param   date1,date2     TT as a 2-part Julian Date
     * @return  eps0            epsilon_0
     *          psia            psi_A
     *          oma             omega_A
     *          bpa             P_A
     *          bqa             Q_A
     *          pia             pi_A
     *          bpia            Pi_A
     *          epsa            obliquity epsilon_A
     *          chia            chi_A
     *          za              z_A
     *          zetaa           theta_A
     *          pa              p_A
     *          gam             F-W angle gamma_J2000
     *          phi             F-W angle phi_J2000
     *          psi             F-W angle psi_J2000
     *
     */
    void iauP06e(double date1, double date2,
                 double *eps0, double *psia, double *oma, double *bpa,
                 double *bqa, double *pia, double *bpia,
                 double *epsa, double *chia, double *za, double *zetaa,
                 double *thetaa, double *pa,
                 double *gam, double *phi, double *psi);


    /**Form the matrix of nutation.
     *
     * @param   epsa            mean obliquity of date
     *          dpsi,deps       nutation
     * @return  rmatn           nutation matrix
     *
     */
    void iauNumat(double epsa, double dpsi, double deps, Matrix<double>& rmatn);


    /**Form the matrix of nutation for a given date, IAU 2006/2000A model.
     *
     * @param   date1,date2     TT as a 2-part Julian Date
     * @return  rmatn           nutation matrix
     *
     */
    void iauNum06a(double date1, double date2, Matrix<double>& rmatn);


    /**Form the matrix of precession-nutation for a given date (including
     * frame bias), IAU 2006 precession and IAU 2000A nutation models.
     *
     * @param   date1,date2     TT as a 2-part Julian Date
     * @return  rnpb            bias-precession-nutation matrix
     *
     */
    void iauPnm06a(double date1, double date2, Matrix<double>& rnpb);


    /**Extract from the bias-precession-nutation matrix the X,Y coordinates
     * of the Celestial Intermediate Pole.
     *
     * @param   rbpn            celestial-to-true matrix
     * @return                  Celestial Intermediate Pole
     *
     */
    void iauBpn2xy(Matrix<double> rbpn, double *x, double *y);


    /**The CIO locator s, positioning the Celestial Intermediate Origin on
     * the equator of the Celestial Intermediate Pole, given the CIP's X,Y
     * coordinates. Compatiable with IAU 2006/2000A precession-nutation.
     *
     * @param   date1,date2     TT as a 2-part Julian Date
     *          x,y             CIP coordinates
     * @return                  the CIO locator s in radians
     *
     */
    double iauS06(double date1, double date2, double x, double y);


    /**The CIO locator s, positioning the Celestial Intermediate Origin on
     * the equator of the Celestial Intermediate Pole, using the IAU 2006
     * precession and IAU 2000A nutation models.
     *
     * @param   date1,date2     TT as a 2-part Julian Date
     * @return                  the CIO locator s in radians
     *
     */
    double iauS06a(double date1, double date2);


    /**Earth rotation angle (IAU 2000 model).
     *
     * @param   dj1,dj2         UT1 as a 2-part Julian Date
     * @return                  Earth rotation angle (radians), range 0-2pi
     *
     */
    double iauEra00(double dj1, double dj2);


    /**Equation of the origins, given the classical NPB matrix and the
     * quantity s.
     *
     * @param   rnpb            classical nutation x precession x bias matrix
     *          s               the quantity s (the CIO locator)
     * @return                  the equation of the origins in radians
     */
    double iauEors(Matrix<double> rnpb, double s);


    /**Greenwich mean sidereal time (consistent with IAU 2006 precession).
     *
     * @param   uta,utb         UT1 as a 2-part Julian Date
     *          tta,ttb         TT as a 2-part Julian Date
     * @return                  Greenwich mean sidereal time (radians)
     *
     */
    double iauGmst06(double uta, double utb, double tta, double ttb);


    /**Greenwich apparent sidereal time, IAU 2006, given the NPB matrix.
     *
     * @param   uta,utb         UT1 as a 2-part Julian Date
     *          tta,ttb         TT as a 2-part Julian Date
     * @return                  Greenwich apparent sidereal time (radians)
     *
     */
    double iauGst06(double uta, double utb, double tta, double ttb,
                    Matrix<double> rnpb);


    /**Greenwich apparent sidereal time (consistent with IAU 2000 and 2006
     * resolutions).
     *
     * @param   uta,utb         UT1 as a 2-part Julian Date
     *          tta,ttb         TT as a 2-part Julian Date
     * @return                  Greenwich apparent sidereal time (radians)
     *
     */
    double iauGst06a(double uta, double utb, double tta, double ttb);


    /**The TIO locator s', positioning the Terrestrial Intermediate Origin
     * on the equator of the Celestial Intermediate Pole.
     *
     * @param   date1,date2     TT as a 2-part Julian Date
     * @return                  the TIO locator s' in radians
     *
     */
    double iauSp00(double date1, double date2);


    /**Form the matrix of polar motion for a given date, IAU 2000.
     *
     * @param   xp,yp           coordinates of the pole (radians)
     *          sp              the TIO locator s' (radians)
     * @return  rpom            polar-motion matrix
     *
     */
    void iauPom00(double xp, double yp, double sp, Matrix<double>& rpom);



    /**Compute doodson's fundamental arguments (BETA) and fundamental
     * arguments for nutation (FNUT).
     *
     * @param   uta,utb         UT1 as a 2-part Julian Date
     *          tta,ttb         TT as a 2-part Julian Date
     * @return  BETA[6]         doodson's fundamental arguments
     *          FNUT[5]         fundamental arguments for nutation
     *
     */
    void DoodsonArguments(const CommonTime& UT1,
                          const CommonTime& TT,
                          double BETA[6], double FNUT[5]);



    /**Provide the diurnal/subdiurnal tidal effects on polar motions ("),
     * UT1 (s) and LOD (s), in time domain.
     *
     * @param   TT              Time in TT
     * @return                  The diurnal/subdiurnal tidal effects on
     *                          polar motions
     *
     */
    Vector<double> PMUT_OCEAN_IERS10(const CommonTime& TT);

    Vector<double> PMUT_OCEAN_Artz12(const CommonTime& TT);

    Vector<double> PMUT_OCEAN_Desai16(const CommonTime& TT);


    /**Provide the diurnal lunisolar effect on polar motion ("), in time
     * domain.
     *
     * @param   TT              Time in TT
     * @return                  The diurnal lunisolar effect on polar
     *                          motions
     *
     */
    Vector<double> PM_LIBR(const CommonTime& TT);


    /**Evaluate the model of subdiurnal libration in the axial component
     * of rotation, expressed by UT1 and LOD.
     *
     * @param   TT              Time in TT
     * @return                  The model of subdiurnal libration in the
     *                          axial component of rotation
     *
     */
    Vector<double> UT_LIBR(const CommonTime& TT);


    /**Compute the diurnal and semidiurnal variations in the earth orientation
     * from the version of Richard Ray's ocean tide model that was listed in
     * IERS Technical Note 21, July 1996.
     * This code includes the variations from 71 diurnal and semidiurnal terms
     * instead of the 8 that are listed in the report.
     *
     * @param   TT              Tim in TT
     * @return                  (delta_x, delta_y, delta_UT1)
     *                          microarcsec for x and y, microsec for UT1
     *
     */
    Vector<double> ortho_eop(const CommonTime& TT);


    /**Compute the time dependent part of the second degree diurnal and
     * semidiurnal tidal potential from the dominant spectral lines in
     * the Cartwright-Tayler-Edden harmonic
     * decomposition.
     *
     * @param   dmjd            Modified Julian Date
     * @return  h[12]           Vector of length 12 with partials of the
     *                          tidal variation with respect to the orthoweights
     */
    void cnmtx(double dmjd, double h[12]);


}  // End of namespace gpstk

#endif // IERS_CONVENTIONS_2010_HPP
