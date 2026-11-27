/* common.h - 医药进销存管理系统 */
/* 第1次课：请根据教学任务，在此文件中定义数据结构和常量 */
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <tchar.h>

#ifdef _WIN32
#include <windows.h>
#endif

/* 字段长度（包含字符串结束标记 '\0'） */
#define MAX_ID_LEN 10
#define MAX_NAME_LEN 31
#define MAX_SPEC_LEN 21
#define MAX_UNIT_LEN 11
#define MAX_MAKER_LEN 31
#define MAX_DATE_LEN 11

/* 系统参数 */
#define WARN_STOCK 20
#define WARN_MONTH 3
#define DATA_FILE "medicine.dat"
#define REC_FILE "records.dat"
#define PASSWORD "123456"

/* 颜色定义（兼容不同版本 EasyX，若已定义则跳过） */
#ifndef ORANGE
#define ORANGE RGB(255, 165, 0)
#endif

/* 药品档案 */
typedef struct {
    char id[MAX_ID_LEN];          /* 唯一药品编号 */
    char name[MAX_NAME_LEN];      /* 药品名称 */
    char spec[MAX_SPEC_LEN];      /* 规格 */
    char unit[MAX_UNIT_LEN];      /* 单位 */
    char maker[MAX_MAKER_LEN];    /* 生产厂家 */
    float buyPrice;              /* 进货单价 */
    float sellPrice;             /* 销售单价 */
    int stock;                   /* 当前库存 */
    int expY;                    /* 有效期年份 */
    int expM;                    /* 有效期月份 */
} Medicine;

/* 进销单据 */
typedef struct {
    int type;                    /* 1=进货，2=销售 */
    char date[MAX_DATE_LEN];      /* 单据日期 */
    char mid[MAX_ID_LEN];         /* 关联药品编号 */
    char mname[MAX_NAME_LEN];     /* 药品名称 */
    int qty;                     /* 数量 */
    float price;                 /* 单价 */
    float amount;                /* 金额：qty * price */
} Record;

/* 链表节点 */
typedef struct MedNode {
    Medicine data;
    struct MedNode *next;
} MedNode;

typedef struct RecNode {
    Record data;
    struct RecNode *next;
} RecNode;

/* 全局变量在 main.cpp 中定义 */
extern MedNode *medHead;
extern int medCount;
extern RecNode *recHead;
extern int recCount;

/* 公共辅助函数，在 storage.cpp 中实现 */
void loadData(void);                         /* 从文件加载数据 */
void saveData(void);                         /* 保存数据到文件 */
MedNode *findMedById(const char *id);         /* 按编号查找药品节点 */
int monthsLeft(MedNode *p);                  /* 计算距有效期剩余月数 */
void today(char *buf);                       /* 写入当前日期字符串 */

#endif /* COMMON_H */
