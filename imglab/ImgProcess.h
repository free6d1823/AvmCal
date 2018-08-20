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
static    bool doHomoTransform(
            double s, double t, double &u, double &v, double h[3][3]);
static IMAGE* initImage(int w, int h);
static void freeImage(IMAGE* pImage);
static IMAGE* loadImage();
static IMAGE* loadImageArea(int idArea);
static void ApplyFec(unsigned char* pSrc, int width, int inStride,  int height,
                        unsigned char* pTar, int outStride, FecParam* pFec);

    int updateUv(vector <QVector2D> &uv);
    /*!<S= HT, S=source, T=target on stitched view */
static void findHomoMatreix(dbPOINT s[4], dbPOINT t[4], double hcoef[3][3]);
private:
    int m_nId; /*!<Area ID,0~4, where 0 is front view */
    AreaSettings* m_pAs;

};

#endif // ImgProcess_H
