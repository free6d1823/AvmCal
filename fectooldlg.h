#ifndef FECTOOLDLG_H
#define FECTOOLDLG_H

#include <QDialog>
#include <QDoubleSpinBox>
#include <QLabel>
#include "homowin.h"
#include "fecwin.h"
#include "imgview.h"

class FecToolDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FecToolDlg(QWidget *parent = 0);
    ~FecToolDlg();
    void UpdateUi();
private slots:
    void onApply();
    void onUpdate();
    void onSave();

private:
    QPushButton*    m_btnApply;
    QPushButton*    m_btnUpdate;
    QPushButton*    m_btnSave;

    QDoubleSpinBox* m_spinX[10];
    QDoubleSpinBox* m_spinY[10];
    QLabel* m_labelH[MAX_FP_AREA][3];
};

#endif // FECTOOLDLG_H
