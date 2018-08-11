#include "ImgProcess.h"
#define PROFILE_NAME    "/home/cj/workspace/avmsettings.ini"
#include "./inifile/inifile.h"

AreaSettings    m_as[MAX_CAMERAS] = {

};
ImgProcess::ImgProcess(int id): m_nId(id)
{

    LoadSettings(PROFILE_NAME);
}
ImgProcess::~ImgProcess()
{
    SaveSettings(PROFILE_NAME);
}
bool ImgProcess::LoadSettings(const char* path)
{
    char key[32];
    char data[256];

    GetProfileString("system", "max_cam", data, sizeof(data), "", path);
    printf("--%s \n", data);

    return true;
}

bool ImgProcess::SaveSettings(const char* path)
{

    WriteProfileString("system", "max_cam", "4", path);


    return true;
}

int ImgProcess::updateUv(vector <QVector2D> &uv)
{
    return 0;
}
