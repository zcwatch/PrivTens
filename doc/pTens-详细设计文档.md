# pTens-v2.0 详细设计文档

## 文档信息
- **项目名称**: pTens-v2.0 TENS经皮电神经刺激治疗仪
- **版本**: V1.0
- **硬件平台**: GD32F30x (ARM Cortex-M3)
- **开发工具**: MDK-Keil

---

## 1. 系统概述

### 1.1 系统功能

pTens-v2.0是一款多通道TENS经皮电神经刺激治疗设备，通过皮肤表面电极输出低频脉冲电流来刺激神经，达到止痛和物理治疗的目的。

**主要功能:**
- 多通道电刺激输出 (0-30级强度调节)
- 10种预设疗法处方
- 振动按摩功能 (0-5模式)
- 热疗功能 (4档温度: 30°C/40°C/50°C/60°C)
- 蓝牙无线通信
- 锂电池供电与充电管理
- LCD显示界面

### 1.2 系统架构

```
┌─────────────────────────────────────────────────────────────────┐
│                         main.c                                  │
│              (SysTick中断 / 定时任务调度)                       │
└─────────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
┌───────────────┐   ┌───────────────┐   ┌───────────────┐
│  tensProcess  │   │prescoutProcess│   │tprotoDecode  │
│   (状态机)    │   │  (处方波)     │   │  (协议解析)   │
└───────────────┘   └───────────────┘   └───────────────┘
        │                     │                     │
        ▼                     ▼                     ▼
┌───────────────┐   ┌───────────────┐   ┌───────────────┐
│ buttonProcess │   │  pwmoutProcess│   │  rbuf (环形)  │
│  (按键处理)   │   │   (PWM输出)   │   │   (数据缓冲)  │
└───────────────┘   └───────────────┘   └───────────────┘
        │                     │                     │
        ▼                     ▼                     ▼
┌───────────────┐   ┌───────────────┐   ┌───────────────┐
│  按键/GPIO    │   │ Timer0+DMA    │   │   USART1      │
│   输入检测    │   │  (PWM生成)    │   │  (蓝牙通信)   │
└───────────────┘   └───────────────┘   └───────────────┘
```

---

## 2. 核心模块设计

### 2.1 TENS核心模块 (tens.c/h)

#### 2.1.1 主要数据结构

```c
typedef struct {
    unsigned char state;              // TENS状态机
    unsigned char keyFunc;            // 当前按键功能: 振动/加热/时间
    unsigned char tensMode;           // TENS模式
    unsigned char tensChannel;         // 输出通道
    unsigned char chargeState:1;      // 充电状态
    unsigned int chargeCnt;           // 充电计时
    unsigned char btState:1;          // 蓝牙状态
    unsigned char pwrState:2;         // 电源状态
    unsigned short pwrCount;          // 电源按键计数
    
    unsigned char heatLevel;          // 加热等级 0-4
    unsigned char vibMode;            // 振动模式 0-5
    unsigned char channelCnt;        // 通道切换计数
    
    unsigned char prescId;            // 当前处方ID
    
    unsigned short runTime;           // 运行总时间
    unsigned short runTimeCount;     // 已运行时间计数
    unsigned short defaultTime;       // 默认时间
    
    unsigned char tensLevel;          // 当前强度
    unsigned char tarLevel;          // 目标强度
    
    // 组件
    MotorTip_t motorTip;
    TensProto_t proto;
    Button_t button;
    HC595_t hc595;
    Battery_t battery;
    Heat_t heat;
    Vib_t vib;
    TensOutput_t tensOutput;
    TherapyManager_t therapyManager;
} Tens_t;
```

#### 2.1.2 状态机

| 状态 | 值 | 说明 |
|------|-----|------|
| TENS_NONE | 0 | 初始状态 |
| TENS_IDLE | 1 | 待机状态 |
| TENS_RUN | 2 | 运行状态 |
| TENS_PAUSE | 3 | 暂停状态 |
| TENS_FW | 4 | 固件升级状态 |

#### 2.1.3 按键功能

| 功能 | 值 | 说明 |
|------|-----|------|
| KEYFUNC_VIB | 0 | 振动调节 |
| KEYFUNC_HEAT | 1 | 加热调节 |
| KEYFUNC_TIME | 2 | 时间调节 |

#### 2.1.4 按键映射

| 按键 | 状态变化处理 | 长按处理 |
|------|-------------|----------|
| TENS+ | 强度+1 | 连续增加 |
| TENS- | 强度-1 | 连续减少 |
| CONFIRM | 开始/暂停/继续 | 停止 |
| POWER | 3次短按关机 | - |
| SET | 切换功能 | - |
| LEFT | 上一处方 | - |
| RIGHT | 下一处方 | - |

#### 2.1.5 核心函数

| 函数 | 功能 |
|------|------|
| `tensInit()` | 初始化TENS模块 |
| `tensProcess()` | 主处理循环 |
| `tensStartOutput()` | 启动输出 |
| `tensStopOutput()` | 停止输出 |
| `tensPauseOutput()` | 暂停输出 |
| `tensSetLevel()` | 设置强度 |
| `tensSetHeat()` | 设置加热 |
| `tensSetVib()` | 设置振动 |
| `tensLoadPrescData()` | 加载处方数据 |

---

### 2.2 通信协议模块 (tensproto.c/h)

#### 2.2.1 协议帧格式

```
┌──────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
│ HEAD │ LEN │ VER │ SEQ │ TY  │ GCMD│ CMD │DATA │ CRC │ END│
│ 0xAA │ 2B  │ 1B  │ 1B  │ 1B  │ 1B  │ 1B  │ N   │ 1B  │0x55│
└──────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
```

- **HEAD**: 帧头 0xAA
- **LEN**: 数据长度 (包括头到CRC)
- **VER**: 协议版本 0x01
- **SEQ**: 序列号
- **TY**: 消息类型 (PC/设备/ACK)
- **GCMD**: 命令组 (系统/控制/数据)
- **CMD**: 命令码
- **DATA**: 数据负载
- **CRC**: 校验 (0x55 XOR所有字节)
- **END**: 帧尾 0x55

#### 2.2.2 命令定义

**系统命令 (GCMD=0x01)**

| CMD | 命令 | 功能 |
|-----|------|------|
| 0x01 | SYSCMD_HS | 握手 |
| 0x02 | SYSCMD_INFO | 获取设备信息 |
| 0x03 | SYSCMD_VER | 获取版本 |
| 0x04 | SYSCMD_HEARTBEAT | 心跳 |
| 0x05 | SYSCMD_ERR | 错误上报 |
| 0x06 | SYSCMD_FW | 固件升级 |

**控制命令 (GCMD=0x02)**

| CMD | 命令 | 功能 |
|-----|------|------|
| 0x01 | CTRLCMD_OUTPUTTIME | 输出时间 |
| 0x02 | CTRLCMD_CTRL | 启停控制 |
| 0x03 | CTRLCMD_STRENGTH | 强度设置 |
| 0x04 | CTRLCMD_CHANNEL | 通道设置 |

**数据命令 (GCMD=0x03)**

| CMD | 命令 | 功能 |
|-----|------|------|
| 0x01 | DATACMD_THERAPY | 疗法数据 |

#### 2.2.3 握手响应

```c
// 响应: 系统命令 + 握手成功 + 设备信息
{0x01, 0x01, 0x00, 0x??, 0x??}  // GCMD, SYSCMD_HS, 状态, 随机数取反x2
```

---

### 2.3 TENS输出模块 (tensoutput.c/h)

#### 2.3.1 数据结构

```c
typedef struct {
    unsigned char state;           // 输出状态
    unsigned char outType;          // 输出类型: 处方/实时
    RtDataOutput_t rtDataOutput;    // 实时数据输出
    PrescOutput_t prescOutput;      // 处方输出
    PwmOutput_t pwmOutput;          // PWM输出
} TensOutput_t;
```

#### 2.3.2 输出状态

| 状态 | 值 | 说明 |
|------|-----|------|
| TENS_OUT_STAT_STOP | 0 | 停止 |
| TENS_OUT_STAT_RUN | 1 | 运行 |
| TENS_OUT_STAT_PAUSE | 2 | 暂停 |

#### 2.3.3 PWM参数

- **载波频率**: 300kHz (HIGH_FREQ)
- **PWM周期**: pwmPeriod

---

### 2.4 疗法管理模块 (therapy.c/h)

#### 2.4.1 数据存储结构

```
[Mark 2字节] [数据包总长度 4字节] [疗法数量 1字节] [疗法数据...]
```

#### 2.4.2 疗法数据

```
[疗法ID 4字节] [疗法数据长度 2字节] [通道数 1字节] [通道1类型] [通道1处方数据]...
```

#### 2.4.3 主要函数

| 函数 | 功能 |
|------|------|
| `tmInit()` | 初始化疗法管理器 |
| `tmLoadTherapy()` | 加载疗法数据 |
| `tmFindPrescription()` | 查找处方数据 |

---

### 2.5 高压输出模块 (hv.c/h)

#### 2.5.1 DAC电平表

```c
// HV_MAXLEVEL = 30
unsigned short dacLevel[30] = {
    134, 204, 274, 344, 414, 484, 554, 624, 694, 764,  // 1-10
    834, 904, 974, 1044, 1114, 1184, 1254, 1324, 1394, 1464,  // 11-20
    1534, 1604, 1674, 1744, 1814, 1894, 1964, 2034, 2104, 2234  // 21-30
};
```

#### 2.5.2 函数

| 函数 | 功能 |
|------|------|
| `hvSetLevel(level)` | 设置输出强度 |
| `hvShutdown()` | 关闭输出 |

---

### 2.6 按键模块 (button.c/h)

#### 2.6.1 消抖处理

- **KEY_ANTI**: 消抖次数
- **KEY_REPEAT**: 重复间隔

#### 2.6.2 按键处理流程

```
读取GPIO -> 状态变化计数 < KEY_ANTI? -> 是: 计数++
                                -> 否: 触发变化回调
                                
消抖后状态稳定 -> 检测长按 -> 触发长按回调 (间隔KEY_REPEAT)
```

---

### 2.7 电池管理模块 (battery.c/h)

#### 2.7.1 ADC到电量转换

电池电量通过ADC采样获取，有两组查表:
- `batCharge[]`: 充电状态电量表
- `batSupply[]`: 放电状态电量表

#### 2.7.2 滤波算法

使用16次采样滑动平均滤波:
```c
adcSum = sum(adcBuff[16]);
bat->adc = adcSum >> 4;  // 除以16
```

---

### 2.8 加热模块 (heat.c/h)

#### 2.8.1 温度等级

| 等级 | 温度 |
|------|------|
| 0 | 关闭 |
| 1 | 30°C |
| 2 | 40°C |
| 3 | 50°C |
| 4 | 60°C |

---

### 2.9 振动模块 (vib.c/h)

#### 2.9.1 振动模式

| 模式 | 占空比 |
|------|--------|
| 0 | 关闭 |
| 1 | 25% |
| 2 | 50% |
| 3 | 75% |
| 4 | 自动循环模式1 |
| 5 | 自动循环模式2 |

---

### 2.10 环形缓冲模块 (rbuf.c/h)

#### 2.10.1 数据结构

```c
typedef struct {
    unsigned int size;     // 缓冲区大小
    unsigned int rIdx;     // 读索引
    unsigned int wIdx;     // 写索引
    unsigned char *buf;    // 数据指针
} RBuf_t;
```

---

## 3. 硬件资源分配

### 3.1 中断向量

| 中断 | 功能 |
|------|------|
| SysTick_Handler | 系统tick (1ms) |
| USART1_IRQHandler | 蓝牙串口接收 |
| TIMER2_IRQHandler | 马达提示音 |
| DMA0_Channel1_IRQHandler | PWM DMA完成 |

### 3.2 外设使用

| 外设 | 用途 |
|------|------|
| TIMER0 | PWM输出 |
| TIMER2 | 马达提示音PWM |
| TIMER11 | 系统tick计数 |
| USART1 | 蓝牙串口 |
| DMA0_CH1 | PWM DMA传输 |
| ADC | 电池/高压采样 |
| GPIO | 按键/LCD/HC595/继电器 |

### 3.3 Flash布局

| 地址 | 大小 | 用途 |
|------|------|------|
| 0x08001800 | 60KB | APP程序 |
| 0x08010800 | 60KB | 固件升级区 |
| 0x08027000 | 8KB | 设备信息 |
| 0x08027800 | 8KB | 涂鸦数据 |
| 0x0802F000 | 8KB | 疗法处方数据 |

---

## 4. 主程序流程

### 4.1 启动流程

```
main()
├── halHardwareInit()       // 硬件初始化
├── lcdInit()               // LCD初始化
├── tensInit()              // TENS初始化
│   ├── tensdataInit()      // 数据初始化
│   ├── buttonInit()       // 按键初始化
│   ├── tprotoInit()       // 协议初始化
│   ├── batteryInit()      // 电池初始化
│   ├── heatInit()         // 加热初始化
│   ├── vibInit()          // 振动初始化
│   ├── tensoutInit()      // 输出初始化
│   └── tmInit()           // 疗法管理器初始化
│
├── 等待电源键长按1秒
│
├── tensPowerUp()           // 开机
│   ├── halPowerEn(1)
│   └── guiInit()
│
└── 主循环
    ├── tensProcess()      // 状态处理
    ├── prescoutProcess()  // 处方处理
    ├── pwmoutProcess()    // PWM处理
    └── tprotoDecode()    // 协议解析
```

### 4.2 主循环 (10ms tick)

```c
while(1) {
    if(tick) {
        tick = 0;
        tensProcess(&privateTens);           // TENS状态处理
        
        if(secTick) {                         // 1秒周期
            secTick = 0;
            tensRunTimeUpdate(&privateTens); // 运行时更新
            tensLevelUpProcess(&privateTens); // 渐变处理
        }
    }
    prescoutProcess(&tensOutput.prescOutput); // 处方输出
    pwmoutProcess(&tensOutput.pwmOutput);     // PWM输出
    tprotoDecode(&proto);                     // 协议解析
}
```

---

## 5. 接口定义

### 5.1 硬件抽象层 (board.h)

```c
// 电源管理
void halPowerEn(unsigned char state);    // 电源开关

// 按键读取
unsigned char halKeyPower(void);         // 电源键
unsigned char halKeyTensUp(void);       // 强度+
unsigned char halKeyTensDown(void);     // 强度-
unsigned char halKeyTensConfirm(void);  // 确认键
unsigned char halScanKey(void);         // 扫描键
unsigned char halReadChargeSta(void);   // 充电状态

// 输出控制
void halHVEnable(unsigned char state);  // 高压使能
void halSetDac1(unsigned short val);     // DAC设置
void halHeat(unsigned char state);       // 加热控制
void halMotor(unsigned char state);      // 振动控制
void halMotorWarn(unsigned char state);  // 马达提示

// 移位寄存器
void halHC595SDA(unsigned char state);
void halHC595CLK(unsigned char state);
void halHC595LA(unsigned char state);

// ADC
unsigned short halGetBatAdc(void);       // 电池ADC
unsigned short halGetHVAdc(void);       // 高压ADC

// LCD
void halLcdWriteData(unsigned char data);
void halLcdWriteCmd(unsigned char cmd);

// Flash
int halFlashWrite(unsigned int addr, unsigned char *data, unsigned int len);
void halFlashErasePage(unsigned int addr);
int halFlashErase(unsigned int addr, unsigned int size);

// 通信
void btSenddata(unsigned char *data, int len);
```

---

## 6. 配置参数

### 6.1 版本信息

| 参数 | 值 |
|------|-----|
| FWVERSION | 0x10, 0x00, 0x03 |
| HWVERSION | 0x10, 0x01, 0x00 |
| PROTO_VERSION | 0x10, 0x00, 0x00 |

### 6.2 系统参数

| 参数 | 值 | 说明 |
|------|-----|------|
| DEFAULT_RUN_TIME | 1800秒 | 默认治疗时间30分钟 |
| TICK_CNT | 10 | 10ms tick |
| PRESC_DATA_SIZE | 10 | 处方数量 |
| HV_MAXLEVEL | 30 | 最大强度等级 |
| FWMARK | 0xAA55 | 固件标记 |
| TPMARK | 0xAA55 | 疗法数据标记 |

---

## 7. 错误处理

### 7.1 看门狗

- Release模式下启用看门狗 (WDT_EN=1)
- 1秒周期喂狗

### 7.2 错误上报

通过协议命令 `SYSCMD_ERR` 上报错误:
- 帧CRC错误
- Flash写入失败
- 疗法数据加载失败

---

## 8. 构建开发环境

### 8.1 Keil MDK环境

- **IDE**: Keil uVision V5.06+
- **芯片包**: GigaDevice.GD32F30x_DFP
- **项目文件**: `prj/privateTens.uvprojx`

### 8.2 ARM GCC (Linux/Ubuntu) 环境

项目支持在Ubuntu下使用ARM GCC工具链编译：

**安装工具链**:
```bash
sudo apt update && sudo apt install -y gcc-arm-none-eabi
```

**编译命令**:
```bash
make              # Debug版本
make BUILD=release # Release版本
make clean        # 清理构建文件
make size        # 查看占用空间
```

**输出文件**:
- `build/privateTens.bin` - 二进制固件 (~53KB)
- `build/privateTens.elf` - ELF调试文件

**构建配置**:
| 配置 | 值 |
|------|-----|
| CPU | GD32F303CC (Cortex-M4) |
| Flash | 256KB (0x08000000) |
| RAM | 48KB (0x20000000) |
| 编译器 | arm-none-eabi-gcc 9.x |
| 优化 | -Og (Debug) / -O2 (Release) |

**创建的文件**:
- `Makefile` - 构建规则
- `startup_gd32f30x_hd.S` - GCC启动文件
- `ldscript.ld` - 链接脚本

**烧录方式**:
```bash
# J-Link
JLinkExe -device GD32F303CC -if SWD - CommanderScript flash.jlink

# OpenOCD
openocd -f interface/jlink.cfg -c "transport select swd" -c "source [find target/stm32f1x.cfg]" -c "flash write_image erase build/privateTens.bin 0x08000000"
```

### 8.3 项目结构

```
PrivTens/
├── Makefile              # GCC构建文件
├── startup_gd32f30x_hd.S # 启动代码
├── ldscript.ld          # 链接脚本
├── prj/                 # Keil项目
├── source/              # 应用源码
├── tenslibs/             # 处方库
├── eeprom_in_flash/      # EEPROM模拟
└── GD32F30x_Firmware_Library/ # GD32固件库
```
