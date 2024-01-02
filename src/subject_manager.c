#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "subject_manager.h"
#include "sql.h"

// 课程管理系统初始化
int subject_manager_init(const char *database_name)
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
            ret = sql_add_table(CLASSINFO);
            if( ret < 0 )
            {
                printf("sql_add_table CLASSINFO fail.\n");
            }
        }
    }

    return ret;
}

// 课程管理系统反初始化
int subject_manager_uninit()
{
    sql_uninit();
}

// 添加课程信息
int subject_manager_add( TimeTable* sf )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(TimeTable));
    if( ab == NULL )
    {
        printf("subject_manager_add malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(TimeTable));

    ab->info = CLASSINFO;

    int i = 0;
    printf("1、按输入添加课程信息\n");
    printf("2、按文件导入课程信息\n");

    scanf("%d", &i);

    char filename[512] = {0};

    do
    {
        if( i == 1 )
        {
            TimeTable* tt = (TimeTable*)ab->data;
            printf("请依此输入课程号、课程名字、任课老师：\n");
            scanf("%d%s%s", &tt->_kcid, tt->_name, tt->_teacher);
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

// 删除课程信息
int subject_manager_del( TimeTable* sf )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(TimeTable));
    if( ab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(TimeTable));

    ab->info = CLASSINFO;

    printf("1、按课程号删除\n");
    printf("2、按课程名字删除\n");
    printf("3、按任课老师删除\n");
    printf("0、返回上一层\n");

    int i = 0;
    do
    {
        scanf("%d", &i);

        if( i == 1 )
        {
            ab->qt._kcid = 1;
            ab->qt._subject = 0;
            ab->qt._teacher = 0;

            printf("请输入课程号：\n");
            scanf("%d", &(((TimeTable*)ab->data)->_kcid));
        }
        else if( i == 2 )
        {
            ab->qt._kcid = 0;
            ab->qt._subject = 1;
            ab->qt._teacher = 0;

            printf("请输入课程名字：\n");
            scanf("%s", ((TimeTable*)ab->data)->_name);
        }
        else if( i == 3 )
        {
            ab->qt._kcid = 0;
            ab->qt._subject = 0;
            ab->qt._teacher = 1;

            printf("请输入任课老师：\n");
            scanf("%s", ((TimeTable*)ab->data)->_teacher);
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
    } while (i != 1 && i != 2 && i != 3);
    
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

// 清空课程信息
int subject_manager_clear( TimeTable* sf )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(TimeTable));
    if( ab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(TimeTable));

    ab->info = CLASSINFO;

    printf("注意！清空后无法恢复，请选择：\n");
    printf("\t1、确定清空\n");
    printf("\t2、返回上层\n");

    int i = 0;
    do
    {
        scanf("%d", &i);

        if( i == 1 )
        {
            ab->qt._kcid = 0;
            ab->qt._subject = 0;
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

// 查询课程信息
int subject_manager_select( TimeTable* sf )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(TimeTable));
    if( ab == NULL )
    {
        printf("user_manager_select malloc is fail: %s\n", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(TimeTable));

    ab->info = CLASSINFO;

    printf("1、查找全部\n");
    printf("2、按课程号查找\n");
    printf("3、按课程名字查找\n");
    printf("4、按任课老师查找\n");
    printf("0、返回上一层\n");

    int i = 0;
    do{
        scanf("%d", &i);

        if( i == 1 )
        {
            ab->qt._kcid = 0;
            ab->qt._subject = 0;
            ab->qt._teacher = 0;
            ab->qt._all = 1;
        }
        else if( i == 2 )
        {
            ab->qt._kcid = 1;
            ab->qt._subject = 0;
            ab->qt._teacher = 0;
            ab->qt._all = 0;

            printf("请输入课程号：\n");
            scanf("%d", &(((TimeTable*)ab->data)->_kcid));
        }
        else if( i == 3)
        {
            ab->qt._kcid = 0;
            ab->qt._subject = 1;
            ab->qt._teacher = 0;
            ab->qt._all = 0;

            printf("请输入课程名字：\n");
            scanf("%s", ((TimeTable*)ab->data)->_name);
        }
        else if( i == 4 )
        {
            ab->qt._kcid = 0;
            ab->qt._subject = 0;
            ab->qt._teacher = 1;
            ab->qt._all = 0;

            printf("请输入任课老师：\n");
            scanf("%s", ((TimeTable*)ab->data)->_teacher);
        }
        else
        {
            printf("输入错误，请重新输入!\n");
        }
    }while(i != 1 && i != 2 && i !=  3 && i != 4);

    sql_select(ab);

    return ret;
}

// 修改课程信息
int subject_manager_alter( TimeTable* sf )
{
    int ret = -1;
    AddObject *nab = (AddObject *)malloc(sizeof(AddObject) + sizeof(TimeTable));
    if( nab == NULL )
    {
        printf("user_manager_select malloc is fail: %s\n", strerror(errno));
        return ret;
    }

    AddObject *oab = (AddObject *)malloc(sizeof(AddObject) + sizeof(TimeTable));
    if( oab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        free(nab);
        return ret;
    }

    memset(nab, 0, sizeof(AddObject) + sizeof(TimeTable));
    memset(oab, 0, sizeof(AddObject) + sizeof(TimeTable));

    nab->info = CLASSINFO;
    oab->info = CLASSINFO;

    int i = 0;
    printf("1、按课程号查找修改\n");
    printf("2、按课程名字查找修改\n");
    printf("3、按任课老师查找修改\n");
    printf("4、返回上一层\n");

    do{
        scanf("%d", &i);
        if( i == 1 )
        {
            oab->qt._kcid = 1;
            oab->qt._subject = 0;
            oab->qt._teacher = 0;
            oab->qt._all = 0;

            printf("请输入课程号:\n");
            scanf("%d", &(((TimeTable*)oab->data)->_kcid));

            break;
        }
        else if( i == 2 )
        {
            oab->qt._kcid = 0;
            oab->qt._subject = 1;
            oab->qt._teacher = 0;
            oab->qt._all = 0;

            printf("请输入课程名字:\n");
            scanf("%s", ((TimeTable*)oab->data)->_name);

            break;
        }
        else if( i == 3 )
        {
            oab->qt._kcid = 0;
            oab->qt._subject = 1;
            oab->qt._teacher = 0;
            oab->qt._all = 0;

            printf("请输入任课老师:\n");
            scanf("%s", ((TimeTable*)oab->data)->_teacher);

            break;
        }
        else if( i == 4 )
        {
            return 0;
        }
        else
        {
            printf("输入错误，请重新输入!\n");
            continue;
        }
    }while(1);

    sql_select(oab);

    printf("请依此输入课程信息：课程号、课程名字、任课老师\n");
    scanf("%d%s%s", &(((TimeTable*)nab->data)->_kcid), \
                        ((TimeTable*)nab->data)->_name, \
                        ((TimeTable*)nab->data)->_teacher);

    sql_alter(oab, nab);

    free(oab);
    free(nab);

    return ret;

}