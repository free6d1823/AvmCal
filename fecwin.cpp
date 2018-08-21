#include "fecwin.h"
#include "ui_fecwin.h"
#include <QFile>
#include <QResizeEvent>
#include "./imglab/ImgProcess.h"

#define PI      3.1415967
#define PI_2    (3.1415967*2.0)
#define RADIAN_TO_DEGREE(r) (r*180.0/PI)
#define DEGREE_TO_RADIAN(d) (d*PI/180.0)

void YuyvToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb, bool uFirst);
ImgProcess gImgEngine;
FecWin* gFecWin = NULL;
FecWin::FecWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FecWin)
{
    ui->setupUi(this);
    connect(ui->btnPreview, SIGNAL(clicked()), SLOT(onPreviewClicked()));
    connect(ui->spinFov, SIGNAL(valueChanged(double)), SLOT(onFovValueChanged(double)));
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
    ui->radioCam0->setChecked(true);
    connect(ui->radioCam0, SIGNAL(toggled(bool)), SLOT(onArea0Changed(bool)));
    connect(ui->radioCam1, SIGNAL(toggled(bool)), SLOT(onArea1Changed(bool)));
    connect(ui->radioCam2, SIGNAL(toggled(bool)), SLOT(onArea2Changed(bool)));
    connect(ui->radioCam3, SIGNAL(toggled(bool)), SLOT(onArea3Changed(bool)));

    memset(&m_as, 0, sizeof(AreaSettings));
    gFecWin  = this;

    m_pFecParam = &m_as.fec;

    m_pImgView  = new ImgView(this);
    m_curAreaId = 0;
    m_bShowRuler = false;

    ui->layoutFec->addWidget(m_pImgView);
    m_pHomoWin = new HomoWin(this);
    m_pFecToolDlg = new FecToolDlg(this);

    onAreaIdChanged(m_curAreaId);
    onShowHomoChanged(ui->checkShowHomo->isChecked());
}

void FecWin::CalculateHomoMatrix()
{
    ImgProcess::calculateHomoMatrix(m_as.fps, m_as.fpt, m_as.homo);
}

/*!<Feature points settings are modified, update ImgView and do homograph transform*/
void FecWin::UpdateFeaturePoints(bool fromImgView)
{
    if(fromImgView){
        m_pImgView->saveFps();
        CalculateHomoMatrix();
    }else {
        m_pImgView->loadFps();
    }

    //
    m_pHomoWin->UpdateUI();
    //now update HomoWin
    m_pHomoWin->setSourceImage(m_pImgView->getImage(), m_curAreaId);

}
/*!<Save AreaSettings to ini*/
void FecWin::SaveFeaturePoints()
{
    SaveAreaSettings(&m_as, m_curAreaId);
}

bool FecWin::LoadImage(int nID)
{
    IMAGE* pImg = ImgProcess::loadImageArea(nID, m_pFecParam);
    if (!pImg)
        return false;
    m_pImgView->setImage(pImg);
    m_pHomoWin->setSourceImage(pImg, nID);
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
            gImgEngine.doFec(u,v,x,y, m_pFecParam);

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
    if(m_pHomoWin)
        m_pHomoWin->close();
    if(m_pFecToolDlg)
        m_pFecToolDlg->close();
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
void FecWin::onArea0Changed(bool value){if(value)onAreaIdChanged(0);}
void FecWin::onArea1Changed(bool value){if(value)onAreaIdChanged(1);}
void FecWin::onArea2Changed(bool value){if(value)onAreaIdChanged(2);}
void FecWin::onArea3Changed(bool value){if(value)onAreaIdChanged(3);}

void FecWin::onAreaIdChanged(int value)
{
    m_curAreaId = value;
    //update parameters
    LoadAreaSettings(&m_as, value);
   //update homo coefficents
    CalculateHomoMatrix();
    //inform HowView to change AreaSettings
    m_pHomoWin->setAreaSettings(&m_as);
    m_pFecToolDlg->UpdateUi();

    //update m_pFecParam to UI
    UpdateUI();
    //update image
    LoadImage(value);
    m_pImgView->loadFps();
}
void FecWin::onShowRulerChanged(int value)
{
    if (m_pImgView) m_pImgView->showRuler(value > 0);
}
void FecWin::onShowHomoChanged(int value)
{
    m_pImgView->showFp(value !=0);
    if (value > 0){

        m_pHomoWin->show();
        m_pFecToolDlg->show();
    }
    else {
        m_pHomoWin->hide();
        m_pFecToolDlg->hide();
    }
}

void FecWin::SetFecParam(FecParam* pParam)
{
    memcpy(m_pFecParam, pParam, sizeof(FecParam));
}

void FecWin::onFovValueChanged(double value)
{
    //degree to gradian
    m_pFecParam->fov = DEGREE_TO_RADIAN(value);
    //update image
    LoadImage(m_curAreaId);

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

    SaveAreaSettings(&m_as, m_curAreaId);
    UpdateUI();
    LoadImage(m_curAreaId);
}
void FecWin::onPreviewClicked()
{
    LoadImage(m_curAreaId);
}
