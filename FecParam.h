#ifndef FECPARAM_H
#define FECPARAM_H

/**
 * \struct _FecParam
 * \brief FEC processing parameters.
 */

/*!<Maximum cameras used in AVM*/
#define MAX_CAMERAS  4
/*!<feature points area, at least one camera use one FP_AREA */
#define MAX_FP_AREA	4
/*!<numbersof FP per area*/
#define FP_COUNTS			10


typedef struct _dbSIZE {
    double cx;
    double cy;
}dbSIZE;
typedef struct _dbPOINT {
    double x;
    double y;
}dbPOINT;
typedef struct _dbRECT {
    double l;
    double t; //top
    double r;
    double b; //bottom
}dbRECT;

/*!<Fisheye Correction parametters*/
typedef struct _FecParam{
    dbPOINT ptCenter;		/*!<symmetry center of image relative to top-left of rcInput, in pixels*/
    double	pitch;			/*!<pitch angle in radiun */
    double	yaw;			/*!<rotate with respect to y-axis */
    double	roll;			/*!<rotate with respect to z-axis */
    double	fov;			/*!<hoizontal FOV factor, 0 to PI*/
    double	k1;             /*!<radical coefficient 1 */
    double	k2;             /*!<radical coefficient 2 */
    double a;				/*!<intrinsic arameters, x- scale factor */
    double b;				/*!<intrinsic arameters, y- scale factor */
    double c; 				/*!<intrinsic arameters, skew  */
}FecParam;

/*!<Homograph Transformation parametters*/
typedef struct _HomoParam{
    int fp_index[4];    /*!<feature points indices of FP array, in the homo region*/
    double h[3][3];     /*!<Homography Matrix coef. of the region */
 }HomoParam;


typedef struct _AreaSettings {
    dbRECT  range;                  /*!<the coordinates on final normalized image, 1.0x1.0 of this area  */
    FecParam fec;                   /*!<FEC applied to all area */
    dbPOINT fpt[FP_COUNTS];         /*!<feature points at final image*/
    dbPOINT fps[FP_COUNTS];         /*!<feature points at rectified image*/
    dbRECT	region[MAX_FP_AREA];      /*!<the normalized coordinates of homo_region on final image*/
    HomoParam homo[MAX_FP_AREA];    /*!<homo apply to selected region */
}AreaSettings;

bool    LoadFecParam(FecParam* pFecParam, int nArea);
bool    LoadAreaSettings(AreaSettings* pParam, int nArea);
bool    LoadHomoParam(HomoParam* pParam, int nArea, int nFp);
bool    SaveFecParam(FecParam* pFecParam, int nArea);
bool    SaveHomoParam(HomoParam* pParam, int nArea, int nFp);

bool    SaveAreaSettings(AreaSettings* pParam, int nArea);

#endif // FECPARAM_H
