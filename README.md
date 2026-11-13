# DCPS — 数控电源（Digital Control Power Supply）

基于 STM32G431 的四开关 Buck-Boost 数控电源，采用 MATLAB/Simulink 生成嵌入式 PID 控制代码，在 MCU 上实现闭环控制与前馈补偿。

> **项目状态**：闭环控制移植已完成，未上机实测。后续因时间原因暂停推进。

---

## 一、硬件概述

| 部件 | 规格 |
|------|------|
| MCU | STM32G431CBU6（Cortex-M4F，170 MHz，单精度 FPU） |
| 功率拓扑 | 四开关 Buck-Boost（Four Switch Buck-Boost, FSBB） |
| Buck 开关管 PWM | TIM8 CH1（PB6），50 kHz，ARR=3399 |
| Boost 开关管 PWM | TIM4 CH2（PA12），50 kHz，ARR=3399 |
| 使能控制 | SD1 / SD2 GPIO（拉高使能开关管驱动） |
| 电压采样 | ADC1 + OPAMP1（输出电压）、ADC2 + OPAMP2（输入电压） |
| 电流采样 | INA226（I2C2，分流电阻 + 电流/功率测量） |
| 控制环定时器 | TIM3，500 Hz / 2 ms 周期，中断驱动 |

时钟配置：HSE 8 MHz → PLL ×85 → SYSCLK 170 MHz，APB1/APB2 均不分频，定时器时钟 170 MHz。

---

## 二、软件架构

```
┌──────────────────────────────────────────────────────────┐
│                      main.c (Core/Src)                    │
│                                                          │
│  HAL_Init → 外设初始化 → fsbb_init() → TIM3 开中断       │
│                       ↓                                  │
│  while(1): measure_flag → 读ADC/INA226 → fsbb_run()      │
└──────────────────────────┬───────────────────────────────┘
                           │
          ┌────────────────┴────────────────┐
          ▼                                 ▼
┌─────────────────────┐          ┌──────────────────────┐
│   Hardware/INA226   │          │   Hardware/FSBB      │
│                     │          │                      │
│  ina226_getcurrent  │          │  fsbb_init()         │
│  adc_getvoltage*    │          │  fsbb_run()          │
│  (传感器层)          │          │  fsbb_start/stop()   │
└─────────────────────┘          │  (控制+驱动层)        │
                                 │                      │
                                 │  内部调用:            │
                                 │  DCPS_model_step()   │
                                 │  (MATLAB生成代码)     │
                                 └──────────┬───────────┘
                                            │
                                            ▼
                                 ┌──────────────────────┐
                                 │  DCPS_model.c/.h     │
                                 │  rtwtypes.h          │
                                 │  (Simulink生成的PID)  │
                                 └──────────────────────┘
```

### 分层说明

- **Core/**：STM32CubeMX 生成的 HAL 工程（main.c、tim.c、adc.c、i2c.c、usart.c 等），负责外设初始化与主循环。
- **Hardware/INA226/**：INA226 电流传感器驱动 + ADC 电压采样封装，提供 `ina226_getcurrent_A()`、`adc_getvoltageoutput_v()`、`adc_getvoltageinput_v()` 等接口。
- **Hardware/FSBB/**：四开关 Buck-Boost 驱动与控制核心，包含：
  - `FSBB.c/.h` — PWM 驱动 + 闭环控制封装（前馈 + PID + Buck/Boost 切换）
  - `DCPS_model.c/.h` — MATLAB/Simulink 生成的 PID 控制代码
  - `rtwtypes.h` — 数据类型定义（`real32_T = float`）
- **MATLAB/**：Simulink 模型源文件（`DCPS.m`、`DCPS.slx`）及代码生成原档，供查阅模型结构与重新生成用。

---

## 三、控制算法

### 3.1 Simulink 模型

模型文件 `MATLAB/DCPS.slx`，由 Simulink Coder R2025b 生成嵌入式 C 代码。

- **采样周期**：Ts = 0.002 s（2 ms），与 TIM3 中断周期对齐
- **模式切换**：Switch Case 按 `state` 输入切换
  - `state == 1`：恒压模式（CV），PID 对 `V_target - V_value` 运算
  - 其它：恒流模式（CC），PID 对 `I_target - I_value` 运算
- **PID 结构**：并联式（Ideal），含比例 P、积分 I（Forward Euler）、带滤波微分 D（N=100）
- **输出饱和**：限幅 [0.01, 0.99]
- **数据类型**：`real32_T`（单精度 float），适配 STM32G4 的单精度 FPU
- **参数化**：PID 参数（V_Kp/Ki/Kd、I_Kp/Ki/Kd）从模型常量改为可运行时调整的变量，在 `DCPS_model_initialize()` 中赋初值

### 3.2 模型接口

```c
// 输入结构体
typedef struct {
    real32_T state;     // 模式：1=CV, 其它=CC
    real32_T V_target;  // 目标电压 (V)
    real32_T V_value;   // 实测输出电压 (V)
    real32_T I_target;  // 目标电流 (A)
    real32_T I_value;   // 实测电流 (A)
} ExtU_DCPS_model_T;

// 输出结构体
typedef struct {
    real32_T duty[2];   // duty[0]=CV支路输出, duty[1]=CC支路输出
} ExtY_DCPS_model_T;
```

> 注意：模型每拍只执行一个支路（CV 或 CC），非活跃支路的 `duty` 值是陈旧的。`fsbb_run()` 按 `state` 取活跃支路的输出。

### 3.3 前馈 + PID 复合控制

`fsbb_run()` 实现前馈控制，以减小启动过冲和模式切换瞬态：

```
1. 填传感器数据 → DCPS_model_U.V_value / I_value
2. DCPS_model_step() → PID 输出 ctrl_duty (0.01~0.99)
3. 计算前馈 D_ff:
     Buck (V_target ≤ Vin):  D_ff = V_target / Vin
     Boost (V_target > Vin):  D_ff = 1 - Vin / V_target
     (除零保护: 分母 < 0.1 时回退 0.5)
4. PID 修正量: delta = ctrl_duty - D_ff, 钳位 ±FF_DELTA_MAX(0.3)
5. 最终占空比: duty = D_ff + delta
6. Buck/Boost 分配:
     Buck 模式:  buck_duty = duty, boost_duty = 0.99 (常通)
     Boost 模式: buck_duty = 0.99 (常通), boost_duty = duty
7. fsbb_start() → 限幅 [0.01, 0.99] → 转 CCR → 写 PWM 寄存器
```

前馈使启动首拍占空比接近稳态值（而非 PID 饱和到 0.99），显著减小过冲。模式切换时 D_ff 按新模式重算，占空比连续无跳变。PID 仍在前馈附近 ±0.3 范围内自由调节，补偿负载扰动与前馈模型误差。

---

## 四、闭环数据流

```
TIM3 中断 (2ms)
    │
    ▼
measure_flag = 1
    │
    ▼ (主循环响应)
读 ADC → output_voltage, input_voltage
读 INA226 → current
计算 power = current × output_voltage
    │
    ▼
fsbb_run():
    填 DCPS_model_U.V_value / I_value
    DCPS_model_step()           → PID 运算
    前馈计算 + 钳位
    Buck/Boost 模式判断
    fsbb_start()                → 写 TIM8/TIM4 CCR
    │
    ▼
measure_flag = 0
```

传感器读取（ADC/I2C 阻塞操作）放在主循环而非中断中，避免 ISR 溢出。TIM3 中断仅置标志位，控制环周期由 TIM3 保证实时性。

---

## 五、目录结构

```
DCPS/
├── CMakeLists.txt              # CMake 构建配置
├── CMakePresets.json            # 构建预设
├── DCPS.ioc                     # STM32CubeMX 工程配置
├── startup_stm32g431xx.s        # 启动汇编
├── STM32G431xx_FLASH.ld         # 链接脚本
│
├── Core/                        # CubeMX 生成的 HAL 工程
│   ├── Inc/                     #   头文件
│   └── Src/                     #   main.c, tim.c, adc.c, i2c.c, ...
│
├── Hardware/
│   ├── FSBB/                    # 四开关 Buck-Boost 驱动 + 控制核心
│   │   ├── FSBB.c / FSBB.h      #   PWM 驱动 + 前馈闭环封装
│   │   ├── DCPS_model.c / .h    #   MATLAB 生成的 PID 控制代码
│   │   └── rtwtypes.h           #   数据类型定义
│   └── INA226/                  # 电流传感器 + ADC 封装
│       ├── INA226.c / INA226.h  #   INA226 I2C 驱动
│       └── (adc 接口在 INA226.h 中声明)
│
├── MATLAB/                      # Simulink 模型源文件 + 生成代码原档
│   ├── DCPS.slx                 #   Simulink 模型
│   └── DCPS_model_ert_rtw/      #   生成代码原档（参考用，实际编译用 Hardware/FSBB/ 下的副本）
│
├── Drivers/                     # STM32 HAL 库
├── cmake/                       # CMake 构建脚本
└── build/                       # 构建输出
```

---

## 六、构建与烧录

### 工具链

- 编译器：arm-none-eabi-gcc
- 构建：CMake ≥ 3.22 + Ninja 或 Make
- 烧录：OpenOCD / ST-Link Utility

### 构建步骤

```bash
# 配置
cmake -B build -G Ninja

# 编译
cmake --build build

# 输出: build/DCPS.bin / DCPS.elf
```

烧录使用 ST-Link 连接 SWD 接口。

---

## 七、关键参数

### 控制参数

| 参数 | 值 | 位置 | 说明 |
|------|----|------|------|
| Ts（控制周期） | 0.002 s | DCPS_model.c | 与 TIM3 的 2ms 中断对齐 |
| V_Kp / V_Ki / V_Kd | 5.0 / 1.0 / 0.1 | DCPS_model_initialize() | CV 支路 PID（可调） |
| I_Kp / I_Ki / I_Kd | 5.0 / 1.0 / 0.1 | DCPS_model_initialize() | CC 支路 PID（可调） |
| FF_DELTA_MAX | 0.3 | FSBB.c | PID 相对前馈的最大修正量 |
| ALWAYS_ON_DUTY | 0.99 | FSBB.c | 非活跃侧开关管常通占空比 |

### 默认设定值

| 参数 | 值 | 位置 |
|------|----|------|
| state | 1 (CV) | fsbb_init() |
| V_target | 5.0 V | fsbb_init() |
| I_target | 1.0 A | fsbb_init() |

### PWM 参数

| 定时器 | 通道 | 引脚 | 频率 | ARR | 用途 |
|--------|------|------|------|-----|------|
| TIM8 | CH1 | PB6 | 50 kHz | 3399 | Buck 开关管 |
| TIM4 | CH2 | PA12 | 50 kHz | 3399 | Boost 开关管 |
| TIM3 | — | — | 500 Hz | 1999 | 控制环中断（2ms） |

---

## 八、接口参考

### FSBB 接口（`FSBB.h`）

```c
typedef enum {
    MODE_CC = 0,   // 恒流
    MODE_CV = 1,   // 恒压
    MODE_STOP      // 停止
} fsbb_mode_t;

void fsbb_init(void);                          // 初始化模型 + 设默认设定值
void fsbb_run(void);                           // 执行一步闭环（前馈+PID+PWM）
void fsbb_start(float buck, float boost);      // 直接设占空比并启动 PWM
void fsbb_stop(void);                          // 停止 PWM + 关使能
void fsbb_debugon(void);                       // 安全占空比启动（Buck=0.5, Boost=0.99）
```

### 传感器接口（`INA226.h`）

```c
float adc_getvoltageoutput_v(void);   // 输出电压 (V)
float adc_getvoltageinput_v(void);    // 输入电压 (V)
float ina226_getcurrent_A(void);      // 电流 (A)
float ina226_getpower_W(void);        // 功率 (W)
```

### 模型接口（`DCPS_model.h`）

```c
extern ExtU_DCPS_model_T DCPS_model_U;   // 输入（state, V/I_target, V/I_value）
extern ExtY_DCPS_model_T DCPS_model_Y;   // 输出（duty[2]）

void DCPS_model_initialize(void);        // 初始化 PID 参数 + 清状态
void DCPS_model_step(void);              // 执行一步 PID（每 2ms 调一次）
```

---

## 九、当前状态与已知问题

### 已完成

- [x] STM32CubeMX HAL 工程配置（ADC/I2C/TIM/OPAMP/UART）
- [x] INA226 电流传感器驱动
- [x] ADC 电压采样封装（输入 + 输出）
- [x] FSBB 四开关 Buck-Boost PWM 驱动
- [x] MATLAB/Simulink PID 模型生成与数据类型适配（single）
- [x] PID 参数参数化（可运行时调整）
- [x] 闭环控制集成（fsbb_init / fsbb_run）
- [x] 前馈 + PID 复合控制
- [x] Buck/Boost 模式自动切换

### 未完成 / 已知问题

- [ ] **过压/过流急停**：当前无硬件保护，闭环异常可能损坏器件
- [ ] **VOFA 波形上报**：未接入上位机调参工具，PID 整定缺乏可视化
- [ ] **Buck/Boost 切换迟滞**：临界点（V_target ≈ Vin）无迟滞，可能频繁切换
- [ ] **设定值外部输入**：state/V_target/I_target 当前硬编码，未接串口/上位机
- [ ] **实测验证**：闭环控制未经上机验证

### 注意事项

- `MATLAB/DCPS_model_ert_rtw/` 下的代码是生成原档，实际编译使用的是 `Hardware/FSBB/` 下的副本（含参数化修改）。重新生成模型后需手动将改动同步到 `Hardware/FSBB/`。
- `ert_main.c` 是 PC 示例代码（含 `main()`），不可纳入构建，否则与 STM32 的 `main()` 冲突。

---

## 十、后续可扩展方向

1. **安全保护**：在 `fsbb_run()` 开头加过压/过流检查，超限则 `fsbb_stop()`
2. **波形上报**：USART2 接 VOFA+ 上位机，实时上报 V/I/duty 波形用于 PID 整定
3. **模式切换迟滞**：V_target ≈ Vin 临界点加 ±0.3V 迟滞窗口
4. **串口协议**：上位机下发 state/V_target/I_target，替代硬编码
5. **软启动**：上电时占空比从 0 缓升至目标值，进一步减小启动冲击
6. **PID 自整定**：基于继电反馈法或模型辨识自动整定参数

---

## 十一、开发环境

- IDE：CMake + arm-none-eabi-gcc（非 STM32CubeIDE）
- MATLAB：R2025b + Simulink Coder 25.2
- MCU 配置工具：STM32CubeMX
- 调试器：ST-Link V2/V3（SWD 接口）

---


