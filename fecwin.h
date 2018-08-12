#ifndef FECWIN_H
#define FECWIN_H

#include <QDialog>

namespace Ui {
class FecWin;
}

class FecWin : public QDialog
{
    Q_OBJECT

public:
    explicit FecWin(QWidget *parent = 0);
    ~FecWin();
private slots:
    void onFovChanged(int value);
    void onK1Changed(int value);
    void onK2Changed(int value);
    void onIntricaChanged(int value);
    void onIntricbChanged(int value);
    void onIntriccChanged(int value);
private:
    Ui::FecWin *ui;
};

#endif // FECWIN_H
