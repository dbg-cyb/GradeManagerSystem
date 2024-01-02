#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "user_manager.h"
#include "sql.h"

// 学生管理系统初始化
int user_manager_init(const char *database_name)
{
    int ret = -1;
    if( database_name )
    {
        ret = sql_init(database_name);
        if( ret < 0 )
        {
            printf("sql_init is fail.\n");
        }
        else
        {
            ret = sql_add_table(STUINFO);
            if( ret < 0 )
            {
                printf("sql_add_table STUINFO fail.\n");
            }
        }
    }

    return ret;
}

// 学生管理系统反初始化
int user_manager_uninit()
{
    sql_uninit();
}

// 添加用户
int user_manager_add( StuInfo* sf )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(StuInfo));
    if( ab == NULL )
    {
        printf("user_manager_add malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(StuInfo));

    ab->info = STUINFO;

    int i = 0;
    printf("1、按输入添加用户\n");
    printf("2、按文件导入用户\n");

    scanf("%d", &i);

    char filename[512] = {0};

    do
    {
        if( i == 1 )
        {
            StuInfo* si = (StuInfo*)ab->data;
            printf("请依此输入学生学号、姓名、年龄、地址、电话：\n");
            scanf("%d%s%d%s%s", &si->_id, si->_name, &si->_age, si->_addr, si->_tel);
        }
        else if( i == 2 )
        {
            printf("请输入文件路径:\n");
            scanf("%s", filename);
        }
        else
        {
            printf("输入错误，请重新输入!\n");
        }
    }while( i != 1 && i != 2 );

    ret = sql_add(ab, filename);
    if( ret != 0 )
    {
        printf("添加失败!\n");
    }
    else
    {
        printf("添加成功!\n");
    }

    free(ab);
    return ret;
}

// 删除用户
int user_manager_del( StuInfo* sf )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(StuInfo));
    if( ab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(StuInfo));

    ab->info = STUINFO;

    printf("1、按学号删除\n");
    printf("2、按姓名删除\n");
    printf("0、返回上一层\n");

    int i = 0;
    do
    {
        scanf("%d", &i);

        if( i == 1 )
        {
            ab->qt._id = 1;
            ab->qt._name = 0;

            printf("请输入学号：\n");
            scanf("%d", &(((StuInfo*)ab->data)->_id));
        }
        else if( i == 2 )
        {
            ab->qt._name = 1;
            ab->qt._id = 0;

            printf("请输入姓名：\n");
            scanf("%s", ((StuInfo*)ab->data)->_name);
        }
        else if( i== 0 )
        {
            free(ab);
            return 0;
        }
        else
        {
            printf("输入错误，请重新输入!\n");
        }
    } while (i != 1 && i != 2);
    
    ret = sql_del(ab);
    if( ret < 0 )
    {
        printf("删除失败!\n");
    }
    else
    {
        printf("删除成功!\n");
    }

    free(ab);

    return ret;
}

// 清空用户
int user_manager_clear( StuInfo* sf )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(StuInfo));
    if( ab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(StuInfo));

    ab->info = STUINFO;

    printf("注意！清空后无法恢复，请选择：\n");
    printf("\t1、确定清空\n");
    printf("\t2、返回上层\n");

    int i = 0;
    do
    {
        scanf("%d", &i);

        if( i == 1 )
        {
            ab->qt._id = 0;
            ab->qt._name = 0;
            ab->qt._all = 1;
        }
        else if( i == 2 )
        {
            free(ab);
            return 0;
        }
        else
        {
            printf("输入错误，请重新输入!\n");
        }
    } while (i != 1 && i != 2);
    
    ret = sql_del(ab);
    if( ret < 0 )
    {
        printf("清空失败!\n");
    }
    else
    {
        printf("清空成功!\n");
    }

    free(ab);

    return ret;
}

// 查询用户
int user_manager_select( StuInfo* sf )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(StuInfo));
    if( ab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(StuInfo));

    ab->info = STUINFO;

    printf("1、查找全部\n");
    printf("2、按学号查找\n");
    printf("3、按姓名查找\n");
    printf("0、返回上一层\n");

    int i = 0;
    do{
        scanf("%d", &i);

        if( i == 2 )
        {
            ab->qt._id = 1;
            ab->qt._name = 0;
            printf("请输入学号：\n");
            scanf("%d", &(((StuInfo*)ab->data)->_id));
        }
        else if( i == 3)
        {
            ab->qt._name = 1;
            ab->qt._id = 0;

            printf("请输入姓名：\n");
            scanf("%s", ((StuInfo*)ab->data)->_name);
        }
        else if( i == 1 )
        {
            ab->qt._name = 0;
            ab->qt._id = 0;
        }
        else if( i == 0 )
        {
            free(ab);
            return 0;
        }
        else
        {
            printf("输入错误，请重新输入!\n");
        }
    }while(i != 1 && i != 2 && i !=  3);

    sql_select(ab);

    return ret;
}

// 修改用户
int user_manager_alter( StuInfo* sf )
{
    int ret = -1;
    AddObject *nab = (AddObject *)malloc(sizeof(AddObject) + sizeof(StuInfo));
    if( nab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        return ret;
    }

    AddObject *oab = (AddObject *)malloc(sizeof(AddObject) + sizeof(StuInfo));
    if( oab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        free(nab);
        return ret;
    }

    memset(nab, 0, sizeof(AddObject) + sizeof(StuInfo));
    memset(oab, 0, sizeof(AddObject) + sizeof(StuInfo));

    nab->info = STUINFO;
    oab->info = STUINFO;

    printf("请输入需要修改用户的学号:\n");
    scanf("%d", &(((StuInfo*)oab->data)->_id));

    // 查询
    oab->qt._id = 1;
    oab->qt._name = 0;
    oab->qt._all = 0;

    sql_select(oab);

    printf("请依此输入用户的学号、姓名、年龄、地址、电话：\n");
    scanf("%d%s%d%s%s", &(((StuInfo*)nab->data)->_id), \
                        ((StuInfo*)nab->data)->_name, \
                        &(((StuInfo*)nab->data)->_age), \
                        ((StuInfo*)nab->data)->_addr, \
                        ((StuInfo*)nab->data)->_tel);

    sql_alter(oab, nab);

    free(oab);
    free(nab);

    return ret;



}


