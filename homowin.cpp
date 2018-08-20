#include "homowin.h"
#include "ui_homowin.h"
HomoWin::~HomoWin()
{
    delete ui;
    if(m_pImgView)
        delete m_pImgView;
}
HomoWin::HomoWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HomoWin)
{
    ui->setupUi(this);
    connect(ui->btnGen, SIGNAL(clicked()), SLOT(onGenTable()));
    connect(ui->btnRedraw, SIGNAL(clicked()), SLOT(onRedrawAll()));
    connect(ui->checkShowGride, SIGNAL(stateChanged(int)), SLOT(onShowGrideChanged(int)));

    m_pAs = NULL;
    m_pImgView  = new HomoView(this);
    ui->layoutHomo->addWidget(m_pImgView);


}
void HomoWin::setAreaSettings(AreaSettings* pAs){
    m_pAs = pAs;
    UpdateUI();
    if(m_pImgView) {
        m_pImgView->loadFeaturePoints();
    }

}
void HomoWin::UpdateUI()
{
    if(!m_pAs)
        return;
 }
void HomoWin::setSourceImage(IMAGE* pImg, int areaId)
{
    if(m_pImgView) {
        m_pImgView->setImage(pImg, areaId);
    }
}


void HomoWin::onGenTable()
{

}

void HomoWin::onRedrawAll()
{
    if(m_pImgView) {
        m_pImgView->drawAl();
    }
}

void HomoWin::onShowGrideChanged(int value)
{
    if(m_pImgView)m_pImgView->showRuler(value !=0);
}
