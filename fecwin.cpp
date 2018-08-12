#include "fecwin.h"
#include "ui_fecwin.h"

FecWin::FecWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FecWin)
{
    ui->setupUi(this);
}

FecWin::~FecWin()
{
    delete ui;
}
void FecWin::onFovChanged(int value)
{
    printf("FOV = %f\n", value);
}
void FecWin::onK1Changed(int value)
{

}
void FecWin::onK2Changed(int value)
{

}

void FecWin::onIntricaChanged(int value)
{
}
void FecWin::onIntricbChanged(int value)
{

}

void FecWin::onIntriccChanged(int value)
{

}
