#include "ImgProcess.h"
#include "FecParam.h"

AreaSettings    m_as[MAX_CAMERAS] = {

};
ImgProcess::ImgProcess(int id): m_nId(id)
{

    LoadSettings();
}
ImgProcess::~ImgProcess()
{
    SaveSettings();
}
bool ImgProcess::LoadSettings()
{
    for(int i=0; i<MAX_CAMERAS; i++){
        if (!LoadAreaSettings(&m_as[i], i))
            return false;
    }

    return true;
}

bool ImgProcess::SaveSettings( )
{

    for(int i=0; i<MAX_CAMERAS; i++){
        if (!SaveAreaSettings(&m_as[i], i))
            return false;
    }


    return true;
}

int ImgProcess::updateUv(vector <QVector2D> &uv)
{
    return 0;
}
