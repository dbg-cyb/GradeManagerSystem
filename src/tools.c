#include <stdio.h>
#include <assert.h>
#include "user_manager.h"
#include "subject_manager.h"
#include "grade_manager.h"
#include "tools.h"
#include "common.h"

// 管理系统初始化
int tools_manager_init(const char *database_name)
{
    assert(database_name);

    int ret = -1;
    // 学生管理模块初始化
    ret = user_manager_init(database_name);
    if( ret < 0 )
    {
        printf("user_manager init is fail.\n");
        goto QUIT;
    }

    // 课程管理模块初始化
    ret = subject_manager_init(database_name);
    if( ret < 0 )
    {
        printf("subject_manager init is fail.\n");
        goto USER_MANAGER_UNINIT;
    }

    // 成绩管理模块初始化
    ret = grade_manager_init(database_name);
    if( ret < 0 )
    {
        printf("grade_manager init is fail.\n");
        goto SUBJECT_MANAGER_UNINIT;
    }

    return ret;

    // 初始化失败，反初始化
SUBJECT_MANAGER_UNINIT:
    subject_manager_uninit();
USER_MANAGER_UNINIT:
    user_manager_uninit();
QUIT:
    return ret;
}

// 管理系统反初始化
int tools_manager_uninit()
{
    // 学生管理系统反初始化
    user_manager_uninit();

    // 课程管理系统初始化
    subject_manager_uninit();

    // 成绩管理系统初始化
    grade_manager_uninit();

    return 0;
}


/********************************************/
/*              用户管理模块                 */
/********************************************/

// 添加用户
void user_add()
{
    user_manager_add(NULL);
}

// 删除用户
void user_del()
{
    user_manager_del(NULL);
}

// 修改用户
void user_alter()
{
    user_manager_alter(NULL);
}

// 查询用户
void user_query()
{
    user_manager_select(NULL);
}

// 清空用户
void user_clear()
{
    user_manager_clear(NULL);
}


/********************************************/
/*              课程管理模块                 */
/********************************************/

// 添加用户
void sbuject_add()
{
    subject_manager_add(NULL);
}

// 删除用户
void sbuject_del()
{
    subject_manager_del(NULL);
}

// 修改用户
void sbuject_alter()
{
    subject_manager_alter(NULL);
}

// 查询用户
void sbuject_query()
{
    subject_manager_select(NULL);
}

// 清空用户
void sbuject_clear()
{
    subject_manager_clear(NULL);
}



/********************************************/
/*              成绩管理模块                 */
/********************************************/

// 添加成绩
void grade_add()
{
    grade_manager_add(NULL);
}

// 删除成绩
void grade_del()
{
    grade_manager_del(NULL);
}

// 修改成绩
void grade_alter()
{
    grade_manager_alter(NULL);
}

// 查询成绩
void grade_query()
{
    grade_manager_select(NULL);
}

// 清空成绩
void grade_clear()
{
    grade_manager_clear(NULL);
}

// 排序查询
void grade_sort()
{

}

