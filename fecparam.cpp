#include "./inifile/inifile.h"
#include <QCoreApplication>
#include "FecParam.h"

#define PI      3.1415967
char  INIFILE[256] = {0};
const char DEFAULTINI[] ="/default.ini";

bool    LoadFecParam(FecParam* pFecParam, int nArea)
{
    char section[32];
    sprintf(section, "fecparam_%d", nArea);

    if(!GetProfilePointDouble(section, "ptCenter", &pFecParam->ptCenter, INIFILE))
    {
        pFecParam->ptCenter.x = 0.5;
        pFecParam->ptCenter.y = 0.5;
    }
    pFecParam->fov = GetProfileDouble(section, "fov", (170.0*PI/180.0), INIFILE);
    pFecParam->k1 = GetProfileDouble(section, "k1", -8.0, INIFILE);
    pFecParam->k2 = GetProfileDouble(section, "k2", 0.0, INIFILE);
    pFecParam->a = GetProfileDouble(section, "a", 1.0, INIFILE);
    pFecParam->b = GetProfileDouble(section, "b", 1.0, INIFILE);
    pFecParam->c = GetProfileDouble(section, "c", 0.0, INIFILE);
    pFecParam->pitch = GetProfileDouble(section, "pitch", 0.0, INIFILE);
    pFecParam->yaw = GetProfileDouble(section, "yaw", 0.0, INIFILE);
    pFecParam->roll = GetProfileDouble(section, "roll", 0.0, INIFILE);

    return true;
}
bool    LoadHomoParam(HomoParam* pParam, int nArea, int nFp)
{
    char section[32];

    sprintf(section,"homoparam_%d_%d", nArea, nFp);
    if(! GetProfileArrayDouble(section, "h0", pParam->h[0], 3, INIFILE)){
        pParam->h[0][0]=1;pParam->h[0][1]=0;pParam->h[0][2]=0;
    }
    if(! GetProfileArrayDouble(section, "h1", pParam->h[1], 3, INIFILE)){
        pParam->h[1][0]=0;pParam->h[1][1]=1;pParam->h[1][2]=0;
    }
    if(! GetProfileArrayDouble(section, "h2", pParam->h[2], 3, INIFILE)){
        pParam->h[2][0]=0;pParam->h[2][1]=0;pParam->h[2][2]=1;
    }

    if(! GetProfileArrayInt(section, "fp_index", pParam->fp_index, 4, INIFILE)){
        fprintf(stderr, "Failed to read [%s fp_index !\n", section);
    }
    return true;
}
bool    LoadAreaSettings(AreaSettings* pParam, int nArea)
{
    if (INIFILE[0] == 0){
        QString path = QCoreApplication::applicationDirPath();
        path = path.append(DEFAULTINI);
        strncpy(INIFILE, path.toLocal8Bit().data(), sizeof(INIFILE));
    }
    char section[32];
    sprintf(section, "area_%d", nArea);
    if(!GetProfileRectDouble(section, "range", &pParam->range, INIFILE)){
        fprintf(stderr, "[%s] range= not found!\n", section);
    }
    char key[32];
    for (int i=0; i< FP_COUNTS; i++) {
        sprintf(key, "fpt_%d", i);
        if(!GetProfilePointDouble(section, key, &pParam->fpt[i], INIFILE)){
            fprintf(stderr, "%s value not found!\n", key);
        }
        sprintf(key, "fps_%d", i);
        if(!GetProfilePointDouble(section, key, &pParam->fps[i], INIFILE)){
            fprintf(stderr, "[%s] %s value not found!\n", section, key);
        }
    }
    LoadFecParam(&pParam->fec, nArea);
    for (int i=0; i< MAX_FP_AREA; i++) {
        sprintf(key, "region_%d", i);
        if(!GetProfileRectDouble(section, key, &pParam->region[i], INIFILE)){

        }

        LoadHomoParam(&pParam->homo[i], nArea, i);
    }
    return true;
}
bool    SaveFecParam(FecParam* pFecParam, int nArea)
{
    char section[32];
    sprintf(section,"fecparam_%d", nArea);

    if (!WriteProfilePointDouble(section, "ptCenter", &pFecParam->ptCenter, INIFILE))
        return false;
    if(!WriteProfileDouble(section, "fov", pFecParam->fov, INIFILE))
        return false;
    if(!WriteProfileDouble(section, "k1", pFecParam->k1, INIFILE))
        return false;
    if(!WriteProfileDouble(section, "k2", pFecParam->k2, INIFILE))
        return false;
    if(!WriteProfileDouble(section, "a", pFecParam->a, INIFILE))
        return false;
    if(!WriteProfileDouble(section, "b", pFecParam->b, INIFILE))
        return false;
    if(!WriteProfileDouble(section, "c", pFecParam->c, INIFILE))
        return false;
    if(!WriteProfileDouble(section, "pitch", pFecParam->pitch, INIFILE))
        return false;
    if(!WriteProfileDouble(section, "yaw", pFecParam->yaw, INIFILE))
        return false;
    if(!WriteProfileDouble(section, "roll", pFecParam->roll, INIFILE))
        return false;
    return true;
}
bool    SaveHomoParam(HomoParam* pParam, int nArea, int nFp)
{
    char section[32];

    sprintf(section,"homoparam_%d_%d", nArea, nFp);
    if(! WriteProfileArrayDouble(section, "h0", pParam->h[0], 3, INIFILE))
        return false;
    if(! WriteProfileArrayDouble(section, "h1", pParam->h[1], 3, INIFILE))
        return false;
    if(! WriteProfileArrayDouble(section, "h2", pParam->h[2], 3, INIFILE))
        return false;

    if(! WriteProfileArrayInt(section, "fp_index", pParam->fp_index, 4, INIFILE))
        return false;

    return true;
}

bool    SaveAreaSettings(AreaSettings* pParam, int nArea)
{
    char section[32];
    sprintf(section, "area_%d", nArea);
    if(!WriteProfileRectDouble(section, "range", &pParam->range, INIFILE))
        return false;
    char key[32];
    for (int i=0; i< FP_COUNTS; i++) {
        sprintf(key, "fpt_%d", i);
        if(!WriteProfilePointDouble(section, key, &pParam->fpt[i], INIFILE))
            return false;
        sprintf(key, "fps_%d", i);
        if(!WriteProfilePointDouble(section, key, &pParam->fps[i], INIFILE))
            return false;
    }

    if(!SaveFecParam(&pParam->fec, nArea))
        return false;

    for (int i=0; i< MAX_FP_AREA; i++) {
        sprintf(key, "region_%d", i);
        if(!WriteProfileRectDouble(section, key, &pParam->region[i], INIFILE)){
        }

        SaveHomoParam(&pParam->homo[i], nArea, i);
    }
    return true;
}
