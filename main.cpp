/* main.cpp - 医药进销存管理系统 GUI版（纯GUI架构）
 * 
 * 架构说明：
 * - 本文件：GUI层，负责所有用户交互
 * - medicine.cpp：药品管理业务逻辑（学生实现）
 * - record.cpp：业务处理逻辑（学生实现）
 * - report.cpp：报表决策逻辑（学生实现）
 * 
 * 学生只需实现 business 层的纯C函数，无需关心GUI
 */
#include "common.h"
#include "medicine.h"
#include "record.h"
#include "report.h"
#include <graphics.h>
#include <windows.h>

/* 全局变量 */
MedNode *medHead = NULL;
int     medCount = 0;
RecNode *recHead = NULL;
int     recCount = 0;

/* ========== 辅助函数 ========== */

/* UTF-8转宽字符串 */
void utf8_to_wchar(const char *src, wchar_t *dst, int dstSize)
{
    MultiByteToWideChar(CP_UTF8, 0, src, -1, dst, dstSize);
    dst[dstSize - 1] = L'\0';
}

/* 显示消息框（UTF-8文本） */
static void showMsgBox(const char* msg, const char* title, UINT type)
{
    wchar_t wMsg[500], wTitle[100];
    utf8_to_wchar(msg, wMsg, 500);
    utf8_to_wchar(title, wTitle, 100);
    MessageBoxW(GetHWnd(), wMsg, wTitle, type);
}

/* 输出中文字符串 */
void outtextxy_char(int x, int y, const char *str)
{
    wchar_t buf[256];
    utf8_to_wchar(str, buf, 256);
    outtextxy(x, y, buf);
}

/* 设置字体 */
void setfont(const char *fontName, int height)
{
    wchar_t wfont[50];
    utf8_to_wchar(fontName, wfont, 50);
    settextstyle(height, 0, wfont);
}

/* 绘制背景 */
void drawBackground(void)
{
    for (int y = 0; y < 600; y++) {
        int r = 220 + (240 - 220) * y / 600;
        int g = 235 + (245 - 235) * y / 600;
        int b = 255;
        setlinecolor(RGB(r, g, b));
        line(0, y, 800, y);
    }
    setbkmode(TRANSPARENT);
}

/* ========== 输入弹窗（独立窗口，宽字符版解决中文乱码） ========== */

/* 注册弹窗窗口类（只调用一次） */
static void registerPopupClass(void)
{
    static int registered = 0;
    if (registered) return;
    
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"PopupClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassExW(&wc);
    registered = 1;
}

static int showInputPopupAt(const char* title, const char* prompt, char* result, int maxLen, int password, int x, int y)
{
    result[0] = '\0';
    registerPopupClass();
    
    /* UTF-8 转宽字符 */
    wchar_t wTitle[100], wPrompt[200];
    utf8_to_wchar(title, wTitle, 100);
    utf8_to_wchar(prompt, wPrompt, 200);
    
    /* 创建弹窗窗口（使用自定义类，标题只显示在标题栏） */
    HWND hPopup = CreateWindowExW(
        WS_EX_TOPMOST, L"PopupClass", wTitle,
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        x, y, 320, 160, NULL, NULL, GetModuleHandleW(NULL), NULL
    );
    
    if (!hPopup) return 0;
    
    HFONT hFont = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"宋体");
    
    /* 提示标签 */
    HWND hLabel = CreateWindowExW(0, L"STATIC", wPrompt,
        WS_CHILD | WS_VISIBLE | SS_LEFT, 15, 12, 290, 20, hPopup, NULL, GetModuleHandleW(NULL), NULL);
    SendMessageW(hLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    /* 输入框 */
    DWORD editStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER;
    if (password) editStyle |= ES_PASSWORD;
    
    HWND hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", editStyle,
        15, 38, 290, 28, hPopup, NULL, GetModuleHandleW(NULL), NULL);
    SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    SetFocus(hEdit);
    
    /* 确定按钮 */
    HWND hBtnOk = CreateWindowExW(0, L"BUTTON", L"确定",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 60, 80, 90, 32, hPopup, NULL, GetModuleHandleW(NULL), NULL);
    SendMessageW(hBtnOk, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    /* 取消按钮 */
    HWND hBtnCancel = CreateWindowExW(0, L"BUTTON", L"取消",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 170, 80, 90, 32, hPopup, NULL, GetModuleHandleW(NULL), NULL);
    SendMessageW(hBtnCancel, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    /* 等待用户输入（带消息泵，防止窗口冻结） */
    int done = 0;
    while (!done) {
        MSG msg;
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                DestroyWindow(hPopup);
                DeleteObject(hFont);
                return 0;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        
        Sleep(10);
        
        if (!IsWindow(hPopup)) {
            DeleteObject(hFont);
            return strlen(result) > 0;
        }
        
        /* 按回车确认 */
        if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
            wchar_t wResult[256];
            GetWindowTextW(hEdit, wResult, 256);
            WideCharToMultiByte(CP_UTF8, 0, wResult, -1, result, maxLen, NULL, NULL);
            done = 1;
        }
        
        /* 按ESC取消 */
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            result[0] = '\0';
            done = 1;
        }
        
        /* 点击按钮 */
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hPopup, &pt);
            
            /* 确定按钮 (60-150, 80-112) */
            if (pt.x >= 60 && pt.x <= 150 && pt.y >= 80 && pt.y <= 112) {
                wchar_t wResult[256];
                GetWindowTextW(hEdit, wResult, 256);
                WideCharToMultiByte(CP_UTF8, 0, wResult, -1, result, maxLen, NULL, NULL);
                done = 1;
                while (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                    }
                    Sleep(10);
                }
            }
            /* 取消按钮 (170-260, 80-112) */
            else if (pt.x >= 170 && pt.x <= 260 && pt.y >= 80 && pt.y <= 112) {
                result[0] = '\0';
                done = 1;
                while (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                    }
                    Sleep(10);
                }
            }
        }
    }
    
    DeleteObject(hFont);
    DestroyWindow(hPopup);
    return strlen(result) > 0;
}

/* 默认位置弹窗 */
static int showInputPopup(const char* title, const char* prompt, char* result, int maxLen, int password)
{
    return showInputPopupAt(title, prompt, result, maxLen, password, 250, 200);
}

/* 居中弹窗（800x600 窗口） */
static int showInputPopupCentered(const char* title, const char* prompt, char* result, int maxLen, int password)
{
    /* 获取 EasyX 窗口位置，计算居中坐标 */
    HWND hMain = GetHWnd();
    RECT rc;
    GetWindowRect(hMain, &rc);
    int x = rc.left + (rc.right - rc.left - 320) / 2;
    int y = rc.top + (rc.bottom - rc.top - 180) / 2;
    return showInputPopupAt(title, prompt, result, maxLen, password, x, y);
}

/* ========== 文本显示弹窗（可滚动，用于显示长列表） ========== */

/* 将 \n 转为 \r\n（EDIT 控件需要） */
static void fixNewlines(const char* src, char* dst, int dstSize)
{
    int j = 0;
    for (int i = 0; src[i] && j < dstSize - 2; i++) {
        if (src[i] == '\n' && (i == 0 || src[i-1] != '\r')) {
            dst[j++] = '\r';
        }
        dst[j++] = src[i];
    }
    dst[j] = '\0';
}

static void showTextPopup(const char* title, const char* text)
{
    registerPopupClass();
    
    char fixedText[8000];
    fixNewlines(text, fixedText, 8000);
    
    wchar_t wTitle[100], wText[8000];
    utf8_to_wchar(title, wTitle, 100);
    utf8_to_wchar(fixedText, wText, 8000);
    
    HWND hMain = GetHWnd();
    RECT rc;
    GetWindowRect(hMain, &rc);
    int w = 900, h = 450;
    int x = rc.left + (rc.right - rc.left - w) / 2;
    int y = rc.top + (rc.bottom - rc.top - h) / 2;
    
    HWND hPopup = CreateWindowExW(
        WS_EX_TOPMOST, L"PopupClass", wTitle,
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        x, y, w, h, NULL, NULL, GetModuleHandleW(NULL), NULL
    );
    if (!hPopup) return;
    
    HFONT hFont = CreateFontW(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
    
    /* 多行文本框（带垂直滚动条，自动换行） */
    HWND hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", wText,
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL,
        10, 10, w - 30, h - 60, hPopup, NULL, GetModuleHandleW(NULL), NULL);
    SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    /* 确定按钮 */
    HWND hBtn = CreateWindowExW(0, L"BUTTON", L"确定",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, w/2 - 50, h - 45, 100, 32,
        hPopup, NULL, GetModuleHandleW(NULL), NULL);
    SendMessageW(hBtn, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    int done = 0;
    while (!done) {
        MSG msg;
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                DestroyWindow(hPopup);
                DeleteObject(hFont);
                return;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        Sleep(10);
        if (!IsWindow(hPopup)) { DeleteObject(hFont); return; }
        
        if (GetAsyncKeyState(VK_RETURN) & 0x8000) done = 1;
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) done = 1;
        
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hPopup, &pt);
            if (pt.x >= w/2 - 50 && pt.x <= w/2 + 50 && pt.y >= h - 45 && pt.y <= h - 13) {
                done = 1;
                while (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                    }
                    Sleep(10);
                }
            }
        }
    }
    
    DeleteObject(hFont);
    DestroyWindow(hPopup);
}

/* ========== 登录功能 ========== */
int login(void)
{
    drawBackground();
    settextcolor(RGB(30, 60, 120));
    setfont("微软雅黑", 32);
    outtextxy_char(280, 130, "医药进销存管理系统");
    
    /* 绘制登录按钮 */
    int btnX = 330, btnY = 320, btnW = 140, btnH = 45;
    setfillcolor(RGB(100, 150, 220));
    fillroundrect(btnX, btnY, btnX + btnW, btnY + btnH, 10, 10);
    setfont("宋体", 16);
    settextcolor(WHITE);
    outtextxy_char(btnX + 45, btnY + 12, "登 录");
    
    /* 版权信息（居中） */
    setfont("宋体", 12);
    settextcolor(RGB(150, 150, 150));
    outtextxy_char(280, 540, "版权所有：医药进销存管理系统 v1.0");
    
    /* 等待点击登录按钮 */
    int clicked = 0;
    while (!clicked) {
        if (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                if (msg.x >= btnX && msg.x <= btnX + btnW &&
                    msg.y >= btnY && msg.y <= btnY + btnH) {
                    clicked = 1;
                }
            }
        }
        Sleep(10);
    }
    
    /* 弹出密码输入框（居中显示） */
    char password[50] = "";
    showInputPopupCentered("用户登录", "请输入系统密码：", password, 50, 1);
    
    /* 用户取消（关闭弹窗或按ESC） */
    if (strlen(password) == 0) {
        return login();  /* 重新显示登录界面 */
    }
    
    if (strcmp(password, PASSWORD) == 0) {
        drawBackground();
        setfont("微软雅黑", 24);
        settextcolor(RGB(30, 120, 60));
        outtextxy_char(280, 300, "登录成功！");
        Sleep(1000);
        return 1;
    } else {
        drawBackground();
        setfont("微软雅黑", 24);
        settextcolor(RGB(200, 60, 60));
        outtextxy_char(280, 300, "密码错误！");
        Sleep(1500);
        return login();
    }
}

/* ========== 主菜单 ========== */
void drawMainMenu(void)
{
    drawBackground();
    settextcolor(RGB(30, 60, 120));
    setfont("微软雅黑", 32);
    outtextxy_char(230, 10, "医药进销存管理系统");
    
    int btnW = 140, btnH = 40, gapX = 20, gapY = 15;
    int startX = 80, y = 70;
    
    /* 药品档案 */
    setfont("宋体", 14);
    settextcolor(RGB(60, 60, 60));
    outtextxy_char(startX, y, "【药品档案】");
    y += 25;
    setfont("宋体", 15);
    setfillcolor(RGB(100, 150, 220));
    fillroundrect(startX, y, startX + btnW, y + btnH, 8, 8);
    settextcolor(WHITE);
    outtextxy_char(startX + 20, y + 10, "1.录入药品");
    setfillcolor(RGB(100, 150, 220));
    fillroundrect(startX + btnW + gapX, y, startX + 2*btnW + gapX, y + btnH, 8, 8);
    outtextxy_char(startX + btnW + gapX + 20, y + 10, "2.浏览药品");
    setfillcolor(RGB(100, 150, 220));
    fillroundrect(startX + 2*(btnW + gapX), y, startX + 3*btnW + 2*gapX, y + btnH, 8, 8);
    outtextxy_char(startX + 2*(btnW + gapX) + 20, y + 10, "3.查询药品");
    y += btnH + gapY;
    setfillcolor(RGB(100, 150, 220));
    fillroundrect(startX, y, startX + btnW, y + btnH, 8, 8);
    settextcolor(WHITE);
    outtextxy_char(startX + 20, y + 10, "4.修改药品");
    setfillcolor(RGB(100, 150, 220));
    fillroundrect(startX + btnW + gapX, y, startX + 2*btnW + gapX, y + btnH, 8, 8);
    outtextxy_char(startX + btnW + gapX + 20, y + 10, "5.删除药品");
    y += btnH + gapY + 20;
    
    /* 业务处理 */
    setfont("宋体", 14);
    settextcolor(RGB(60, 60, 60));
    outtextxy_char(startX, y, "【业务处理】");
    y += 25;
    setfont("宋体", 15);
    setfillcolor(RGB(80, 180, 120));
    fillroundrect(startX, y, startX + btnW, y + btnH, 8, 8);
    settextcolor(WHITE);
    outtextxy_char(startX + 20, y + 10, "6.进货入库");
    setfillcolor(RGB(80, 180, 120));
    fillroundrect(startX + btnW + gapX, y, startX + 2*btnW + gapX, y + btnH, 8, 8);
    outtextxy_char(startX + btnW + gapX + 20, y + 10, "7.销售出库");
    setfillcolor(RGB(80, 180, 120));
    fillroundrect(startX + 2*(btnW + gapX), y, startX + 3*btnW + 2*gapX, y + btnH, 8, 8);
    outtextxy_char(startX + 2*(btnW + gapX) + 20, y + 10, "8.单据浏览");
    y += btnH + gapY + 20;
    
    /* 决策辅助 */
    setfont("宋体", 14);
    settextcolor(RGB(60, 60, 60));
    outtextxy_char(startX, y, "【决策辅助】");
    y += 25;
    setfont("宋体", 15);
    setfillcolor(RGB(200, 150, 80));
    fillroundrect(startX, y, startX + btnW, y + btnH, 8, 8);
    settextcolor(WHITE);
    outtextxy_char(startX + 20, y + 10, "9.库存预警");
    setfillcolor(RGB(200, 150, 80));
    fillroundrect(startX + btnW + gapX, y, startX + 2*btnW + gapX, y + btnH, 8, 8);
    outtextxy_char(startX + btnW + gapX + 20, y + 10, "10.统计报表");
    y += btnH + gapY + 20;
    
    /* 系统 */
    setfont("宋体", 14);
    settextcolor(RGB(60, 60, 60));
    outtextxy_char(startX, y, "【系统】");
    y += 25;
    setfont("宋体", 15);
    setfillcolor(RGB(150, 100, 180));
    fillroundrect(startX, y, startX + btnW, y + btnH, 8, 8);
    settextcolor(WHITE);
    outtextxy_char(startX + 20, y + 10, "11.导出报表");
    setfillcolor(RGB(220, 80, 80));
    fillroundrect(600, y, 740, y + btnH, 10, 10);
    settextcolor(WHITE);
    outtextxy_char(630, y + 10, "0.退出系统");
    
    /* 底部统计 */
    setfont("宋体", 13);
    settextcolor(RGB(80, 80, 80));
    char info[100];
    sprintf(info, "药品品种：%d 种    单据记录：%d 条", medCount, recCount);
    outtextxy_char(80, 540, info);
}

/* ========== 业务功能（调用学生实现的函数） ========== */

/* 1. 录入药品 */
void guiAddMedicine(void)
{
    Medicine med;
    char input[256];
    
    /* GUI层：收集输入（任一弹窗关闭则取消操作） */
    if (!showInputPopupCentered("录入药品", "药品编号（如Y001）*：", med.id, MAX_ID_LEN, 0)) return;
    if (strlen(med.id) == 0) { showMsgBox("药品编号不能为空！", "错误", MB_OK | MB_ICONERROR); return; }
    
    if (!showInputPopupCentered("录入药品", "药品名称*：", med.name, MAX_NAME_LEN, 0)) return;
    if (strlen(med.name) == 0) { showMsgBox("药品名称不能为空！", "错误", MB_OK | MB_ICONERROR); return; }
    
    if (!showInputPopupCentered("录入药品", "规格（如0.25g*24粒）：", med.spec, MAX_SPEC_LEN, 0)) return;
    if (!showInputPopupCentered("录入药品", "单位（盒/瓶/支）：", med.unit, MAX_UNIT_LEN, 0)) return;
    if (!showInputPopupCentered("录入药品", "生产厂家：", med.maker, MAX_MAKER_LEN, 0)) return;
    
    if (!showInputPopupCentered("录入药品", "进价（元）*：", input, 256, 0)) return;
    if (strlen(input) == 0) { showMsgBox("进价不能为空！", "错误", MB_OK | MB_ICONERROR); return; }
    med.buyPrice = atof(input);
    
    if (!showInputPopupCentered("录入药品", "售价（元）*：", input, 256, 0)) return;
    if (strlen(input) == 0) { showMsgBox("售价不能为空！", "错误", MB_OK | MB_ICONERROR); return; }
    med.sellPrice = atof(input);
    
    if (!showInputPopupCentered("录入药品", "库存数量*：", input, 256, 0)) return;
    if (strlen(input) == 0) { showMsgBox("库存数量不能为空！", "错误", MB_OK | MB_ICONERROR); return; }
    med.stock = atoi(input);
    
    if (!showInputPopupCentered("录入药品", "有效期年（如2027）*：", input, 256, 0)) return;
    if (strlen(input) == 0) { showMsgBox("有效期年不能为空！", "错误", MB_OK | MB_ICONERROR); return; }
    med.expY = atoi(input);
    
    if (!showInputPopupCentered("录入药品", "有效期月（1-12）*：", input, 256, 0)) return;
    if (strlen(input) == 0) { showMsgBox("有效期月不能为空！", "错误", MB_OK | MB_ICONERROR); return; }
    med.expM = atoi(input);
    
    /* 调用业务层 */
    int result = addMedicine(&med);
    
    /* GUI层：显示结果 */
    if (result) {
        wchar_t msg[200];
        char buf[200];
        sprintf(buf, "? 录入成功！\n\n药品：%s（%s）", med.name, med.id);
        utf8_to_wchar(buf, msg, 200);
        MessageBoxW(GetHWnd(), msg, L"成功", MB_OK | MB_ICONINFORMATION);
    } else {
        showMsgBox("录入失败！编号可能已存在。", "错误", MB_OK | MB_ICONERROR);
    }
}

/* 2. 浏览药品 */
void guiListMedicine(void)
{
    Medicine list[100];
    int count = 100;
    getAllMedicines(list, &count);
    
    if (count == 0) {
        showMsgBox("暂无药品记录。", "提示", MB_OK | MB_ICONINFORMATION);
        return;
    }
    
    char display[3000] = "";
    char line[300];
    
    for (int i = 0; i < count; i++) {
        sprintf(line, "【%s】%s  规格:%s  单位:%s  进价:%.2f  售价:%.2f  库存:%d  有效期:%04d-%02d\r\n",
                list[i].id, list[i].name, list[i].spec, list[i].unit,
                list[i].buyPrice, list[i].sellPrice, list[i].stock,
                list[i].expY, list[i].expM);
        strcat(display, line);
        
        if (i < count - 1) {
            strcat(display, "────────────────────────────────────────\r\n");
        }
    }
    
    sprintf(line, "\r\n共 %d 种药品。", count);
    strcat(display, line);
    
    wchar_t wdisplay[3000];
    utf8_to_wchar(display, wdisplay, 3000);
    showTextPopup("全部药品档案", display);
}

/* 3. 查询药品 */
void guiSearchMedicine(void)
{
    char keyword[MAX_NAME_LEN];
    showInputPopupCentered("查询药品", "请输入药品名称关键字：", keyword, MAX_NAME_LEN, 0);
    
    /* 用户取消（未输入关键字） */
    if (strlen(keyword) == 0) return;
    
    MedNode* p = findMedicineByName(keyword);
    
    if (p != NULL) {
        char info[500];
        sprintf(info, "找到药品：\n\n");
        sprintf(info + strlen(info), "编号: %s\n", p->data.id);
        sprintf(info + strlen(info), "名称: %s\n", p->data.name);
        sprintf(info + strlen(info), "规格: %s\n", p->data.spec);
        sprintf(info + strlen(info), "单位: %s\n", p->data.unit);
        sprintf(info + strlen(info), "厂家: %s\n", p->data.maker);
        sprintf(info + strlen(info), "进价: %.2f\n", p->data.buyPrice);
        sprintf(info + strlen(info), "售价: %.2f\n", p->data.sellPrice);
        sprintf(info + strlen(info), "库存: %d\n", p->data.stock);
        sprintf(info + strlen(info), "有效期: %04d-%02d\n", p->data.expY, p->data.expM);
        
        wchar_t winfo[500];
        utf8_to_wchar(info, winfo, 500);
        MessageBoxW(GetHWnd(), winfo, L"查询结果", MB_OK | MB_ICONINFORMATION);
    } else {
        char msg[100];
        sprintf(msg, "未找到包含 \"%s\" 的药品。", keyword);
        wchar_t wmsg[100];
        utf8_to_wchar(msg, wmsg, 100);
        MessageBoxW(GetHWnd(), wmsg, L"提示", MB_OK | MB_ICONWARNING);
    }
}

/* 4. 修改药品 */
void guiModifyMedicine(void)
{
    char id[MAX_ID_LEN];
    showInputPopupCentered("修改药品", "请输入要修改的药品编号：", id, MAX_ID_LEN, 0);
    
    if (strlen(id) == 0) return;
    
    MedNode* p = findMedicineById(id);
    if (p == NULL) {
        char msg[100];
        sprintf(msg, "未找到编号为 %s 的药品。", id);
        wchar_t wmsg[100];
        utf8_to_wchar(msg, wmsg, 100);
        MessageBoxW(GetHWnd(), wmsg, L"提示", MB_OK | MB_ICONWARNING);
        return;
    }
    
    /* 显示当前信息并选择修改项 */
    char info[600];
    sprintf(info, "【%s】%s\n", p->data.id, p->data.name);
    sprintf(info + strlen(info), "规格: %s  单位: %s\n", p->data.spec, p->data.unit);
    sprintf(info + strlen(info), "厂家: %s\n", p->data.maker);
    sprintf(info + strlen(info), "进价: %.2f  售价: %.2f\n", p->data.buyPrice, p->data.sellPrice);
    sprintf(info + strlen(info), "库存: %d  有效期: %04d-%02d\n\n", p->data.stock, p->data.expY, p->data.expM);
    sprintf(info + strlen(info), "请选择要修改的项：\n");
    sprintf(info + strlen(info), "1.名称  2.规格  3.单位  4.厂家\n");
    sprintf(info + strlen(info), "5.进价  6.售价  7.库存\n");
    sprintf(info + strlen(info), "8.有效期年  9.有效期月\n");
    sprintf(info + strlen(info), "0.取消修改");
    
    wchar_t winfo[600];
    utf8_to_wchar(info, winfo, 600);
    MessageBoxW(GetHWnd(), winfo, L"当前药品信息", MB_OK | MB_ICONINFORMATION);
    
    /* 选择修改项 */
    char choice[10];
    showInputPopupCentered("修改药品", "请输入要修改的编号（0-9）：", choice, 10, 0);
    
    if (strlen(choice) == 0) return;
    int field = atoi(choice);
    if (field < 0 || field > 9) return;
    if (field == 0) return;  /* 取消 */
    
    /* 输入新值 */
    char input[256];
    char prompt[100];
    int modified = 0;
    
    switch (field) {
        case 1:
            sprintf(prompt, "新名称（当前: %s）：", p->data.name);
            if (showInputPopupCentered("修改药品", prompt, input, 256, 0)) {
                strcpy(p->data.name, input); modified = 1;
            }
            break;
        case 2:
            sprintf(prompt, "新规格（当前: %s）：", p->data.spec);
            if (showInputPopupCentered("修改药品", prompt, input, 256, 0)) {
                strcpy(p->data.spec, input); modified = 1;
            }
            break;
        case 3:
            sprintf(prompt, "新单位（当前: %s）：", p->data.unit);
            if (showInputPopupCentered("修改药品", prompt, input, 256, 0)) {
                strcpy(p->data.unit, input); modified = 1;
            }
            break;
        case 4:
            sprintf(prompt, "新厂家（当前: %s）：", p->data.maker);
            if (showInputPopupCentered("修改药品", prompt, input, 256, 0)) {
                strcpy(p->data.maker, input); modified = 1;
            }
            break;
        case 5:
            sprintf(prompt, "新进价（当前: %.2f）：", p->data.buyPrice);
            if (showInputPopupCentered("修改药品", prompt, input, 256, 0)) {
                p->data.buyPrice = atof(input); modified = 1;
            }
            break;
        case 6:
            sprintf(prompt, "新售价（当前: %.2f）：", p->data.sellPrice);
            if (showInputPopupCentered("修改药品", prompt, input, 256, 0)) {
                p->data.sellPrice = atof(input); modified = 1;
            }
            break;
        case 7:
            sprintf(prompt, "新库存（当前: %d）：", p->data.stock);
            if (showInputPopupCentered("修改药品", prompt, input, 256, 0)) {
                p->data.stock = atoi(input); modified = 1;
            }
            break;
        case 8:
            sprintf(prompt, "新有效期年（当前: %d）：", p->data.expY);
            if (showInputPopupCentered("修改药品", prompt, input, 256, 0)) {
                p->data.expY = atoi(input); modified = 1;
            }
            break;
        case 9:
            sprintf(prompt, "新有效期月（当前: %d）：", p->data.expM);
            if (showInputPopupCentered("修改药品", prompt, input, 256, 0)) {
                p->data.expM = atoi(input); modified = 1;
            }
            break;
    }
    
    if (modified) {
        saveData();
        showMsgBox("修改成功！", "成功", MB_OK | MB_ICONINFORMATION);
    }
}

/* 5. 删除药品 */
void guiDeleteMedicine(void)
{
    char keyword[MAX_NAME_LEN];
    showInputPopupCentered("删除药品", "请输入药品编号或名称：", keyword, MAX_NAME_LEN, 0);
    
    if (strlen(keyword) == 0) return;
    
    /* 先按编号找，再按名称找 */
    MedNode* p = findMedicineById(keyword);
    if (p == NULL) {
        p = findMedicineByName(keyword);
    }
    
    if (p == NULL) {
        char msg[100];
        sprintf(msg, "未找到编号或名称为 \"%s\" 的药品。", keyword);
        wchar_t wmsg[100];
        utf8_to_wchar(msg, wmsg, 100);
        MessageBoxW(GetHWnd(), wmsg, L"提示", MB_OK | MB_ICONWARNING);
        return;
    }
    
    /* 显示药品信息 */
    char info[300];
    sprintf(info, "即将删除：\n\n");
    sprintf(info + strlen(info), "编号: %s\n", p->data.id);
    sprintf(info + strlen(info), "名称: %s\n", p->data.name);
    sprintf(info + strlen(info), "规格: %s\n", p->data.spec);
    sprintf(info + strlen(info), "库存: %d\n", p->data.stock);
    sprintf(info + strlen(info), "\n确认删除吗？");
    
    wchar_t winfo[300];
    utf8_to_wchar(info, winfo, 300);
    int ret = MessageBoxW(GetHWnd(), winfo, L"确认删除", MB_OKCANCEL | MB_ICONWARNING);
    
    if (ret == IDOK) {
        int result = deleteMedicine(p->data.id);
        if (result) {
            showMsgBox("? 删除成功！", "成功", MB_OK | MB_ICONINFORMATION);
        } else {
            showMsgBox("删除失败！可能有库存。", "错误", MB_OK | MB_ICONERROR);
        }
    }
}

/* 6. 进货入库 */
void guiPurchase(void)
{
    char medId[MAX_ID_LEN];
    char input[256];
    
    showInputPopupCentered("进货入库", "请输入药品编号：", medId, MAX_ID_LEN, 0);
    
    if (strlen(medId) == 0) return;
    
    MedNode* p = findMedicineById(medId);
    if (p == NULL) {
        char msg[100];
        sprintf(msg, "未找到编号为 %s 的药品！", medId);
        wchar_t wmsg[100];
        utf8_to_wchar(msg, wmsg, 100);
        MessageBoxW(GetHWnd(), wmsg, L"错误", MB_OK | MB_ICONERROR);
        return;
    }
    
    /* 显示药品信息 */
    char info[200];
    sprintf(info, "药品名称: %s\n当前库存: %d %s\n参考进价: %.2f 元", 
            p->data.name, p->data.stock, p->data.unit, p->data.buyPrice);
    wchar_t winfo[200];
    utf8_to_wchar(info, winfo, 200);
    MessageBoxW(GetHWnd(), winfo, L"药品信息", MB_OK | MB_ICONINFORMATION);
    
    showInputPopupCentered("进货入库", "进货数量：", input, 256, 0);
    int qty = atoi(input);
    
    showInputPopupCentered("进货入库", "进货单价（留空使用参考价）：", input, 256, 0);
    float price = strlen(input) > 0 ? atof(input) : p->data.buyPrice;
    
    Record rec;
    int result = purchase(medId, qty, price, &rec);
    
    if (result) {
        char success[300];
        sprintf(success, "? 进货成功！\n\n单号: %s\n药品: %s\n数量: %d %s\n单价: %.2f 元\n金额: %.2f 元\n新库存: %d %s",
                rec.date, rec.mname, rec.qty, p->data.unit, rec.price, rec.amount, p->data.stock, p->data.unit);
        wchar_t wsuccess[300];
        utf8_to_wchar(success, wsuccess, 300);
        MessageBoxW(GetHWnd(), wsuccess, L"成功", MB_OK | MB_ICONINFORMATION);
    } else {
        showMsgBox("进货失败！请检查参数。", "错误", MB_OK | MB_ICONERROR);
    }
}

/* 7. 销售出库 */
void guiSell(void)
{
    char medId[MAX_ID_LEN];
    char input[256];
    
    showInputPopupCentered("销售出库", "请输入药品编号：", medId, MAX_ID_LEN, 0);
    
    if (strlen(medId) == 0) return;
    
    MedNode* p = findMedicineById(medId);
    if (p == NULL) {
        char msg[100];
        sprintf(msg, "未找到编号为 %s 的药品！", medId);
        wchar_t wmsg[100];
        utf8_to_wchar(msg, wmsg, 100);
        MessageBoxW(GetHWnd(), wmsg, L"错误", MB_OK | MB_ICONERROR);
        return;
    }
    
    if (p->data.stock <= 0) {
        showMsgBox("该药品库存为0，无法销售！", "错误", MB_OK | MB_ICONERROR);
        return;
    }
    
    char info[200];
    sprintf(info, "药品名称: %s\n当前库存: %d %s\n参考售价: %.2f 元",
            p->data.name, p->data.stock, p->data.unit, p->data.sellPrice);
    wchar_t winfo[200];
    utf8_to_wchar(info, winfo, 200);
    MessageBoxW(GetHWnd(), winfo, L"药品信息", MB_OK | MB_ICONINFORMATION);
    
    showInputPopupCentered("销售出库", "销售数量：", input, 256, 0);
    int qty = atoi(input);
    
    showInputPopupCentered("销售出库", "销售单价（留空使用参考价）：", input, 256, 0);
    float price = strlen(input) > 0 ? atof(input) : p->data.sellPrice;
    
    Record rec;
    int result = sell(medId, qty, price, &rec);
    
    if (result) {
        char success[300];
        sprintf(success, "? 销售成功！\n\n单号: %s\n药品: %s\n数量: %d %s\n单价: %.2f 元\n金额: %.2f 元\n剩余库存: %d %s",
                rec.date, rec.mname, rec.qty, p->data.unit, rec.price, rec.amount, p->data.stock, p->data.unit);
        wchar_t wsuccess[300];
        utf8_to_wchar(success, wsuccess, 300);
        MessageBoxW(GetHWnd(), wsuccess, L"成功", MB_OK | MB_ICONINFORMATION);
    } else {
        showMsgBox("销售失败！库存不足或参数错误。", "错误", MB_OK | MB_ICONERROR);
    }
}

/* 8. 单据浏览 */
void guiListRecords(void)
{
    Record list[200];
    int count = 200;
    getAllRecords(list, &count);
    
    if (count == 0) {
        showMsgBox("暂无单据记录。", "提示", MB_OK | MB_ICONINFORMATION);
        return;
    }
    
    char result[4000] = "";
    char line[300];
    
    for (int i = 0; i < count; i++) {
        sprintf(line, "【%s】%s %s\n", 
                list[i].date,
                list[i].type == 1 ? "进货" : "销售",
                list[i].mname);
        strcat(result, line);
        
        sprintf(line, "  编号: %s  数量: %d  单价: %.2f  金额: %.2f\n",
                list[i].mid, list[i].qty, list[i].price, list[i].amount);
        strcat(result, line);
        
        if (i < count - 1) {
            strcat(result, "─────────────────────────────\n");
        }
    }
    
    sprintf(line, "\n共 %d 条单据记录。", count);
    strcat(result, line);
    
    wchar_t wresult[4000];
    utf8_to_wchar(result, wresult, 4000);
    showTextPopup("单据记录", result);
}

/* 9. 库存预警 */
void guiWarning(void)
{
    WarningReport report;
    getWarningReport(&report);
    showTextPopup("库存与效期预警", report.warningList);
}

/* 10. 统计报表 */
void guiStatistics(void)
{
    StatisticsReport report;
    getStatisticsReport(&report);
    
    char result[1000];
    sprintf(result, "===== 统计报表 =====\n\n");
    sprintf(result + strlen(result), "药品总数: %d 种\n", report.totalMedicines);
    sprintf(result + strlen(result), "单据总数: %d 条\n\n", report.totalRecords);
    sprintf(result + strlen(result), "【进货统计】\n");
    sprintf(result + strlen(result), "进货次数: %d 次\n", report.purchaseCount);
    sprintf(result + strlen(result), "进货总额: %.2f 元\n\n", report.totalPurchase);
    sprintf(result + strlen(result), "【销售统计】\n");
    sprintf(result + strlen(result), "销售次数: %d 次\n", report.salesCount);
    sprintf(result + strlen(result), "销售总额: %.2f 元\n\n", report.totalSales);
    sprintf(result + strlen(result), "【利润统计】\n");
    sprintf(result + strlen(result), "毛利润: %.2f 元\n", report.profit);
    if (report.totalPurchase > 0) {
        sprintf(result + strlen(result), "利润率: %.2f%%\n", report.profitRate);
    }
    
    wchar_t wresult[1000];
    utf8_to_wchar(result, wresult, 1000);
    showTextPopup("统计报表", result);
}

/* 11. 导出报表 */
void guiExportReport(void)
{
    time_t t = time(NULL);
    struct tm* now = localtime(&t);
    char filename[100];
    sprintf(filename, "report_%04d%02d%02d.txt", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
    
    int result = exportReport(filename);
    
    if (result) {
        char msg[200];
        sprintf(msg, "? 报表已导出到：\n%s\n\n文件保存在程序目录下。", filename);
        wchar_t wmsg[200];
        utf8_to_wchar(msg, wmsg, 200);
        MessageBoxW(GetHWnd(), wmsg, L"导出成功", MB_OK | MB_ICONINFORMATION);
    } else {
        showMsgBox("导出失败！无法创建文件。", "错误", MB_OK | MB_ICONERROR);
    }
}

/* ========== 主函数 ========== */
int main(void)
{
    initgraph(800, 600);
    loadData();
    
    if (!login()) {
        closegraph();
        return 0;
    }
    
    int running = 1;
    while (running) {
        drawMainMenu();
        
        MOUSEMSG msg;
        int clicked = 0;
        
        while (!clicked) {
            if (MouseHit()) {
                msg = GetMouseMsg();
                if (msg.uMsg == WM_LBUTTONDOWN) {
                    int x = msg.x, y = msg.y;
                    
                    /* 药品档案 */
                    if (y >= 95 && y <= 135) {
                        if (x >= 80 && x <= 220) { guiAddMedicine(); clicked = 1; }
                        else if (x >= 240 && x <= 380) { guiListMedicine(); clicked = 1; }
                        else if (x >= 400 && x <= 540) { guiSearchMedicine(); clicked = 1; }
                    }
                    if (y >= 150 && y <= 190 && !clicked) {
                        if (x >= 80 && x <= 220) { guiModifyMedicine(); clicked = 1; }
                        else if (x >= 240 && x <= 380) { guiDeleteMedicine(); clicked = 1; }
                    }
                    
                    /* 业务处理 */
                    if (y >= 250 && y <= 290 && !clicked) {
                        if (x >= 80 && x <= 220) { guiPurchase(); clicked = 1; }
                        else if (x >= 240 && x <= 380) { guiSell(); clicked = 1; }
                        else if (x >= 400 && x <= 540) { guiListRecords(); clicked = 1; }
                    }
                    
                    /* 决策辅助 */
                    if (y >= 350 && y <= 390 && !clicked) {
                        if (x >= 80 && x <= 220) { guiWarning(); clicked = 1; }
                        else if (x >= 240 && x <= 380) { guiStatistics(); clicked = 1; }
                    }
                    
                    /* 系统 */
                    if (y >= 450 && y <= 490 && !clicked) {
                        if (x >= 80 && x <= 220) { guiExportReport(); clicked = 1; }
                        else if (x >= 600 && x <= 740) { running = 0; clicked = 1; }
                    }
                }
            }
            Sleep(10);
        }
    }
    
    saveData();
    closegraph();
    return 0;
}
