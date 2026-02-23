# pTens-v2.0 通信协议详细设计

## 文档信息
- **项目名称**: pTens-v2.0 TENS经皮电神经刺激治疗仪
- **版本**: V1.0
- **更新时间**: 2024

---

## 1. 协议概述

### 1.1 通信方式

| 参数 | 值 |
|------|-----|
| 物理接口 | USART1 |
| 波特率 | 115200 bps |
| 数据位 | 8 |
| 停止位 | 1 |
| 校验位 | 无 |
| 流控制 | 无 |

### 1.2 通信模式

设备支持两种协议实现:
1. **TensProtocol** - 旧版协议 (tensprotocol.h)
2. **TensProto** - 新版协议 (tensproto.h) - **当前使用**

---

## 2. TensProto 协议 (当前使用)

### 2.1 帧格式

```
┌──────┬────────┬─────┬─────┬─────┬─────┬─────┬─────┐
│ HEAD │  LEN   │ VER │ SEQ │ TY  │GCMD │ CMD │DATA │
│ 1B   │  2B    │ 1B  │ 1B  │ 1B  │ 1B  │ 1B  │ N   │
├──────┼────────┼─────┼─────┼─────┼─────┼─────┼─────┤
│ 0xAA │ len+4  │ 0x01│ seq │ type│gcmd │ cmd │data │
└──────┴────────┴─────┴─────┴─────┴─────┴─────┴─────┘

┌──────┐
│ CRC  │ 1B
└──────┘

┌──────┐
│ END  │ 1B
└──────┘
```

### 2.2 字段说明

| 字段 | 长度 | 说明 | 值 |
|------|------|------|-----|
| HEAD | 1 | 帧头 | 0xAA |
| LEN | 2 | 长度(高字节在前) | 从VER到DATA的字节数 |
| VER | 1 | 协议版本 | 0x01 |
| SEQ | 1 | 序列号 | 0x00-0xFF |
| TY | 1 | 消息类型 | 见下表 |
| GCMD | 1 | 命令组 | 见下表 |
| CMD | 1 | 命令码 | 见下表 |
| DATA | N | 数据负载 | 命令相关数据 |
| CRC | 1 | 校验码 | 0x55 XOR 所有字节 |
| END | 1 | 帧尾 | 0x55 |

### 2.3 消息类型 (TY)

| 值 | 类型 | 说明 |
|----|------|------|
| 0x00 | TY_ACK | 响应/确认 |
| 0x01 | TY_PC | PC端发起 |
| 0x02 | TY_DEV | 设备端发起 |

### 2.4 命令组 (GCMD) 与命令码 (CMD)

#### 2.4.1 系统命令 (GCMD = 0x01)

| CMD | 命令名称 | 功能 | 数据方向 |
|-----|----------|------|----------|
| 0x01 | SYSCMD_HS | 握手 | 双向 |
| 0x02 | SYSCMD_INFO | 获取设备信息 | PC→设备 |
| 0x03 | SYSCMD_VER | 获取版本 | PC→设备 |
| 0x04 | SYSCMD_HEARTBEAT | 心跳 | 双向 |
| 0x05 | SYSCMD_ERR | 错误上报 | 设备→PC |
| 0x06 | SYSCMD_FW | 固件升级 | PC→设备 |

#### 2.4.2 控制命令 (GCMD = 0x02)

| CMD | 命令名称 | 功能 | 数据方向 |
|-----|----------|------|----------|
| 0x01 | CTRLCMD_OUTPUTTIME | 输出时间设置/读取 | 双向 |
| 0x02 | CTRLCMD_CTRL | 启停控制 | PC→设备 |
| 0x03 | CTRLCMD_STRENGTH | 强度设置 | 双向 |
| 0x04 | CTRLCMD_CHANNEL | 通道设置 | PC→设备 |

#### 2.4.3 数据命令 (GCMD = 0x03)

| CMD | 命令名称 | 功能 | 数据方向 |
|-----|----------|------|----------|
| 0x01 | DATACMD_THERAPY | 疗法数据下载 | PC→设备 |

---

## 3. 命令详解

### 3.1 系统命令

#### 3.1.1 握手命令 (SYSCMD_HS)

**请求 (PC→设备)**
```
HEAD(1) + LEN(2) + VER(1) + SEQ(1) + TY(1) + GCMD(1) + CMD(1) + DATA + CRC(1) + END
         0xAA    0x00 0x07   0x01   seq    0x02    0x01    0x01    r1 r2   crc    0x55
```

**DATA字段**
| 字段 | 长度 | 说明 |
|------|------|------|
| R/W | 1 | 0x01=读, 0x02=写 |
| 随机数1 | 1 | 随机字节 |
| 随机数2 | 1 | 随机字节 |

**响应 (设备→PC)**
```
HEAD + LEN + VER + SEQ + TY + GCMD + CMD + DATA + CRC + END
0xAA  len   0x01  seq   0x00  0x01   0x01   0x00 ~r1 ~r2  crc  0x55
```

**DATA字段**
| 字段 | 长度 | 说明 |
|------|------|------|
| 状态 | 1 | 0x00=成功 |
| ~随机数1 | 1 | 随机数取反 |
| ~随机数2 | 1 | 随机数取反 |

---

#### 3.1.2 获取设备信息 (SYSCMD_INFO)

**请求 (PC→设备)**
```
GCMD=0x01, CMD=0x02, DATA[0]=0x01(读)
```

**响应 (设备→PC)**
```
DATA字段 (共40字节):
+00: 0x01  - 设备类型
+01: 0x02  - 预留
+02: 0x01  - 预留
+03: 0x01  - 预留
+04~+15: "P00000000001-001"  - 设备ID (12字节)
+16~+19: "0123"              - 协议版本 (4字节)
+20~+35: 16字节全0           - 预留
```

---

#### 3.1.3 获取版本 (SYSCMD_VER)

**请求 (PC→设备)**
```
GCMD=0x01, CMD=0x03, DATA[0]=0x01(读)
```

**响应 (设备→PC)**
```
DATA字段 (共9字节):
+00: 0x01 - 固件主版本
+00: 0x00 - 固件次版本
+02: 0x18 - 固件修订号 (24)
+03: 0x07 - 固件构建号 (7)
+04: 0x07 - 硬件版本
+05: 0x01 - 预留
+06: 0x00 - 预留
+07: 0x18 - 年份 (2024)
+08: 0x01 - 日期 (1月1日)
```

---

#### 3.1.4 心跳 (SYSCMD_HEARTBEAT)

**请求 (PC→设备)**
```
GCMD=0x01, CMD=0x04
```

**响应 (设备→PC)**
```
DATA字段 (共4字节):
+00: 电量百分比 (0-100)
+01: TENS状态 (1=待机, 2=运行, 3=暂停)
+02: 剩余时间低字节
+03: 剩余时间高字节
```

---

#### 3.1.5 错误上报 (SYSCMD_ERR)

**主动上报 (设备→PC)**
```
GCMD=0x01, CMD=0x05, DATA=错误码
```

---

#### 3.1.6 固件升级 (SYSCMD_FW)

**流程:**
1. PC发送分片数据
2. 设备写入Flash
3. 设备返回确认

**请求 (PC→设备)**
```
DATA字段:
+00: 包序号 (0开始)
+01: 总包数-1 (即包总数的高字节? 见下方代码)
+02~+03: 数据长度 (小端)
+04~: 数据

if (data[2] == 0) {  // 第一包
    pkgCount = 0;
    tensdataEraseFWFlash();  // 擦除固件区
}

pkgLen = GETSHORT(data, 3);
halFlashWrite(FWFILE_ADDR + 6 + pkgMax*data[2], data + 5, pkgLen);

// 最后一包
if(data[1] == data[2] + 1) {
    dataLen = FWMARK;  // 0xAA55
    halFlashWrite(FWFILE_ADDR, (unsigned char*)&dataLen, 2);
    halFlashWrite(FWFILE_ADDR + 2, (unsigned char*)&dataWIdx, 4);
}
```

**响应 (设备→PC)**
```
DATA字段:
+00: GCMD (0x01)
+01: CMD (0x06)
+02: 状态 (0x00)
+03: 包序号 (与请求一致)
```

---

### 3.2 控制命令

#### 3.2.1 输出时间 (CTRLCMD_OUTPUTTIME)

**请求 (PC→设备)**
```
DATA[0]: 0x01=读, 0x02=写
DATA[1~2]: 时间低/高字节 (写时)
```

**响应 (设备→PC)**
```
DATA字段:
+00: GCMD (0x02)
+01: CMD (0x01)
+02: 状态 (0x00)
+03: 总时间低字节
+04: 总时间高字节
+05: 剩余时间低字节
+06: 剩余时间高字节
```

---

#### 3.2.2 启停控制 (CTRLCMD_CTRL)

**请求 (PC→设备)**
```
DATA[0]: 0x01=读, 0x02=写
DATA[1]: 控制码
  0x01: 开始
  0x02: 暂停
  0x03: 停止
  0x04: 关机
  0x05: 关机
```

**响应 (设备→PC)**
```
DATA字段:
+00: GCMD (0x02)
+01: CMD (0x02)
+02: 状态 (0x00)
+03: 控制码 (回显)
```

---

#### 3.2.3 强度设置 (CTRLCMD_STRENGTH)

**请求 (PC→设备)**
```
DATA[0]: 0x01=读, 0x02=写
DATA[1]: 强度值 (0-30)
```

**响应 (设备→PC)**
```
DATA字段:
+00: GCMD (0x02)
+01: CMD (0x03)
+02: 状态 (0x00)
+03: 固定0x02
+04: 强度值 (实际设置值)
+05: 强度值 (实际设置值)
```

---

#### 3.2.4 通道设置 (CTRLCMD_CHANNEL)

**请求 (PC→设备)**
```
DATA[0]: 0x01=读, 0x02=写
DATA[1]: 通道参数
```

**响应 (设备→PC)**
```
DATA字段:
+00: GCMD (0x02)
+01: CMD (0x04)
+02: 状态 (0x00)
+03: 固定0x15
```

---

### 3.3 数据命令

#### 3.3.1 疗法数据 (DATACMD_THERAPY)

**下载疗法数据 (PC→设备)**

用于从APP下载疗法处方数据到设备Flash存储。

**请求格式:**
```
DATA[0]: 0x01=读, 0x02=写
DATA[1]: 总包数
DATA[2]: 当前包序号 (0开始)
DATA[3~4]: 数据长度 (小端)
DATA[5~]: 疗法数据
```

**数据存储格式:**
```
[Mark 2字节: 0xAA55] [数据总长度 4字节] [疗法数量 1字节] [疗法1] [疗法2] ...
```

**单疗法数据格式:**
```
[疗法ID 4字节] [疗法数据长度 2字节] [通道数 1字节] [通道1类型] [通道1数据] [通道2数据]...
```

**处理流程:**
```c
if(pkgIdx == 0) {  // 第一包
    tpPkgCount = 0;
    tpDataWIdx = 6;
    tensdataEraseTherapyFlash();  // 擦除疗法数据区
}

halFlashWrite(PRESC_DATA_ADDR + tpDataWIdx, data + 5, pkgLen);
tpDataWIdx += pkgLen;

// 最后包
if(pkgIdx + 1 == pkgTotal) {
    pkgLen = TPMARK;  // 0xAA55
    halFlashWrite(PRESC_DATA_ADDR, (unsigned char*)&pkgLen, 2);
    halFlashWrite(PRESC_DATA_ADDR + 2, (unsigned char*)&tpDataWIdx, 4);
}
```

**响应 (设备→PC)**
```
DATA字段:
+00: GCMD (0x03)
+01: CMD (0x01)
+02: 状态 (0x00)
+03: 包序号
```

---

## 4. 通信流程

### 4.1 典型通信流程

```
1. APP发起连接
   │
2. APP发送握手
   │  PC→设备: SYSCMD_HS
   │  ←── 设备: ACK响应
   │
3. APP获取设备信息 (可选)
   │  PC→设备: SYSCMD_INFO
   │  ←── 设备: 设备信息
   │
4. APP发送控制命令
   │  PC→设备: CTRLCMD_xxx
   │  ←── 设备: ACK响应
   │
5. 定时心跳 (可选)
   │  双向: SYSCMD_HEARTBEAT
   │
6. 疗法数据下载 (可选)
   │  PC→设备: DATACMD_THERAPY (多包)
   │  ←── 设备: ACK响应
```

### 4.2 心跳机制

- APP应定期发送心跳命令
- 设备返回当前状态信息
- 用于保持连接和获取实时状态

---

## 5. 状态码定义

### 5.1 TENS状态

| 值 | 状态 | 说明 |
|----|------|------|
| 0 | TENS_NONE | 初始状态 |
| 1 | TENS_IDLE | 待机 |
| 2 | TENS_RUN | 运行中 |
| 3 | TENS_PAUSE | 暂停 |
| 4 | TENS_FW | 固件升级 |

### 5.2 响应状态

| 值 | 状态 | 说明 |
|----|------|------|
| 0x00 | 成功 | 操作成功 |
| 0x01 | 失败 | 操作失败 |
| 0x02 | 忙碌 | 设备忙碌 |

---

## 6. CRC校验

### 6.1 算法

```c
unsigned char tprotoCrc(unsigned char *data, int len)
{
    unsigned char crc = 0x55;
    int i;
    for(i = 0; i < len; i++) {
        crc ^= data[i];
    }
    return crc;
}
```

### 6.2 校验范围

从LEN字段到DATA字段的所有字节，不包含HEAD和CRC本身。

---

## 7. 错误处理

### 7.1 错误类型

| 错误码 | 说明 |
|--------|------|
| 0x01 | CRC校验失败 |
| 0x02 | 帧格式错误 |
| 0x03 | 命令不支持 |
| 0x04 | 数据长度错误 |
| 0x05 | Flash写入失败 |

### 7.2 错误响应

设备主动上报错误:
```
GCMD=0x01, CMD=0x05, DATA=错误码
```

---

## 8. 附录: 数据结构

### 8.1 TensCmd_t

```c
typedef struct {
    unsigned char gcmd;        // 命令组
    unsigned char cmd;         // 命令码
    CmdDealFunc deal;          // 处理函数
} TensCmd_t;
```

### 8.2 TensProto_t

```c
typedef struct {
    unsigned char decodeState;     // 解码状态机
    unsigned short decodeLen;     // 解码长度
    unsigned char *decodeBuf;      // 解码缓冲
    unsigned char decodeSeq;       // 序列号
    unsigned char decodeType;      // 消息类型
    unsigned char decodeCmd;       // 命令码
    unsigned char decodeGCmd;      // 命令组
    
    unsigned short enLen;         // 编码长度
    unsigned char *enBuf;          // 编码缓冲
    
    unsigned char cmdCnt;         // 命令数量
    TensCmd_t *cmds;              // 命令表
    RBuf_t *rBuf;                 // 接收缓冲
    UartSendFunc send;            // 发送函数
} TensProto_t;
```

### 8.3 环形缓冲 (RBuf_t)

```c
typedef struct {
    unsigned int size;        // 缓冲区大小
    unsigned int rIdx;        // 读索引
    unsigned int wIdx;        // 写索引
    unsigned char *buf;       // 数据指针
} RBuf_t;
```

---

## 9. 版本历史

| 版本 | 日期 | 修改内容 |
|------|------|----------|
| V1.0 | 2024 | 初始版本 |
