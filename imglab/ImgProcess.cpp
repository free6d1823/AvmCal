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

#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)


// YUV -> RGB
#define C(Y) ( (Y) - 16  )
#define D(U) ( (U) - 128 )
#define E(V) ( (V) - 128 )

#define YUV2R(Y, U, V) CLIP(( 298 * C(Y)              + 409 * E(V) + 128) >> 8)
#define YUV2G(Y, U, V) CLIP(( 298 * C(Y) - 100 * D(U) - 208 * E(V) + 128) >> 8)
#define YUV2B(Y, U, V) CLIP(( 298 * C(Y) + 516 * D(U)              + 128) >> 8)

//////
/// \brief ImgProcess::YuyvToRgb32 YUV420 to RGBA 32
/// \param pYuv
/// \param width
/// \param stride
/// \param height
/// \param pRgb     output RGB32 buffer
/// \param uFirst   true if pYuv is YUYV, false if YVYU
///
void ImgProcess::YuyvToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb, bool uFirst)
{
    //YVYU - format
    int nBps = width*4;
    unsigned char* pY1 = pYuv;

    unsigned char* pV;
    unsigned char* pU;

    if (uFirst) {
        pU = pY1+1; pV = pU+2;
    } else {
        pV = pY1+1; pU = pV+2;
    }


    unsigned char* pLine1 = pRgb;

    unsigned char y1,u,v;
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j+=2)
        {
            y1 = pY1[2*j];
            u = pU[2*j];
            v = pV[2*j];
            pLine1[j*4] = YUV2B(y1, u, v);//b
            pLine1[j*4+1] = YUV2G(y1, u, v);//g
            pLine1[j*4+2] = YUV2R(y1, u, v);//r
            pLine1[j*4+3] = 0xff;

            y1 = pY1[2*j+2];
            pLine1[j*4+4] = YUV2B(y1, u, v);//b
            pLine1[j*4+5] = YUV2G(y1, u, v);//g
            pLine1[j*4+6] = YUV2R(y1, u, v);//r
            pLine1[j*4+7] = 0xff;
        }
        pY1 += stride;
        pV += stride;
        pU += stride;
        pLine1 += nBps;

    }
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
    img->bRef = false;
    printf ("ImgProcess::initImage#%d (%dx%d)\n", img->seq, w, h);
    return img;
}
IMAGE* ImgProcess::refImage(unsigned char* data, int w, int h)
{
    IMAGE* img = (IMAGE*)malloc(sizeof(IMAGE) );
    if (!img)
        return img;
    img->seq = ++seq;
    img->buffer = data;
    img->width = w;
    img->stride = w*4;
    img->height = h;
    img->bRef = true;
    printf ("ImgProcess::initImage#%d (%dx%d)\n", img->seq, w, h);
    return img;
}

void ImgProcess::freeImage(IMAGE* pImage)
{
    if(pImage) {
printf("ImgProcess::freeImage,#%d - %dx%d\n", pImage->seq, pImage->width,
       pImage->height);
        if(!pImage->bRef && pImage->buffer) free(pImage->buffer);
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
IMAGE* ImgProcess::loadImageArea(int idArea, FecParam* pFec)
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

    ApplyFec(pIn, pOut->width, pSrc->stride, pOut->height,
             pOut->buffer, pOut->stride, pFec);
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

#define IsInRect(x,y, rc) (x>=rc.l && x<rc.r && y>= rc.t && y< rc.b)
void ImgProcess::genCameraAreaTable(AreaSettings* as, int* cam, int* ft, int xIntv, int yIntv)
{
    int offset;
    int i,j, k,m;
    double s,t;
    for (i=0; i<= yIntv; i++) {
        t = ((double)i)/(double)yIntv;
        for (j=0; j<=xIntv; j++) {
            s = ((double)j)/(double)xIntv;
            offset = i*(xIntv+1)+j;
            for (m=0; m<MAX_CAMERAS; m++){
                if(IsInRect(s,t, as[m].range)){
                    for (k=0; k<MAX_FP_AREA; k++)
                    {
                        if (IsInRect(s,t, as[m].region[k]))
                            break;
                    }
                    break;
                }
            }
            if (m != MAX_CAMERAS && k != MAX_FP_AREA){
                if(m==0&& k==0) {m=3;k=3;}
                else if (m==1&& k==0) {m=0;k=3;}
                else if (m==2&& k==0) {m=1;k=3;}
                else if (m==3&& k==0) {m=2;k=3;}

                cam[offset]=m;
                ft[offset] = k;
            } else {//in center?
                cam[offset]=0;
                ft[offset] = 0;
            }
        }
    }
}

int ImgProcess::updateUv(vector <QVector2D> &uv, int xIntv, int yIntv)
{

    uv.clear();
#ifdef NO_FEC
    int i, j;
    double s,t;

    for (i=0; i<= yIntv; i++) {
        t = ((double)i)/(double)yIntv;
        for (j=xIntv; j>=0; j--) {
            s = ((double)j)/(double)xIntv;
            uv.push_back(QVector2D(s, t));
        }
    }
#else
    int i,j, m,k;
    AreaSettings as[MAX_CAMERAS];
    for (m=0; m< MAX_CAMERAS; m++) {
       LoadAreaSettings(&as[m], m);
       //
       calculateHomoMatrix(as[m].fps, as[m].fpt, as[m].homo);
    }
#define LOOKUP_TABLE(p, x,y,w)   (p[x+y*w])
    //make camera look-up table
    int* camera_table = (int*) malloc((xIntv+1)*(yIntv+1)*sizeof(int));
    int* fp_table = (int*) malloc((xIntv+1)*(yIntv+1)*sizeof(int));
    genCameraAreaTable(as, camera_table, fp_table, xIntv, yIntv);
     double s,t,u,v;
    double x=0.0,y=0.0;
    QPointF offset[4]={QPointF(0,0), QPointF(0.5, 0),
            QPointF(0, 0.5), QPointF(0.5, 0.5)};


    for (i=0; i<= yIntv; i++) {
        t = ((double)i)/(double)yIntv;
        for (j=0; j<=xIntv; j++) {
            s = 1-((double)j)/(double)xIntv;
            m = LOOKUP_TABLE(camera_table,
                             (xIntv-j),i, (xIntv+1));
            k = LOOKUP_TABLE(fp_table, (xIntv-j),i, (xIntv+1));

            if (ImgProcess::doHomoTransform(s,t,u,v, as[m].homo[k].h)) {
                ImgProcess::doFec(u,v,x,y, &as[m].fec);
                x = x*0.5+ offset[m].x();
                y = y*0.5+ offset[m].y();

                //
            }else{
                x=y=0.5;
            }
            //
            uv.push_back(QVector2D(x, y));
        }
    }
    free(camera_table);
    free(fp_table);
#endif
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
void ImgProcess::calculateHomoMatrix(dbPOINT* fps, dbPOINT* fpt, HomoParam* homo)
{
    dbPOINT s[MAX_FP_AREA][4] = {{fps[0], fps[1], fps[6], fps[5]},
                     {fps[1], fps[2], fps[7], fps[6]},
                     {fps[2], fps[3], fps[8], fps[7]},
                     {fps[3], fps[4], fps[9], fps[8]}};
    dbPOINT t[MAX_FP_AREA][4] = {{fpt[0], fpt[1], fpt[6], fpt[5]},
                     {fpt[1], fpt[2], fpt[7], fpt[6]},
                     {fpt[2], fpt[3], fpt[8], fpt[7]},
                     {fpt[3], fpt[4], fpt[9], fpt[8]}};

    for(int i=0; i<MAX_FP_AREA; i++) {

        ImgProcess::findHomoMatreix(s[i],t[i], homo[i].h);
    }

}
/////////////////////////////////////
/// \brief s_offsetCam each camera position in video frame
QPointF TexProcess::s_offsetCam[MAX_CAMERAS]={QPointF(0,0), QPointF(0.5, 0),
                          QPointF(0, 0.5), QPointF(0.5, 0.5)};

TexProcess::TexProcess()

{
    m_xIntv = 0;
    m_yIntv = 0;
    m_camTable = NULL;
    m_fpTable  = NULL;

}
/////////////
/// \brief TexProcess::init call init if AreaSettings has changed
/// \param xIntv horizontal intervals numbers
/// \param yIntv vertical interval numbers
/// \return false if any error
///
bool TexProcess::init(int xIntv, int yIntv)
{
    if(xIntv!= m_xIntv || yIntv != m_yIntv ){
        if(m_camTable) free(m_camTable);
        m_camTable = (int*) malloc((xIntv+1)*(yIntv+1)*sizeof(int));
        if(m_fpTable) free(m_fpTable);
        m_fpTable  = (int*) malloc((xIntv+1)*(yIntv+1)*sizeof(int));
    }
    for (int m=0; m< MAX_CAMERAS; m++) {
       LoadAreaSettings(&m_as[m], m);
       ImgProcess::calculateHomoMatrix(m_as[m].fps, m_as[m].fpt, m_as[m].homo);
    }

    return true;
}

TexProcess::~TexProcess()
{
    if(m_camTable)
        free(m_camTable);
    if(m_fpTable)
        free(m_fpTable);
}

int TexProcess::updateUv(vector <QVector2D> &uv)
{
    uv.clear();
#ifdef NO_FEC
    int i, j;
    double s,t;

    for (i=0; i<= yIntv; i++) {
        t = ((double)i)/(double)yIntv;
        for (j=xIntv; j>=0; j--) {
            s = ((double)j)/(double)xIntv;
            uv.push_back(QVector2D(s, t));
        }
    }
#else
    int i,j, m,k;
#define LOOKUP_TABLE(p, x,y,w)   (p[x+y*w])
    double s,t,u,v;
    double x=0.0,y=0.0;

    for (i=0; i<= m_yIntv; i++) {
        t = ((double)i)/(double)m_yIntv;
        for (j=0; j<=m_xIntv; j++) {
            s = 1-((double)j)/(double)m_xIntv;
            m = LOOKUP_TABLE(m_camTable,
                             (m_xIntv-j),i, (m_xIntv+1));
            k = LOOKUP_TABLE(m_fpTable, (m_xIntv-j),i, (m_xIntv+1));

            if (ImgProcess::doHomoTransform(s,t,u,v, m_as[m].homo[k].h)) {
                ImgProcess::doFec(u,v,x,y, &m_as[m].fec);
                x = x*0.5+ s_offsetCam[m].x();
                y = y*0.5+ s_offsetCam[m].y();

                //
            }else{
                x=y=0.5;
            }
            //
            uv.push_back(QVector2D(x, y));
        }
    }
#endif
    return 0;
}
//////////
/// \brief TexProcess::genCameraAreaTable
///
void TexProcess::genCameraAreaTable()
{
    int offset;
    int i,j, k,m;
    double s,t;
    for (i=0; i<= m_yIntv; i++) {
        t = ((double)i)/(double)m_yIntv;
        for (j=0; j<=m_xIntv; j++) {
            s = ((double)j)/(double)m_xIntv;
            offset = i*(m_xIntv+1)+j;
            for (m=0; m<MAX_CAMERAS; m++){
                if(IsInRect(s,t, m_as[m].range)){
                    for (k=0; k<MAX_FP_AREA; k++)
                    {
                        if (IsInRect(s,t, m_as[m].region[k]))
                            break;
                    }
                    break;
                }
            }
            if (m != MAX_CAMERAS && k != MAX_FP_AREA){
                if(m==0&& k==0) {m=3;k=3;}
                else if (m==1&& k==0) {m=0;k=3;}
                else if (m==2&& k==0) {m=1;k=3;}
                else if (m==3&& k==0) {m=2;k=3;}

                m_camTable[offset]=m;
                m_fpTable[offset] = k;
            } else {//in center?
                m_camTable[offset]=0;
                m_fpTable[offset] = 0;
            }
        }
    }
}
////////////////
/// \brief TexProcess::saveTable save uv location to table
/// \param path
/// \return
///
bool TexProcess::saveTable(const char* path)
{
    return true;
}
