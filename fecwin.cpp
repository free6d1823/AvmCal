#include "fecwin.h"
#include "ui_fecwin.h"
#include <QFile>

#define PI      3.1415967
#define PI_2    (3.1415967*2.0)
#define RADIAN_TO_DEGREE(r) (r*180.0/PI)
#define DEGREE_TO_RADIAN(d) (d*PI/180.0)

void YuyvToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb, bool uFirst);


FecWin::FecWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FecWin)
{
    ui->setupUi(this);
    connect(ui->btnPreview, SIGNAL(clicked()), SLOT(onPreviewClicked()));
    connect(ui->spinFov, SIGNAL(valueChanged(double)), SLOT(onFovValueChanged(double)));
    connect(ui->spinAreaId, SIGNAL(valueChanged(int)), SLOT(onAreaIdChanged(int)));
    connect(ui->checkShowRuler, SIGNAL(stateChanged(int)), SLOT(onShowRulerChanged(int)));
    connect(ui->spina, SIGNAL(valueChanged(double)), SLOT(onIntricAChanged(double)));
    connect(ui->spinb, SIGNAL(valueChanged(double)), SLOT(onIntricBChanged(double)));
    connect(ui->spinc, SIGNAL(valueChanged(double)), SLOT(onIntricCChanged(double)));
    connect(ui->spinK1, SIGNAL(valueChanged(double)), SLOT(onK1ValueChanged(double)));
    connect(ui->spinK2, SIGNAL(valueChanged(double)), SLOT(onK2ValueChanged(double)));
    connect(ui->spinCenterX, SIGNAL(valueChanged(double)), SLOT(onCenterXChanged(double)));
    connect(ui->spinCenterY, SIGNAL(valueChanged(double)), SLOT(onCenterYChanged(double)));
    connect(ui->spinPitch, SIGNAL(valueChanged(double)), SLOT(onPitchChanged(double)));
    connect(ui->spinYaw, SIGNAL(valueChanged(double)), SLOT(onYawChanged(double)));
    connect(ui->spinRoll, SIGNAL(valueChanged(double)), SLOT(onRollChanged(double)));

    connect(ui->checkShowHomo, SIGNAL(stateChanged(int)), SLOT(onShowHomoChanged(int)));

    memset(&m_as, 0, sizeof(AreaSettings));
    m_pFecParam = &m_as.fec;

    m_pImgView  = new ImgView(this);
    m_curAreaId = 0;
    m_bShowRuler = false;
    onAreaIdChanged(m_curAreaId);

    ui->layoutFec->addWidget(m_pImgView);

    m_pHomoWin = new HomoWin(&m_as);
    onShowHomoChanged(ui->checkShowHomo->isChecked());



}
bool FecWin::LoadImage(const char* path, int nID)
{
    QFile fp(path);
    if(!fp.open(QIODevice::ReadOnly))
            return false;
    int inWidth = 1800;
    int inStride = inWidth*2;
    int inHeight = 1440;
    int outWidth = 900;
    int outHeight = 720;
    int outStride = outWidth*4;

    unsigned char* pSrc = (unsigned char*) malloc(inStride*inHeight);
    unsigned char* pRgb = (unsigned char*) malloc(inWidth*4*inHeight);
    unsigned char* pOut = (unsigned char*) malloc(outStride*outHeight);

    fp.read((char* )pSrc, inStride*inHeight);
    fp.close();
    //convert YUV to RGB32
    YuyvToRgb32(pSrc, inWidth, inStride, inHeight, pRgb, true);
    inStride = inWidth*4;

    unsigned char* pIn;
    switch (nID) {
    case 0:
        pIn = pRgb;
        break;
    case 1:
        pIn = pRgb+ outStride;
        break;
    case 2:
        pIn = pRgb + inStride*outHeight;
        break;
    case 3:
    default:
        pIn = pRgb + inStride*outHeight+ outStride;
        break;
    }
    //apply algorithm
    ApplyFec(pIn, outWidth, inStride,  outHeight, pOut, outStride);
    m_pImgView->setImageRGB32(pOut, outWidth, outStride,  outHeight);
    free(pSrc);
    free(pOut);
    free(pRgb);
    return true;
}

//RGB32
void FecWin::ApplyFec(unsigned char* pSrc, int width, int inStride,  int height,
                        unsigned char* pTar, int outStride)
{
    double x,y,u,v;
    int nX, nY;
    for (int i=0; i< height; i++) {
        v = (double)i/(double)height;
        for (int j=0; j<width; j++) {
            u = (double)j/(double)width;
            PosMapNormalize(u,v,x,y);
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

FecWin::~FecWin()
{
    delete ui;
    if(m_pImgView){

        delete m_pImgView;
        m_pImgView = NULL;
    }

}
void FecWin::UpdateUI()
{
    ui->spinFov->setValue(RADIAN_TO_DEGREE(m_pFecParam->fov));
    ui->spina->setValue(m_pFecParam->a);
    ui->spinb->setValue(m_pFecParam->b);
    ui->spinc->setValue(m_pFecParam->c);
    ui->spinK1->setValue(m_pFecParam->k1);
    ui->spinK2->setValue(m_pFecParam->k2);
    ui->spinPitch->setValue(RADIAN_TO_DEGREE(m_pFecParam->pitch));
    ui->spinYaw->setValue(RADIAN_TO_DEGREE(m_pFecParam->yaw));
    ui->spinRoll->setValue(RADIAN_TO_DEGREE(m_pFecParam->roll));

}

void FecWin::onAreaIdChanged(int value)
{
    m_curAreaId = value;
    //update parameters
    LoadAreaSettings(&m_as, value);
    //update m_pFecParam to UI
    UpdateUI();
    //update image
    LoadImage(":/camera1800x1440.yuv", value);


}
void FecWin::onShowRulerChanged(int value)
{
    if (m_pImgView) m_pImgView->showRuler(value > 0);
}
void FecWin::onShowHomoChanged(int value)
{
    if (value > 0)
        m_pHomoWin->show();
    else
        m_pHomoWin->hide();
}

void FecWin::SetFecParam(FecParam* pParam)
{
    memcpy(m_pFecParam, pParam, sizeof(FecParam));
}


void FecWin::PosMapNormalize(double u, double v, double &x, double &y)
{
    double  u1, v1;
    double x1,y1;
    //transform uu = M x u
    u1 = u-0.5;
    v1 = v-0.5;
    double fr = 2*tan(m_pFecParam->fov/2); //focus length of rectified image

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
        rq  = (1+m_pFecParam->k1* rq2+ m_pFecParam->k2* rq2*rq2);
        x1 = u1/rq;
        //t
        y1 = v1/rq+m_pFecParam->c*rq2;
    }
    double x2,y2;
    //pitch
    double phi = atan(y1*2);//assume r=0.5
    double sy = 1+tan(m_pFecParam->pitch)*tan(phi);
    x2 = x1/sy;
    y2 = (y1/sy)/cos(m_pFecParam->pitch);
    //yaw
    double theda = atan(x2*2);
    double sx = 1+tan(m_pFecParam->yaw)*tan(theda);
    x2 = (x2/sx)/cos(m_pFecParam->yaw);
    y2 = y2/sx;

    //spin - z-axis
    x1 = cos(m_pFecParam->roll)* x2 - sin(m_pFecParam->roll)*y2;
    y1 = sin(m_pFecParam->roll)* x2 + cos(m_pFecParam->roll)*y2;
    //intrinsic parameters calibration
    //t 	x1 = param.a*x1 + param.c*y1;		//x-scale and de-skewness
    x1 = m_pFecParam->a*x1;		//x-scale and de-skewness
    y1 = m_pFecParam->b*y1;								//y- scale
    x = (x1+ m_pFecParam->ptCenter.x);
    y = (y1+ m_pFecParam->ptCenter.y);

}

void FecWin::onFovValueChanged(double value)
{
    //degree to gradian
    m_pFecParam->fov = DEGREE_TO_RADIAN(value);
    //update image
    LoadImage(":/camera1800x1440.yuv", m_curAreaId);

}
void FecWin::onIntricAChanged(double value){    m_pFecParam->a = value;}
void FecWin::onIntricBChanged(double value){    m_pFecParam->b = value;}
void FecWin::onIntricCChanged(double value){    m_pFecParam->c = value;}
void FecWin::onK1ValueChanged(double value){    m_pFecParam->k1 = value;}
void FecWin::onK2ValueChanged(double value){    m_pFecParam->k2 = value;}
void FecWin::onCenterXChanged(double value){    m_pFecParam->ptCenter.x = value;}
void FecWin::onCenterYChanged(double value){    m_pFecParam->ptCenter.y = value;}
void FecWin::onPitchChanged(double value){    m_pFecParam->pitch = DEGREE_TO_RADIAN(value);}
void FecWin::onYawChanged(double value){    m_pFecParam->yaw = DEGREE_TO_RADIAN(value);}
void FecWin::onRollChanged(double value){    m_pFecParam->roll = DEGREE_TO_RADIAN(value);}

void FecWin::accept()
{

    SaveFecParam(m_pFecParam, m_curAreaId);
    UpdateUI();
    LoadImage(":/camera1800x1440.yuv", m_curAreaId);
}
void FecWin::onPreviewClicked()
{
    LoadImage(":/camera1800x1440.yuv", m_curAreaId);
}
