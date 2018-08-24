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
    bool bRef; /*!<true: buffer is referenced, don't free */

}IMAGE;

class ImgProcess
{
public:
    ImgProcess();
    ~ImgProcess();
static void updateImageData(IMAGE* pImg, unsigned char* data, int w, int h);
static void YuyvToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb, bool uFirst);
static    void doFec(double u, double v, double &x, double &y, FecParam* m_pFec);
static void calculateHomoMatrix(dbPOINT* fps, dbPOINT* fpt, HomoParam* homo);

static    bool doHomoTransform(
            double s, double t, double &u, double &v, double h[3][3]);
static IMAGE* initImage(int w, int h);
static IMAGE* refImage(unsigned char* data, int w, int h);
static void freeImage(IMAGE* pImage);
static void ApplyFec(unsigned char* pSrc, int width, int inStride,  int height,
                        unsigned char* pTar, int outStride, FecParam* pFec);

    /*!<S= HT, S=source, T=target on stitched view */
static void findHomoMatreix(dbPOINT s[4], dbPOINT t[4], double hcoef[3][3]);


};
class ImgSource
{
public:
    ImgSource();
    ~ImgSource();
    bool setImageFileName(const char* file, int width, int height);
    bool setImageFromRgb32(void* data, int width, int height);
    IMAGE* loadImage();
    IMAGE* loadImageArea(int idArea, FecParam* pFec);
private:
    unsigned char* m_pRgb32;
    int  IMAGE_WIDTH;
    int  IMAGE_HEIGHT;
    int  IMAGE_AREA_WIDTH;
    int  IMAGE_AREA_HEIGHT;

};

class TexProcess
{
public:
    TexProcess();
    ~TexProcess();
    bool init();
    int createVertices(vector<QVector3D> & vert, vector<unsigned short>& indices);
    int updateUv(vector <QVector2D> &uv);
    int reloadIndices(vector<unsigned short>& indices);

private:
    void initVertices(vector<QVector3D> & vert, dbRECT region);
    void updateIndices(vector<unsigned short>& indices, int nCam, int nRegion);
    AreaSettings m_as[MAX_CAMERAS];
static   QPointF s_offsetCam[MAX_CAMERAS];
    ///
    /// \brief m_RegionMap indicates the region should be shown or hiden
    ///        0 for hidden
    int m_RegionMap[MAX_CAMERAS][MAX_FP_AREA];
};

#endif // ImgProcess_H
