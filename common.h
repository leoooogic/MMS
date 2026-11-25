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

/* TODO 第1次课：定义字段长度常量 */
/* MAX_ID_LEN, MAX_NAME_LEN, MAX_SPEC_LEN, MAX_UNIT_LEN, MAX_MAKER_LEN, MAX_DATE_LEN */

/* TODO 第1次课：定义系统参数常量 */
/* WARN_STOCK, WARN_MONTH, DATA_FILE, REC_FILE, PASSWORD */

/* 颜色定义（兼容不同版本 EasyX，若已定义则跳过） */
#ifndef ORANGE
#define ORANGE RGB(255, 165, 0)
#endif

/* TODO 第1次课：定义 Medicine 结构体（药品档案） */
/* 字段：id, name, spec, unit, maker, buyPrice, sellPrice, stock, expY, expM */

/* TODO 第1次课：定义 Record 结构体（进销单据） */
/* 字段：type, date, mid, mname, qty, price, amount */

/* TODO 第1次课：定义 MedNode 和 RecNode 链表节点 */

/* TODO 第1次课：声明全局变量 *medHead, medCount, *recHead, recCount */

/* TODO 第1次课：声明函数 loadData, saveData, *findMedById（按编号查找药品结点）, monthsLeft, today */

#endif /* COMMON_H */
