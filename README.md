# Virtual-mouse-auto-clicker-controlled-using-keyboard

### README

# 鍵盤模擬與指令錄製工具

這是一個用於錄製和執行鍵盤按鍵行為的工具。使用者可以錄製自訂的按鍵按下與釋放時間，並將其儲存為指令檔案。這些指令檔案可以在以後被執行，模擬原始的鍵盤行為。此工具適用於 Windows 系統，並使用 C++ 撰寫，搭配 Windows API 來模擬滑鼠與鍵盤操作。

### 功能

1. **錄製模式**：
   - 在這個模式下，程式將記錄按下與釋放按鍵的時間戳，並儲存為檔案。
2. **手動操作模式**：
   - 用戶可以直接操作鍵盤，並以視覺化方式看到當前的按鍵狀態。
3. **執行指令模式**：
   - 用戶可以選擇已錄製的指令檔案並執行它們，模擬鍵盤行為。
4. **新增按鍵**:
   - 直接輸入按鍵英文和按鍵座標來新增按鍵
   - 或是顯示滑鼠位置的同時順便在該位置新增按鍵(需選用功能6)
5. **模擬按下按鍵事件**
   - 此功能與功能1不同處在於，功能1是透過電腦鍵盤來模擬滑鼠點即螢幕上的特定座標，而此功能則是直接模擬預設的鍵盤輸入事件。
6. **顯示滑鼠當前位置**
   - 將鼠標移到想查詢的座標後，在終端機選用功能6，即顯示位置。
   - 顯示位置後，會詢問是否順便新增按鍵，選擇(y)，則可以在該位置設置一個對應的鍵盤。

### 使用方法

1. 在程式啟動後，選擇操作模式：
   - 錄製模式：錄製鍵盤指令並保存為檔案。
   - 手動操作模式：手動操作鍵盤並顯示其狀態。
   - 執行指令模式：選擇已保存的指令檔案並執行。
   - 新增按鍵模式：直接輸入按鍵英文與座標，或搭配功能6顯示滑鼠座標並選擇新增按鍵。
   - 模擬按鍵事件模式：直接模擬鍵盤輸入事件，而非透過滑鼠座標模擬。
   - 顯示滑鼠位置模式：顯示當前滑鼠座標，並可選擇是否在該位置新增按鍵。

2. 按照提示選擇操作，程序會依據使用者的選擇進行操作。

3. 執行模式中的指令是根據時間戳精確地模擬鍵盤按鍵的按下與釋放。

4. 在新增按鍵時，若選擇顯示滑鼠位置，需先將滑鼠移動至目標位置，再執行功能6，顯示座標後可選擇是否立即在該座標設置按鍵。

### 編譯與運行

1. 開啟一個 C++ 編譯環境，並將源代碼保存在 `.cpp` 檔案中。
2. 編譯並運行程式。

### 系統要求

- Windows 10 或更高版本
- 支援 C++11 以上的編譯器（例如 Visual Studio）

### 免責聲明

本專案僅供學術與個人學習用途，無論在任何情況下，作者對由於使用本程式所產生的任何損失、損害或責任概不負責。請在使用本程式時遵守當地法律法規，並確保不侵犯其他人的權利。

### 授權條款

本項目根據 GNU 通用公共許可證 v3.0 獲得許可。
有關詳細資訊，請參閱[LICENSE](./LICENSE)檔。

---

### README

# Keyboard Simulation and Command Recording Tool

This is a tool designed for recording and executing keyboard key presses. Users can record custom key presses and release timings, then save them as command files. These command files can later be executed to simulate the original keyboard behavior. This tool is developed for Windows systems and written in C++ using Windows API to simulate mouse and keyboard operations.

### Features

1. **Recording Mode**:
   - In this mode, the program records the timestamps of key press and release events and saves them as a file.
2. **Manual Operation Mode**:
   - Users can directly operate the keyboard and visually see the current key status.
3. **Execute Command Mode**:
   - Users can select recorded command files and execute them to simulate keyboard behavior.
4. **Add Key**:
   - Directly input the key name and its coordinates to add a new key.
   - Alternatively, display the mouse position and add a key at that position (requires Feature 6).
5. **Simulate Key Press Event**:
   - Unlike Recording Mode, which simulates mouse clicks at specific screen coordinates, this feature directly simulates default keyboard input events.
6. **Display Current Mouse Position**:
   - Move the mouse to the desired position and select Feature 6 in the terminal to display its coordinates.
   - After displaying the position, the program will ask if you want to add a key at that location. Selecting 'y' will set a corresponding keyboard input at that position.

### How to Use

1. After launching the program, choose an operation mode:
   - Recording Mode: Record keyboard commands and save them as a file.
   - Manual Operation Mode: Operate the keyboard manually and display its status.
   - Execute Command Mode: Select and execute saved command files.
   - Add Key Mode: Directly input the key name and coordinates, or use Feature 6 to display mouse coordinates and choose to add a key.
   - Simulate Key Press Event Mode: Directly simulate keyboard input events instead of using mouse coordinates.
   - Display Mouse Position Mode: Show current mouse coordinates and optionally add a key at that position.

2. Follow the on-screen prompts to choose an operation. The program will execute based on the user’s selection.

3. In Execute Command Mode, the program accurately simulates key press and release events based on recorded timestamps.

4. When adding a key, if you choose to display the mouse position, first move the mouse to the target position, then execute Feature 6. After displaying the coordinates, you can choose whether to immediately add a key at that location.

### Compilation and Running

1. Open a C++ development environment and save the source code in a `.cpp` file.
2. Compile and run the program.

### System Requirements

- Windows 10 or later
- A C++11 or newer compatible compiler (e.g., Visual Studio)

### Disclaimer

This project is intended for academic and personal learning purposes only. The author assumes no liability for any loss, damage, or legal issues that may arise from using this program. Please comply with local laws and regulations, and ensure that you do not infringe on others' rights while using this tool.

## License
This project is licensed under the GNU General Public License v3.0.  
See the [LICENSE](./LICENSE) file for details.

