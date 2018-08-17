#ifndef IMGVIEW_H
#define IMGVIEW_H

#include <QWidget>
#include <QImage>
#include "FecParam.h"

class ImgView: public QWidget
{
    Q_OBJECT

public:
    explicit ImgView(QWidget *parent = 0);
    ~ImgView();

    void setImage(unsigned char* pYuv, int width, int stride, int height);
    void setImageRGB32(unsigned char* pRgb, int width, int stride, int height);
    unsigned char* getImageData();
    int getImageWidth(){return m_width;}
    int getImageHeight(){return m_height;}
    int getImageStride(){return m_stride;}

    // QWidget
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void showRuler(bool bShow);
    void showFp(bool bShow);
    void saveFps();
    void loadFps();
protected:
    int checkPointOnFp(QPointF pt);
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
private:
    bool    m_bDragMode;
    bool    m_bDrawRuler;
    QPointF  m_ptStart;
    QPointF  m_ptEnd;

    unsigned char* m_pImgData;//RGB32
    int m_width;
    int m_height;
    int m_stride;

    bool    m_bShowFp;
    int     m_focusFpId;
    //FP coordinates of rectified image, on display window, in pixels
     QPointF m_fps[FP_COUNTS];


};

#endif // IMGVIEW_H
