#include <stdio.h>

#include "mem/FixMemPool.h"
#include <sys/ipc.h>
#include "test/ShmTableTest.h"
#include "test/MD5Test.h"
#include <string.h>
#include <stdlib.h>
#include <crypt/Base64.h>
#include "test/TeaTest.h"
#include "test/JsonTest.h"
#include "test/UriTest.h"
#include "test/CommandTest.h"

void printFixMemPoolInfo(LPFIXMEMPOOL pstFixMemPool)
{
    int i;
    int iFree;
    char* pszMem;

    if (NULL == pstFixMemPool) return;

    printf("pool freeNum:%d, block num:%d, block size:%d\n", pstFixMemPool->dwFreeNum, pstFixMemPool->dwBlockNum, pstFixMemPool->dwBlockSize);

    for (i = 0; i < pstFixMemPool->dwBlockNum; i++)
    {
        if (pstFixMemPool->piBlock[i] & 0x80000000)
        {
            printf("0x%x ", pstFixMemPool->piBlock[i]);
        }
        else
        {
            printf("%d ", pstFixMemPool->piBlock[i]);
        }
    }
    printf("\n");

    for (i = 0; i < pstFixMemPool->dwBlockNum; i++)
    {
        iFree = 0;
        pszMem = fix_mem_pool_at(pstFixMemPool, i, &iFree);

        if (pszMem == NULL || iFree == 1) continue;
        printf("%s ", pszMem);
    }
    printf("\n");
}

void printMemChar(char* aszMem[], int iNum)
{
    int i;

    for (i = 0; i < iNum; i++)
    {
        printf("%s ", aszMem[i]);
    }

    printf("\n");
}

void testFixMemPool()
{
    char szMemData[1024];
    FIXMEMPOOL stFixMemPool;
    int iRet;
    int aiIndex[5];
    char* aszMem[5];
    int i;

    iRet = init_fix_mem_pool(&stFixMemPool, szMemData, sizeof(szMemData), 32);

    if (iRet != 0)
    {
        printf("error to init fix mem pool:%d\n", iRet);
        return;
    }

    printFixMemPoolInfo(&stFixMemPool);

    for (i = 0; i < 5; i++)
    {
        aszMem[i] = fix_mem_pool_malloc(&stFixMemPool, &aiIndex[i]);
        if (aszMem[i] == NULL)
        {
            printf("error to allocate mem for:%d\n", i);
            return;
        }

        sprintf(aszMem[i], "Hello_%d", i);

        printf("i=%d, memIndex=%d\n", i, aiIndex[i]);
    }

    printFixMemPoolInfo(&stFixMemPool);
    printMemChar(aszMem, 5);

    iRet = fix_mem_pool_free(&stFixMemPool, aiIndex[3]);

    if (iRet != 0)
    {
        printf("1.error to free mem:%d\n", iRet);
    }

    printFixMemPoolInfo(&stFixMemPool);

    iRet = fix_mem_pool_free(&stFixMemPool, aiIndex[3]);

    if (iRet != 0)
    {
        printf("2.error to free mem:%d\n", iRet);
    }

    printFixMemPoolInfo(&stFixMemPool);

    aszMem[3] = fix_mem_pool_malloc(&stFixMemPool, &aiIndex[3]);
    if (aszMem[3] == NULL)
    {
        printf("error to allocate mem for %d", 3);
        return;
    }

    sprintf(aszMem[3], "New_%d", 3);

    printFixMemPoolInfo(&stFixMemPool);
    printMemChar(aszMem, 5);

    for (i = 0; i < 5; i++)
    {
        iRet = fix_mem_pool_free(&stFixMemPool, aiIndex[i]);
        if (iRet != 0)
        {
            printf("error to free mem for:%d, cause:%d\n", i, iRet);
            return;
        }
    }

    printFixMemPoolInfo(&stFixMemPool);
}

int main(int argc, const char* argv[])
{
    char chType;
    //int chFlag = 0664 | IPC_CREAT;
    //int iFlag = chFlag & (~IPC_CREAT);
    //size_t t = 0xFFFFFFFF;

    //printf("0x%x,0x%x, 0x%x, %d, 0x%x, 0x%zd, %d\n", IPC_CREAT, iFlag, chFlag, iFlag & IPC_CREAT, 0664, t, sizeof(size_t));

    if (argc <= 1)
    {
        printf("not enough parameter!need command type!\n");
        return -1;
    }

    chType = *argv[1];

    switch (chType)
    {
    case 'm':   /* 内存池测试 */
        testFixMemPool();
        break;
    case 't':
        {
            int iType = argc > 2 ? *argv[2] : 'p';
            int iID = argc > 3 ? atoi(argv[3]) : 1;
            test_shm_table(iType, iID);
        }
        break;
    case '5':
        {
            if (argc > 2)
            {
                test_md5(argv[2], strlen(argv[2]));
            }
            else
            {
                char xx[10];
                int i;
                for (i = 0; i < 10; i++)
                {
                    xx[i] = i;
                }
                test_md5(xx, 10);
            }
            break;
        }
    case '6':
        {
            if (argc > 2)
            {
                char output[1024];
                char buf[1024];
                int len = base64_encode(output, argv[2], strlen(argv[2]));
                printf("%d\n%s\n", len, output);

                len = base64_decode(buf, output, len);
                printf("%d\n%s\n", len, buf);
            }
            else
            {
                char xx[10];
                int i;
                for (i = 0; i < 10; i++)
                {
                    xx[i] = i;
                }

                char output[1024];
                char buf[1024];
                int len = base64_encode(output, xx, 10);
                printf("%d\n%s\n", len, output);

                len = base64_decode(buf, output, len);
                printf("%d\n", len);

                for (i = 0; i < len; i++)
                {
                    printf("%d ", buf[i]);
                }
                printf("\n");
            }
            break;
        }
    case '7':
        test_tea();
        break;
    case '8':
        if (argc <= 2)
        {
            printf("not enough parameter!need json file!\n");
            return -1;
        }
        test_json(argv[2]);
        break;
    case '9':
        test_uri();
        break;
    case 'a':
        if (argc <= 2)
        {
            printf("not enough parameter!need command file!\n");
            return -1;
        }
        test_command_parse(argv[2]);
        break;
    }

    return 0;
}

