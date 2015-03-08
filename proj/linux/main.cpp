/// main.cpp
//
#include "BaseID.h"
#include "Manager_if.h"
#include "Factory_if.h"
#include "command_if.h"


/// -------------------------------------------------
/// ǿ������.a���е�һЩԭ���Զ�ִ�е�ȫ�ֹ�������
/// -------------------------------------------------
CPPBUILDUNIT_FORCELINK(vRegOsTaskStubFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegOsSemStubFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCObjectManagerToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCDispatchToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCNotifyToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCControlToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCTimerToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCScheduleToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCStatusToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCResponseToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCModelToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCDataToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCDataMemToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCDataFileToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCDataMySQLToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCTlvToSQLToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCConnectToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCProxyToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCSessionToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCUserToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCSecureToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCAccessToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCCommandToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCHttpServerToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCHttpRequestToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCHttpProcessToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCHttpJsonToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCHttpResponseToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCMonitorToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCAppBaseToFactoryFunc, 0);
CPPBUILDUNIT_FORCELINK(vRegCPythonXToFactoryFunc, 0);

/// -------------------------------------------------
/// ���δ������ԡ�The Cert C Secure Coding Standard��
/// -------------------------------------------------
#define STRING(n)       STRING_AGAIN(n)
#define STRING_AGAIN(n) #n
#define CHARS_TO_READ   255

/// -------------------------------------------------
/// ȫ����ڶ���
/// -------------------------------------------------
IManager *g_piManager = NULL;
ICommand *g_piCommand = NULL;

/*******************************************************
  �� �� ��: DcopServer_Output
  ��    ��: ���
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void DcopServer_Output(const char *info, void *)
{
    if (!info) return;

    printf("%s", info);
    fflush(stdout);
}

/*******************************************************
  �� �� ��: DcopServer_Start
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void DcopServer_Start()
{
    /// ����־��¼
    DebugLogStatus(3);

    /// ���ڴ����
    DebugMemStatus(1);

    printf("-----------------------------------------------\n");
    printf(" Dcop Server Start ... \n");
    printf("-----------------------------------------------\n");
    
    objBase *piBase = objBase::GetInstance();
    if (!piBase)
    {
        printf("  Dcop Server Start Failed! (objBase Kernel Null) \n");
        return;
    }

    g_piManager = (IManager *)piBase->Deploy("../../res/server.xml");
    if (!g_piManager)
    {
        printf("  Dcop Server Start Failed! (Because Deploy Failed) \n");
        return;
    }

    DCOP_QUERY_OBJECT_REFER(ICommand, DCOP_OBJECT_COMMAND, g_piManager, 0, g_piCommand);
    if (!g_piCommand)
    {
        printf("  Dcop Server Start Failed! (Query Command Failed) \n");
        return;
    }

    g_piCommand->Out((LOG_PRINT)DcopServer_Output);
    printf("  Dcop Server Start OK! \n");
}

/*******************************************************
  �� �� ��: DcopServer_Welcome
  ��    ��: ��ӭ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void DcopServer_Welcome(const char *username)
{
    if (!g_piCommand) return;

    g_piCommand->Welcome(username);
}

/*******************************************************
  �� �� ��: DcopServer_Command
  ��    ��: ����
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void DcopServer_Command(const char *command)
{
    if (!g_piCommand) return;

    g_piCommand->Line(command);
}

/*******************************************************
  �� �� ��: DcopServer_Stop
  ��    ��: ֹͣ
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
void DcopServer_Stop()
{
    DCOP_RELEASE_INSTANCE_REFER(0, g_piCommand);

    if (g_piManager)
    {
        delete g_piManager;
        g_piManager = NULL;
    }

    printf("-----------------------------------------------\n");
    printf(" Dcop Server Stop! \n");
    printf("-----------------------------------------------\n");
}

/*******************************************************
  �� �� ��: main
  ��    ��: �������
  ��    ��: 
  ��    ��: 
  ��    ��: 
  �޸ļ�¼: 
 *******************************************************/
int main(int argc, char *argv[])
{
    /// ��ʽ��� : �������������
    DcopServer_Start();

    printf(">>>>>>>>> Enter The Program, ('exit' to exit)! \n");

    char command[CHARS_TO_READ + 1];
    memset(command, 0, sizeof(command));
    bool bExitMode = false;

    DcopServer_Welcome("root");

    for (;;)
    {
        /// ���տ���̨���������ַ���
        /// scanf("%"STRING(CHARS_TO_READ)"s", command);
        if (fgets(command, sizeof(command), stdin) == NULL)
        {
            continue;
        }

        /// �˳�ģʽ�£��ж��Ƿ�ȷ�ϣ�����ͻָ�������ģʽ
        if (bExitMode)
        {
            if (!strcmp(command, "Y\n"))
            {
                break;
            }

            bExitMode = false;
            DcopServer_Command("\n");
            continue;
        }

        /// �жϽ��յ������Ƿ����˳�����
        if (!strcmp(command, "exit\n"))
        {
            bExitMode = true;
            printf("Are you sure to exit? (Y/N)");
            continue;
        }

        /// ��������������
        DcopServer_Command(command);
    }

    printf(">>>>>>>>> Exit The Program, Bye! \n");
    DcopServer_Stop();

    return 0;
}
