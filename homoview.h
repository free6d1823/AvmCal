#ifndef HOMOVIEW_H
#define HOMOVIEW_H

#include <QWidget>
#include "FecParam.h"
#include "homoview.h"
#include "./imglab/ImgProcess.h"

class HomoWin;
class HomoView : public QWidget
{

    Q_OBJECT
public:
    explicit HomoView(HomoWin *parent);
    ~HomoView();
    void setImage(IMAGE* pImg, int nAreaID);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void showRuler(bool bShow);
    void loadFeaturePoints();
    void drawAl();
private:
    void HomoTransform();

    HomoWin * m_pOwner;
    bool    m_bDrawRuler;
    QPointF m_fp[FP_COUNTS];    
    QRectF  m_region[MAX_FP_AREA];
    IMAGE* m_pRef;
    IMAGE* m_pImg;

    int m_nAreaId;          /*<! area of current area source imgae*/

    unsigned char* m_pImgRef;/*<! source imgae, current area*/
    int m_width;/*<! source imgae width*/
    int m_height;
    int m_stride;
    unsigned char* m_pImgData;//RGB32

};

#endif // HOMOVIEW_H
