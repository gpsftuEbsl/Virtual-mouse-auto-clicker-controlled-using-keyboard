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

        Key(char code, int xPos, int yPos): keyCode(code), x(xPos), y(yPos), pressed(false), newState(false) {
            num++;
        }

        //拷貝建構元 引數必須為某物件的參照
        Key(const Key& key) { 
            keyCode = key.keyCode;
            x = key.x;
            y = key.y;
            num++;
        }

        //用來存取private靜態資料成員num
        static void showNum(void) {
            cout << "目前共有" << Key::num << "個Key物件" << endl;
        }
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
    {'W', int(screenWidth * (double)15 / 64), int(screenHeight * (double)5 / 8)},
    {'S', int(screenWidth * (double)15 / 64), int(screenHeight * (double)7 / 8)},
    {'A', int(screenWidth * (double)11 / 64), int(screenHeight * (double)6 / 8)},
    {'D', int(screenWidth * (double)19 / 64), int(screenHeight * (double)6 / 8)},
    {'H', int(screenWidth * (double)32 / 64), int(screenHeight * (double)4 / 8)},
    {VK_NUMPAD3, int(screenWidth * (double)53 / 64), int(screenHeight * (double)25 / 32)},
    {VK_NUMPAD2, int(screenWidth * (double)11 / 16), int(screenHeight * (double)6 / 8)},
    {VK_NUMPAD1, int(screenWidth * (double)10 / 16), int(screenHeight * (double)6 / 8)},
    {VK_NUMPAD6, int(screenWidth * (double)53 / 64), int(screenHeight * (double)9 / 16)},
};

// 畫出圓形標記與鍵盤字母
void drawCircle(int x, int y, int radius, COLORREF color) {
    HDC hdc = GetDC(NULL);

    // 設定要清除的區域（圓形的外接正方形）
    RECT rect = {x - radius, y - radius, x + radius, y + radius};
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH)); // 清成白色背景

    HPEN hPen = CreatePen(PS_SOLID, 2, color);
    HBRUSH hBrush = CreateSolidBrush(color);
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);

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
                    press(key.x, key.y); // 模擬按下
                }
                else {
                    release(key.x, key.y); // 模擬放開
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

// 4.新增鍵盤
void addKeyboard(void) {
    char inputName;
    string fraction, nStr; //fractionHeightAndWidth
    int a = -1, b = -1, c = -1, d = -1; // *a/b *c/d 從字串中取得數字
    do {
        cout << "請輸入要添加的鍵盤按鍵名稱、在螢幕中的寬度、高度 例如:(W)" << endl;
        cin >> inputName; cin.get();
        if (!isupper(inputName)) {
            cout << "請輸入A~Z大寫英文字母" << endl;
        }
        else {
            break;
        }
    } while (true);
    do {
        cout << "請輸入要添加的鍵盤按鍵在螢幕中的寬度、高度 例如:(15/64 20/32)(1/64約為一公分寬 1/32約為一公分寬)" << endl;
        getline(cin, fraction);
        int cntLine = 0, cntSpace = 0;
        for (int i = 0; i < fraction.size(); i++) {
            if (fraction[i] == '/') {
                cntLine++;
            }
            if (fraction[i] == ' ') {
                cntSpace++;
            }
        }
        if (cntLine == 2 && cntSpace == 1) {

        }
    } while (true);
    int strSize = fraction.size(), cnt = 0, head = 0;
    for (int i = 0; i < strSize; i++) {
        if (fraction[i] == '/' || fraction[i] == ' ') {
            nStr = fraction.substr(head, cnt);
            int n = stoi(nStr);
            head = i + 1;
            cnt = 0;
            if (a == -1) {
                a = n;
            }
            else if (b == -1) {
                b = n;
            }
            else if (c == -1) {
                c = n;
            }
        }
        else {
            cnt++;
        }
    }
    nStr = fraction.substr(head, cnt); //額外處理最後的d
    d = stoi(nStr);
    Key userDefine(inputName, int(screenWidth * (double)a / b), int(screenHeight * (double)c / d));
    keys.push_back(userDefine);
    Key::showNum(); //用來計算Key物件總數
    cout << "其中vector中有" << keys.size() << "個鍵" << endl;
}

int Key::num = 0; //定義一個用來記錄物件數量的靜態資料成員

// 主程式，選擇模式並能退出程式
int main() {
    while (true) {
        int mode = 0;
        cout << "請選擇模式：\n1. 錄製模式\n2. 手動操作模式\n3. 執行已錄製指令模式\n4. 新增自訂義按鍵\n0. 退出程式\n";
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
        case 0:
            cout << "退出程式...\n";
            return 0;
        default:
            cout << "無效選項。\n";
        }
    }
}
