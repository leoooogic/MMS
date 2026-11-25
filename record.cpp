/* record.cpp - 业务处理模块（学生实现）
 * 
 * 说明：本文件包含纯业务逻辑，不涉及任何I/O操作
 * 学生只需实现这些函数，GUI层会负责所有用户交互
 *
 * 教学进度：
 *   第6次课：purchase, sell
 *   第7次课：getAllRecords
 */
#include "common.h"
#include "record.h"

/* 辅助函数：生成单据编号（已提供，无需修改）
 * type: 1=进货, 2=销售
 * 进货编号格式：J + 年月日 + 3位序号
 * 销售编号格式：X + 年月日 + 3位序号
 */
static void generateRecId(char* buf, int type)
{
    time_t t = time(NULL);
    struct tm* p = localtime(&t);
    
    if (type == 1) {
        sprintf(buf, "J%04d%02d%02d%03d", 
                p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, recCount + 1);
    } else {
        sprintf(buf, "X%04d%02d%02d%03d", 
                p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, recCount + 1);
    }
}

/* TODO 第6次课：进货入库
 * 参数：medId - 药品编号，qty - 数量，price - 单价，outRec - 输出记录
 * 返回：1=成功，0=失败
 */
int purchase(const char* medId, int qty, float price, Record* outRec)
{
    /* 请在这里实现你的代码 */
    return 0;
}

/* TODO 第6次课：销售出库
 * 参数：medId - 药品编号，qty - 数量，price - 单价，outRec - 输出记录
 * 返回：1=成功，0=失败
 */
int sell(const char* medId, int qty, float price, Record* outRec)
{
    /* 请在这里实现你的代码 */
    return 0;
}

/* TODO 第7次课：获取所有单据
 * 参数：list - 输出数组，count - 输入最大数量/输出实际数量
 */
void getAllRecords(Record* list, int* count)
{
    /* 请在这里实现你的代码 */
    *count = 0;
}

/* 按类型获取单据（1=进货，2=销售）- 选做 */
void getRecordsByType(int type, Record* list, int* count)
{
    /* 请在这里实现你的代码 */
    *count = 0;
}
