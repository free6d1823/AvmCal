#include "imgview.h"
#include <QPainter>
#include <QResizeEvent>

void YuyvToRgb32(unsigned char* pYuv, int width, int stride, int height, unsigned char* pRgb, bool uFirst);

ImgView::ImgView(QWidget *parent)
    :   QWidget(parent)
{
    m_pImgData = NULL;
    m_width = m_height = 0;
    m_bDragMode = false;
    m_bDrawRuler = false;
    m_ptStart = QPointF(0,0);
    m_ptEnd =QPointF(0,0);
}
ImgView::~ImgView()
{
    if (m_pImgData) {
        delete m_pImgData;
        m_pImgData = NULL;
    }
}
void ImgView::setImageRGB32(unsigned char* pRgb, int width, int stride, int height)
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
void ImgView::setImage(unsigned char* pYuv, int width, int stride, int height)
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
    YuyvToRgb32(pYuv, width, stride, height, m_pImgData, true);

    update();
}

void ImgView::paintEvent(QPaintEvent * /*event*/)
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
        if(m_bDrawRuler) {
            painter.setPen(Qt::red);
            painter.drawLine(m_ptStart, m_ptEnd);
        }
    } else {
        painter.fillRect(rcTarget, Qt::yellow);

    }
}

void ImgView::showRuler(bool bShow)
{
    m_bDrawRuler = bShow;
    update();
}
void ImgView::resizeEvent(QResizeEvent *event)
{
    if (event->size() != event->oldSize()){
     //   createPixmaps(event->size());
    }
}
void ImgView::mousePressEvent(QMouseEvent *e)
{
    if(m_bDrawRuler) {
        m_bDragMode = true;
        m_ptStart = e->localPos();
        m_ptEnd = e->localPos();
        update();
    }
}
void ImgView::mouseMoveEvent(QMouseEvent *e)
{
    if (m_bDragMode){
        m_ptEnd = e->localPos();
        update();
    }
}
void ImgView::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_bDragMode) {
        m_ptEnd = e->localPos();
        m_bDragMode = false;
    }
    update();

}

