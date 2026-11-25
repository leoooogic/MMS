/* medicine.cpp - 药品管理模块（学生实现）
 * 
 * 说明：本文件包含纯业务逻辑，不涉及任何I/O操作
 * 学生只需实现这些函数，GUI层会负责所有用户交互
 *
 * 教学进度：
 *   第2次课：addMedicine, getAllMedicines
 *   第3次课：findMedicineById, findMedicineByName, modifyMedicine
 *   第4次课：deleteMedicine
 */
#include "common.h"
#include "medicine.h"

/* TODO 第2次课：添加药品到链表
 * 参数：med - 要添加的药品信息
 * 返回：1=成功，0=失败（编号已存在或内存不足）
 */
int addMedicine(Medicine* med)
{
    /* 请在这里实现你的代码 */
    return 0;
}

/* TODO 第3次课：按编号查找药品
 * 参数：id - 药品编号
 * 返回：节点指针，未找到返回NULL
 */
MedNode* findMedicineById(const char* id)
{
    /* 请在这里实现你的代码 */
    return NULL;
}

/* TODO 第3次课：按名称模糊查找药品
 * 参数：keyword - 关键字
 * 返回：第一个匹配的节点指针，未找到返回NULL
 */
MedNode* findMedicineByName(const char* keyword)
{
    /* 请在这里实现你的代码 */
    return NULL;
}

/* TODO 第3次课：修改药品信息
 * 参数：id - 药品编号，newMed - 新的药品信息
 * 返回：1=成功，0=失败（未找到）
 */
int modifyMedicine(const char* id, Medicine* newMed)
{
    /* 请在这里实现你的代码 */
    return 0;
}

/* TODO 第4次课：删除药品
 * 参数：id - 药品编号
 * 返回：1=成功，0=失败（未找到或库存>0）
 */
int deleteMedicine(const char* id)
{
    /* 请在这里实现你的代码 */
    return 0;
}

/* TODO 第2次课：获取所有药品列表
 * 参数：list - 输出数组，count - 输入最大数量/输出实际数量
 */
void getAllMedicines(Medicine* list, int* count)
{
    /* 请在这里实现你的代码 */
  
}
