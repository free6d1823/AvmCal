#ifndef IMGVIEW_H
#define IMGVIEW_H

#include <QWidget>
#include <QImage>

class ImgView: public QWidget
{
    Q_OBJECT

public:
    explicit ImgView(QWidget *parent = 0);
    ~ImgView();

    void setImage(unsigned char* pYuv, int width, int stride, int height);
    void setImageRGB32(unsigned char* pRgb, int width, int stride, int height);
    // QWidget
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void showRuler(bool bShow);
protected:
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



};

#endif // IMGVIEW_H
