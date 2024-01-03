#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "subject_manager.h"
#include "sql.h"

// 课程管理模块初始化
int subject_manager_init(const char *database_name)
{
    int ret = -1;
    if( database_name )
    {
        // 初始化sql模块
        ret = sql_init(database_name);
        if( ret != 0 )
        {
            printf("sql_init is fail.\n");
        }
        else
        {
            // 添加表
            ret = sql_add_table(CLASSINFO);
            if( ret != 0 )
            {
                printf("sql_add_table CLASSINFO fail.\n");
                ret = -1;
            }
        }
    }

    return ret;
}

// 课程管理模块反初始化
int subject_manager_uninit()
{
    // sql模块反初始化
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
    int enter = 0;
    char filename[512] = {0};

    printf("1、按输入添加课程信息\n");
    printf("2、按文件导入课程信息\n");

    do
    {
        enter = 0;
        scanf("%d", &i);

        if( i == 1 )    // 手动输入添加课程信息
        {
            TimeTable* tt = (TimeTable*)ab->data;
            printf("请依此输入课程号、课程名字、任课老师：\n");
            scanf("%d%s%s", &tt->_kcid, tt->_name, tt->_teacher);
        }
        else if( i == 2 )   // 从文件中导入信息
        {
            printf("请输入文件路径:\n");
            scanf("%s", filename);
        }
        else
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
    int enter = 0;

    do
    {
        enter = 0;
        scanf("%d", &i);

        if( i == 1 )    // 按课程号删除
        {
            ab->qt._kcid = 1;
            ab->qt._subject = 0;
            ab->qt._teacher = 0;
            ab->qt._all = 0;

            printf("请输入课程号：\n");
            scanf("%d", &(((TimeTable*)ab->data)->_kcid));
        }
        else if( i == 2 )   // 按科目名删除
        {
            ab->qt._kcid = 0;
            ab->qt._subject = 1;
            ab->qt._teacher = 0;
            ab->qt._all = 0;

            printf("请输入课程名字：\n");
            scanf("%s", ((TimeTable*)ab->data)->_name);
        }
        else if( i == 3 )   // 按任课老师删除
        {
            ab->qt._kcid = 0;
            ab->qt._subject = 0;
            ab->qt._teacher = 1;
            ab->qt._all = 0;

            printf("请输入任课老师：\n");
            scanf("%s", ((TimeTable*)ab->data)->_teacher);
        }
        else if( i== 0 )    // 返回课程管理菜单界面
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
        printf("删除失败，信息不存在或成绩表中的该课程信息未删除!\n");
    }else{
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
    int enter = 0;

    do
    {
        enter = 0;
        scanf("%d", &i);

        if( i == 1 )    // 清空即删除全部
        {
            ab->qt._kcid = 0;
            ab->qt._subject = 0;
            ab->qt._name = 0;
            ab->qt._all = 1;
        }
        else if( i == 2 )   // 返回上一层
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
        printf("清空失败，请确保成绩表中的课程信息已删除!\n");
    }else{
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
    int enter = 0;

    do{
        enter = 0;
        scanf("%d", &i);

        if( i == 1 )    // 查找全部
        {
            ab->qt._kcid = 0;
            ab->qt._subject = 0;
            ab->qt._teacher = 0;
            ab->qt._all = 1;
        }
        else if( i == 2 )   // 按课程号查找
        {
            ab->qt._kcid = 1;
            ab->qt._subject = 0;
            ab->qt._teacher = 0;
            ab->qt._all = 0;

            printf("请输入课程号：\n");
            scanf("%d", &(((TimeTable*)ab->data)->_kcid));
        }
        else if( i == 3)    // 按科目名查找
        {
            ab->qt._kcid = 0;
            ab->qt._subject = 1;
            ab->qt._teacher = 0;
            ab->qt._all = 0;

            printf("请输入课程名字：\n");
            scanf("%s", ((TimeTable*)ab->data)->_name);
        }
        else if( i == 4 )   // 按任课老师查找
        {
            ab->qt._kcid = 0;
            ab->qt._subject = 0;
            ab->qt._teacher = 1;
            ab->qt._all = 0;

            printf("请输入任课老师：\n");
            scanf("%s", ((TimeTable*)ab->data)->_teacher);
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
    }else
    {
        sql_free(si);
    }

    return ret;
}

// 修改课程信息
int subject_manager_alter( TimeTable* sf )
{
    int ret = -1;
    // 保存修改后的数据
    AddObject *nab = (AddObject *)malloc(sizeof(AddObject) + sizeof(TimeTable));
    if( nab == NULL )
    {
        printf("user_manager_select malloc is fail: %s\n", strerror(errno));
        return ret;
    }

    // 保存修改前的数据
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
    int enter = 0;

    printf("1、按课程号查找修改\n");
    printf("2、按课程名字查找修改\n");
    printf("3、按任课老师查找修改\n");
    printf("4、返回上一层\n");

    do{
        enter = 0;
        scanf("%d", &i);

        if( i == 1 )    // 按课程号查找修改
        {
            oab->qt._kcid = 1;
            oab->qt._subject = 0;
            oab->qt._teacher = 0;
            oab->qt._all = 0;

            printf("请输入课程号:\n");
            scanf("%d", &(((TimeTable*)oab->data)->_kcid));

            break;
        }
        else if( i == 2 )   // 按科目名查找修改
        {
            oab->qt._kcid = 0;
            oab->qt._subject = 1;
            oab->qt._teacher = 0;
            oab->qt._all = 0;

            printf("请输入课程名字:\n");
            scanf("%s", ((TimeTable*)oab->data)->_name);

            break;
        }
        else if( i == 3 )   // 按任课老师查找修改
        {
            oab->qt._kcid = 0;
            oab->qt._subject = 0;
            oab->qt._teacher = 1;
            oab->qt._all = 0;

            printf("请输入任课老师:\n");
            scanf("%s", ((TimeTable*)oab->data)->_teacher);

            break;
        }
        else if( i == 4 )   // 返回上一层
        {
            free(oab);
            free(nab);
            return 0;
        }
        else
        {
            enter = 1;
            printf("输入错误，请重新输入!\n");
            continue;
        }
    }while(enter);
    
    // 先显示原有信息
    SelectInfo* si = sql_select(oab);
    if( si == NULL )
    {
        printf("课程信息未找到!\n");
    }
    else
    {
        // 释放查找时的空间
        sql_free(si);
        
        printf("请依此输入课程信息：课程号、课程名字、任课老师\n");
        scanf("%d%s%s", &(((TimeTable*)nab->data)->_kcid), \
                            ((TimeTable*)nab->data)->_name, \
                            ((TimeTable*)nab->data)->_teacher);

        sql_alter(oab, nab);
    }

    free(oab);
    free(nab);

    return ret;

}