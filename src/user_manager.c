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
        // 数据库模块初始化
        ret = sql_init(database_name);
        if( ret != 0 )
        {
            printf("sql_init is fail.\n");
        }
        else
        {
            // 数据库添加表
            ret = sql_add_table(USERINFO);
            if( ret != 0 )
            {
                printf("sql_add_table USERINFO fail.\n");
                ret = -1;
            }
        }
    }

    return ret;
}

// 学生管理模块反初始化
int user_manager_uninit()
{
    // 数据库反初始化
    sql_uninit();
}

// 添加用户
int user_manager_add( UserInfo* sf )
{
    int ret = -1;
    // 初始化sql模块操作对象
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(UserInfo));
    if( ab == NULL )
    {
        printf("user_manager_add malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(UserInfo));

    ab->info = USERINFO;

    int i = 0;
    int enter = 0;
    printf("1、按输入添加用户\n");
    printf("2、按文件导入用户\n");

    scanf("%d", &i);

    char filename[512] = {0};

    do
    {
        enter = 0;
        if( i == 1 )    // 手动输入信息
        {
            UserInfo* si = (UserInfo*)ab->data;
            printf("请依此输入学生学号、姓名、年龄、地址、电话：\n");
            scanf("%d%s%d%s%s", &si->_id, si->_name, &si->_age, si->_addr, si->_tel);
        }
        else if( i == 2 )   // 文件导入信息
        {
            printf("请输入文件路径:\n");
            scanf("%s", filename);
        }
        else    // 输入非法数据
        {
            enter = 1;
            printf("输入错误，请重新输入!\n");
        }
    }while( enter );

    ret = sql_add(ab, (filename[0] == '\0') ? NULL : filename);
    if( ret != 0 ){
        printf("添加失败!\n");
    }else{
        printf("添加成功!\n");
    }

    free(ab);
    return ret;
}

// 删除用户
int user_manager_del( UserInfo* sf )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(UserInfo));
    if( ab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(UserInfo));

    ab->info = USERINFO;

    printf("1、按学号删除\n");
    printf("2、按姓名删除\n");
    printf("0、返回上一层\n");

    int i = 0;
    int enter = 0;
    
    do
    {
        enter = 0;
        scanf("%d", &i);

        if( i == 1 )        // 输入学号删除
        {
            ab->qt._id = 1;
            ab->qt._name = 0;
            ab->qt._all = 0;

            printf("请输入需要删除的用户学号：\n");
            scanf("%d", &(((UserInfo*)ab->data)->_id));
        }
        else if( i == 2 )   // 输入姓名删除
        {
            ab->qt._name = 1;
            ab->qt._id = 0;
            ab->qt._all = 0;

            printf("请输入需要删除的用户姓名：\n");
            scanf("%s", ((UserInfo*)ab->data)->_name);
        }
        else if( i== 0 )
        {
            free(ab);
            return 0;
        }
        else
        {
            enter = 1;
            printf("输入错误，请重新输入!\n");
        }
    }while(enter);
    
    ret = sql_del(ab);
    if( ret != 0 ){
        printf("删除失败，用户不存在或成绩表中的该用户信息未删除!\n");
    }else{
        printf("删除成功!\n");
    }

    free(ab);

    return ret;
}

// 清空用户
int user_manager_clear( UserInfo* sf )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(UserInfo));
    if( ab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(UserInfo));

    ab->info = USERINFO;

    printf("注意！清空后无法恢复，请选择：\n");
    printf("\t1、确定清空\n");
    printf("\t2、返回上层\n");

    int i = 0;
    int enter = 0;
    do
    {
        enter = 0;
        scanf("%d", &i);

        if( i == 1 )            // 确认清空，即删除全部内容
        {
            ab->qt._id = 0;
            ab->qt._name = 0;
            ab->qt._all = 1;
        }
        else if( i == 2 )       // 取消清空操作
        {
            free(ab);
            return 0;
        }
        else
        {
            enter = 1;
            printf("输入错误，请重新输入!\n");
        }
    } while (enter);
    
    ret = sql_del(ab);
    if( ret != 0 ){
        printf("清空失败，请确保成绩表中的用户信息已删除!\n");
    }else{
        printf("清空成功!\n");
    }

    free(ab);

    return ret;
}

// 查询用户
int user_manager_select( UserInfo* sf )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(UserInfo));
    if( ab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(UserInfo));

    ab->info = USERINFO;

    printf("1、查找全部\n");
    printf("2、按学号查找\n");
    printf("3、按姓名查找\n");
    printf("0、返回上一层\n");

    int i = 0;
    int enter = 0;

    do{
        enter = 0;
        scanf("%d", &i);

        if( i == 2 )            // 按学号查找
        {
            ab->qt._id = 1;
            ab->qt._name = 0;
            ab->qt._all = 0;
            printf("请输入学号：\n");
            scanf("%d", &(((UserInfo*)ab->data)->_id));
        }
        else if( i == 3)        // 按姓名查找
        {
            ab->qt._name = 1;
            ab->qt._id = 0;
            ab->qt._all = 0;

            printf("请输入姓名：\n");
            scanf("%s", ((UserInfo*)ab->data)->_name);
        }
        else if( i == 1 )       // 查找全部
        {
            ab->qt._name = 0;
            ab->qt._id = 0;
            ab->qt._all = 1;
        }
        else if( i == 0 )
        {
            free(ab);
            return 0;
        }
        else
        {
            enter = 1;
            printf("输入错误，请重新输入!\n");
        }
    }while(enter);

    SelectInfo* si = sql_select(ab);
    if( !si ){
        printf("查询内容为空!\n");
    }else{
        sql_free(si);
    }

    return ret;
}

// 修改用户
int user_manager_alter( UserInfo* sf )
{
    int ret = -1;
    // 保存修改后的信息
    AddObject *nab = (AddObject *)malloc(sizeof(AddObject) + sizeof(UserInfo));
    if( nab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        return ret;
    }

    // 保存修改前的信息
    AddObject *oab = (AddObject *)malloc(sizeof(AddObject) + sizeof(UserInfo));
    if( oab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        free(nab);
        return ret;
    }

    memset(nab, 0, sizeof(AddObject) + sizeof(UserInfo));
    memset(oab, 0, sizeof(AddObject) + sizeof(UserInfo));

    nab->info = USERINFO;
    oab->info = USERINFO;

    printf("请输入需要修改的用户学号: \n");
    scanf("%d", &(((UserInfo*)oab->data)->_id));

    // 查询
    oab->qt._id = 1;
    oab->qt._name = 0;
    oab->qt._all = 0;

    // 首先查询信息，把原有信息打印出来
    SelectInfo* si = sql_select(oab);
    if( si == NULL )
    {
        printf("用户未找到!\n");
    }
    else
    {
        // 释放查找时的空间
        sql_free(si);

        // 重新输入信息
        printf("请依此输入用户的学号、姓名、年龄、地址、电话：\n");
        scanf("%d%s%d%s%s", &(((UserInfo*)nab->data)->_id), \
                            ((UserInfo*)nab->data)->_name, \
                            &(((UserInfo*)nab->data)->_age), \
                            ((UserInfo*)nab->data)->_addr, \
                            ((UserInfo*)nab->data)->_tel);

        ret = sql_alter(oab, nab);
        if( ret != 0 ){
            printf("修改失败!\n");
        }else{
            printf("修改成功!\n");
        }
    }

    free(oab);
    free(nab);

    return ret;



}


