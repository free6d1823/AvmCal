#ifndef FECWIN_H
#define FECWIN_H

#include <QDialog>
#include "imgview.h"
#include "FecParam.h"
#include "homowin.h"

namespace Ui {
class FecWin;
}


class FecWin : public QDialog
{
    Q_OBJECT

public:
    explicit FecWin(QWidget *parent = 0);
    bool LoadImage(const char* path, int nID);
    void SetFecParam(FecParam* pParam);
    void ApplyFec(unsigned char* pSrc, int width, int inStride,  int height,
                  unsigned char* pTar, int outStride);
    ~FecWin();
private slots:

    void accept() override;
    void onAreaIdChanged(int value);
    void onShowRulerChanged(int value);
    void onFovValueChanged(double value);
    void onIntricAChanged(double value);
    void onIntricBChanged(double value);
    void onIntricCChanged(double value);
    void onK1ValueChanged(double value);
    void onK2ValueChanged(double value);
    void onCenterXChanged(double value);
    void onCenterYChanged(double value);
    void onPitchChanged(double value);
    void onYawChanged(double value);
    void onRollChanged(double value);
    void onPreviewClicked();

    //Homo
    void onShowHomoChanged(int value);
private:
    void PosMapNormalize(double u, double v, double &x, double &y);
    void UpdateUI();
    int m_curAreaId;
    bool    m_bShowRuler;
    ImgView*    m_pImgView;
    Ui::FecWin *ui;

    FecParam*   m_pFecParam;
    AreaSettings m_as;

    HomoWin*    m_pHomoWin;
};

#endif // FECWIN_H
