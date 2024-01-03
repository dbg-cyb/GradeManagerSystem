#ifndef  __USER_MANAGER_H__
#define __USER_MANAGER_H__

#include "common.h"

// 学生管理系统初始化
int user_manager_init(const char *database_name);

// 学生管理系统反初始化
int user_manager_uninit();

// 添加用户
int user_manager_add( UserInfo* sf );

// 删除用户
int user_manager_del( UserInfo* sf );

// 清空用户
int user_manager_clear( UserInfo* sf );

// 查询用户
int user_manager_select( UserInfo* sf );

// 修改用户
int user_manager_alter( UserInfo* sf );

#endif

