/**
 * @file
 * @brief initialization file read and write API
 *	-size of the ini file must less than 16K
 *	-after '=' in key value pair, can not support empty char. this would not like WIN32 API
 *	-support comment using ';' prefix
 *	-can not support multi line
 * @author Deng Yangjun
 * @date 2007-1-9
 * @version 0.2
 */
 
#ifndef INI_FILE_H_
#define INI_FILE_H_

#include "ImgProcess.h"

bool GetProfileString( const char *section, const char *key,char *value, int size,const char *default_value, const char *file);
bool WriteProfileString( const char *section, const char *key,const char *value, const char *file);

int GetProfileInt( const char *section, const char *key,int default_value, const char *file);
bool WriteProfileInt( const char * lpSecName,const char * lpKeyName, int value, const char * lpFileName);

float GetProfileFloat(const char *lpSecName, const char *lpKeyName, float fDefault,  const char *lpFileName);
bool WriteProfileFloat(  const char *lpSecName, const char * lpKeyName, float value,  const char*  lpFileName);

double GetProfileDouble(const char *lpSecName, const char *lpKeyName, double dbDefault,  const char *lpFileName);
bool WriteProfileDouble(  const char *lpSecName, const char * lpKeyName, double value,  const char*  lpFileName);

bool WriteProfilePointDouble(const char *lpSecName, const char *lpKeyName,  dbPOINT *pPoint, const char *lpFileName);
bool GetProfilePointDouble(const char * lpSecName, const char * lpKeyName,  dbPOINT* pPoint, const char * lpFileName);

bool WriteProfileArrayDouble(const char * lpSecName, const char * lpKeyName,  double* pValue, int nElements, const char * lpFileName);
bool GetProfileArrayDouble(const char * lpSecName, const char * lpKeyName,   double* pValue, int nElements,  const char * lpFileName);
bool	WriteProfileRectDouble(const char * lpSecName, const char * lpKeyName,  dbRECT* pRect, const char * lpFileName);
bool GetProfileRectDouble(const char * lpSecName, const char * lpKeyName,  dbRECT* pRect, const char * lpFileName);
bool WriteProfileArrayInt(const char * lpSecName, const char * lpKeyName,  int* pValue, int nElements, const char * lpFileName);
bool GetProfileArrayInt(const char * lpSecName, const char * lpKeyName,   int* pValue, int nElements,  const char * lpFileName);



#endif //end of INI_FILE_H_

