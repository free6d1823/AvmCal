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
    connect(ui->btnPreview, SIGNAL(clicked()), SLOT(onPreviewClicked()));
    connect(ui->btnSave, SIGNAL(clicked()), SLOT(onSaveClicked()));
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
void HomoWin::setSourceImage(void* pRgb, int width, int stride, int height, int areaId)
{
    if(m_pImgView) {
m_pImgView->setImageRGB32((unsigned char*) pRgb, width, stride, height, areaId);
    }
}


void HomoWin::onPreviewClicked()
{

}

void HomoWin::onSaveClicked()
{

}

void HomoWin::onShowGrideChanged(int value)
{
    if(m_pImgView)m_pImgView->showRuler(value !=0);
}
