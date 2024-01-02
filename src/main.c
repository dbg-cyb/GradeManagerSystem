#include <stdio.h>
#include <stdlib.h>
#include "tools.h"

#define MANAGER_DATABASE "./manager.db"

void print_menu()
{
    printf("\t\t\t|==============================================|\n");
    printf("\t\t\t|                                              |\n");
    printf("\t\t\t|             欢迎使用信息管理系统             |  \n");
    printf("\t\t\t|                                              |\n");
    printf("\t\t\t|==============================================|\n");
    printf("\t\t\t|              请选择要操作的命令              |  \n");
    printf("\t\t\t|----------------------------------------------|\n");
    printf("\t\t\t|               1、学生信息管理                | \n");
    printf("\t\t\t|               2、科目信息管理                | \n");
    printf("\t\t\t|               3、成绩信息管理                | \n");
    printf("\t\t\t|               4、退出管理系统                | \n");
    printf("\t\t\t|               0、显示菜单界面                | \n");
    printf("\t\t\t|                                              |\n");
    printf("\t\t\t|==============================================|\n");
}

void print_stu()
{
    printf("\t\t\t|==============================================|\n");
    printf("\t\t\t|              请选择要操作的命令              |  \n");
    printf("\t\t\t|----------------------------------------------|\n");
    printf("\t\t\t|               1、查询学生信息                | \n");
    printf("\t\t\t|               2、添加学生信息                | \n");
    printf("\t\t\t|               3、删除学生信息                | \n");
    printf("\t\t\t|               4、修改学生信息                | \n");
    printf("\t\t\t|               5、清空学生信息                | \n");
    printf("\t\t\t|               6、返回管理界面                | \n");
    printf("\t\t\t|               0、显示操作界面                | \n");
    printf("\t\t\t|                                              |\n");
    printf("\t\t\t|==============================================|\n");
}

void print_subject()
{
    printf("\t\t\t|==============================================|\n");
    printf("\t\t\t|              请选择要操作的命令              |  \n");
    printf("\t\t\t|----------------------------------------------|\n");
    printf("\t\t\t|               1、查询科目信息                | \n");
    printf("\t\t\t|               2、添加科目信息                | \n");
    printf("\t\t\t|               3、删除科目信息                | \n");
    printf("\t\t\t|               4、修改科目信息                | \n");
    printf("\t\t\t|               5、清空科目信息                | \n");
    printf("\t\t\t|               6、返回管理界面                | \n");
    printf("\t\t\t|               0、显示操作界面                | \n");
    printf("\t\t\t|                                              |\n");
    printf("\t\t\t|==============================================|\n");
}

void print_score()
{
    printf("\t\t\t|==============================================|\n");
    printf("\t\t\t|              请选择要操作的命令              |  \n");
    printf("\t\t\t|----------------------------------------------|\n");
    printf("\t\t\t|               1、查询成绩信息                | \n");
    printf("\t\t\t|               2、添加成绩信息                | \n");
    printf("\t\t\t|               3、删除成绩信息                | \n");
    printf("\t\t\t|               4、修改成绩信息                | \n");
    printf("\t\t\t|               5、清空成绩信息                | \n");
    printf("\t\t\t|               6、成绩排序显示                | \n");
    printf("\t\t\t|               7、返回管理界面                | \n");
    printf("\t\t\t|               0、显示操作界面                | \n");
    printf("\t\t\t|                                              |\n");
    printf("\t\t\t|==============================================|\n");
}

// 学生管理系统
void _user_manager_enter()
{
    int run = 1;
    int ret = -1;
    int choose = 0;

    print_stu();
    while(run)
    {   
        printf("请选择要操作的命令: \n");
        scanf("%d", &choose);

        switch (choose)
        {
            case 1:         // 查询
                user_query();
                break;
            case 2:         // 添加
                user_add();
                break;
            case 3:         // 删除
                user_del();
                break;
            case 4:         // 修改
                user_alter();
                break;
            case 5:         // 清空
                user_clear();
                break;
            case 6:
                run = 0;
                system("clear");
                break;
            case 0:
                system("clear");
                print_stu();
                break;
            
            default:
                break;
        }

    }
}

// 课程管理系统
void _subject_manager_enter()
{
    int run = 1;
    int ret = -1;
    int choose = 0;

    print_subject();
    while(run)
    {   
        printf("请选择要操作的命令: \n");
        scanf("%d", &choose);

        switch (choose)
        {
            case 1:
                sbuject_query();
                break;
            case 2:
                sbuject_add();
                break;
            case 3:
                sbuject_del();
                break;
            case 4:
                sbuject_alter();
                break;
            case 5:
                sbuject_clear();
                break;
            case 6:
                run = 0;
                system("clear");
                break;
            case 0:
                system("clear");
                print_subject();
                break;
            
            default:
                break;
        }

    }
}

// 成绩管理系统
void _grade_manager_enter()
{
    int run = 1;
    int ret = -1;
    int choose = 0;

    print_score();
    while(run)
    {   
        printf("请选择要操作的命令: \n");
        scanf("%d", &choose);

        switch (choose)
        {
            case 1:
                grade_query();
                break;
            case 2:
                grade_add();
                break;
            case 3:
                grade_del();
                break;
            case 4:
                grade_alter();
                break;
            case 5:
                grade_clear();
                break;
            case 6:
                grade_sort();
                break;
            case 7:
                run = 0;
                system("clear");
                break;
            case 0:
                system("clear");
                print_score();
                break;
            
            default:
                break;
        }

    }
}

int main()
{
    int ret = -1;
    int choose = 0;

    // 初始化管理系统
    ret = tools_manager_init(MANAGER_DATABASE);
    if( ret < 0 )
    {
        printf("manager init is fail.\n");
        return ret;
    }

    print_menu();

    while(1)
    {
        printf("请选择要操作的命令: \n");
        scanf("%d", &choose);

        switch (choose)
        {
            case 1:
                system("clear");
                _user_manager_enter();
                print_menu();
                break;
            case 2:
                system("clear");
                _subject_manager_enter();
                print_menu();
                break;
            case 3:
                system("clear");
                _grade_manager_enter();
                print_menu();
                break;
            case 4:
                printf("欢迎下次使用，再见\n");
                goto OUT;
                break;
            case 0:
                system("clear");
                print_menu();
                break;
            default:
                printf("请输入正确的选项!\n");
                break;
        }
    }

OUT:
    tools_manager_uninit();

    return 0;
}