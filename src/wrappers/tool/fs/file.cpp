/// -------------------------------------------------
/// file.cpp : �ļ�������װʵ���ļ�
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "fs/file.h"
#include <errno.h>


/*******************************************************
  �� �� ��: DCOP_SaveToFile
  ��    ��: ���浽�ļ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD DCOP_SaveToFile(const char *szFile, const void *cpBuf, DWORD dwCount, DWORD dwOffset)
{
    FILE *pf = 0;
    DWORD dwRc = SUCCESS;

    pf = fopen(szFile, "r+b");
    if (!pf)
    {
        pf = fopen(szFile, "w+b");
        if (!pf)
        {
            dwRc = (DWORD)errno;
            return (dwRc)? dwRc : FAILURE;
        }
    }

    int iRc = fseek(pf, (long)dwOffset, SEEK_SET);
    if (iRc)
    {
        fclose(pf);
        dwRc = (DWORD)errno;
        return (dwRc)? dwRc : FAILURE;
    }

    size_t numwritten = fwrite(cpBuf, sizeof(BYTE), dwCount, pf);
    fclose(pf);

    if (numwritten < dwCount)
    {
        dwRc = (DWORD)errno;
        return (dwRc)? dwRc : FAILURE;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: DCOP_RestoreFromFile
  ��    ��: ���ļ��лָ�
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD DCOP_RestoreFromFile(const char *szFile, void *cpBuf, DWORD dwCount, DWORD dwOffset)
{
    FILE *pf = 0;
    DWORD dwRc = SUCCESS;

    pf = fopen(szFile, "rb");
    if (!pf)
    {
        return FAILURE;
    }

    int iRc = fseek(pf, (long)dwOffset, SEEK_SET);
    if (iRc)
    {
        fclose(pf);
        dwRc = (DWORD)errno;
        return (dwRc)? dwRc : FAILURE;
    }

    size_t numread = fread(cpBuf, sizeof(BYTE), dwCount, pf);
    fclose(pf);

    if (numread < dwCount)
    {
        dwRc = (DWORD)errno;
        return (dwRc)? dwRc : FAILURE;
    }

    return SUCCESS;
}

/*******************************************************
  �� �� ��: DCOP_IsFileExist
  ��    ��: �ļ��Ƿ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
BOOL DCOP_IsFileExist(const char *szFile)
{
    FILE *pf = fopen(szFile, "rb");
    if (!pf)
    {
        return FALSE;
    }

    fclose(pf);
    return TRUE;
}

/*******************************************************
  �� �� ��: DCOP_GetFileLen
  ��    ��: ��ȡ�ļ�����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD DCOP_GetFileLen(const char *szFile)
{
    FILE *pf = fopen(szFile, "rb");
    if (!pf)
    {
        return 0;
    }

    int iRc = fseek(pf, (long)0, SEEK_END);
    if (iRc)
    {
        fclose(pf);
        return 0;
    }

    long position = ftell(pf);
    fclose(pf);

    if (position < 0)
    {
        return 0;
    }

    return (DWORD)position;
}


