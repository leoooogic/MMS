/* report.h - 报表与决策辅助模块接口 */
/* 第1次课：请根据教学任务，在此文件中定义报表结构体和声明函数 */
#ifndef REPORT_H
#define REPORT_H

#include "common.h"

/* 库存与有效期预警报表 */
typedef struct {
    int lowStockCount;          /* stock < WARN_STOCK 的药品数量 */
    int nearExpiryCount;        /* 距到期不超过 WARN_MONTH 个月的药品数量 */
    int expiredCount;           /* 已过期药品数量 */
    char warningList[2000];     /* 预警详情文本 */
} WarningReport;

/* 经营统计报表，利润口径按课程任务约定 */
typedef struct {
    int totalMedicines;         /* 药品种类总数 */
    int totalRecords;           /* 单据记录总数 */
    int purchaseCount;          /* 进货单据数量 */
    int salesCount;             /* 销售单据数量 */
    float totalPurchase;        /* 进货总金额 */
    float totalSales;           /* 销售总金额 */
    float profit;               /* totalSales - totalPurchase */
    float profitRate;           /* profit / totalPurchase * 100% */
} StatisticsReport;

void getWarningReport(WarningReport *report);        /* 生成库存预警报告 */
void getStatisticsReport(StatisticsReport *report);  /* 生成经营统计报表 */
int exportReport(const char *filename);             /* 导出报表到文件 */

#endif /* REPORT_H */
