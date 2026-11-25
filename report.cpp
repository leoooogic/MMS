/* report.cpp - 报表与决策辅助模块（学生实现）
 * 
 * 说明：本文件包含纯业务逻辑，不涉及任何I/O操作
 * 学生只需实现这些函数，GUI层会负责所有用户交互
 *
 * 教学进度：
 *   第7次课：getWarningReport, getStatisticsReport
 *   第8次课：exportReport
 */
#include "common.h"
#include "report.h"

/* TODO 第7次课：获取库存预警报告
 * 参数：report - 输出参数，预警报告
 */
void getWarningReport(WarningReport* report)
{
    /* 请在这里实现你的代码 */
    report->lowStockCount = 0;
    report->nearExpiryCount = 0;
    report->expiredCount = 0;
    report->warningList[0] = '\0';
}

/* TODO 第7次课：获取统计报表
 * 参数：report - 输出参数，统计报表
 */
void getStatisticsReport(StatisticsReport* report)
{
    /* 请在这里实现你的代码 */
    report->totalMedicines = 0;
    report->totalRecords = 0;
    report->purchaseCount = 0;
    report->salesCount = 0;
    report->totalPurchase = 0;
    report->totalSales = 0;
    report->profit = 0;
    report->profitRate = 0;
}

/* TODO 第8次课：导出报表到文件
 * 参数：filename - 导出文件路径
 * 返回：1=成功，0=失败
 */
int exportReport(const char* filename)
{
    /* 请在这里实现你的代码 */
    return 0;
}
