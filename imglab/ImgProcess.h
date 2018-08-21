#ifndef ImgProcess_H
#define ImgProcess_H
#include "../FecParam.h"
#include <vector>
#include <QVector2D>
using namespace std;

typedef struct _IMAGE{
    int seq;
    unsigned char* buffer;
    int width;
    int height;
    int stride;
}IMAGE;

class ImgProcess
{
public:
    ImgProcess();
    ~ImgProcess();

static    void doFec(double u, double v, double &x, double &y, FecParam* m_pFec);
static void calculateHomoMatrix(dbPOINT* fps, dbPOINT* fpt, HomoParam* homo);

static    bool doHomoTransform(
            double s, double t, double &u, double &v, double h[3][3]);
static IMAGE* initImage(int w, int h);
static void freeImage(IMAGE* pImage);
static IMAGE* loadImage();
static IMAGE* loadImageArea(int idArea, FecParam* pFec);
static void ApplyFec(unsigned char* pSrc, int width, int inStride,  int height,
                        unsigned char* pTar, int outStride, FecParam* pFec);

static  int updateUv(vector <QVector2D> &uv, int xIntv, int yIntv);
    /*!<S= HT, S=source, T=target on stitched view */
static void findHomoMatreix(dbPOINT s[4], dbPOINT t[4], double hcoef[3][3]);
private:
static    void genCameraAreaTable(AreaSettings* as, int* cam, int* ft, int xIntv, int yIntv);

};

class TexProcess
{
public:
    TexProcess();
    ~TexProcess();
    bool init(int xIntv, int yIntv);
    int updateUv(vector <QVector2D> &uv);
    bool saveTable(const char* path);
private:
    void genCameraAreaTable();
    AreaSettings m_as[MAX_CAMERAS];
static   QPointF s_offsetCam[MAX_CAMERAS];
    int*    m_camTable; /*!<uv position index to camera area table*/
    int*    m_fpTable;  /*!<uv position index to feature-points table */
    int m_xIntv;
    int m_yIntv;
}

#endif // ImgProcess_H
