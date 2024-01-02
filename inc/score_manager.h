#ifndef __SCORE_MANAGER_H__
#define __SCORE_MANAGER_H__

#include "common.h"

// 成绩管理系统初始化
int grade_manager_init(const char *database_name);

// 成绩管理系统反初始化
int grade_manager_uninit();

// 添加成绩信息
int grade_manager_add( GradeTable* gt );

// 删除成绩信息
int grade_manager_del( GradeTable* gt );

// 清空成绩信息
int grade_manager_clear( GradeTable* gt );

// 查询成绩信息
int grade_manager_select( GradeTable* gt );

// 修改成绩信息
int grade_manager_alter( GradeTable* gt );


#endif // !__SCORE_MANAGER_H__
