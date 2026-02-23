# Fixed-Outline Floorplanning (B\*-Tree + Simulated Annealing)


- **Soft Modules**（可變長寬比、固定面積）使用 **B\*-Tree** 表示與操作  
- **Hard / Fixed Modules**（固定位置與尺寸）作為不可移動障礙  
- 以 **Simulated Annealing (SA)** 進行擾動搜尋，目標在 **固定晶片外框（ChipSize）限制下** 找到 **wirelength 較小** 的可行解（feasible）配置

---

## 1. 專案重點

- **資料結構**
  - `BStarTree.h`：B\*-Tree（插入/刪除/交換/旋轉/調整尺寸等操作）
  - `Contour.h`：Contour packing（用輪廓線快速計算放置後的 y 座標/高度）
- **演算法**
  - `FloorPlanAlogrithm.h`：核心流程（初始化、cost 計算、Simulated Annealing）
- **I/O**
  - `FixedOutlineFloorplanning.cpp`：main，讀 input / 控制多執行緒 / 輸出結果
  - `FloorplanFile.h`：輸入切字與輸出檔案格式

---

## 2. 編譯方式

專案提供 `makefile`（g++、C++17、-O3、pthread）：

```bash
# 建議先建立輸出資料夾
mkdir -p ../bin

make
```

預設會產生可執行檔（見 `makefile`）：
- `../bin/main`

---

## 3. 執行方式

```bash
../bin/main <input.txt> <output.txt> [ENABLE_ONLY_ONE_FEASIBLE] [ENABLE_WRITE_OUT_FILE] [MAXIMUM_THREADS]
```

參數意義（對應 `FixedOutlineFloorplanning.cpp` / `SettingParameters.h`）：
- `ENABLE_ONLY_ONE_FEASIBLE`（0/1）：若為 1，SA 一旦找到 feasible solution 就可提前停止（用於測試）
- `ENABLE_WRITE_OUT_FILE`（0/1）：若 input 檔名包含 `public1~public4`，可直接輸出內建答案（用於測試）
- `MAXIMUM_THREADS`：同時跑幾個 thread（預設 5）

---

## 4. Input 格式（文字檔）

main 會依序讀取以下區塊（以空白分隔）：

- **晶片外框尺寸**
  ```
  ChipSize <maxWidth> <maxHeight>
  ```

- **Soft modules**
  ```
  NumSoftModules <N>
  SoftModule <name> <minArea>
  SoftModule <name> <minArea>
  ...
  ```

- **Fixed modules (Hard modules)**
  ```
  NumFixedModules <M>
  FixedModule <name> <x> <y> <w> <h>
  FixedModule <name> <x> <y> <w> <h>
  ...
  ```

- **Nets**
  ```
  NumNets <K>
  Net <moduleA> <moduleB> <weight>
  Net <moduleA> <moduleB> <weight>
  ...
  ```

---

## 5. Output 格式

`FloorplanFile::writeOutputFile(...)` 會輸出：

```
Wirelength <total_wirelength>

NumSoftModules <N>
<name> <x> <y> <w> <h>
<name> <x> <y> <w> <h>
...
```

---

## 6. 演算法說明

### 6.1 B\*-Tree + Contour Packing

- B\*-Tree 的節點對應一個 soft module（`Node` / `Module`）
- 將樹轉成實際座標時，使用 `Contour` 來維護目前的輪廓線，快速決定每個模組的放置高度（y）與整體寬高  
- `FixedOutlineFloorplanning::toFloorplan(tree)` 會把 B\*-Tree 轉換為 floorplan（並可用 `isFeasibleFloorplan()` 檢查是否在 `ChipSize` 之內）

### 6.2 Cost 計算

在 `FloorPlanAlogrithm::calculateCost(...)` 中：
- 先 `toFloorplan(tree)`
- 計算：
  - `area = calculateArea(tree)`
  - `wirelength = calculateWirelength(tree)`
- 並標記 `feasible`（是否滿足 fixed-outline 限制）

> 目前程式中 `cost->setCost(area);`：主要 cost 以 **area** 為主（wirelength 仍會被計算並在最後挑 feasible 解時用來比較）。

---

## 7. Simulated Annealing（重點）

### 7.1 擾動（Perturbation Moves）

`FloorPlanAlogrithm::perturbSolution(tree)` 會隨機選一種 move：

1. **Resize**：隨機挑一個 node，改變其 aspect ratio（維持面積）
2. **Rotate**：旋轉某個 module
3. **Delete & Insert**：刪除一個 node 再插回樹中（改變拓樸）
4. **Swap**：交換兩個不同 node

（move 選擇用 `rand() % MOVE_SIZE`）

### 7.2 初始溫度（Initial Temperature）

`getInitialTemperature(...)` 的概念：
- 先從初始解開始做擾動
- 蒐集「上坡（cost 變大）」的平均增量
- 令接受上坡的機率約為 0.95，推回初始溫度 `T0`

### 7.3 SA 主迴圈與接受準則

`SimulatedAnnealing(...)` 的主要參數：
- 冷卻率 `R = 0.95`
- `K = 20`
- 內層迭代規模 `N = K * (numSoftModules^2)`
- 終止條件：
  - **超時**：`TIME_LIMIT`
  - **reject rate 太高**：`rejectionRate > 0.95`

接受準則：
- 若 `Δcost <= 0`：直接接受
- 若 `Δcost > 0`：以機率 `exp(-Δcost / T)` 接受

---

### 7.4 流程圖（對應程式邏輯）
![flowchart](./images/flowchart.jpg)


---

## 8. 多執行緒（Parallel SA）

`FixedOutlineFloorplanning.cpp` 會同時啟動 `MAXIMUM_THREADS` 個 async task：

- 每個 thread 都會：
  1. `oldInitializeFloorplan(...)` 取得一個初始解
  2. 跑 `SimulatedAnnealing(...)`
- 主程式收集所有 thread 的解：
  - **優先挑 feasible solutions**
  - 若 feasible 不為空：回傳 **wirelength 最小** 的 feasible 解
  - 否則：回傳任一最佳解

---

## 9. 可調參數

在 `SettingParameters.h`：
- Soft module 初始 aspect ratio 範圍：
  - `INITAL_MINIMUM_ASPECT_RATIO = 0.5`
  - `INITAL_MAXIMUM_ASPECT_RATIO = 2`
- `TIME_LIMIT = 570000 ms`
- `MAXIMUM_THREADS = 5`（可由 CLI 覆蓋）
- `ENABLE_ONLY_ONE_FEASIBLE / ENABLE_WRITE_OUT_FILE`

---

## 10. 常見建議

- 如果你想讓 SA 更在意 wirelength：可以把 `Cost::setCost(...)` 改成加權形式，例如  
  `cost = alpha * area + beta * wirelength (+ penalty)`  
  並針對 infeasible 加上 penalty。
- 若遇到 timeout：可以減少 `MAXIMUM_THREADS` 或調整 `TIME_LIMIT`。


