/* medicine.h - 药品管理模块接口 */
/* 第1次课：请根据教学任务，在此文件中声明药品管理函数 */
#ifndef MEDICINE_H
#define MEDICINE_H

#include "common.h"

int addMedicine(Medicine *med);                       /* 添加药品 */
MedNode *findMedicineById(const char *id);             /* 按编号查找药品 */
MedNode *findMedicineByName(const char *keyword);      /* 按名称关键字查找药品 */
int modifyMedicine(const char *id, Medicine *newMed);  /* 修改药品信息 */
int deleteMedicine(const char *id);                   /* 删除药品档案 */
void getAllMedicines(Medicine *list, int *count);      /* 输入容量，输出列表及实际数量 */

#endif /* MEDICINE_H */
