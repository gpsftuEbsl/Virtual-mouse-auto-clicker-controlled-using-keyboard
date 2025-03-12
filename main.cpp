// Copyright (C) 2025 user ray
// This file is part of a project licensed under the GNU General Public License v3.0.
// See the LICENSE file for more details.
#include <windows.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>

using namespace std;

// 定義 Key 類別
class Key {
private:
    static int num; //計算總共有幾個key
public:
    char keyCode;       // 按鍵代碼（例如 'W', 'A', 'S', 'D', ' '）
    int x, y;           // 按鍵的螢幕座標
    bool pressed;       // 是否按下
    bool newState;      // 是否是新狀態

    Key(char code, int xPos, int yPos) : keyCode(code), x(xPos), y(yPos), pressed(false), newState(false) {
        num++;
    }

    Key(const Key& key) {
        keyCode = key.keyCode;
        x = key.x;
        y = key.y;
        num++;
    }
    ~Key() {
        num--;
        cout << "Key物件被銷毀，目前剩餘 " << num << " 個Key物件" << endl;
    }
    //用來存取private靜態資料成員num
    static void showNum(void) {
        cout << "目前共有" << Key::num << "個Key物件" << endl;
    }
};

// 設定一個全域變數來指示是否繼續顯示滑鼠位置
atomic<bool> keepShowing(true);

// 常數定義
const int circleSize = 20; // 固定圓形的大小
constexpr int circleOffsetX = 0;
constexpr int circleOffsetY = 0;
constexpr float stretchFactorX = 1.25f;
constexpr float stretchFactorY = 1.25f;
constexpr int clickDuration = 500; // 按鍵長按時間 (毫秒)

// 獲取螢幕尺寸
const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

// 使用 std::vector 儲存 Key 物件
vector<Key> keys = {
    {'W', 358, 538},
    {'S', 358, 755},
    {'A', 265, 647},
    {'D', 456, 648}
    /*, {'H', int(screenWidth * (double)32 / 64), int(screenHeight * (double)4 / 8)},
    {VK_NUMPAD3, int(screenWidth * (double)53 / 64), int(screenHeight * (double)25 / 32)},
    {VK_NUMPAD2, int(screenWidth * (double)11 / 16), int(screenHeight * (double)6 / 8)},
    {VK_NUMPAD1, int(screenWidth * (double)10 / 16), int(screenHeight * (double)6 / 8)},
    {VK_NUMPAD6, int(screenWidth * (double)53 / 64), int(screenHeight * (double)9 / 16)},*/
};

// 畫出圓形標記與鍵盤字母
void drawCircle(int x, int y, int radius, COLORREF color) {
    HDC hdc = GetDC(NULL);
    HPEN hPen = CreatePen(PS_SOLID, 2, color);
    HBRUSH hBrush = CreateSolidBrush(color);
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);

    // 還原原始畫筆與畫刷
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);

    DeleteObject(hPen);
    DeleteObject(hBrush);
    ReleaseDC(NULL, hdc);
}


void drawKeys(vector<Key> keys) {
    // 顯示對應的數字或字母
    HDC hdc = GetDC(NULL);
    SetTextColor(hdc, RGB(0, 0, 0)); // 設定文字顏色為黑色
    SetBkMode(hdc, TRANSPARENT);      // 設定文字背景透明

    // 繪製按鍵位置圓形
    for (auto& key : keys) {
        // 繪製圓形，顏色根據按鍵是否按下而改變
        drawCircle(
            static_cast<int>(key.x * stretchFactorX + circleOffsetX),
            static_cast<int>(key.y * stretchFactorY + circleOffsetY),
            circleSize, key.pressed ? RGB(100, 100, 200) : RGB(230, 230, 250));
        // 根據 keyCode 來構造按鍵文字
        string keyText = "";
        if (key.keyCode == VK_NUMPAD1) keyText = "1";
        else if (key.keyCode == VK_NUMPAD2) keyText = "2";
        else if (key.keyCode == VK_NUMPAD3) keyText = "3";
        else if (key.keyCode == VK_NUMPAD6) keyText = "6";
        else keyText = string(1, key.keyCode); // 其他字母或符號直接顯示字符

        // 計算文字位置，將圓心的坐標調整為文字的起始位置
        int textX = static_cast<int>(key.x * stretchFactorX + circleOffsetX) - circleSize / 2;
        int textY = static_cast<int>(key.y * stretchFactorY + circleOffsetY) - circleSize / 2;

        // 顯示文字
        TextOut(hdc, textX, textY, keyText.c_str(), keyText.length());
    }

    // 釋放設備上下文
    ReleaseDC(NULL, hdc);

    // 防止 CPU 過度使用
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// 模擬滑鼠行為
void pressMouse(int x, int y) {
    SetCursorPos(x, y);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
}

void releaseMouse(int x, int y) {
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

// 模擬鍵盤行為
void pressKey(WORD key) {
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;

    // 發送按下鍵的事件
    SendInput(1, &input, sizeof(INPUT));
}

void releaseKey(WORD key) {
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.dwFlags = KEYEVENTF_KEYUP; //放開鍵的事件

    SendInput(1, &input, sizeof(INPUT));
}

// 1.錄製指令
void recordCommands() {

    vector<string> commands;
    auto startTime = chrono::high_resolution_clock::now();

    cout << "開始錄製指令，按 ESC 結束錄製...\n";

    while (true) {
        for (auto& key : keys) {
            const bool isPressed = (GetAsyncKeyState(key.keyCode) & 0x8000);
            if (isPressed && !key.pressed) {
                key.pressed = true;
                auto currentTime = chrono::high_resolution_clock::now();
                auto elapsedTime = chrono::duration_cast<chrono::milliseconds>(currentTime - startTime).count();
                commands.push_back(to_string(elapsedTime) + "ms press" + key.keyCode);
            }
            else if (!isPressed && key.pressed) {
                key.pressed = false;
                auto currentTime = chrono::high_resolution_clock::now();
                auto elapsedTime = chrono::duration_cast<chrono::milliseconds>(currentTime - startTime).count();
                commands.push_back(to_string(elapsedTime) + "ms release" + key.keyCode);
            }
        }

        drawKeys(keys);

        // 使用 ESC 鍵作為退出條件
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); //降低CPU負載
    }

    // 將指令存入檔案
    static int fileCounter = 1;
    string fileName = "指令_" + to_string(fileCounter++) + ".txt";
    ofstream outFile(fileName);
    for (const auto& cmd : commands) {
        outFile << cmd << endl;
    }
    outFile.close();

    cout << "指令已儲存至 " << fileName << "\n";
}

// 2.手動操作
void manualOperation() {
    cout << "可手動操作的有以下這些鍵..." << endl;
    for (auto& key : keys) {
        cout << key.keyCode << ' ';
    }
    cout << endl;
    while (true) {
        // 按鍵狀態更新
        for (auto& key : keys) {
            const bool isPressed = (GetAsyncKeyState(key.keyCode) & 0x8000);
            if (isPressed != key.pressed) {
                key.newState = isPressed; // 更新新狀態

                if (isPressed) {
                    pressMouse(key.x, key.y); // 模擬按下
                }
                else {
                    releaseMouse(key.x, key.y); // 模擬放開
                }

                key.pressed = isPressed; // 更新狀態
            }
        }
        drawKeys(keys); // 確認狀態顯示正確

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// 3.執行已錄製的指令
void executeCommands() {
    // 列出可用指令檔案
    system("dir 指令_*.txt /b > file_list.txt");
    ifstream fileList("file_list.txt");
    vector<string> files;
    string fileName;

    while (getline(fileList, fileName)) {
        files.push_back(fileName);
    }

    fileList.close();
    remove("file_list.txt");

    if (files.empty()) {
        cout << "沒有可用的指令檔案。\n";
        return;
    }

    cout << "可用的指令檔案：\n";
    for (size_t i = 0; i < files.size(); i++) {
        cout << i + 1 << ". " << files[i] << "\n";
    }

    cout << "選擇要執行的指令（輸入編號）：";
    int choice;
    cin >> choice;

    if (choice < 1 || choice > files.size()) {
        cout << "選擇無效。\n";
        return;
    }

    ifstream commandFile(files[choice - 1]);
    string command;
    int lastTime = 0;

    while (getline(commandFile, command)) {
        size_t msPos = command.find("ms");
        size_t keyPos = command.find("press");

        if (msPos != string::npos && keyPos != string::npos) {
            int time = stoi(command.substr(0, msPos));
            char key = command[keyPos + 5];

            // 延遲到指令的執行時間
            std::this_thread::sleep_for(std::chrono::milliseconds(time - lastTime));
            lastTime = time;

            // 找到對應的鍵位置並模擬press
            for (auto& k : keys) {
                if (k.keyCode == key) {
                    k.pressed = true; // 更新按下狀態
                    pressMouse(k.x, k.y);
                    break;
                }
            }
        }
        else if ((msPos = command.find("release")) != string::npos) {
            int time = stoi(command.substr(0, msPos));
            char key = command[msPos + 7];

            // 延遲到指令的執行時間
            std::this_thread::sleep_for(std::chrono::milliseconds(time - lastTime));
            lastTime = time;

            // 找到對應的鍵位置並模擬release
            for (auto& k : keys) {
                if (k.keyCode == key) {
                    k.pressed = false; // 更新釋放狀態
                    releaseMouse(k.x, k.y);
                    break;
                }
            }
        }
    }
}

// 4.新增鍵盤(多載)
void addKeyboard(void) {
    char key_alpha; //用來存按鍵英文字母
    int x, y;
    do {
        cout << "請輸入要添加的鍵盤按鍵名稱和座標，例如:(W 100 100)" << endl;
        cin >> key_alpha >> x >> y;
        if (isupper(key_alpha) && x > 0 && x < 1535 && y > 0 && y < 863 ) {
            break; //成功輸入 跳離迴圈
        }
        else {
            cout << "格式錯誤input error" << endl;
        }
    } while (true);
    Key userDefine(key_alpha, x, y);
    keys.push_back(userDefine);
    Key::showNum(); //用來計算Key物件總數
    cout << "其中vector中有" << keys.size() << "個鍵" << endl;
}
void addKeyboard(int x, int y) {
    char key_alpha; //用來存按鍵英文字母
    do {
        cout << "請輸入要添加的鍵盤按鍵名稱，例如:(W)" << endl;
        cin >> key_alpha;
        if (!isupper(key_alpha)) {
            cout << "請輸入A~Z大寫英文字母" << endl;
        }
        else {
            break;
        }
    } while (true);
    Key userDefine(key_alpha, x, y);
    keys.push_back(userDefine);
    Key::showNum(); //用來計算Key物件總數
    cout << "其中vector中有" << keys.size() << "個鍵" << endl;
}

// 5.模擬按鍵事件
void executeKeyEventCommands() {
    ifstream ifile;
    string s;
    while (getline(ifile, s)) {

    }
    Sleep(2000);  // 2秒時間切到目標程式，方便觀察效果
    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
        // 模擬按下 'W'
        pressKey('W');
        Sleep(500);   // 模擬按住 0.5 秒
        releaseKey('W');
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
        Sleep(500);
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
        // 模擬按下 'A'
        pressKey('A');
        Sleep(500);
        releaseKey('A');
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
        Sleep(500);
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
        // 模擬按下 'S'
        pressKey('S');
        Sleep(500);
        releaseKey('S');
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
        Sleep(500);
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
        // 模擬按下 'D'
        pressKey('D');
        Sleep(500);
        releaseKey('D');
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
        //因為已經Sleep(500);所以不用加std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// 6. 顯示鼠標座標的函數
void showCursorPosition() {
    bool is_printed = 0; //紀錄打印座標
    char ans; //是否要加入新鍵盤
    POINT p;
    while (true) {
        if (GetCursorPos(&p) && !is_printed) {
            // 顯示滑鼠座標
            cout << "Mouse position: (" << p.x << ", " << p.y << ")" << endl;
            is_printed = true;
            do {
                cout << "do you want to add a new key?(y/n)" << endl;
                cin >> ans;
                if (ans == 'y' || ans == 'Y' || ans == 'n' || ans == 'N') {
                    break;
                }
                else {
                    cout << "input error! please input again..." << endl;
                }
            } while (true);
            if (ans == 'Y' || ans == 'y') {
                addKeyboard(p.x, p.y); //呼叫addKeyboard(int, int)的函數
            }
            else if (ans == 'N' || ans == 'n') {
                cout << "press ESC to continue..." << endl;
            }
        }
        else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
    }
    system("cls"); //清除畫面
}

int Key::num = 0; //定義一個用來記錄物件數量的靜態資料成員

// 主程式，選擇模式並能退出程式
int main() {
    while (true) {
        int mode = 0;
        cout << "請選擇模式：\n1. 錄製模式\n2. 手動操作模式\n3. 執行已錄製的事件\n4. 新增自訂義按鍵\n5. 模擬按鍵事件\n6. 顯示屬標座標\n0. 退出程式\n";
        cin >> mode;

        switch (mode) {
        case 1:
            recordCommands();
            break;
        case 2:
            manualOperation();
            break;
        case 3:
            executeCommands();
            break;
        case 4:
            addKeyboard();
            break;
        case 5:
            executeKeyEventCommands();
            break;
        case 6:
            showCursorPosition();
            break;
        case 0:
            cout << "退出程式...\n";
            return 0;
        default:
            cout << "無效選項。\n";
        }
    }

    return 0;
}
