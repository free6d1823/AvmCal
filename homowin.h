#ifndef HOMOWIN_H
#define HOMOWIN_H

#include <QDialog>
#include "FecParam.h"
#include "homoview.h"
#include "./imglab/ImgProcess.h"

namespace Ui {
class HomoWin;
}

class HomoWin : public QDialog
{
    friend class HomoView;
    Q_OBJECT

public:
    explicit HomoWin(QWidget *parent = 0);
    ~HomoWin();
    void setAreaSettings(AreaSettings* pAs);
    AreaSettings*  getAreaSettings(){return m_pAs;}
    void setSourceImage(IMAGE* pImg, int areaId);
    void UpdateUI();

private slots:
    void onShowGrideChanged(int value);
     void onGenTable();
    void onRedrawAll();

private:
    Ui::HomoWin *ui;

    HomoView* m_pImgView;
protected:
    AreaSettings* m_pAs;
};

#endif // HOMOWIN_H
