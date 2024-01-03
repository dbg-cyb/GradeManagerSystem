#include "sql.h"
#include <unistd.h>
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>

// 用户管理模块创建表命令
#define CREATE_STU_TABLE_STR "CREATE TABLE IF NOT EXISTS USERINFO(ID INT PRIMARY KEY, \
                                                NAME VARCHAR(255) NOT NULL, \
                                                AGE INT, \
                                                ADDR TEXT DEFAULT '长沙', \
                                                TEL TEXT, \
                                                CHECK (ID > 0), \
                                                CHECK (AGE >= 0));"

// 课程管理模块创建表命令
#define CREATE_TIME_TABLE_STR "CREATE TABLE IF NOT EXISTS TIMETABLE(KCID INT PRIMARY KEY, \
                                                NAME VARCHAR(255) NOT NULL, \
                                                TEACHER TEXT, \
                                                CHECK (KCID > 0));"

// 创建管理模块创建表命令
#define CREATE_GRADE_TABLE_STR "CREATE TABLE IF NOT EXISTS GRADETABLE(ID INT, \
                                                KCH INT, \
                                                SCORE FLOAT, \
                                                FOREIGN KEY(ID) REFERENCES USERINFO(ID), \
                                                FOREIGN KEY(KCH) REFERENCES TIMETABLE(KCID));"

// sqlite3 操作句柄
sqlite3* pdb = NULL;

// 初始化数据库，默认每次打开的数据库为同一个
int sql_init(const char *DataBaseName)
{
    // 参数判断
    assert(DataBaseName);

    int ret = 0;

    // 未初始化
    if( !pdb )
    {
        // 打开数据库
        ret = sqlite3_open(DataBaseName, &pdb);
        if( ret != SQLITE_OK )
        {
            printf("sqlite3_open fail: %s\n", sqlite3_errmsg(pdb));
            ret = -1;
            return ret;
        }
    }

    // 打开外键功能 PRAGMA foreign_keys = ON;
    ret = sqlite3_exec(pdb, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
    if( ret != SQLITE_OK)
    {
        //printf("cmd: %s\n", sql_cmd);
        printf("sqlite3 open exec fail: %s\n", sqlite3_errmsg(pdb));
        ret = -1;
    }

    return ret;
}

// 反初始化数据库
// 默认只打开一个数据库
int sql_uninit()
{
    int ret = 0;
    if( pdb )
    {
        // 关闭数据库
        ret = sqlite3_close(pdb);
        pdb = NULL;
    }

    return ret;
}

// 添加表格
int sql_add_table(TABLEINFO info)
{
    int ret = 0;
    char sql_cmd[1024] = {0};

    // 根据表类型创建表格
    switch (info)
    {
        case USERINFO:
            // 学生信息表
            memcpy(sql_cmd, CREATE_STU_TABLE_STR, 1024);
            break;
        case CLASSINFO:
            // 课程表
            memcpy(sql_cmd, CREATE_TIME_TABLE_STR, 1024);
            break;
        case GRADEINFO:
            // 成绩表
            memcpy(sql_cmd, CREATE_GRADE_TABLE_STR, 1024);
            break;
        default:
            // 错误的参数
            printf("input table info[%d] is invalid.\n", info);
            ret = -1;
            break;
    }

    if( !ret )
    {
        // 执行sql语句
        ret = sqlite3_exec(pdb, sql_cmd, NULL, NULL, NULL);
        if( ret != SQLITE_OK)
        {
            //printf("cmd: %s\n", sql_cmd);
            printf("sqlite3_exec fail: %s\n", sqlite3_errmsg(pdb));
            ret = -1;
        }
    }

    return ret;
}

// 数据查询
SelectInfo *sql_select(AddObject *info)
{
    // 参数判断
    assert(info);

    int ret = 0;
    char sql_cmd[2048] = {0};
    // 保存查询出来的信息，目前未使用
    SelectInfo *result_info = NULL;

    memcpy(sql_cmd, "SELECT ", strlen("SELECT "));

    // 根据表类型查询表格
    switch (info->info)
    {
        case USERINFO:  // 学生信息表，查询结果按学号升序排名
        {
            UserInfo* si = (UserInfo*)info->data;
            char tmp[1024] = {0};

            if( info->qt._id ) // 按学号查找
            {
                sprintf(tmp, "* FROM USERINFO WHERE ID = %d;", si->_id);
                strcat(sql_cmd, tmp);
            }
            else if( info->qt._name )    // 按姓名查找
            {
                sprintf(tmp, "* FROM USERINFO WHERE NAME = '%s' ORDER BY ID ASC;", si->_name);
                strcat(sql_cmd, tmp);
            }
            else    // 查找全部
            {
                strcat(sql_cmd, "* FROM USERINFO ORDER BY ID ASC;");
            }
            break;
        }
        case CLASSINFO:     // 课程表
        {
            TimeTable* tt = (TimeTable*)info->data;
            char tmp[1024] = {0};

            if( info->qt._kcid )    // 按课程号查找
            {
                sprintf(tmp, "* FROM TIMETABLE WHERE KCID = %d;", tt->_kcid);
                strcat(sql_cmd, tmp);
            }
            else if( info->qt._subject )    // 按科目名字查找
            {
                sprintf(tmp, "* FROM TIMETABLE WHERE NAME = '%s' ORDER BY KCID ASC;", tt->_name);
                strcat(sql_cmd, tmp);
            }
            else if( info->qt._teacher )    // 按老师查找
            {
                sprintf(tmp, "* FROM TIMETABLE WHERE TEACHER = '%s' ORDER BY KCID ASC;", tt->_teacher);
                strcat(sql_cmd, tmp);
            }
            else
            {
                strcat(sql_cmd, "* FROM TIMETABLE ORDER BY KCID ASC;");
            }
            break;
        }
        case GRADEINFO:     // 成绩表
        {
            GradeTable* gt = (GradeTable*)info->data;
            char tmp[1024] = {0};

            // 拼接sql语句，联表查询，默认显示字段
            sprintf(tmp, "USERINFO.ID, USERINFO.NAME, \
                            TIMETABLE.NAME, GRADETABLE.SCORE \
                            FROM USERINFO, TIMETABLE, GRADETABLE \
                            WHERE USERINFO.ID = GRADETABLE.ID \
                            AND TIMETABLE.KCID = GRADETABLE.KCH" );
            if( info->qt._all )     // 查找全部
            {
                // 无成绩排名，我们使用学号排名
                char str[512] = {0};
                sprintf(str, " ORDER BY GRADETABLE.ID ASC ");
                strcat(tmp, str);
            }
            else if( info->qt._id && !info->qt._subject )   // 按学号查找
            {
                char str[512] = {0};
                sprintf(str, " AND GRADETABLE.ID = %d ", gt->_id);
                strcat(tmp, str);
            }
            else if( info->qt._subject && !info->qt._id )   // 按科目查找
            {
                char str[512] = {0};
                sprintf(str, " AND TIMETABLE.NAME = '%s' ", gt->_subject);
                strcat(tmp, str);
            }
            else    // 按学号和科目联查
            {
                char str[512] = {0};
                sprintf(str, " AND GRADETABLE.ID = %d AND TIMETABLE.NAME = '%s' ", gt->_id, gt->_subject);
                strcat(tmp, str);
            }

            // 排序方式
            if( info->qt._sort == NSC ){
                strcat(tmp, ";");
            }else if( info->qt._sort == ASC ){
                strcat(tmp, " ORDER BY GRADETABLE.SCORE ASC;");
            }else if( info->qt._sort == DESC ){
                strcat(tmp, " ORDER BY GRADETABLE.SCORE DESC;");
            }else{
                strcat(tmp, ";");
            }
            
            strcat(sql_cmd, tmp);
            break;
        }
        case MYDEFINE:      // 自定义查询，自定义需要显示的字段
        {
            // 判断是否为第一个
            int ufirst = 0;
            if(info->qt._id){
                strcat(sql_cmd, "STU.ID ");
                ufirst = 1;
            }
                
            if(info->qt._name)
            {
                if( ufirst ){
                    strcat(sql_cmd, ";STU.NAME ");
                } else{
                    strcat(sql_cmd, "STU.NAME ");
                    ufirst = 1;
                }
            }

            if(info->qt._age)
            {
                if( ufirst ){
                    strcat(sql_cmd, ";STU.AGE ");
                } else{
                    strcat(sql_cmd, "STU.AGE ");
                    ufirst = 1;
                }
            }

            if(info->qt._addr)
            {
                if( ufirst ){
                    strcat(sql_cmd, ";STU.ADDR ");
                } else{
                    strcat(sql_cmd, "STU.ADDR ");
                    ufirst = 1;
                }
            }

            if(info->qt._tel)
            {
                if( ufirst ){
                    strcat(sql_cmd, ";STU.TEL ");
                } else{
                    strcat(sql_cmd, "STU.TEL ");
                    ufirst = 1;
                }
            }

            if(info->qt._kcid)
            {
                if( ufirst ){
                    strcat(sql_cmd, ";TIMETABLE.KCID ");
                } else{
                    strcat(sql_cmd, "TIMETABLE.KCID ");
                    ufirst = 1;
                }
            }

            if(info->qt._subject)
            {
                if( ufirst ){
                    strcat(sql_cmd, ";TIMETABLE.NAME ");
                } else{
                    strcat(sql_cmd, "TIMETABLE.NAME ");
                    ufirst = 1;
                }
            }

            if(info->qt._teacher)
            {
                if( ufirst ){
                    strcat(sql_cmd, ";TIMETABLE.TEACHER ");
                } else{
                    strcat(sql_cmd, "TIMETABLE.TEACHER ");
                    ufirst = 1;
                }
            }

            if(info->qt._score)
            {
                if( ufirst ){
                    strcat(sql_cmd, ";GRADETABLE.SCORE ");
                } else{
                    strcat(sql_cmd, "GRADETABLE.SCORE ");
                    ufirst = 1;
                }
            }

            // 如果没有添加选项
            if(!ufirst)
            {
                printf("please select your choose.\n");
                ret = -1;
            }

            // 添加尾部
            // 成绩表
            int tt = *(int *)(info->data);
            char tmp[1024] = {0};
            // 拼接sql语句
            sprintf(tmp, "WHERE USERINFO.ID = GRADETABLE.ID \
                            AND TIMETABLE.KCID = GRADETABLE.KCH" );
            if( tt == SUBALL )
            {
                strcat(tmp, ";");
            }
            else
            {
                char str[512] = {0};
                sprintf(tmp, " ADN TIMETABLE.KCID = %d;", tt);
                strcat(tmp, str);
            }

            strcat(sql_cmd, tmp);

            break;
        }
        default:
            // 错误的参数
            printf("input table info is invalid.\n");
            ret = -1;
            break;
    }

    // 执行sql语句，获取查询结果
    if( !ret )
    {
        char **result = NULL;
        int rows = 0;
        int cols = 0;
        //printf("%s\n", sql_cmd);

        ret = sqlite3_get_table(pdb, sql_cmd, &result, &rows, &cols, NULL);
        if( ret !=  SQLITE_OK)
        {
            printf("sqlite3_get_table fail: %s\n", sqlite3_errmsg(pdb));
            ret = -1;
        }
        else
        {
            // 查询成功 打印结果
            if( rows && cols )
            {
                printf("\n---------------------------------------------\n");
                for (int i = 0; i < rows + 1; i++) {
                    for (int j = 0; j < cols; j++) {
                        printf("%s\t", result[(i * cols) + j]);
                    }
                    printf("\n---------------------------------------------\n");
                }

                result_info = (SelectInfo *)malloc(sizeof(SelectInfo));

                result_info->info = info->info;
                result_info->rows = rows;
                result_info->cols = cols;
                result_info->result = (void **)result;

                printf("\n\n");
            }
        }

        // 释放结果占用的内存
        //sqlite3_free_table(result);
    }

    return result_info;
}

// 释放sql查询资源
int sql_free( SelectInfo *data )
{
    assert( data );

    sqlite3_free_table((char **)data->result);

    free(data);

    return 0;
}

// 添加数据，两种模式：文件添加 和 输入添加
int sql_add(AddObject *info, char *filename)
{
    // 参数判断
    assert(info);
    //assert(filename);

    int ret = 0;
    char sql_cmd[2048] = {0};

    if( info->info == USERINFO )    // 用户信息
    {
        if( !filename )     // 手动添加
        {
            UserInfo* si = (UserInfo *)info->data;

            sprintf(sql_cmd, "INSERT INTO USERINFO VALUES(%d, '%s', %d, '%s', '%s');", 
                                            si->_id, si->_name, si->_age, si->_addr, si->_tel);
        }
        else        // 文件添加
        {
            // 判断文件是否存在
            ret = access(filename, F_OK);
            if( ret < 0 )
            {
                printf("文件[%s]不存在.\n", filename);
                return ret;
            }
            UserInfo* si = (UserInfo *)info->data;

            // 打开文件
            FILE* fp = fopen(filename, "r");
            if( !fp )
            {
                printf("打开文件[%s]失败: %s\n", filename, strerror(errno));
                ret = -1;
                return ret;
            }

            // 开始导入数据
            int i = 0;
            printf("导入中...\n");
            while(1)
            {
                memset(si, 0, sizeof(UserInfo));
                // 格式化获取数据内容
                ret = fscanf(fp, "%d%s%d%s%s", &si->_id, si->_name, &si->_age, si->_addr, si->_tel);
                if( ret != 5 )
                {
                    if( feof(fp) )  // 到文件末尾
                    {
                        ret = 0;
                        break;
                    }
                    continue;
                }

                // 拼接sql语句
                sprintf(sql_cmd, "INSERT INTO USERINFO VALUES(%d, '%s', %d, '%s', '%s');", 
                                            si->_id, si->_name, si->_age, si->_addr, si->_tel);

                //printf("%s\n", sql_cmd);
                // 插入数据到数据库表中
                ret = sqlite3_exec(pdb, sql_cmd, NULL, NULL, NULL);
                if( ret != SQLITE_OK ){
                    //printf("sqlite3_exec fail: [%d]%s\n", ret, sqlite3_errmsg(pdb));
                    printf("添加失败: [%d %s]\n", si->_id, si->_name);
                    continue;
                }
                
                i++;
            }

            printf("导入完成, 共导入%d条信息!\n", i);

            fclose(fp);
            return ret;
        }
    }
    else if( info->info == CLASSINFO )  // 课程信息
    {
        if( !filename )         // 手动添加
        {
            TimeTable* tt = (TimeTable *)info->data;

            sprintf(sql_cmd, "INSERT INTO TIMETABLE VALUES(%d, '%s', '%s');", 
                                                    tt->_kcid, tt->_name, tt->_teacher);
        }
        else        // 通过文件导入
        {
            ret = access(filename, F_OK);
            if( ret < 0 )
            {
                printf("文件[%s]不存在.\n", filename);
                return ret;
            }
            TimeTable* tt = (TimeTable *)info->data;

            // 打开文件
            FILE* fp = fopen(filename, "r");
            if( !fp )
            {
                printf("打开文件[%s]失败: %s\n", filename, strerror(errno));
                ret = -1;
                return ret;
            }

            int i = 0;
            printf("导入中...\n");
            while(1)
            {
                memset(tt, 0, sizeof(TimeTable));

                ret = fscanf(fp, "%d%s%s", &tt->_kcid, tt->_name, tt->_teacher);
                if( ret != 3 )
                {
                    if( feof(fp) )
                    {
                        ret = 0;
                        break;
                    }
                    continue;
                }

                sprintf(sql_cmd, "INSERT INTO TIMETABLE VALUES(%d, '%s', '%s');", 
                                            tt->_kcid, tt->_name, tt->_teacher);

                //printf("%s\n", sql_cmd);
                ret = sqlite3_exec(pdb, sql_cmd, NULL, NULL, NULL);
                if( ret != SQLITE_OK )
                {
                    //printf("sqlite3_exec fail: [%d]%s\n", ret, sqlite3_errmsg(pdb));
                    printf("添加失败: [%d %s]\n", tt->_kcid, tt->_name);
                    continue;
                }

                i++;
            }

            printf("导入完成, 共导入%d条信息!\n", i);

            fclose(fp);
            return ret;
        }
    }
    else if( info->info == GRADEINFO )      // 成绩表
    {
        if( !filename )     // 手动输入
        {
            GradeTable* gt = (GradeTable *)info->data;

            sprintf(sql_cmd, "INSERT INTO GRADETABLE VALUES(%d, %d, %f);", 
                                                    gt->_id, gt->_kch, gt->_score);
        }
        else        // 通过文件导入
        {
            ret = access(filename, F_OK);
            if( ret < 0 )
            {
                printf("文件[%s]不存在.\n", filename);
                return ret;
            }
            GradeTable* gt = (GradeTable *)info->data;

            // 打开文件
            FILE* fp = fopen(filename, "r");
            if( !fp )
            {
                printf("打开文件[%s]失败: %s\n", filename, strerror(errno));
                ret = -1;
                return ret;
            }

            int i = 0;
            printf("导入中...\n");
            while(1)
            {
                memset(gt, 0, sizeof(GradeTable));

                ret = fscanf(fp, "%d%d%f", &gt->_id, &gt->_kch, &gt->_score);
                if( ret != 3 )
                {
                    if( feof(fp) )
                    {
                        ret = 0;
                        break;
                    }
                    continue;
                }

                sprintf(sql_cmd, "INSERT INTO GRADETABLE VALUES(%d, %d, %f);", 
                                            gt->_id, gt->_kch, gt->_score);

                //printf("%s\n", sql_cmd);
                ret = sqlite3_exec(pdb, sql_cmd, NULL, NULL, NULL);
                if( ret != SQLITE_OK )
                {
                    //printf("sqlite3_exec fail: [%d]%s\n", ret, sqlite3_errmsg(pdb));
                    printf("添加失败: [%d %d]\n", gt->_id, gt->_kch);
                    continue;
                }

                i++;
            }

            printf("导入完成, 共导入%d条信息!\n", i);

            fclose(fp);
            return ret;
        }
    }
    else
    {
        ret = -1;
    }

    if( !ret )
    {
        // 手动输入添加走这里
        ret = sqlite3_exec(pdb, sql_cmd, NULL, NULL, NULL);
        if( ret != SQLITE_OK )
        {
            //printf("sqlite3_exec fail: %s\n", sqlite3_errmsg(pdb));
        }
    }

    return ret;
}

// 删除数据
int sql_del(AddObject *info)
{
    assert(info);

    int ret = 0;
    char sql_cmd[2048] = {0};

    if( info->info == USERINFO )        // 用户信息
    {
        UserInfo* si = (UserInfo *)info->data;

        if( info->qt._id )      // 按学号删除
        {
            sprintf(sql_cmd, "DELETE FROM USERINFO WHERE ID = %d;", si->_id);
        }
        else if( info->qt._name )   // 按姓名删除
        {
            sprintf(sql_cmd, "DELETE FROM USERINFO WHERE NAME = '%s';", si->_name);
        }
        else if( info->qt._all )    // 删除全部，清空
        {
            sprintf(sql_cmd, "DELETE FROM USERINFO;");
        }
        else
        {
            ret = -1;
        }
    }
    else if( info->info == CLASSINFO )      // 课程表
    {
        TimeTable* tt = (TimeTable *)info->data;

        if( info->qt._kcid )    // 按课程号删除
        {
            sprintf(sql_cmd, "DELETE FROM TIMETABLE WHERE KCID = %d;", tt->_kcid);
        }
        else if( info->qt._subject )    // 按科目名删除
        {
            sprintf(sql_cmd, "DELETE FROM TIMETABLE WHERE NAME = '%s';", tt->_name);
        }
        else if( info->qt._teacher )    // 按任课老师删除
        {
            sprintf(sql_cmd, "DELETE FROM TIMETABLE WHERE TEACHER = '%s';", tt->_teacher);        
        }
        else if( info->qt._all )    // 删除全部，清空
        {
            sprintf(sql_cmd, "DELETE FROM TIMETABLE;");
        }
        else
        {
            ret = -1;
        }
    }
    else if( info->info == GRADEINFO )      // 成绩表
    {
        GradeTable* gt = (GradeTable *)info->data;
        if( info->qt._id )      // 按学号删除
        {
            if( info->qt._kcid )    // 按学号和课程号删除
            {
                sprintf(sql_cmd, "DELETE FROM GRADETABLE WHERE ID = %d AND KCH = %d;", gt->_id, gt->_kch);
            }
            else if( info->qt._subject ) // 按学号和科目名删除
            {
                sprintf(sql_cmd, "DELETE FROM GRADETABLE WHERE ID = %d AND \
                            (SELECT TIMETABLE.KCID FROM TIMETABLE WHERE NAME = '%s') = KCH;", \
                                gt->_id, gt->_subject);
            }
            else        // 只按学号删除
            {
                sprintf(sql_cmd, "DELETE FROM GRADETABLE WHERE ID = %d;", gt->_id);
            }
        }
        else if( info->qt._kcid )   // 按课程号删除
        {
            sprintf(sql_cmd, "DELETE FROM GRADETABLE WHERE KCH = '%d';", gt->_kch);
        }
        else if( info->qt._subject )    // 按科目名删除
        {
            sprintf(sql_cmd, "DELETE FROM GRADETABLE WHERE \
                            (SELECT TIMETABLE.KCID FROM TIMETABLE) = KCH \
                            AND (SELECT TIMETABLE.NAME FROM TIMETABLE) = '%s';", gt->_subject);
        }
        else if( info->qt._all )    // 删除全部，清空
        {
            sprintf(sql_cmd, "DELETE FROM GRADETABLE;");
        }
        else
        {
            ret = -1;
        }
    }
    else
    {
        ret = -1;
    }

    if( !ret )
    {
        //printf("%s\n", sql_cmd);
        // 执行sql语句
        ret = sqlite3_exec(pdb, sql_cmd, NULL, NULL, NULL);
        if( ret != SQLITE_OK )
        {
            //printf("sqlite3_exec fail: %s\n", sqlite3_errmsg(pdb));
        }
    }

    return ret;
}

// 修改数据
// old_info: 保存原始数据
// new_info: 保存修改后的数据
int sql_alter(AddObject *old_info, AddObject *new_info)
{
    assert( new_info );
    assert( old_info );

    int ret = 0;
    char sql_cmd[2048] = {0};

    switch (new_info->info)
    {
        case USERINFO:  // 用户管理
        {
            UserInfo* osi = (UserInfo *)old_info->data;
            UserInfo* nsi = (UserInfo *)new_info->data;

            if( old_info->qt._id ){
                sprintf(sql_cmd, "UPDATE USERINFO SET ID=%d,NAME='%s',AGE=%d,ADDR='%s',TEL='%s' WHERE ID=%d", \
                            nsi->_id, nsi->_name, nsi->_age, nsi->_addr, nsi->_tel, osi->_id);
            }else{
                ret = -1;
            }

            break;
        }
        case CLASSINFO: // 课程管理
        {
            TimeTable* ntt = (TimeTable *)new_info->data;
            TimeTable* ott = (TimeTable *)old_info->data;

            if( old_info->qt._kcid )    // 按课程号修改
            {
                sprintf(sql_cmd, "UPDATE TIMETABLE SET KCID=%d,NAME='%s',TEACHER='%s' WHERE KCID=%d", \
                            ntt->_kcid, ntt->_name, ntt->_teacher, ott->_kcid);
            }
            else if( old_info->qt._subject ) // 按科目名修改
            {
                sprintf(sql_cmd, "UPDATE TIMETABLE SET KCID=%d,NAME='%s',TEACHER='%s' WHERE NAME='%s'", \
                            ntt->_kcid, ntt->_name, ntt->_teacher, ott->_name);
            }
            else if( old_info->qt._teacher ) // 按任课老师修改
            {
                sprintf(sql_cmd, "UPDATE TIMETABLE SET KCID=%d,NAME='%s',TEACHER='%s' WHERE TEACHER='%s'", \
                            ntt->_kcid, ntt->_name, ntt->_teacher, ott->_teacher);
            }
            else
            {
                ret = -1;
            }

            break;
        }
        case GRADEINFO:     // 成绩管理
        {
            GradeTable* ngt = (GradeTable *)new_info->data;
            GradeTable* ogt = (GradeTable *)old_info->data;

            // 通过学号及科目名修改
            if( old_info->qt._id && old_info->qt._subject ){
                sprintf(sql_cmd, "update GRADETABLE set SCORE=%f where ID=%d and \
                                    (select KCID from TIMETABLE) = KCH and \
                                    (select NAME from TIMETABLE) = '%s';", \
                            ngt->_score, ogt->_id, ogt->_subject);
            }else{
                ret = -1;
            }

            break;
        }
        default:
            ret = -1;
            break;
    }

    if( !ret )
    {
        // 执行sql语句
        ret = sqlite3_exec(pdb, sql_cmd, NULL, NULL, NULL);
        if( ret != SQLITE_OK )
        {
            printf("sqlite3_exec fail: %s\n", sqlite3_errmsg(pdb));
        }
    }

    return ret;

}
