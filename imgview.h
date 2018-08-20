#ifndef IMGVIEW_H
#define IMGVIEW_H

#include <QWidget>
#include <QImage>
#include "FecParam.h"
#include "./imglab/ImgProcess.h"

class ImgView: public QWidget
{
    Q_OBJECT

public:
    explicit ImgView(QWidget *parent = 0);
    ~ImgView();

    void setImage(IMAGE* pImg);
    IMAGE* getImage(){return m_pImg;}


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

    IMAGE* m_pImg;

    bool    m_bShowFp;
    int     m_focusFpId;

    //FP coordinates of rectified image, on display window, in pixels
     QPointF m_fps[FP_COUNTS];


};

#endif // IMGVIEW_H
