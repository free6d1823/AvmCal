#include "ImgProcess.h"
#include "Mat.h"
#include <QFile>

#define PI_2    (3.1415967*2.0)
#define IMAGE_PATH  ":/camera1800x1440.yuv"
#define IMAGE_WIDTH 1800
#define IMAGE_HEIGHT    1440
#define IMAGE_AREA_WIDTH    900
#define IMAGE_AREA_HEIGHT   720

void YuyvToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb, bool uFirst);


ImgProcess::ImgProcess()
{

}
ImgProcess::~ImgProcess()
{
}

static int seq=0;
IMAGE* ImgProcess::initImage(int w, int h)
{

    IMAGE* img = (IMAGE*)malloc(sizeof(IMAGE) );
    if (!img)
        return img;
    img->seq = ++seq;
    img->buffer = (unsigned char*)malloc( w*4*h);
    img->width = w;
    img->stride = w*4;
    img->height = h;

    printf ("ImgProcess::initImage#%d (%dx%d)\n", img->seq, w, h);

    return img;
}

void ImgProcess::freeImage(IMAGE* pImage)
{
    if(pImage) {
printf("ImgProcess::freeImage,#%d - %dx%d\n", pImage->seq, pImage->width,
       pImage->height);
        if(pImage->buffer) free(pImage->buffer);
        free(pImage);//include freeing pImge->buffer
    }
}

IMAGE* ImgProcess::loadImage()
{
    QFile fp(IMAGE_PATH);
    if(!fp.open(QIODevice::ReadOnly))
            return NULL;

    IMAGE* pImg = initImage(IMAGE_WIDTH, IMAGE_HEIGHT);
    if (!pImg){
       fp.close();
       return pImg;
    }
    do {
        unsigned char* pSrc = (unsigned char*) malloc(IMAGE_WIDTH*2*IMAGE_HEIGHT);
        if (!pSrc)
            break;
        fp.read((char* )pSrc, IMAGE_WIDTH*2*IMAGE_HEIGHT);
        YuyvToRgb32(pSrc, IMAGE_WIDTH, IMAGE_WIDTH*2, IMAGE_HEIGHT, pImg->buffer, true);
        free(pSrc);
    }while(0);
   fp.close();
   return pImg;
}
IMAGE* ImgProcess::loadImageArea(int idArea)
{
    IMAGE* pSrc = loadImage();
    if (!pSrc)
        return NULL;
    IMAGE* pOut = initImage(IMAGE_AREA_WIDTH, IMAGE_AREA_HEIGHT);
    unsigned char* pIn;
    switch (idArea) {
    case 0:
        pIn = pSrc->buffer;
        break;
    case 1:
        pIn = pSrc->buffer+ IMAGE_AREA_WIDTH*4;
        break;
    case 2:
        pIn = pSrc->buffer + IMAGE_WIDTH*4*IMAGE_AREA_HEIGHT;
        break;
    case 3:
    default:
        pIn = pSrc->buffer + IMAGE_WIDTH*4*IMAGE_AREA_HEIGHT+ IMAGE_AREA_WIDTH*4;
        break;
    }
    FecParam fec;
    LoadFecParam(&fec, idArea);
    ApplyFec(pIn, pOut->width, pSrc->stride, pOut->height,
             pOut->buffer, pOut->stride, &fec);
    ImgProcess::freeImage(pSrc);
    return pOut;
}
void ImgProcess::ApplyFec(unsigned char* pSrc, int width, int inStride,  int height,
                        unsigned char* pTar, int outStride, FecParam* pFec)
{
    double x,y,u,v;
    int nX, nY;
    for (int i=0; i< height; i++) {
        v = (double)i/(double)height;
        for (int j=0; j<width; j++) {
            u = (double)j/(double)width;
            ImgProcess::doFec(u,v,x,y, pFec);

            if ( x>=0 && x<1 && y>=0 && y< 1){
                nX = (int) (x * (double) width+0.5);
                nY = (int) (y * (double) height+0.5);

                pTar[i*outStride + j*4  ] = pSrc[nY*inStride+nX*4  ];//B
                pTar[i*outStride + j*4+1] = pSrc[nY*inStride+nX*4+1];//G
                pTar[i*outStride + j*4+2] = pSrc[nY*inStride+nX*4+2];//R
                pTar[i*outStride + j*4+3] = pSrc[nY*inStride+nX*4+3];//A
            }
            else {
                pTar[i*outStride + j*4  ] = 0;
                pTar[i*outStride + j*4+1] = 0;
                pTar[i*outStride + j*4+2] = 0;
                pTar[i*outStride + j*4+3] = 0xff;
            }
        }
    }
}

void ImgProcess::doFec(double u, double v, double &x, double &y, FecParam* m_pFec)
{
    double  u1, v1;
    double x1,y1;
    //transform uu = M x u
    u1 = u-0.5;
    v1 = v-0.5;
    double fr = 2*tan(m_pFec->fov/2); //focus length of rectified image

    double rp = sqrt(u1*u1+v1*v1); //radius of point (u1,v1) on rectified image
    double rq;	//fisheye

    double rq1;

    if(1) //fisheye
        rq1 = atan(rp*fr)/PI_2;		//0~1
    else //normal lens
        rq1 = rp; //if no fec

    //LDC
    if (rp <= 0) {
        x1 = y1 = 0;
    } else {
        u1  = u1 * rq1/rp;
        v1  = v1 * rq1/rp;

        double rq2 = rq1*rq1;
        rq  = (1+m_pFec->k1* rq2+ m_pFec->k2* rq2*rq2);
        x1 = u1/rq;
        //t
        y1 = v1/rq+m_pFec->c*rq2;
    }
    double x2,y2;
    //pitch
    double phi = atan(y1*2);//assume r=0.5
    double sy = 1+tan(m_pFec->pitch)*tan(phi);
    x2 = x1/sy;
    y2 = (y1/sy)/cos(m_pFec->pitch);
    //yaw
    double theda = atan(x2*2);
    double sx = 1+tan(m_pFec->yaw)*tan(theda);
    x2 = (x2/sx)/cos(m_pFec->yaw);
    y2 = y2/sx;

    //spin - z-axis
    x1 = cos(m_pFec->roll)* x2 - sin(m_pFec->roll)*y2;
    y1 = sin(m_pFec->roll)* x2 + cos(m_pFec->roll)*y2;
    //intrinsic parameters calibration
    //t 	x1 = param.a*x1 + param.c*y1;		//x-scale and de-skewness
    x1 = m_pFec->a*x1;		//x-scale and de-skewness
    y1 = m_pFec->b*y1;								//y- scale
    x = (x1+ m_pFec->ptCenter.x);
    y = (y1+ m_pFec->ptCenter.y);

}

int ImgProcess::updateUv(vector <QVector2D> &uv)
{
    return 0;
}
void ImgProcess::findHomoMatreix(dbPOINT s[4], dbPOINT t[4], double hcoef[3][3])
{
    int i;
    //normalize source FP
    //Ah=b
    Mat A(8,8);
    Mat b(1,8);
    int j=4;
    for (i=0;i<4;i++) {
        A.Set(0,i, t[i].x); A.Set(1, i, t[i].y); A.Set(2,i,1); A.Set(3,i, 0); A.Set(4,i, 0); A.Set(5,i, 0);
        A.Set(6,i, -t[i].x*s[i].x); A.Set(7, i, -t[i].y*s[i].x);
        b.Set(0,i, s[i].x);

        A.Set(0,j, 0); A.Set(1,j, 0); A.Set(2,j, 0);
        A.Set(3,j, t[i].x); A.Set(4, j, t[i].y); A.Set(5,j,1);
        A.Set(6,j, -t[i].x*s[i].y); A.Set(7, j, -t[i].y*s[i].y);
        b.Set(0,j, s[i].y);
        j++;
    }
    Mat h (1,8);
    Mat AI(8,8);
    if(A.FindInverse(AI)) {
            h.Multiply(AI, b);
            for(int i=0;i<8;i++)
                hcoef[i/3][i%3] = 	h.Get(0,i);
            hcoef[2][2] = 1;
//verify
#if 0
    printf("derived Matrix A-1=\n");
    AI.Print();
    printf("Find Vector h=\n");
    h.Print();

Mat c(1,8);
c.Multiply(A, h);
printf("Verify A.h = b'\n");
c.Print();
#endif
    }else {
            for(int i=0;i<8;i++)
                hcoef[i/3][i%3] = -1;
    }
}
bool ImgProcess::doHomoTransform(double s, double t, double &u, double &v, double h[3][3])
{
    u = h[0][0] * s +h[0][1]*t + h[0][2];
    v = h[1][0] * s + h[1][1]*t + h[1][2];
    double scale = h[2][0] * s + h[2][1]*t + 1;
    if (scale != 0) {
            u = u/scale;
            v = v/scale;
            if(u>=1 || v>=1 ||u<0 ||v <0) return false;
            return true;
    }
    return false;
}
