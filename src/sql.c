#include "sql.h"
#include <unistd.h>
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#define CREATE_STU_TABLE_STR "CREATE TABLE IF NOT EXISTS STUINFO(ID INT PRIMARY KEY, \
                                                NAME VARCHAR(255) NOT NULL, \
                                                AGE INT, \
                                                ADDR TEXT DEFAULT '长沙', \
                                                TEL TEXT, \
                                                CHECK (ID > 0), \
                                                CHECK (AGE >= 0));"

#define CREATE_TIME_TABLE_STR "CREATE TABLE IF NOT EXISTS TIMETABLE(KCID INT PRIMARY KEY, \
                                                NAME VARCHAR(255) NOT NULL, \
                                                TEACHER TEXT, \
                                                CHECK (KCID > 0));"

#define CREATE_GRADE_TABLE_STR "CREATE TABLE IF NOT EXISTS GRADETABLE(ID INT, \
                                                KCH INT, \
                                                SCORE FLOAT, \
                                                FOREIGN KEY(ID) REFERENCES STUINFO(ID), \
                                                FOREIGN KEY(KCH) REFERENCES TIMETABLE(KCID));"

sqlite3* pdb = NULL;

// 初始化数据库
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
        }
    }

    return ret;
}

// 反初始化数据库
int sql_uninit()
{
    int ret = 0;
    if( pdb )
    {
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
        case STUINFO:
            // 学生信息表
            memcpy(sql_cmd, CREATE_STU_TABLE_STR, 1024);
            break;
        case CLASSINFO:
            // 课程表
            memcpy(sql_cmd, CREATE_TIME_TABLE_STR, 1024);
            break;
        case SCOREINFO:
            // 成绩表
            memcpy(sql_cmd, CREATE_GRADE_TABLE_STR, 1024);
            break;
        default:
            // 错误的参数
            printf("input table info is invalid.\n");
            ret = -1;
            break;
    }

    if( !ret )
    {
        ret = sqlite3_exec(pdb, sql_cmd, NULL, NULL, NULL);
        if( ret !=  SQLITE_OK)
        {
            printf("cmd: %s\n", sql_cmd);
            printf("sqlite3_exec fail: %s\n", sqlite3_errmsg(pdb));
            ret = -1;
        }
    }

    return ret;
}

// 数据查询, 目前默认查询全部内容
void *sql_select(AddObject *info)
{
    assert(info);

    int ret = 0;
    char sql_cmd[2048] = {0};
    SelectInfo *result_info = NULL;

    memcpy(sql_cmd, "SELECT ", strlen("SELECT "));

    // 根据表类型查询表格
    switch (info->info)
    {
        case STUINFO:
        {
            // 学生信息表
            StuInfo* si = (StuInfo*)info->data;

            if( info->qt._id == 0 && info->qt._name == 0 )  // 查找全部
            {
                strcat(sql_cmd, "* FROM STUINFO;");
            }
            else if( info->qt._id ) // 按学号查找
            {
                char tmp[1024] = {0};
                sprintf(tmp, "* FROM STUINFO WHERE ID = %d", si->_id);
                strcat(sql_cmd, tmp);
            }
            else    // 按姓名查找
            {
                char tmp[1024] = {0};
                sprintf(tmp, "* FROM STUINFO WHERE NAME = '%s'", si->_name);
                strcat(sql_cmd, tmp);
            }
            break;
        }
        case CLASSINFO:
        {
            // 课程表
            TimeTable* tt = (TimeTable*)info->data;

            if( info->qt._kcid )    // 按课程号查找
            {
                char tmp[1024] = {0};
                sprintf(tmp, "* FROM TIMETABLE WHERE KCID = %d", tt->_kcid);
                strcat(sql_cmd, tmp);
            }
            else if( info->qt._subject )    // 按科目名字查找
            {
                char tmp[1024] = {0};
                sprintf(tmp, "* FROM TIMETABLE WHERE NAME = '%s'", tt->_name);
                strcat(sql_cmd, tmp);
            }
            else if( info->qt._teacher )    // 按老师查找
            {
                char tmp[1024] = {0};
                sprintf(tmp, "* FROM TIMETABLE WHERE TEACHER = '%s'", tt->_teacher);
                strcat(sql_cmd, tmp);
            }
            else
            {
                strcat(sql_cmd, "* FROM TIMETABLE;");
            }
            break;
        }
        case SCOREINFO:
        {
            // 成绩表
            GradeTable* gt = (GradeTable*)info->data;

            char tmp[1024] = {0};
            // 拼接sql语句
            sprintf(tmp, "STUINFO.ID, STUINFO.NAME, \
                            TIMETABLE.NAME, GRADETABLE.SCORE \
                            FROM STUINFO, TIMETABLE, GRADETABLE \
                            WHERE STUINFO.ID = GRADETABLE.ID \
                            AND TIMETABLE.KCID = GRADETABLE.KCH" );
            if( info->qt._all )
            {
                //strcat(tmp, ";");
            }
            else if( info->qt._id && !info->qt._subject )
            {
                char str[512] = {0};
                sprintf(str, " AND GRADETABLE.ID = %d ", gt->_id);
                strcat(tmp, str);
            }
            else if( info->qt._subject && !info->qt._id )
            {
                char str[512] = {0};
                sprintf(str, " AND TIMETABLE.NAME = '%s' ", gt->_subject);
                strcat(tmp, str);
            }
            else
            {
                char str[512] = {0};
                sprintf(str, " AND GRADETABLE.ID = %d AND TIMETABLE.NAME = '%s' ", gt->_id, gt->_subject);
                strcat(tmp, str);
            }

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
        case MYDEFINE:
        {
            // 自定义查询
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
            sprintf(tmp, "WHERE STUINFO.ID = GRADETABLE.ID \
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

    if( !ret )
    {
        char **result;
        int rows, cols;
        printf("%s\n", sql_cmd);
        ret = sqlite3_get_table(pdb, sql_cmd, &result, &rows, &cols, NULL);
        if( ret !=  SQLITE_OK)
        {
            printf("sqlite3_get_table fail: %s\n", sqlite3_errmsg(pdb));
            ret = -1;
        }
        else
        {
            // 查询成功
            // 打印结果
            if( rows && cols )
            {
                printf("\n---------------------------------------------\n");
                for (int i = 0; i < rows + 1; i++) {
                    for (int j = 0; j < cols; j++) {
                        printf("%s\t", result[(i * cols) + j]);
                    }
                    printf("\n---------------------------------------------\n");
                }

                printf("\n\n");
            }
            else
            {
                printf("查询内容为空!\n");
            }
           
            //printf("strlen(result): %ld\n", strlen(*result));
        }

        // 释放结果占用的内存
        sqlite3_free_table(result);
    }

    return (void *)result_info;
}

// 添加数据
int sql_add(AddObject *info, char *filename)
{
    // 参数判断
    assert(info);
    assert(filename);

    int ret = 0;
    char sql_cmd[2048] = {0};

    if( info->info == STUINFO )
    {
        if( filename[0] == '\0' )
        {
            StuInfo* si = (StuInfo *)info->data;

            sprintf(sql_cmd, "INSERT INTO STUINFO VALUES(%d, '%s', %d, '%s', '%s');", 
                                            si->_id, si->_name, si->_age, si->_addr, si->_tel);
        }
        else
        {
            ret = access(filename, F_OK);
            if( ret < 0 )
            {
                printf("文件[%s]不存在.\n", filename);
                return ret;
            }
            StuInfo* si = (StuInfo *)info->data;

            // 打开文件
            FILE* fp = fopen(filename, "r");
            if( !fp )
            {
                printf("打开文件[%s]失败: %s\n", filename, strerror(errno));
                ret = -1;
                return ret;
            }

            printf("导入中...\n");
            while(1)
            {
                memset(si, 0, sizeof(StuInfo));

                ret = fscanf(fp, "%d%s%d%s%s", &si->_id, si->_name, &si->_age, si->_addr, si->_tel);
                if( ret != 5 )
                {
                    if( feof(fp) )
                    {
                        ret = 0;
                        break;
                    }
                    continue;
                }

                sprintf(sql_cmd, "INSERT INTO STUINFO VALUES(%d, '%s', %d, '%s', '%s');", 
                                            si->_id, si->_name, si->_age, si->_addr, si->_tel);

                //printf("%s\n", sql_cmd);
                ret = sqlite3_exec(pdb, sql_cmd, NULL, NULL, NULL);
                if( ret != SQLITE_OK )
                {
                    //printf("sqlite3_exec fail: [%d]%s\n", ret, sqlite3_errmsg(pdb));
                    printf("添加失败: [%d %s]\n", si->_id, si->_name);
                }

            }

            printf("导入完成!\n");

            fclose(fp);
            return ret;
        }
    }
    else if( info->info == CLASSINFO )
    {
        if( filename[0] == '\0' )
        {
            TimeTable* tt = (TimeTable *)info->data;

            sprintf(sql_cmd, "INSERT INTO TIMETABLE VALUES(%d, '%s', '%s');", 
                                                    tt->_kcid, tt->_name, tt->_teacher);
        }
        else
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
                }

            }

            printf("导入完成!\n");

            fclose(fp);
            return ret;
        }
    }
    else if( info->info == SCOREINFO )
    {
        if( filename[0] == '\0' )
        {
            GradeTable* gt = (GradeTable *)info->data;

            sprintf(sql_cmd, "INSERT INTO GRADETABLE VALUES(%d, %d, %f);", 
                                                    gt->_id, gt->_kch, gt->_score);
        }
        else
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
                }

            }

            printf("导入完成!\n");

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
        ret = sqlite3_exec(pdb, sql_cmd, NULL, NULL, NULL);
        if( ret != SQLITE_OK )
        {
            printf("sqlite3_exec fail: %s\n", sqlite3_errmsg(pdb));
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

    if( info->info == STUINFO )
    {
        StuInfo* si = (StuInfo *)info->data;

        if( info->qt._id )
        {
            sprintf(sql_cmd, "DELETE FROM STUINFO WHERE ID = %d;", si->_id);
        }
        else if( info->qt._name )
        {
            sprintf(sql_cmd, "DELETE FROM STUINFO WHERE NAME = '%s';", si->_name);
        }
        else if( info->qt._all )
        {
            sprintf(sql_cmd, "DELETE FROM STUINFO;");
        }
        else
        {
            ret = -1;
        }
    }
    else if( info->info == CLASSINFO )
    {
        TimeTable* tt = (TimeTable *)info->data;

        if( info->qt._kcid )
        {
            sprintf(sql_cmd, "DELETE FROM TIMETABLE WHERE KCID = %d;", tt->_kcid);
        }
        else if( info->qt._subject )
        {
            sprintf(sql_cmd, "DELETE FROM TIMETABLE WHERE NAME = '%s';", tt->_name);
        }
        else if( info->qt._teacher )
        {
            sprintf(sql_cmd, "DELETE FROM TIMETABLE WHERE TEACHER = '%s';", tt->_teacher);        
        }
        else if( info->qt._all )
        {
            sprintf(sql_cmd, "DELETE FROM TIMETABLE;");
        }
        else
        {
            ret = -1;
        }
    }
    else if( info->info == SCOREINFO )
    {
        GradeTable* gt = (GradeTable *)info->data;
        if( info->qt._id )
        {
            if( info->qt._kcid )
            {
                sprintf(sql_cmd, "DELETE FROM GRADETABLE WHERE ID = %d AND KCH = %d;", gt->_id, gt->_kch);
            }
            else if( info->qt._subject )
            {
                sprintf(sql_cmd, "DELETE FROM GRADETABLE WHERE ID = %d AND TIMETABLE.NAME = '%s' AND TIMETABLE.KCID = KCH;", gt->_id, gt->_subject);
            }
            else
            {
                sprintf(sql_cmd, "DELETE FROM GRADETABLE WHERE ID = %d;", gt->_id);
            }
        }
        else if( info->qt._kcid )
        {
            sprintf(sql_cmd, "DELETE FROM GRADETABLE WHERE KCH = '%d';", gt->_kch);
        }
        else if( info->qt._subject )
        {
            sprintf(sql_cmd, "DELETE FROM GRADETABLE WHERE GRADETABLE.KCH = TIMETABLE.KCID AND TIMETABLE.NAME = '%s';", gt->_subject);
        }
        else if( info->qt._all )
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
        printf("%s\n", sql_cmd);
        ret = sqlite3_exec(pdb, sql_cmd, NULL, NULL, NULL);
        if( ret != SQLITE_OK )
        {
            printf("sqlite3_exec fail: %s\n", sqlite3_errmsg(pdb));
        }
    }

    return ret;
}

// 修改数据
int sql_alter(AddObject *old_info, AddObject *new_info)
{
    assert( new_info );
    assert( old_info );

    int ret = 0;
    char sql_cmd[2048] = {0};

    //memcpy(sql_cmd, "update ", strlen("update "));

    switch (new_info->info)
    {
        case STUINFO:
        {
            StuInfo* osi = (StuInfo *)old_info->data;
            StuInfo* nsi = (StuInfo *)new_info->data;

            if( osi->_id )
            {
                sprintf(sql_cmd, "update STUINFO set ID=%d,NAME='%s',AGE=%d,ADDR='%s',TEL='%s' where id=%d", \
                            nsi->_id, nsi->_name, nsi->_age, nsi->_addr, nsi->_tel, osi->_id);
            }
            else
            {
                ret = -1;
            }

            break;
        }
        case CLASSINFO:
        {
            TimeTable* ntt = (TimeTable *)new_info->data;
            TimeTable* ott = (TimeTable *)old_info->data;

            if( ott->_kcid )
            {
                sprintf(sql_cmd, "update TIMETABLE set KCID=%d,NAME='%s',TEACHER='%s' where KCID=%d", \
                            ntt->_kcid, ntt->_name, ntt->_teacher, ott->_kcid);
            }
            else if( ott->_name[0] != '\0' )
            {
                sprintf(sql_cmd, "update TIMETABLE set KCID=%d,NAME='%s',TEACHER='%s' where NAME='%s'", \
                            ntt->_kcid, ntt->_name, ntt->_teacher, ott->_name);
            }
            else if(ott->_teacher[0] != '\0')
            {
                sprintf(sql_cmd, "update TIMETABLE set KCID=%d,NAME='%s',TEACHER='%s' where TEACHER='%s'", \
                            ntt->_kcid, ntt->_name, ntt->_teacher, ott->_teacher);
            }
            else
            {
                ret = -1;
            }

            break;
        }
        case SCOREINFO:
        {
            GradeTable* ngt = (GradeTable *)new_info->data;
            GradeTable* ogt = (GradeTable *)old_info->data;

            if( ogt->_id && ogt->_kch )
            {
                sprintf(sql_cmd, "update TIMETABLE set SCORE='%f' where ID=%d and KCH=%d", \
                            ngt->_score, ngt->_id, ngt->_kch);
            }
            else
            {
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
        ret = sqlite3_exec(pdb, sql_cmd, NULL, NULL, NULL);
        if( ret != SQLITE_OK )
        {
            printf("sqlite3_exec fail: %s\n", sqlite3_errmsg(pdb));
        }
    }

    return ret;

}
