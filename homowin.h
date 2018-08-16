#ifndef HOMOWIN_H
#define HOMOWIN_H

#include <QWidget>
#include "FecParam.h"
#include "homoview.h"

namespace Ui {
class HomoWin;
}

class HomoWin : public QWidget
{
    Q_OBJECT

public:
    explicit HomoWin(AreaSettings* pAs, QWidget *parent = 0);
    ~HomoWin();
    void setAreaSettings(AreaSettings* pAs){
        m_pAs = pAs;
        UpdateUI();
    }
    void UpdateUI();
private:
    Ui::HomoWin *ui;

    HomoView* m_pImgView;
    AreaSettings* m_pAs;
};

#endif // HOMOWIN_H
