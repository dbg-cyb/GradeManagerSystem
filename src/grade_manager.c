#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "grade_manager.h"
#include "sql.h"

// 成绩管理模块初始化
int grade_manager_init(const char *database_name)
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
            // 数据库添加成绩表
            ret = sql_add_table(GRADEINFO);
            if( ret != 0 )
            {
                printf("sql_add_table GRADEINFO fail.\n");
                ret = -1;
            }
        }
    }

    return ret;
}

// 成绩管理模块反初始化
int grade_manager_uninit()
{
    // sql模块反初始化
    sql_uninit();
}

// 添加成绩信息
int grade_manager_add( GradeTable* gt )
{
    int ret = -1;
    // 实例化对象
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(GradeTable));
    if( ab == NULL )
    {
        printf("subject_manager_add malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(GradeTable));

    ab->info = GRADEINFO;

    int i = 0;
    int enter = 0;
    char filename[512] = {0};

    printf("1、按输入添加成绩信息\n");
    printf("2、按文件导入成绩信息\n");

    do
    {
        enter = 0;
        scanf("%d", &i);

        if( i == 1 )        // 手动输入数据添加
        {
            GradeTable* gt = (GradeTable*)ab->data;
            printf("请依此输入学号、课程号、学生成绩：\n");
            scanf("%d%d%f", &gt->_id, &gt->_kch, &gt->_score);
        }
        else if( i == 2 )   // 通过文件导入数据
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

    // 先查找看是否查找该信息
    ab->qt._id = 1;
    ab->qt._kcid = 1;
    ab->qt._subject = 0;
    ab->qt._all = 0;

    // 先查询判断信息是否已经存在
    SelectInfo* si = sql_select(ab);
    if( si )
    {
        // 已经存在信息，无法添加
        printf("该用户成绩信息已存在！\n");
        sql_free(si);
    }
    else
    {
        // 信息未存在，可以添加
        ret = sql_add(ab, (filename[0] == '\0') ? NULL : filename);
        if( ret != 0 ){
            printf("添加失败，请确保该用户信息或者课程信息存在!\n");
        }else{
            printf("添加成功!\n");
        }
    }

    free(ab);
    return ret;
}

// 删除成绩信息
int grade_manager_del( GradeTable* gt )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(GradeTable));
    if( ab == NULL )
    {
        printf("grade_manager_del malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(GradeTable));

    ab->info = GRADEINFO;

LABLE:
    printf("1、批量删除\n");
    printf("2、选择删除\n");
    printf("0、返回上一层\n");

    int i = 0;
    int enter_1 = 0;

    do
    {
        enter_1 = 0;
        scanf("%d", &i);

        if( i == 1 )       // 批量删除，删除某个学号、课程号、课程名的所有信息
        {
            printf("1、按学号删除\n");
            //printf("2、按课程号删除\n");  // 课程号删除不需要
            printf("2、按科目名删除\n");
            printf("0、返回上一层\n");

            int j = 0;
            int enter_2 = 0;
            
            do
            {
                enter_2 = 0;
                scanf("%d", &j);

                if( j == 1 )        // 按学号删除
                {
                    ab->qt._id = 1;
                    ab->qt._subject = 0;
                    ab->qt._kcid = 0;
                    ab->qt._all = 0;

                    printf("请输入用户学号：\n");
                    scanf("%d", &(((GradeTable*)ab->data)->_id));
                }
                else if( j == 2 )   // 按科目名删除
                {
                    ab->qt._id = 0;
                    ab->qt._subject = 1;
                    ab->qt._kcid = 0;
                    ab->qt._all = 0;

                    printf("请输入科目名字：\n");
                    scanf("%s", ((GradeTable*)ab->data)->_subject);
                }
                else if( j == 0 )   // 返回删除选择项
                {
                    goto LABLE;
                }
                else
                {
                    enter_2 = 1;
                    printf("输入错误，请重新输入!\n\n");
                }
            } while (enter_2);
        }
        else if( i == 2 ) // 删除某个用户的某个科目的信息
        {
            printf("请输入需要删除信息的用户学号和科目名:\n");

            scanf("%d%s", &(((GradeTable*)ab->data)->_id), ((GradeTable*)ab->data)->_subject);

            ab->qt._id = 1;
            ab->qt._subject = 1;
            ab->qt._kcid = 0;
            ab->qt._all = 0;
            
        }
        else if( i == 0 )   // 返回上一层，成绩管理菜单界面
        {
            free(ab);
            return 0;
        }
        else
        {
            enter_1 = 1;
            printf("输入错误，请重新输入!\n");
        }
    }while( enter_1 );
    
    ret = sql_del(ab);
    if( ret != 0 ){
        printf("删除失败!\n");
    }else{
        printf("删除成功!\n");
    }

    free(ab);

    return ret;
}

// 清空成绩信息
int grade_manager_clear( GradeTable* gt )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(GradeTable));
    if( ab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(GradeTable));

    ab->info = GRADEINFO;

    printf("注意！清空后无法恢复，请选择：\n");
    printf("\t1、确定清空\n");
    printf("\t2、返回上层\n");

    int i = 0;
    int enter = 0;

    do
    {
        enter = 0;
        scanf("%d", &i);

        if( i == 1 )
        {
            ab->qt._id = 0;
            ab->qt._kcid = 0;
            ab->qt._subject = 0;
            ab->qt._all = 1;
        }
        else if( i == 2 )
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
        printf("清空失败!\n");
    }else{
        printf("清空成功!\n");
    }

    free(ab);

    return ret;
}

// 查询成绩信息
int grade_manager_select( GradeTable* gt )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(GradeTable));
    if( ab == NULL )
    {
        printf("user_manager_select malloc is fail: %s\n", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(GradeTable));

    ab->info = GRADEINFO;

    printf("1、查找全部\n");
    printf("2、按学号查找\n");
    printf("3、按科目名查找\n");
    printf("4、按学号和科目名查找\n");
    printf("0、返回上一层\n");

    int i = 0;
    int enter = 0;

    do{
        enter = 0;
        scanf("%d", &i);

        if( i == 1 )        //  查找所有信息
        {
            ab->qt._id = 0;
            ab->qt._subject = 0;
            ab->qt._sort = NSC;
            ab->qt._all = 1;
        }
        else if( i == 2 )   // 按学号查找
        {
            ab->qt._subject = 0;
            ab->qt._id = 1;
            ab->qt._all = 0;
            ab->qt._sort = NSC;

            printf("请输入用户学号：\n");
            scanf("%d", &(((GradeTable*)ab->data)->_id));

            printf("请输入成绩排序方式:\n");
            printf("1、默认顺序\n");
            printf("2、升序排序\n");
            printf("3、降序排序\n");

            scanf("%d", &i);

            if( i == 2 ){
                ab->qt._sort = ASC;     // 升序
            }else if( i == 3 ){
                ab->qt._sort = DESC;    // 降序
            }
        }
        else if( i == 3)    // 按科目查找
        {
            ab->qt._subject = 1;
            ab->qt._id = 0;
            ab->qt._all = 0;
            ab->qt._sort = NSC;

            printf("请输入科目名字：\n");
            scanf("%s", ((GradeTable*)ab->data)->_subject);

            printf("请输入成绩排序方式:\n");
            printf("1、默认顺序\n");
            printf("2、升序排序\n");
            printf("3、降序排序\n");

            scanf("%d", &i);

            if( i == 2 ){
                ab->qt._sort = ASC;     // 升序
            }else if( i == 3 ){
                ab->qt._sort = DESC;    // 降序
            }
        }
        else if( i == 4 )   // 查找某个用户的某一科的信息
        {
            ab->qt._subject = 1;
            ab->qt._id = 1;
            ab->qt._all = 0;

            printf("请输入学号和科目名：\n");
            scanf("%d%s", &(((GradeTable*)ab->data)->_id), ((GradeTable*)ab->data)->_subject);
        }
        else if( i == 0 )   // 返回上一层
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

    // 首先查询信息，把原有信息打印出来
    SelectInfo* si = sql_select(ab);
    if( !si ){
        printf("查询内容为空!\n");
    }else{
        sql_free(si);
    }

    free(ab);
    return ret;
}

// 修改成绩信息
int grade_manager_alter( GradeTable* gt )
{
    int ret = -1;
    // 保存修改后数据
    AddObject *nab = (AddObject *)malloc(sizeof(AddObject) + sizeof(GradeTable));
    if( nab == NULL )
    {
        printf("user_manager_select malloc is fail: %s\n", strerror(errno));
        return ret;
    }

    // 保存修改前数据
    AddObject *oab = (AddObject *)malloc(sizeof(AddObject) + sizeof(GradeTable));
    if( oab == NULL )
    {
        printf("user_manager_select malloc is fail: %s", strerror(errno));
        free(nab);
        return ret;
    }

    memset(nab, 0, sizeof(AddObject) + sizeof(GradeTable));
    memset(oab, 0, sizeof(AddObject) + sizeof(GradeTable));

    nab->info = GRADEINFO;
    oab->info = GRADEINFO;

    int i = 0;
    // 默认只修改成绩
    printf("请输入需要修改的用户学号与科目：\n");

    oab->qt._id = 1;
    oab->qt._subject = 1;
    oab->qt._all = 0;

    scanf("%d%s", &(((GradeTable*)oab->data)->_id), ((GradeTable*)oab->data)->_subject);

    // 先输出原有信息
    SelectInfo* si = sql_select(oab);
    if( si == NULL )
    {
        printf("信息未找到!\n");
    }
    else
    {
        // 释放查找时的空间
        sql_free(si);

        printf("请输入该用户修改后的成绩:\n");
        scanf("%f", &(((GradeTable*)nab->data)->_score));

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