#include <QPainter>
#include <QResizeEvent>
#include "imgview.h"
#include "fecwin.h"

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
unsigned char* ImgView::getImageData(){
    return m_pImgData;
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
void ImgView::saveFps()
{
    QSizeF sz = rect().size();
    dbPOINT* fps = gFecWin->getAreaSettings()->fps;
    for(int i=0; i<FP_COUNTS; i++){
        fps[i].x = m_fps[i].x()/sz.width();
        fps[i].y = m_fps[i].y()/sz.height();

    }

}
void ImgView::loadFps()
{
    dbPOINT* pFs = gFecWin->getAreaSettings()->fps;

    int n = FP_COUNTS;
    for(int i=0; i<n; i++) {
        m_fps[i].setX(pFs[i].x*rect().width());
        m_fps[i].setY(pFs[i].y*rect().height());
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

    loadFps();
}


void ImgView::paintEvent(QPaintEvent * /*event*/)
{
    QPointF region[4][4] = {
        {m_fps[0],m_fps[1],m_fps[6],m_fps[5]},
        {m_fps[1],m_fps[2],m_fps[7],m_fps[6]},
        {m_fps[2],m_fps[3],m_fps[8],m_fps[7]},
        {m_fps[3],m_fps[4],m_fps[9],m_fps[8]}};

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
        if(m_bShowFp){
            QPen pen1(Qt::blue);
            pen1.setWidth(1);
            painter.setPen(pen1);
            for(int j=0; j<4; j++ ){
                painter.drawPolygon(region[j], 4);
            }
            pen1.setWidth(7);
            pen1.setColor(Qt::red);
            painter.setPen(pen1);
            for (unsigned int i=0; i<sizeof(m_fps)/sizeof(m_fps[0]); i++) {
                painter.drawPoint(m_fps[i]);
            }
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
void ImgView::showFp(bool bShow)
{
    m_bShowFp = bShow;
    update();
}
void ImgView::resizeEvent(QResizeEvent *event)
{
    if (event->size() != event->oldSize()){
     //   createPixmaps(event->size());
    }
    loadFps();


}
#define FP_EDGE    5
int ImgView::checkPointOnFp(QPointF pt)
{
    int n = -1;
    for(int i=0; i<FP_COUNTS; i++) {
        if((pt.x() > m_fps[i].x() - FP_EDGE)&& (pt.x() < m_fps[i].x() + FP_EDGE) &&
           (pt.y() > m_fps[i].y() - FP_EDGE)&& (pt.y() < m_fps[i].y() + FP_EDGE))
            return i;
    }
    return n;
}

void ImgView::mousePressEvent(QMouseEvent *e)
{
    if(m_bShowFp) {
        int nID = checkPointOnFp(e->localPos());
        if (nID >= 0) {
            m_bDragMode = true;
            setCursor(Qt::CrossCursor);
            m_focusFpId = nID;
            m_ptStart = m_fps[m_focusFpId];
            m_ptEnd = e->localPos();
        }
    }
    else if(m_bDrawRuler) {
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
        if (m_bShowFp){
           m_fps[m_focusFpId] = m_ptEnd;
        }
        update();
    }
}
void ImgView::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_bDragMode) {
        m_ptEnd = e->localPos();
        m_bDragMode = false;
        if (m_bShowFp){
            setCursor(Qt::ArrowCursor);
            m_fps[m_focusFpId] = m_ptEnd;
        }
    }
    update();

}

