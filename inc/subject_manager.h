#ifndef __SUBJECT_MANAGER_H__
#define __SUBJECT_MANAGER_H__

#include "common.h"

// 课程管理系统初始化
int subject_manager_init(const char *database_name);

// 课程管理系统反初始化
int subject_manager_uninit();

// 添加课程信息
int subject_manager_add( TimeTable* sf );

// 删除课程信息
int subject_manager_del( TimeTable* sf );

// 清空课程信息
int subject_manager_clear( TimeTable* sf );

// 查询课程信息
int subject_manager_select( TimeTable* sf );

// 修改课程信息
int subject_manager_alter( TimeTable* sf );


#endif // !__SUBJECT_MANAGER_H__