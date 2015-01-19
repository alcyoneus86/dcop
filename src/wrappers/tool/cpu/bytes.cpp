/// -------------------------------------------------
/// bytes.cpp : �ֽ���ش���
/// -------------------------------------------------
/// Copyright (c) 2015, Wang Yujia <combo.xy@163.com>
/// All rights reserved.
/// -------------------------------------------------

#include "cpu/bytes.h"


/// �ֽ�������
enum BYTE_SORT_TYPE
{
    BYTE_SORT_TYPE_LITTLE_ENDIAN = 0,
    BYTE_SORT_TYPE_BIG_ENDIAN
};


/*******************************************************
  �� �� ��: Bytes_GetHost
  ��    ��: ��õ�ǰ����CPU���ֽ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
inline BYTE_SORT_TYPE Bytes_GetHost()
{
    DWORD dwTmp = 0x00000001L;

    if (*(BYTE *)&dwTmp)
    {
        return BYTE_SORT_TYPE_LITTLE_ENDIAN;
    }

    return BYTE_SORT_TYPE_BIG_ENDIAN;
}

/*******************************************************
  �� �� ��: Bytes_GetWord
  ��    ��: ��BUF�л�ȡ2�ֽ�(�ֽ���ת��)
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
WORD Bytes_GetWord(const BYTE *cpbyBuf)
{
    if (Bytes_GetHost() == BYTE_SORT_TYPE_BIG_ENDIAN)
    {
        return *(WORD *)cpbyBuf;
    }

    WORD wRc;
    BYTE *pbyTmp = (BYTE *)&wRc;
    pbyTmp[1] = cpbyBuf[0];
    pbyTmp[0] = cpbyBuf[1];

    return wRc;
}

/*******************************************************
  �� �� ��: Bytes_SetWord
  ��    ��: ��2�ֽ����õ�BUF��(�ֽ���ת��)
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void Bytes_SetWord(BYTE *pbyBuf, WORD wValue)
{
    if (Bytes_GetHost() == BYTE_SORT_TYPE_BIG_ENDIAN)
    {
        *(WORD *)pbyBuf = wValue;
        return;
    }

    BYTE *pbyTmp = (BYTE *)&wValue;
    pbyBuf[1] = pbyTmp[0];
    pbyBuf[0] = pbyTmp[1];
}

/*******************************************************
  �� �� ��: Bytes_GetDword
  ��    ��: ��BUF�л�ȡ4�ֽ�(�ֽ���ת��)
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD Bytes_GetDword(const BYTE *cpbyBuf)
{
    if (Bytes_GetHost() == BYTE_SORT_TYPE_BIG_ENDIAN)
    {
        return *(DWORD *)cpbyBuf;
    }

    DWORD dwRc;
    BYTE *pbyTmp = (BYTE *)&dwRc;
    pbyTmp[3] = cpbyBuf[0];
    pbyTmp[2] = cpbyBuf[1];
    pbyTmp[1] = cpbyBuf[2];
    pbyTmp[0] = cpbyBuf[3];

    return dwRc;
}

/*******************************************************
  �� �� ��: Bytes_SetDword
  ��    ��: ��4�ֽ����õ�BUF��(�ֽ���ת��)
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void Bytes_SetDword(BYTE *pbyBuf, DWORD dwValue)
{
    if (Bytes_GetHost() == BYTE_SORT_TYPE_BIG_ENDIAN)
    {
        *(DWORD *)pbyBuf = dwValue;
        return;
    }

    BYTE *pbyTmp = (BYTE *)&dwValue;
    pbyBuf[3] = pbyTmp[0];
    pbyBuf[2] = pbyTmp[1];
    pbyBuf[1] = pbyTmp[2];
    pbyBuf[0] = pbyTmp[3];
}

/*******************************************************
  �� �� ��: Bytes_GetDwordValue
  ��    ��: ���ݳ��ȴ�BUF�л�ȡDWORDֵ(���ȿ���Ϊ1��2��4������ֻȡǰ4���ֽ�)
  ��    ��: pbyBuf (������Ƿ�ΪNULL���������ڵ���ǰ���)
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
DWORD Bytes_GetDwordValue(const BYTE *cpbyBuf, DWORD dwBufLen)
{
    if (dwBufLen == sizeof(BYTE))
    {
        return (DWORD)*cpbyBuf;
    }
    else if ((dwBufLen >= sizeof(WORD)) && (dwBufLen < sizeof(DWORD)))
    {
        return (DWORD)Bytes_GetWord(cpbyBuf);
    }
    else
    {
        return Bytes_GetDword(cpbyBuf);
    }
}

/*******************************************************
  �� �� ��: Bytes_SetDwordValue
  ��    ��: ���ݳ�������DWORDֵ��BUF��(���ȿ���Ϊ1��2��4������ֻдǰ4���ֽ�)
  ��    ��: pbyBuf (������Ƿ�ΪNULL���������ڵ���ǰ���)
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void Bytes_SetDwordValue(DWORD dwValue, BYTE *pbyBuf, DWORD dwBufLen)
{
    if (dwBufLen == sizeof(BYTE))
    {
        *(BYTE *)pbyBuf = (BYTE)dwValue;
    }
    else if ((dwBufLen >= sizeof(WORD)) && (dwBufLen < sizeof(DWORD)))
    {
        Bytes_SetWord(pbyBuf, (WORD)dwValue);
    }
    else
    {
        Bytes_SetDword(pbyBuf, dwValue);
    }
}

/*******************************************************
  �� �� ��: Bytes_ChangeOrderByRule
  ��    ��: ��BUF�е����ݰ�������������ת��
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void Bytes_ChangeOrderByRule(const BYTES_CHANGE_RULE *adwRule,
                    DWORD dwRuleCount,
                    void *pBuf,
                    DWORD dwBufLen)
{
    if (!adwRule || !dwRuleCount || !pBuf || !dwBufLen)
    {
        return;
    }

    /// ���������ֽ���
    DWORD dwTest = 1;
    if (0 == *(BYTE *)&dwTest)
    {
        /// ���Խ��Ϊ����ֽ���(�����ֽ���)������ת��
        return;
    }

    /// 'awRule'��3���ֽ�Ϊλ�ã���1���ֽ�Ϊ�ֽ���(2�ֽڻ���4�ֽ�)
    for (DWORD i = 0; i < dwRuleCount; ++i)
    {
        DWORD dwPos = (DWORD)(adwRule[i].pos);
        if (dwPos >= dwBufLen)
        {
            /// λ�ó���BUF����
            continue;
        }

        BYTE bySize = (BYTE)(adwRule[i].size);
        switch (bySize)
        {
            case sizeof(DWORD):
            {
                DWORD dwTemp = *(DWORD *)((BYTE *)pBuf + dwPos);
                *((BYTE *)pBuf + dwPos)     = *((BYTE *)&dwTemp + 3);
                *((BYTE *)pBuf + dwPos + 1) = *((BYTE *)&dwTemp + 2);
                *((BYTE *)pBuf + dwPos + 2) = *((BYTE *)&dwTemp + 1);
                *((BYTE *)pBuf + dwPos + 3) = *((BYTE *)&dwTemp);
            }
                break;
            case sizeof(WORD):
            {
                WORD wTemp = *(WORD *)((BYTE *)pBuf + dwPos);
                *((BYTE *)pBuf + dwPos)     = *((BYTE *)&wTemp + 1);
                *((BYTE *)pBuf + dwPos + 1) = *((BYTE *)&wTemp);
            }
                break;
            default:
                break;
        }
    }
}

