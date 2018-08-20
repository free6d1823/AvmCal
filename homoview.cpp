#include "homoview.h"
#include <QPainter>
#include "fecwin.h"
#include "./imglab/ImgProcess.h"

#define OUT_WIDTH   1000
#define OUT_HEIGHT  1000
#define OUT_STRIDE  (1000*4)

HomoView::HomoView(HomoWin *parent) : QWidget((QWidget*)parent)
{
    m_pOwner = parent;
    m_bDrawRuler = true;
    for (int i=0; i< FP_COUNTS; i++) {
        m_fp[i] = QPointF(0,0);
    }
    m_pImg = ImgProcess::initImage(OUT_WIDTH, OUT_HEIGHT);
    m_pRef = NULL;
}
HomoView::~HomoView()
{
    if (m_pImg) {
        ImgProcess::freeImage(m_pImg);
        m_pImg = NULL;
    }
}

void HomoView::HomoTransform()
{

}
void HomoView::setImage(IMAGE* pImg, int nAreaID)
{
    m_pRef = pImg;
    m_nAreaId = nAreaID;
    //
    unsigned char* pSrc= m_pRef->buffer;
    unsigned char* pTar= m_pImg->buffer;
    HomoParam* homo = gFecWin->getAreaSettings()->homo;
    dbRECT* pRegion = gFecWin->getAreaSettings()->region;

    dbRECT region;
    for(int k=0; k<MAX_FP_AREA; k++){
        region.l = pRegion[k].l * OUT_WIDTH;
        region.r = pRegion[k].r * OUT_WIDTH;
        region.t = pRegion[k].t * OUT_HEIGHT;
        region.b = pRegion[k].b * OUT_HEIGHT;

        for(int i=region.t;i<region.b; i++) {
            double t = (double)i/(double)OUT_HEIGHT;
            for(int j=region.l; j<region.r; j++) {
                double s = (double)j/(double)OUT_WIDTH; //normalized coordingnates
                double u,v;
                if (ImgProcess::doHomoTransform(s,t,u,v, homo[k].h)) {
                    int x = (int)(u * (double) m_pRef->width);
                    int y = (int)(v * (double) m_pRef->height);
                    int offset = y*m_pRef->stride + x*4;

                    pTar[i*OUT_STRIDE+j*4]= pSrc[offset];
                    pTar[i*OUT_STRIDE+j*4+1]= pSrc[offset+1];
                    pTar[i*OUT_STRIDE+j*4+2]= pSrc[offset+2];
                    pTar[i*OUT_STRIDE+j*4+3]= pSrc[offset+3];


                } else {
                    pTar[i*OUT_STRIDE+j*4]=pTar[i*OUT_STRIDE+j*4+1]=pTar[i*OUT_STRIDE+j*4+2] = 0;
                    pTar[i*OUT_STRIDE+j*4+3]=0xff;
                }
            }
        }

    }
    update();
}

void HomoView::drawAl()
{
    AreaSettings as;
   for (int k=0; k< MAX_CAMERAS; k++) {
       memset(&as, 0, sizeof(as));
       LoadAreaSettings(&as, k);
       //we assume h coeffs are updated
       //load whole image
       ImgProcess::loadImage();
   }

    update();
}

void HomoView::paintEvent(QPaintEvent */*event*/)
{
    QPainter painter(this);
    QRect rcTarget = rect();
    QRect rcSrc = QRect(0,0,OUT_WIDTH, OUT_HEIGHT);
    if(m_pImg){
        QImage image(
                m_pImg->buffer,
                m_pImg->width,
                m_pImg->height,
                m_pImg->stride,
                QImage::Format_RGBA8888);
        painter.drawImage(rcTarget, image, rcSrc);

    } else {
        painter.fillRect(rcTarget, Qt::yellow);

    }
    if(m_bDrawRuler) {
        QPen pen1(Qt::blue);
        pen1.setWidth(1);
        painter.setPen(pen1);
        for(int j=0; j<4; j++ ){
            painter.drawRect(m_region[j]);
        }
        pen1.setWidth(3);
        pen1.setColor(Qt::red);
        painter.setPen(pen1);
        for (unsigned int i=0; i<sizeof(m_fp)/sizeof(m_fp[0]); i++) {
            painter.drawPoint(m_fp[i]);
        }

    }
}

void HomoView::resizeEvent(QResizeEvent */*event*/)
{
    loadFeaturePoints();
}

void HomoView::showRuler(bool bShow)
{
    m_bDrawRuler = bShow;
    update();
}
void HomoView::loadFeaturePoints()
{
    QSize dim = rect().size();
    dbPOINT* fp = gFecWin->getAreaSettings()->fpt;
    for (int i=0; i<FP_COUNTS; i++) {
        m_fp[i].setX(fp[i].x*dim.width());
        m_fp[i].setY(fp[i].y*dim.height());
    }
    dbRECT* fr = gFecWin->getAreaSettings()->region;
    for (int i=0; i<MAX_FP_AREA; i++) {
        m_region[i].setLeft(fr[i].l*dim.width());
        m_region[i].setRight(fr[i].r*dim.width());
        m_region[i].setTop(fr[i].t*dim.height());
        m_region[i].setBottom(fr[i].b*dim.height());
    }
}

