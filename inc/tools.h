#ifndef __TOOLS_H__
#define __TOOLS_H__

int tools_manager_init(const char *database_name);

int tools_manager_uninit();

// 添加用户
void user_add();

// 删除用户
void user_del();

// 修改用户
void user_alter();

// 查询用户
void user_query();

// 清空用户
void user_clear();

// ==============================

// 添加科目
void sbuject_add();

// 删除科目
void sbuject_del();

// 修改科目
void sbuject_alter();

// 查询科目
void sbuject_query();

// 清空科目
void sbuject_clear();

// ==============================

// 添加成绩
void grade_add();

// 删除成绩
void grade_del();

// 修改成绩
void grade_alter();

// 查询成绩
void grade_query();

// 清空成绩
void grade_clear();

// 排序查询
void grade_sort();


#endif
