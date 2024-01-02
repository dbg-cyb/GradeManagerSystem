#ifndef __COMMON_H__
#define __COMMON_H__

// 查询类型
typedef enum TABLEINFO
{
    STUINFO = 1,        // 学生信息
    CLASSINFO = 2,      // 课程信息
    SCOREINFO = 3,      // 成绩信息
    MYDEFINE            // 用户自定义
}TABLEINFO;

// 科目
typedef enum SUBJECT
{
    CHINESE = 1,        // 语文
    MATH = 2,           // 数学
    ENGLISH = 3,        // 英语
    SUBALL
}SUBJECT;

// 排序方式
typedef enum SORT
{
    NSC = 1,            // 默认
    ASC = 2,            // 顺序
    DESC = 3            // 逆序
}SORT;

// 查询字段选择标识
typedef struct query_t
{
    char _id;           // 学号
    char _name;         // 名字
    char _age;          // 年龄
    char _addr;         // 地址
    char _tel;          // 电话
    char _kcid;         // 课程号
    char _subject;      // 科目
    char _teacher;      // 老师
    char _score;        // 成绩
    char _all;          // 全部
    char _file;         // 从文件导入
    SORT _sort;         // 排序
}query_t;

// 学生信息表格字段
typedef struct StuInfos
{
    int _id;                // 学号
    char _name[128];        // 名字
    int _age;               // 年龄
    char _addr[512];        // 地址
    char _tel[64];          // 电话号码
}StuInfo;

// 课程表字段
typedef struct TimeTable
{
    int _kcid;              // 课程号
    char _name[128];        // 科目
    char _teacher[128];     // 老师

}TimeTable;

// 成绩表字段
typedef struct GradeTable
{
    int     _id;            // 学号
    int     _kch;           // 课程号
    float   _score;         // 成绩
    char    _subject[32];   // 课程名字

}GradeTable;

// 添加字段信息
typedef struct AddObject
{
    TABLEINFO info;
    query_t qt;
    char data[0];
}AddObject;

// 查询信息返回类型
typedef struct SelectInfo
{
    TABLEINFO info;         // 查询类型
    int rows;               // 行数
    int cols;               // 列数
    void *result;           // 结果集
}SelectInfo;


#endif // !__COMMON_H__
