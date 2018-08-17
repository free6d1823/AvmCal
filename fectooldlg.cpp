#include <QtWidgets>
#include "fectooldlg.h"


FecToolDlg::FecToolDlg(QWidget *parent) :
    QDialog(parent)
{

    QHBoxLayout *bar1 = new QHBoxLayout;
    QHBoxLayout *bar2 = new QHBoxLayout;

    char labelx[32];
    char labely[32];

    for (int i=0;i<10; i++){
        sprintf(labelx, "x%d", i);
        sprintf(labely, "y%d", i);
        QLabel* xx = new QLabel(labelx);
        QLabel* yy = new QLabel(labely);
        m_spinX[i] = new QDoubleSpinBox();
        m_spinX[i]->setMaximum(2.0);
        m_spinX[i]->setMinimum(-1.0);
        m_spinX[i]->setDecimals(4);
        m_spinX[i]->setSingleStep(0.01);
        m_spinY[i] = new QDoubleSpinBox;
        m_spinY[i]->setMaximum(2.0);
        m_spinY[i]->setMinimum(-1.0);
        m_spinY[i]->setDecimals(4);
        m_spinY[i]->setSingleStep(0.01);

        QHBoxLayout *ax = new QHBoxLayout;
        ax->addWidget(xx);ax->addWidget(m_spinX[i]);
        QHBoxLayout *ay = new QHBoxLayout;
        ay->addWidget(yy);ay->addWidget(m_spinY[i]);

        QVBoxLayout *tt = new QVBoxLayout;
        tt->addLayout(ax);
        tt->addLayout(ay);
        if(i<5)
            bar1->addLayout(tt);
        else
            bar2->addLayout(tt);
    }
    QHBoxLayout *bar3 = new QHBoxLayout;
    QFont font = QFont("Ariel", 8);
    for (int i=0; i<4; i++){
        QLabel* h0 = new QLabel("H1");
        h0->setFont(font);
        m_labelH[i][0] = new QLabel;
        m_labelH[i][0]->setFont(font);
        QHBoxLayout *hh0 = new QHBoxLayout;
        hh0->addWidget(h0);
        hh0->addWidget(m_labelH[i][0]);

        QLabel* h1 = new QLabel("H2");
        h1->setFont(font);
        m_labelH[i][1] = new QLabel;
        m_labelH[i][1]->setFont(font);
        QHBoxLayout *hh1 = new QHBoxLayout;
        hh1->addWidget(h1);
        hh1->addWidget(m_labelH[i][1]);

        QLabel* h2 = new QLabel("H3");
        h2->setFont(font);
        m_labelH[i][2] = new QLabel;
        m_labelH[i][2]->setFont(font);
        QHBoxLayout *hh2 = new QHBoxLayout;
        hh2->addWidget(h2);
        hh2->addWidget(m_labelH[i][2]);

        QVBoxLayout *tt = new QVBoxLayout;
        tt->addLayout(hh0);
        tt->addLayout(hh1);
        tt->addLayout(hh2);
        bar3->addLayout(tt);
    }
    m_btnApply = new QPushButton("Apply");
    m_btnUpdate = new QPushButton("Update");
    m_btnSave = new QPushButton("Save");
    QHBoxLayout *bar4 = new QHBoxLayout;
    bar4->addWidget(m_btnUpdate);
    bar4->addWidget(m_btnApply);
    bar4->addWidget(m_btnSave);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addLayout(bar1, 0, 0);
    mainLayout->addLayout(bar2, 1, 0);
    mainLayout->addLayout(bar3, 2, 0);
    mainLayout->addLayout(bar4, 3, 0);

    setLayout(mainLayout);

    setWindowTitle(tr("Feature Points"));
    connect(m_btnUpdate, SIGNAL(clicked()), SLOT(onUpdate()));
    connect(m_btnApply, SIGNAL(clicked()), SLOT(onApply()));
    connect(m_btnSave, SIGNAL(clicked()), SLOT(onSave()));


}

FecToolDlg::~FecToolDlg()
{
}
/*!<Load AreaSettings to UI */
void FecToolDlg::UpdateUi()
{
    AreaSettings* pAs = gFecWin->getAreaSettings();

    for (int i=0;i<FP_COUNTS; i++){
        m_spinX[i]->setValue(pAs->fps[i].x);
        m_spinY[i]->setValue(pAs->fps[i].y);
    }
    char text[64];
    for(int i=0; i< MAX_FP_AREA; i++){
        for(int j=0; j<3; j++) {//row of matrix
            sprintf(text, "%6.4f,%6.4f,%6.4f", pAs->homo[i].h[j][0], pAs->homo[i].h[j][1], pAs->homo[i].h[j][2]);
            m_labelH[i][j]->setText(text);
        }

    }

}
/*!<Load AreaSettings to UI */
void FecToolDlg::onUpdate()
{
    gFecWin->UpdateFeaturePoints(true);
    UpdateUi();
}
/*!<Apply UI to AreaSettings and ImgView and do homograph transform*/
void FecToolDlg::onApply()
{
    AreaSettings* pAs = gFecWin->getAreaSettings();
    for (int i=0;i<FP_COUNTS; i++){
        pAs->fps[i].x = m_spinX[i]->value();
        pAs->fps[i].y = m_spinY[i]->value();
    }
    gFecWin->CalculateHomoMatrix();
    UpdateUi();//refresh H coefficients
    gFecWin->UpdateFeaturePoints(false);
}
/*!<Inform FecWin to save AreaSettings to ini*/
void FecToolDlg::onSave()
{
    gFecWin->SaveFeaturePoints();
}
