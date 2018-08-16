#include "homowin.h"
#include "ui_homowin.h"

HomoWin::HomoWin(AreaSettings* pAs, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomoWin)
{
    ui->setupUi(this);
    connect(ui->btnPreview, SIGNAL(clicked()), SLOT(onPreviewClicked()));
    connect(ui->btnSave, SIGNAL(clicked()), SLOT(onSaveClicked()));
    connect(ui->dbSpinX0, SIGNAL(valueChanged(double)), SLOT(onX0ValueChanged(double)));
    connect(ui->dbSpinY0, SIGNAL(valueChanged(double)), SLOT(onY0ValueChanged(double)));
    connect(ui->dbSpinX1, SIGNAL(valueChanged(double)), SLOT(onX1ValueChanged(double)));
    connect(ui->dbSpinY1, SIGNAL(valueChanged(double)), SLOT(onY1ValueChanged(double)));
    connect(ui->dbSpinX2, SIGNAL(valueChanged(double)), SLOT(onX2ValueChanged(double)));
    connect(ui->dbSpinY2, SIGNAL(valueChanged(double)), SLOT(onY2ValueChanged(double)));
    connect(ui->dbSpinX3, SIGNAL(valueChanged(double)), SLOT(onX3ValueChanged(double)));
    connect(ui->dbSpinY3, SIGNAL(valueChanged(double)), SLOT(onY3ValueChanged(double)));
    connect(ui->dbSpinX4, SIGNAL(valueChanged(double)), SLOT(onX4ValueChanged(double)));
    connect(ui->dbSpinY4, SIGNAL(valueChanged(double)), SLOT(onY4ValueChanged(double)));
    connect(ui->dbSpinX5, SIGNAL(valueChanged(double)), SLOT(onX5ValueChanged(double)));
    connect(ui->dbSpinY5, SIGNAL(valueChanged(double)), SLOT(onY5ValueChanged(double)));
    connect(ui->dbSpinX6, SIGNAL(valueChanged(double)), SLOT(onX6ValueChanged(double)));
    connect(ui->dbSpinY6, SIGNAL(valueChanged(double)), SLOT(onY6ValueChanged(double)));
    connect(ui->dbSpinX7, SIGNAL(valueChanged(double)), SLOT(onX7ValueChanged(double)));
    connect(ui->dbSpinY7, SIGNAL(valueChanged(double)), SLOT(onY7ValueChanged(double)));
    connect(ui->dbSpinX8, SIGNAL(valueChanged(double)), SLOT(onX8ValueChanged(double)));
    connect(ui->dbSpinY8, SIGNAL(valueChanged(double)), SLOT(onY8ValueChanged(double)));
    connect(ui->dbSpinX9, SIGNAL(valueChanged(double)), SLOT(onX9ValueChanged(double)));
    connect(ui->dbSpinY9, SIGNAL(valueChanged(double)), SLOT(onY9ValueChanged(double)));
    connect(ui->checkShowGride, SIGNAL(stateChanged(int)), SLOT(onShowGrideChanged(int)));

    m_pAs = pAs;
    m_pImgView  = new HomoView(this);

    UpdateUI();

    ui->layoutHomo->addWidget(m_pImgView);
}
void HomoWin::UpdateUI()
{
    if(!m_pAs)
        return;
    ui->dbSpinX0->setValue(m_pAs->fps[0].x);
    ui->dbSpinY0->setValue(m_pAs->fps[0].y);
    ui->dbSpinX1->setValue(m_pAs->fps[1].x);
    ui->dbSpinY1->setValue(m_pAs->fps[1].y);
    ui->dbSpinX2->setValue(m_pAs->fps[2].x);
    ui->dbSpinY2->setValue(m_pAs->fps[2].y);
    ui->dbSpinX3->setValue(m_pAs->fps[3].x);
    ui->dbSpinY3->setValue(m_pAs->fps[3].y);
    ui->dbSpinX4->setValue(m_pAs->fps[4].x);
    ui->dbSpinY4->setValue(m_pAs->fps[4].y);
    ui->dbSpinX5->setValue(m_pAs->fps[5].x);
    ui->dbSpinY5->setValue(m_pAs->fps[5].y);
    ui->dbSpinX6->setValue(m_pAs->fps[6].x);
    ui->dbSpinY6->setValue(m_pAs->fps[6].y);
    ui->dbSpinX7->setValue(m_pAs->fps[7].x);
    ui->dbSpinY7->setValue(m_pAs->fps[7].y);
    ui->dbSpinX8->setValue(m_pAs->fps[8].x);
    ui->dbSpinY8->setValue(m_pAs->fps[8].y);
    ui->dbSpinX9->setValue(m_pAs->fps[9].x);
    ui->dbSpinY9->setValue(m_pAs->fps[9].y);

    m_pImgView->setFeaturePoints(m_pAs->fpt, FP_COUNTS);
    m_pImgView->setFeatureRegions(m_pAs->region, MAX_FP_AREA);
}

HomoWin::~HomoWin()
{
    delete ui;
    if(m_pImgView)
        delete m_pImgView;
}
