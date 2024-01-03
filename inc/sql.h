#ifndef __SQL_H__
#define __SQL_H__

#include "common.h"

// 初始化数据库
int sql_init(const char *DataBaseName);

// 反初始化数据库
int sql_uninit();

// 添加表格
int sql_add_table(TABLEINFO info);

// 数据查询
SelectInfo *sql_select(AddObject *info);

// 添加数据
int sql_add(AddObject *info, char *filename);

// 删除数据
int sql_del(AddObject *info);

// 修改数据
int sql_alter(AddObject *old_info, AddObject *new_info);

// 释放sql查询资源
int sql_free( SelectInfo *data );





#endif