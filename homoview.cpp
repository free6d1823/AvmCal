#include "homoview.h"
#include <QPainter>

HomoView::HomoView(QWidget *parent) : QWidget(parent)
{
    m_bDrawRuler = true;
    for (int i=0; i< FP_COUNTS; i++) {
        m_fp[i] = QPointF(0,0);
    }
    m_pImgData = NULL;
     m_width = m_height = 0;
}
HomoView::~HomoView()
{
    if (m_pImgData) {
        delete m_pImgData;
        m_pImgData = NULL;
    }
}

void HomoView::setImageRGB32(unsigned char* pRgb, int width, int stride, int height)
{
    if (m_width != width || m_height != height){
        if (m_pImgData) {
            free (m_pImgData);
        }
        m_width = width;
        m_height = height;
        m_stride = m_width*4;
        m_pImgData = (unsigned char*) malloc(m_stride*height);
    }
    unsigned char* pSrc = pRgb;
    unsigned char* pTar = m_pImgData;

    for (int i=0; i<height; i++) {
        memcpy(pTar, pSrc, m_stride);
        pSrc += stride;
        pTar += m_stride;
    }
    update();
}

void HomoView::paintEvent(QPaintEvent */*event*/)
{
    QPainter painter(this);
    QRect rcTarget = rect();
    QRect rcSrc = QRect(0,0,m_width, m_height);
    if(m_pImgData){
        QImage image(
                m_pImgData,
                m_width,
                m_height,
                m_stride,
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

}

void HomoView::showRuler(bool bShow)
{
    m_bDrawRuler = bShow;
    update();
}
void HomoView::setFeatureRegions(dbRECT* fp, int num)
{
    QSize dim = rect().size();
    for (int i=0; i<num; i++) {
        m_region[i].setLeft(fp[i].l*dim.width());
        m_region[i].setRight(fp[i].r*dim.width());
        m_region[i].setTop(fp[i].t*dim.height());
        m_region[i].setBottom(fp[i].b*dim.height());
    }
}

void HomoView::setFeaturePoints(dbPOINT* fp, int elements)
{
    QSize dim = rect().size();
    for (int i=0; i<elements; i++) {
        m_fp[i].setX(fp[i].x*dim.width());
        m_fp[i].setY(fp[i].y*dim.height());
    }
}
