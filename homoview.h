#ifndef HOMOVIEW_H
#define HOMOVIEW_H

#include <QWidget>
#include "FecParam.h"

class HomoView : public QWidget
{
    Q_OBJECT
public:
    explicit HomoView(QWidget *parent = 0);
    ~HomoView();
    void setImageRGB32(unsigned char* pRgb, int width, int stride, int height);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void showRuler(bool bShow);
    void setFeaturePoints(dbPOINT* fp, int elements);
    void setFeatureRegions(dbRECT* fp, int num);

private:
    bool    m_bDrawRuler;
    QPointF m_fp[FP_COUNTS];
    QRectF  m_region[MAX_FP_AREA];
    unsigned char* m_pImgData;//RGB32
    int m_width;
    int m_height;
    int m_stride;

};

#endif // HOMOVIEW_H
