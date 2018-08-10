#ifndef ImgProcess_H
#define ImgProcess_H
#include "FecParam.h"
#include <vector>
#include <QVector2D>
using namespace std;

class ImgProcess
{
public:
    ImgProcess(int id);
    ~ImgProcess();
    bool LoadSettings(const char* path);
    bool SaveSettings(const char* path);
    int updateUv(vector <QVector2D> &uv);
private:
    int m_nId; /*!<Area ID,0~4, where 0 is front view */
};

#endif // ImgProcess_H
