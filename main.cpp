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
public:
    char keyCode;       // 按鍵代碼（例如 'W', 'A', 'S', 'D', ' '）
    int x, y;           // 按鍵的螢幕座標
    bool pressed;       // 是否按下
    bool newState;      // 是否是新狀態

    Key(char code, int xPos, int yPos)
        : keyCode(code), x(xPos), y(yPos), pressed(false), newState(false) {}
};

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
    {'W', screenWidth * 15 / 64, screenHeight * 5 / 8},
    {'S', screenWidth * 15 / 64, screenHeight * 7 / 8},
    {'A', screenWidth * 11 / 64, screenHeight * 6 / 8},
    {'D', screenWidth * 19 / 64, screenHeight * 6 / 8},
    {VK_NUMPAD3, screenWidth * 53 / 64, screenHeight * 25 / 32},
    {VK_NUMPAD2, screenWidth * 11 / 16, screenHeight * 6 / 8},
    {VK_NUMPAD1, screenWidth * 10 / 16, screenHeight * 6 / 8},
    {VK_NUMPAD6, screenWidth * 53 / 64, screenHeight * 9 / 16}
};

// 畫出圓形標記與鍵盤字母
void drawCircle(int x, int y, int radius = 10, COLORREF color = RGB(255, 0, 0)) {
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

// 模擬按鍵行為
void press(int x, int y) {
    SetCursorPos(x, y);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
}

void release(int x, int y) {
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

// 錄製指令
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

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
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

// 執行已錄製的指令
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
    for (size_t i = 0; i < files.size(); ++i) {
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
                    press(k.x, k.y);
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
                    release(k.x, k.y);
                    break;
                }
            }
        }
    }
}

// 主程式，選擇模式並能退出程式
int main() {
    while (true) {
        int mode = 0;
        cout << "請選擇模式：\n1. 錄製模式\n2. 手動操作模式\n3. 執行已錄製指令模式\n0. 退出程式\n";
        cin >> mode;

        switch (mode) {
        case 1:
            recordCommands();
            break;
        case 2:
            while (true) {
                for (auto& key : keys) {
                    const bool isPressed = (GetAsyncKeyState(key.keyCode) & 0x8000);
                    if (isPressed && !key.pressed) {
                        key.pressed = true;
                    }
                    else if (!isPressed && key.pressed) {
                        key.pressed = false;
                    }
                }

                drawKeys(keys);

                if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                    break;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            break;
        case 3:
            executeCommands();
            break;
        case 0:
            cout << "退出程式...\n";
            return 0;
        default:
            cout << "無效選項。\n";
        }
    }
}
