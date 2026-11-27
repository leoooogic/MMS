/* record.h - 业务处理模块接口 */
/* 第1次课：请根据教学任务，在此文件中声明业务处理函数 */
#ifndef RECORD_H
#define RECORD_H

#include "common.h"

int purchase(const char *medId, int qty, float price, Record *outRec); /* 进货入库 */
int sell(const char *medId, int qty, float price, Record *outRec);     /* 销售出库 */
void getAllRecords(Record *list, int *count);                        /* 获取所有单据 */
void getRecordsByType(int type, Record *list, int *count);           /* 按类型获取单据 */

#endif /* RECORD_H */
