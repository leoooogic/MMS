/* storage.cpp - 数据持久化模块（学生实现）
 * 
 * 说明：本文件包含数据保存/加载功能
 * 辅助函数 findMedById、monthsLeft、today 已提供
 *
 * 教学进度：
 *   第5次课：saveData, loadData
 */
#include "common.h"

/* TODO 第5次课：保存所有数据到文件
 * 将药品链表保存到 DATA_FILE（medicine.dat）
 * 将单据链表保存到 REC_FILE（records.dat）
 * 格式：先写入数量(int)，再逐个写入结构体
 */
void saveData(void)
{
    /* 请在这里实现你的代码 */
}

/* TODO 第5次课：从文件加载所有数据
 * 从 DATA_FILE 加载药品链表
 * 从 REC_FILE 加载单据链表
 * 格式：先读取数量(int)，再逐个读取结构体并创建节点
 */
void loadData(void)
{
    /* 请在这里实现你的代码 */
}

/* ============================================================
 * 以下为辅助函数（已提供，无需修改）
 * ============================================================ */

/* 按编号查找药品（内部使用）
 * 参数：id - 药品编号
 * 返回：节点指针，未找到返回NULL
 */
MedNode *findMedById(const char *id)
{
    MedNode *p;
    for (p = medHead; p != NULL; p = p->next)
        if (strcmp(p->data.id, id) == 0)
            return p;
    return NULL;
}

/* 计算药品距到期还有多少个月
 * 参数：p - 药品节点指针
 * 返回：剩余月数，负数表示已过期
 */
int monthsLeft(MedNode *p)
{
    time_t     t = time(NULL);
    struct tm *now = localtime(&t);
    int curY = now->tm_year + 1900;
    int curM = now->tm_mon + 1;
    return (p->data.expY - curY) * 12 + (p->data.expM - curM);
}

/* 获取当前日期字符串
 * 参数：buf - 输出缓冲区（至少11字节）
 * 格式：yyyy-mm-dd
 */
void today(char *buf)
{
    time_t     t = time(NULL);
    struct tm *p = localtime(&t);
    sprintf(buf, "%04d-%02d-%02d",
            p->tm_year + 1900, p->tm_mon + 1, p->tm_mday);
}
