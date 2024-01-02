#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "score_manager.h"
#include "sql.h"

// 成绩管理系统初始化
int grade_manager_init(const char *database_name)
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
            ret = sql_add_table(SCOREINFO);
            if( ret < 0 )
            {
                printf("sql_add_table SCOREINFO fail.\n");
            }
        }
    }

    return ret;
}

// 成绩管理系统反初始化
int grade_manager_uninit()
{
    sql_uninit();
}

// 添加成绩信息
int grade_manager_add( GradeTable* gt )
{
    int ret = -1;
    AddObject *ab = (AddObject *)malloc(sizeof(AddObject) + sizeof(GradeTable));
    if( ab == NULL )
    {
        printf("subject_manager_add malloc is fail: %s", strerror(errno));
        return ret;
    }

    memset(ab, 0, sizeof(AddObject) + sizeof(GradeTable));

    ab->info = SCOREINFO;

    int i = 0;
    printf("1、按输入添加成绩信息\n");
    printf("2、按文件导入成绩信息\n");

    scanf("%d", &i);

    char filename[512] = {0};

    do
    {
        if( i == 1 )
        {
            GradeTable* gt = (GradeTable*)ab->data;
            printf("请依此输入学号、课程号、学生成绩：\n");
            scanf("%d%d%f", &gt->_id, &gt->_kch, &gt->_score);
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

    ab->info = SCOREINFO;

LABLE:
    printf("1、批量删除\n");
    printf("2、选择删除\n");
    printf("0、返回上一层\n");

    int i = 0;

    do
    {
        scanf("%d", &i);

        if( i == 1 )
        {
            printf("1、按学号删除\n");
            printf("2、按课程号删除\n");
            printf("3、按课程名字删除\n");
            printf("0、返回上一层\n");

            int j = 0;
            do
            {
                scanf("%d", &j);

                if( j == 1 )
                {
                    ab->qt._id = 1;
                    ab->qt._subject = 0;
                    ab->qt._kcid = 0;

                    printf("请输入学号：\n");
                    scanf("%d", &(((GradeTable*)ab->data)->_id));
                }
                else if( j == 2 )
                {
                    ab->qt._id = 0;
                    ab->qt._subject = 0;
                    ab->qt._kcid = 1;

                    printf("请输入课程号：\n");
                    scanf("%d", &((GradeTable*)ab->data)->_kch);
                }
                else if( j == 3 )
                {
                    ab->qt._id = 0;
                    ab->qt._subject = 1;
                    ab->qt._kcid = 0;

                    printf("请输入课程名字：\n");
                    scanf("%s", ((GradeTable*)ab->data)->_subject);
                }
                else if( j == 0 )
                {
                    goto LABLE;
                }
                else
                {
                    printf("输入错误，请重新输入!\n\n");
                }
            } while (i != 1 && i != 2 && i != 3);
        }
        else if( i == 2 )
        {
            printf("请输入需要删除信息的学号和科目名:\n");

            scanf("%d%s", &(((GradeTable*)ab->data)->_id), ((GradeTable*)ab->data)->_subject);

            ab->qt._id = 1;
            ab->qt._subject = 1;
            ab->qt._kcid = 0;
            
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
    }while( i != 1 && i != 2 && i != 3 );
    
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

// 清空成绩信息
int grade_manager_clear( GradeTable* gt )
{

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

    ab->info = SCOREINFO;

    printf("1、查找全部\n");
    printf("2、按学号查找\n");
    printf("3、按科目名查找\n");
    printf("4、按学号和科目名查找\n");
    printf("0、返回上一层\n");

    int i = 0;
    do{
        scanf("%d", &i);

        if( i == 1 )
        {
            ab->qt._id = 0;
            ab->qt._subject = 0;
            ab->qt._sort = NSC;
            ab->qt._all = 1;
        }
        else if( i == 2 )
        {
            ab->qt._subject = 0;
            ab->qt._id = 1;
            ab->qt._all = 0;
            ab->qt._sort = NSC;

            printf("请输入学号：\n");
            scanf("%d", &(((GradeTable*)ab->data)->_id));

            printf("是否需要排序查询:\n");
            printf("1、默认顺序\n");
            printf("2、升序排序\n");
            printf("3、降序排序\n");

            scanf("%d", &i);
            if( i == 2 ){
                ab->qt._sort = ASC;
            }else if( i == 3 ){
                ab->qt._sort = DESC;
            }
        }
        else if( i == 3)
        {
            ab->qt._subject = 1;
            ab->qt._id = 0;
            ab->qt._all = 0;
            ab->qt._sort = NSC;

            printf("请输入课程名字：\n");
            scanf("%s", ((GradeTable*)ab->data)->_subject);

            printf("是否需要排序查询:\n");
            printf("1、默认顺序\n");
            printf("2、升序排序\n");
            printf("3、降序排序\n");

            scanf("%d", &i);
            if( i == 2 ){
                ab->qt._sort = ASC;
            }else if( i == 3 ){
                ab->qt._sort = DESC;
            }
        }
        else if( i == 4 )
        {
            ab->qt._subject = 1;
            ab->qt._id = 1;
            ab->qt._all = 0;

            printf("请输入学号和科目名：\n");
            scanf("%d%s", &(((GradeTable*)ab->data)->_id), ((GradeTable*)ab->data)->_subject);
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
    }while(i != 1 && i != 2 && i !=  3 && i != 4);

    sql_select(ab);

    free(ab);
    return ret;
}

// 修改成绩信息
int grade_manager_alter( GradeTable* gt )
{

}