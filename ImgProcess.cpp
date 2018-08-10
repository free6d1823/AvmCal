#include "ImgProcess.h"
#define PROFILE_NAME    "/home/cj/workspace/avmsettings.ini"
#include "./iniparser/iniparser.h"

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
    dictionary * ini ;
    ini = iniparser_load(path);
    if(ini == NULL)
        return false;

    iniparser_dump(ini, stdout);
    iniparser_freedict(ini);
    return true;
}

bool ImgProcess::SaveSettings(const char* path)
{
    dictionary * ini = iniparser_load(path);
    if(ini == NULL)
        return false;

    iniparser_set(ini, "system", NULL);
       iniparser_set(ini, "system:max_cam", "4");
   iniparser_set(ini, "area_0", NULL);
   iniparser_set(ini, "area_0:range", "0,0,1,0");
   iniparser_set(ini, "area_0:fec_input", "0,0,640,480");
   iniparser_set(ini, "area_0:fec_center", "0,0");
   iniparser_set(ini, "area_0:fec_radius", "510,510");
   iniparser_set(ini, "area_0_fp_0", NULL);
   iniparser_set(ini, "area_0_fp_0:range", "10,10,100,100");
   iniparser_set(ini, "area_0_fp_0:h", "1,2,3,4,5,6,7,8,9");
   iniparser_set(ini, "area_0_fp_0:fpt1", "10,10");
   iniparser_set(ini, "area_0_fp_0:fpt2", "10,10");
   iniparser_set(ini, "area_0_fp_1", NULL);
   iniparser_set(ini, "area_1", NULL);


    return true;
}

int ImgProcess::updateUv(vector <QVector2D> &uv)
{
    return 0;
}
