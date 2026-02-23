# pTens BLE 微信小程序概要设计

## 文档信息
- **项目名称**: pTens BLE 微信小程序
- **版本**: V1.0
- **更新时间**: 2024

---

## 1. 项目概述

### 1.1 功能定位

微信小程序用于配合pTens-v2.0 TENS治疗仪设备，实现以下功能：
- 蓝牙连接与设备发现
- 设备状态监控与控制
- 疗法处方管理
- **固件OTA远程升级**
- 用户数据同步

### 1.2 核心特性

| 功能 | 说明 |
|------|------|
| 蓝牙BLE通信 | 连接pTens设备 |
| 固件下载 | 从服务器获取固件文件 |
| OTA升级 | 通过蓝牙传输固件 |
| 设备控制 | 启停、强度、时间设置 |
| 疗法管理 | 查看/下载处方数据 |

---

## 2. 系统架构

### 2.1 整体架构

```
┌─────────────────────────────────────────────────────────────┐
│                      微信小程序                              │
├─────────────┬─────────────┬─────────────┬─────────────────┤
│   设备页    │   控制页    │   升级页    │    设置页       │
├─────────────┴─────────────┴─────────────┴─────────────────┤
│                      业务逻辑层                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────┐   │
│  │ 设备管理    │  │ 协议通信    │  │ 固件管理        │   │
│  └─────────────┘  └─────────────┘  └─────────────────┘   │
├─────────────────────────────────────────────────────────────┤
│                      蓝牙适配层                             │
│  wx.openBluetoothAdapter / wx.createBLEConnection 等      │
├─────────────────────────────────────────────────────────────┤
│                      微信小程序框架                         │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      云端服务器                              │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────┐   │
│  │ 固件存储    │  │ 用户数据    │  │ 设备信息        │   │
│  └─────────────┘  └─────────────┘  └─────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 页面结构

```
pages/
├── index/              首页/设备发现
│   ├── index.wxml
│   ├── index.wxss
│   ├── index.js
│   └── index.json
├── device/             设备控制
│   ├── device.wxml
│   ├── device.wxss
│   ├── device.js
│   └── device.json
├── upgrade/           固件升级
│   ├── upgrade.wxml
│   ├── upgrade.wxss
│   ├── upgrade.js
│   └── upgrade.json
├── therapy/           疗法下载
│   ├── therapy.wxml
│   ├── therapy.wxss
│   ├── therapy.js
│   └── therapy.json
└── settings/          设置
    ├── settings.wxml
    ├── settings.wxss
    ├── settings.js
    └── settings.json
```

---

## 3. 功能模块设计

### 3.1 蓝牙通信模块

#### 3.1.1 BLE模块 (KT6368A)

根据KT6368A BLE 2.1协议文档

**UUID配置:**

| UUID | 类型 | 说明 |
|------|------|------|
| 服务UUID | 0000FFF0-0000-1000-8000-00805F9B34FB | KT6368A主服务 |
| 写特征值 | 0000FFF1-0000-1000-8000-00805F9B34FB | 写数据到设备 |
| 通知特征值 | 0000FFF2-0000-1000-8000-00805F9B34FB | 接收设备数据 |

#### 3.1.2 连接流程

```
1. 初始化蓝牙
   wx.openBluetoothAdapter()
   │
2. 开始搜索
   wx.startBluetoothDevicesDiscovery()
   │
3. 发现设备 → 用户选择
   wx.onBluetoothDeviceFound()
   │
4. 停止搜索
   wx.stopBluetoothDevicesDiscovery()
   │
5. 建立连接
   wx.createBLEConnection(deviceId)
   │
6. 获取服务
   wx.getBLEDeviceServices(deviceId)
   │
7. 获取特征值
   wx.getBLEDeviceCharacteristics(deviceId, serviceId)
   │
8. 启用通知
   wx.notifyBLECharacteristicValueChange()
   │
9. 开启数据接收
   wx.onBLECharacteristicValueChange()
```

---

### 3.2 协议通信模块

根据KT6368A BLE 2.1协议

#### 3.2.1 数据帧格式

```
┌────────┬──────────┬────────┬───────────┐
│ Start  │ Command  │ Length│ Payload  │
│  0xAA  │  Command │   N   │    N     │
├────────┼──────────┼────────┼───────────┤
│ Checksum│          │        │           │
│  XOR   │          │        │           │
└────────┴──────────┴────────┴───────────┘
```

| 字段 | 长度 | 说明 |
|------|------|------|
| Start | 1 | 帧头 0xAA |
| Command | 1 | 命令码 |
| Length | 1 | 数据长度 (N) |
| Payload | N | 命令参数或数据 |
| Checksum | 1 | XOR校验 (Start~Payload所有字节) |

#### 3.2.2 命令码定义

| 命令码 | 功能 | 说明 |
|--------|------|------|
| 0x10 | 设备信息 | 获取固件版本/硬件ID |
| 0x20 | 开始测量 | 启动TENS输出 |
| 0x21 | 停止测量 | 停止TENS输出 |
| 0x30 | 读取数据 | 获取设备状态 |
| 0x40 | 强度设置 | 设置TENS强度 |
| 0x50 | 固件升级 | OTA升级命令 |
| 0x60 | 疗法数据 | 疗法数据下载 |

#### 3.2.3 响应格式

```
┌────────┬──────────┬────────┐
│ Start  │ Command │Status │
│  0xAA  │ Command │ 0x01  │  = 成功
│        │         │ 0x00  │  = 失败
└────────┴──────────┴────────┘
```

#### 3.2.4 错误码

| 错误码 | 说明 |
|--------|------|
| 0x00 | 成功 |
| 0xFF | 通用错误 |
| 0xFE | 无效参数 |

#### 3.2.5 通信流程

```
1. 连接设备
   │
2. 发送0x10获取设备信息
   │  请求: AA 10 00 checksum
   │  ←── 响应: AA 10 01 firmware_version hardware_id checksum
   │
3. 发送控制命令
   │  0x20: 开始输出
   │  0x21: 停止输出
   │  0x40: 设置强度
   │
4. 定时轮询状态
   │  发送0x30读取数据
   │  ←── 响应: 电量/状态/时间等
   │
```

#### 3.2.6 命令示例

**1. 获取设备信息 (0x10)**
```
请求: AA 10 00 (AA ^ 10 ^ 00 = BA)
响应: AA 10 06 V1.00.03 V1.01.00 checksum
```

**2. 开始TENS输出 (0x20)**
```
请求: AA 20 01 01 checksum   (01=mode)
响应: AA 20 01 01 checksum   (01=OK, 00=Error)
```

**3. 停止输出 (0x21)**
```
请求: AA 21 00 checksum
响应: AA 21 01 01 checksum
```

**4. 读取状态 (0x30)**
```
请求: AA 30 00 checksum
响应: AA 30 04 battery state time_low time_high checksum
```

**5. 设置强度 (0x40)**
```
请求: AA 40 01 level checksum   (level=0-30)
响应: AA 40 01 01 checksum
```

#### 3.2.7 重传机制

- **超时时间**: 100ms
- **重试次数**: 最多3次
- **校验失败**: 丢弃数据包，触发重传

---

### 3.3 固件升级模块 (核心功能)

#### 3.3.1 固件服务器

| 参数 | 值 |
|------|-----|
| 地址 | http://134.175.125.61:88/device/firmware |
| 认证方式 | HTTP Basic Auth |
| 用户名 | root |
| 密码 | miniTENS220528 |

#### 3.3.2 升级流程

```
┌─────────────────────────────────────────────────────────────┐
│                     固件升级流程                             │
├─────────────────────────────────────────────────────────────┤
│  1. 获取固件列表                                            │
│     ├── HTTP请求服务器 (Basic Auth)                         │
│     ├── 解析HTML目录获取固件文件列表                         │
│     └── 按版本号排序显示                                     │
│           │                                                 │
│           ▼                                                 │
│  2. 选择固件                                                │
│     ├── 用户从列表选择目标版本                               │
│     ├── 对比当前设备版本                                     │
│     └── 显示固件信息 (大小/日期/更新内容)                   │
│           │                                                 │
│           ▼                                                 │
│  3. 下载固件                                              │
│     ├── 用户确认开始                                        │
│     ├── 下载固件到手机本地                                  │
│     └── 校验固件完整性                                      │
│           │                                                 │
│           ▼                                                 │
│  4. 进入升级模式                                          │
│     ├── 停止TENS输出                                        │
│     └── 发送进入升级模式指令                                │
│           │                                                 │
│           ▼                                                 │
│  5. 传输固件                                              │
│     ├── 分片发送 (每包≤20字节BLE限制)                      │
│     ├── 等待设备确认                                        │
│     └── 循环直至完成                                        │
│           │                                                 │
│           ▼                                                 │
│  6. 验证重启                                              │
│     ├── 设备自动重启                                        │
│     ├── 重新连接                                            │
│     └── 验证新版本                                          │
└─────────────────────────────────────────────────────────────┘
```

#### 3.3.3 固件选择功能

- **BLE单包限制**: 20字节
- **应用层分包**: 每包 ≤ 240字节
- **KT6368A命令格式**:
  ```
  [Start:0xAA] [Cmd:0x50] [Length] [包序号:1] [总包数:1] [数据长度:2] [数据:240] [Checksum]
  ```
- **确认机制**: 发送一包 → 等待ACK → 超时重试 → 继续下一包
- **传输速率**: ~2KB/s (受BLE带宽限制)

#### 3.3.4 丢包重传机制

##### 3.3.4.1 重传策略设计

```
┌─────────────────────────────────────────────────────────────┐
│                    丢包重传机制                              │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────┐  │
│  │                 发送端                               │  │
│  │  1. 发送数据包                                      │  │
│  │  2. 启动计时器 (TIMER)                              │  │
│  │  3. 等待ACK                                         │  │
│  │     ├── 收到正确ACK → 发送下一包                    │  │
│  │     ├── 超时 → 重发当前包 (MAX_RETRY次)            │  │
│  │     └── 重发失败 → 升级失败                          │  │
│  └─────────────────────────────────────────────────────┘  │
│                            │                                │
│                            ▼                                │
│  ┌─────────────────────────────────────────────────────┐  │
│  │                 接收端 (设备)                         │  │
│  │  1. 接收数据包                                      │  │
│  │  2. 验证CRC                                         │  │
│  │     ├── CRC正确 → 写入Flash → 发送ACK              │  │
│  │     └── CRC错误 → 丢弃 → 不发送ACK (触发重传)     │  │
│  └─────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

##### 3.3.4.2 重传参数

| 参数 | 值 | 说明 |
|------|-----|------|
| MAX_RETRY | 3 | 最大重试次数 |
| ACK_TIMEOUT | 100ms | ACK超时时间 (根据KT6368A协议) |
| PACKET_TIMEOUT | 500ms | 单包超时 (含传输) |
| BLE_NOTIFY_TIMEOUT | 300ms | BLE通知超时 |

##### 3.3.4.3 数据包格式

```
发送数据包:
┌────────┬─────────┬───────┬────────┬───────┐
│ 包序号  │ 总包数-1 │ 长度L │  数据  │ CRC8  │
│  1B    │   1B    │  2B   │   L    │  1B   │
└────────┴─────────┴───────┴────────┴───────┘

响应ACK:
┌────────┬───────┐
│ 包序号  │ 状态  │
│  1B    │  1B   │
└────────┴───────┘
状态: 0x00=成功, 0x01=CRC错误, 0x02=Flash错误
```

##### 3.3.4.4 重传流程实现

```javascript
// KT6368A BLE配置
const BLE_CONFIG = {
  serviceUUID: '0000FFF0-0000-1000-8000-00805F9B34FB',
  writeUUID: '0000FFF1-0000-1000-8000-00805F9B34FB',
  notifyUUID: '0000FFF2-0000-1000-8000-00805F9B34FB',
  cmdFirmware: 0x50,
  cmdTherapy: 0x60,
  cmdDeviceInfo: 0x10,
  cmdStart: 0x20,
  cmdStop: 0x21,
  cmdReadData: 0x30,
  cmdSetStrength: 0x40
};

class FirmwareUploader {
  constructor() {
    this.maxRetry = 3;
    this.ackTimeout = 100;
    this.currentPkg = 0;
    this.retryCount = 0;
    this.isUploading = false;
  }

  // 发送单个数据包
  async sendPacket(pkgData) {
    return new Promise((resolve, reject) => {
      let timer;
      const pkgNum = pkgData[0];
      
      // 启动超时计时器
      timer = setTimeout(() => {
        reject(new Error('ACK_TIMEOUT'));
      }, this.ackTimeout);

      // 等待设备ACK
      this.onAckCallback = (ackPkgNum, status) => {
        clearTimeout(timer);
        if (ackPkgNum === pkgNum && status === 0x00) {
          resolve(true);  // 成功
        } else {
          reject(new Error('ACK_ERROR_' + status));
        }
      };

      // 发送数据
      this.sendBleData(pkgData);
    });
  }

  // 发送并重试
  async sendPacketWithRetry(pkgData) {
    this.retryCount = 0;
    
    while (this.retryCount < this.maxRetry) {
      try {
        await this.sendPacket(pkgData);
        return true;  // 成功
      } catch (error) {
        this.retryCount++;
        console.log(`重试 ${this.retryCount}/${this.maxRetry}`);
        
        if (this.retryCount >= this.maxRetry) {
          return false;  // 失败
        }
        
        // 等待后重试
        await this.sleep(500 * this.retryCount);
      }
    }
    return false;
  }

  // 升级主流程
  async uploadFirmware(firmwareData) {
    const totalPkg = Math.ceil(firmwareData.length / 240);
    
    for (let i = 0; i < totalPkg; i++) {
      const pkgData = this.buildPacket(i, totalPkg, firmwareData);
      
      const success = await this.sendPacketWithRetry(pkgData);
      if (!success) {
        throw new Error(`包 ${i} 传输失败`);
      }
      
      // 更新进度
      this.updateProgress((i + 1) / totalPkg * 100);
    }
    
    return true;
  }

  // 构建KT6368A格式数据包
  buildPacket(pkgIdx, totalPkg, firmwareData) {
    const offset = pkgIdx * 240;
    const data = firmwareData.slice(offset, offset + 240);
    const len = data.length;
    
    // 构建KT6368A帧格式:
    // [Start:0xAA] [Cmd:0x50] [Length] [包序号] [总数-1] [长度2] [数据] [Checksum]
    const payload = [
      pkgIdx,
      totalPkg - 1,
      len & 0xFF, (len >> 8) & 0xFF,
      ...data
    ];
    
    // 构建完整帧
    const frame = [
      0xAA,                        // Start
      BLE_CONFIG.cmdFirmware,       // Command
      payload.length,               // Length
      ...payload
    ];
    
    // 计算XOR校验和
    const checksum = this.calcChecksum(frame);
    frame.push(checksum);
    
    return frame;
  }

  // XOR校验计算
    
    return packet;
  }

  // XOR校验计算 (KT6368A协议)
  // Checksum = Byte0 ^ Byte1 ^ ... ^ ByteN
  calcChecksum(data) {
    let checksum = 0;
    for (let i = 0; i < data.length; i++) {
      checksum ^= data[i];
    }
    return checksum;
  }
}
```

##### 3.3.4.5 设备端ACK处理 (固件)

```c
// 设备端固件处理
static void cmdFwDeal(unsigned char *data, int len)
{
    unsigned char pkgIdx = data[2];
    unsigned short pkgLen = GETSHORT(data, 3);
    unsigned char recvCrc = data[5 + pkgLen];
    unsigned char calcCrc;
    
    // 计算接收数据CRC
    calcCrc = tprotoCrc(data, 5 + pkgLen);
    
    // 验证CRC
    if (recvCrc != calcCrc) {
        // CRC错误,不发送ACK,触发重传
        return;
    }
    
    // 写入Flash
    if (halFlashWrite(FWFILE_ADDR + 6 + pkgMax*pkgIdx, data + 5, pkgLen) != 0) {
        // Flash错误,发送错误ACK
        unsigned char ack[] = {pkgIdx, 0x02};  // 0x02=Flash错误
        tprotoAck(&proto, ack, 2);
        return;
    }
    
    // 发送成功ACK
    unsigned char ack[] = {pkgIdx, 0x00};  // 0x00=成功
    tprotoAck(&proto, ack, 2);
}
```

##### 3.3.4.6 错误处理

| 错误类型 | 错误码 | 处理方式 |
|----------|--------|----------|
| ACK超时 | 0x01 | 重发当前包,最多3次 |
| CRC错误 | 0x02 | 设备丢弃,等待重发 |
| Flash错误 | 0x03 | 终止升级,返回错误 |
| BLE断开 | 0x04 | 断开连接,提示重连 |
| 连续失败 | 0x05 | 终止升级,提示用户 |

##### 3.3.4.7 状态机

```
┌────────┐    发送数据包    ┌─────────┐
│  IDLE  │ ──────────────▶ │ SENDING │
└────────┘                 └─────────┘
     ▲                         │
     │                         │ 收到ACK
     │                    ┌────┴────┐
     │                    │         │
     │              成功   ▼      失败
     │              ┌─────────┐  ┌────────┐
     │              │ NEXT_PKG│  │ RETRY  │
     │              └─────────┘  └────────┘
     │                    │         │
     │                    │      重试<3?
     │                    │    ┌──┐   ┌────┐
     │                    │   是▼  否▼    │
     │                    │ 发送   升级失败│
     │                    │          └────┘
     │                    │
     └────────────────────┘
           全部完成
```

#### 3.3.4 升级页面状态

| 状态 | 说明 |
|------|------|
| idle | 初始状态 |
| fetching_list | 获取固件列表中 |
| list_ready | 固件列表就绪 |
| downloading | 下载固件中 |
| ready | 固件就绪 |
| upgrading | 升级传输中 |
| retrying | 重传中 (包X, 重试Y) |
| success | 升级成功 |
| failed | 升级失败 |

#### 3.3.5 升级进度显示

```
┌────────────────────────┐
│  固件升级中...        │
├────────────────────────┤
│                        │
│  ████████████░░ 78%   │
│                        │
│  包: 156/256           │
│  重试: 0/3             │
│                        │
│  当前: V1.00.04        │
│  目标: V1.00.05        │
│                        │
│  [取消升级]            │
└────────────────────────┘
```

---

### 3.4 设备控制模块

#### 3.4.1 控制功能

| 功能 | 说明 |
|------|------|
| 开始治疗 | 启动TENS输出 |
| 暂停治疗 | 暂停输出 |
| 停止治疗 | 停止输出 |
| 强度调节 | 0-30级滑块 |
| 处方选择 | 选择疗法处方 |
| 定时设置 | 5-60分钟 |

#### 3.4.2 实时显示

| 数据 | 更新频率 |
|------|----------|
| 电量 | 1秒 |
| 运行时间 | 1秒 |
| 当前强度 | 事件触发 |
| 工作状态 | 事件触发 |

---

### 3.5 疗法下载模块

#### 3.5.1 疗法服务器

| 参数 | 值 |
|------|-----|
| 地址 | http://134.175.125.61:88/device/therapy |
| 认证方式 | HTTP Basic Auth |
| 用户名 | root |
| 密码 | miniTENS220528 |

#### 3.5.2 疗法数据结构

设备端疗法数据存储格式:

```
[Mark: 2字节] [数据包总长度: 4字节] [疗法数量: 1字节] [疗法1] [疗法2] ... [疗法N]
```

单疗法数据格式:
```
[疗法ID: 4字节] [疗法数据长度: 2字节] [通道数: 1字节] [通道1类型] [通道1处方数据]...
```

#### 3.5.3 下载流程

```
┌─────────────────────────────────────────────────────────────┐
│                    疗法下载流程                             │
├─────────────────────────────────────────────────────────────┤
│  1. 获取疗法列表                                           │
│     ├── HTTP请求服务器 (Basic Auth)                        │
│     ├── 解析HTML目录获取疗法文件列表                        │
│     └── 显示可下载疗法                                      │
│           │                                                 │
│           ▼                                                 │
│  2. 选择疗法                                               │
│     ├── 用户选择要下载的疗法                                │
│     └── 显示疗法信息 (通道数/波形类型)                     │
│           │                                                 │
│           ▼                                                 │
│  3. 下载疗法数据                                           │
│     ├── 下载疗法文件到手机本地                              │
│     └── 解析疗法数据结构                                    │
│           │                                                 │
│           ▼                                                 │
│  4. 传输到设备                                             │
│     ├── 连接设备 (蓝牙)                                     │
│     ├── 通过DATACMD_THERAPY命令分包传输                    │
│     └── 等待设备写入Flash                                   │
│           │                                                 │
│           ▼                                                 │
│  5. 验证                                                   │
│     ├── 设备返回确认                                        │
│     └── 显示下载结果                                        │
└─────────────────────────────────────────────────────────────┘
```

#### 3.5.4 疗法文件格式

服务器上的疗法文件 (.bin 或 .therapy):

```
┌─────────────────────────────────────────┐
│  文件头 (16字节)                        │
│  +00: Magic "TENS" (4字节)              │
│  +04: 版本号 (2字节)                    │
│  +06: 疗法数量 (2字节)                  │
│  +08: 预留 (8字节)                      │
├─────────────────────────────────────────┤
│  疗法数据块 (N个)                       │
│  +00: 疗法ID (4字节)                    │
│  +04: 疗法名称 (32字节, UTF-8)         │
│  +36: 数据长度 (2字节)                  │
│  +38: 通道数 (1字节)                    │
│  +39: 预留 (1字节)                      │
│  +40: 通道1类型 (1字节)                 │
│  +41: 通道1数据 (长度-41字节)          │
│  ...                                    │
└─────────────────────────────────────────┘
```

疗法通道类型:
| 值 | 类型 |
|----|------|
| 0x01 | TENS标准 |
| 0x02 | TENS脉冲 |
| 0x03 | TENS调制 |
| 0x04 | TENS+热疗 |

#### 3.5.5 传输协议 (KT6368A命令)

使用命令码 `0x60` 传输疗法数据:

**请求 (小程序→设备)**
```
Start(0xAA) + Command(0x60) + Length + Payload + Checksum
```

Payload格式:
| 字段 | 长度 | 说明 |
|------|------|------|
| R/W | 1 | 0x01=读, 0x02=写 |
| 总包数 | 1 | 总共多少包 |
| 包序号 | 1 | 当前包序号 (0开始) |
| 数据长度 | 2 | 本包数据长度 |
| 疗法数据 | N | 疗法数据内容 |

**响应 (设备→小程序)**
```
Start(0xAA) + Command(0x60) + Length(0x02) + Status + 包序号 + Checksum
```

状态: 0x01=成功, 0x00=失败

#### 3.5.6 丢包重传机制

与固件升级类似的ARQ机制:

| 参数 | 值 |
|------|-----|
| 最大重试 | 3次 |
| ACK超时 | 100ms |
| 单包大小 | ≤ 240字节 |

#### 3.5.7 页面状态

| 状态 | 说明 |
|------|------|
| idle | 初始状态 |
| fetching | 获取疗法列表中 |
| list_ready | 疗法列表就绪 |
| downloading | 下载疗法文件中 |
| transferring | 传输到设备中 |
| success | 下载成功 |
| failed | 下载失败 |

#### 3.5.8 疗法页面设计

```
┌────────────────────────┐
│  < 返回    疗法管理   │
├────────────────────────┤
│  当前设备疗法: 5个    │
│                        │
│  ┌─────────────────┐  │
│  │ 疗法01  ✓ 已下载 │  │
│  │ 疗法02  ✓ 已下载 │  │
│  │ 疗法03  ✓ 已下载 │  │
│  │ 疗法04  ✓ 已下载 │  │
│  │ 疗法05  ✓ 已下载 │  │
│  └─────────────────┘  │
│                        │
│  ────── 可下载 ────── │
│                        │
│  ┌─────────────────┐  │
│  │ 疗法06  [下载]  │  │
│  │ 疗法07  [下载]  │  │
│  │ 疗法08  [下载]  │  │
│  └─────────────────┘  │
│                        │
│  [刷新列表]            │
└────────────────────────┘

┌────────────────────────┐
│  < 返回    下载疗法   │
├────────────────────────┤
│  疗法06详情:          │
│                        │
│  名称: 肩周炎理疗     │
│  通道: 2通道          │
│  类型: TENS调制       │
│  时长: 30分钟         │
│                        │
│  ┌─────────────────┐  │
│  │ ████████░░ 65% │  │
│  │ 传输中...       │  │
│  │ 包: 52/80       │  │
│  └─────────────────┘  │
│                        │
│  [取消下载]            │
└────────────────────────┘
```

---

## 4. 页面设计

### 4.1 首页 (设备发现)

```
┌────────────────────────┐
│     pTens 设备       │
├────────────────────────┤
│  ┌──────┐ ┌──────┐   │
│  │ 设备1│ │ 设备2│   │
│  │ RSSI │ │ RSSI │   │
│  └──────┘ └──────┘   │
│                        │
│  ┌──────┐             │
│  │ 设备3│             │
│  │ RSSI │             │
│  └──────┘             │
│                        │
│  [扫描中...  ]        │
│                        │
│  [设备设置]          │
└────────────────────────┘
```

### 4.2 设备控制页

```
┌────────────────────────┐
│  < 返回    设备控制   │
├────────────────────────┤
│  ┌─────────────────┐  │
│  │   TENS 状态     │  │
│  │   ○ 运行中     │  │
│  │   处方: 01     │  │
│  │   时间: 20:00  │  │
│  └─────────────────┘  │
│                        │
│  强度: ████████░░ 20 │
│  ◀ ═══════════▶      │
│                        │
│  [处方选择] [定时]    │
│                        │
│  [暂停/开始] [停止]   │
│                        │
│  🔋 85%  📶 已连接   │
└────────────────────────┘
```

### 4.3 固件升级页

```
┌────────────────────────┐
│  < 返回    固件升级   │
├────────────────────────┤
│  当前版本: V1.00.03   │
│                        │
│  请选择固件版本:      │
│  ┌─────────────────┐  │
│  │ ▼ V1.00.05  ▼  │  │
│  │   V1.00.04      │  │
│  │   V1.00.03      │  │
│  │   V1.00.02      │  │
│  └─────────────────┘  │
│                        │
│  固件信息:             │
│  版本: V1.00.05        │
│  大小: 60KB            │
│  日期: 2024-01-15      │
│                        │
│  更新内容:             │
│  - 优化蓝牙连接        │
│  - 修复已知问题        │
│                        │
│  ┌─────────────────┐  │
│  │ ████████░░ 80% │  │
│  │ 下载固件中...  │  │
│  └─────────────────┘  │
│                        │
│  ┌─────────────────┐  │
│  │ ████████░░ 40% │  │
│  │ 升级传输中...  │  │
│  └─────────────────┘  │
│                        │
│  [下载并升级]         │
└────────────────────────┘
```

---

## 5. 数据结构

### 5.1 设备信息

```javascript
deviceInfo: {
  deviceId: string,        // BLE设备ID
  deviceName: string,      // 设备名称
  rssi: number,           // 信号强度
  services: [],           // 服务列表
  
  // 协议解析
  version: string,        // 固件版本
  hwVersion: string,      // 硬件版本
  battery: number,        // 电量 0-100
  state: number,          // 运行状态
  runTime: number,        // 剩余时间(秒)
  strength: number,        // 当前强度
  prescId: number,        // 当前处方ID
}
```

### 5.2 固件信息

```javascript
firmwareInfo: {
  version: string,        // 版本号 "V1.00.05"
  versionCode: number,    // 数值版本 105
  url: string,           // 下载URL
  size: number,          // 文件大小
  md5: string,           // MD5校验
  changelog: string,     // 更新日志
  releaseDate: string,   // 发布日期
}
```

### 5.3 升级任务

```javascript
upgradeTask: {
  status: 'idle'|'checking'|'found'|'downloading'|'ready'|'upgrading'|'success'|'failed',
  progress: number,       // 0-100
  currentPkg: number,     // 当前包序号
  totalPkg: number,       // 总包数
  errorMsg: string,      // 错误信息
}
```

---

## 6. 接口设计

### 6.1 服务器API

#### 6.1.1 固件服务器配置

| 参数 | 值 |
|------|-----|
| 地址 | http://134.175.125.61:88/device/firmware |
| 认证方式 | HTTP Basic Auth |
| 用户名 | root |
| 密码 | miniTENS220528 |

#### 6.1.2 获取固件列表

```
GET /device/firmware

Authorization: Basic base64(root:miniTENS220528)

Response: HTML页面 (目录列表)
或 JSON格式固件列表

固件文件命名规范:
pTens_v{x.x.xx}.bin

例如:
pTens_v1.00.05.bin
pTens_v1.00.04.bin
pTens_v1.00.03.bin
```

#### 6.1.3 获取固件文件

```
GET /device/firmware/pTens_v1.00.05.bin

Authorization: Basic base64(root:miniTENS220528)

Response: Binary文件流 (application/octet-stream)
```

#### 6.1.4 获取最新固件信息 (备选)

```
GET /api/firmware/latest

Response:
{
  "version": "V1.00.05",
  "versionCode": 105,
  "url": "http://134.175.125.61:88/device/firmware/pTens_v1.00.05.bin",
  "size": 61440,
  "md5": "abc123...",
  "changelog": "更新内容...",
  "releaseDate": "2024-01-01"
}
```

#### 6.1.5 获取固件下载 (备选)

```
GET /api/firmware/download?version=V1.00.05

Response: Binary文件流
```

### 6.2 HTTP Basic Auth 实现

#### 6.2.1 认证Header

```javascript
// 用户名:密码 = root:miniTENS220528
// Base64编码后: cm9vdDptaW5pVEVOUyAyMjA1Mjg=

const authHeader = {
  'Authorization': 'Basic cm9vdDptaW5pVEVOUyAyMjA1Mjg='
}
```

#### 6.2.2 请求示例

```javascript
wx.request({
  url: 'http://134.175.125.61:88/device/firmware',
  method: 'GET',
  header: {
    'Authorization': 'Basic ' + wx.getStorageSync('firmwareAuth')
  },
  success: (res) => {
    // 解析固件列表
    // 服务器可能返回HTML目录或JSON
  }
})
```

#### 6.2.3 固件列表解析

服务器返回HTML目录页面，需要解析提取固件文件名:

```javascript
// 解析HTML中的固件链接
function parseFirmwareList(html) {
  const regex = /pTens_v(\d+\.\d+\.\d+)\.bin/g;
  const list = [];
  let match;
  while ((match = regex.exec(html)) !== null) {
    list.push({
      filename: match[0],
      version: match[1],
      versionCode: parseVersionCode(match[1])
    });
  }
  // 按版本号排序 (降序)
  return list.sort((a, b) => b.versionCode - a.versionCode);
}

function parseVersionCode(version) {
  const parts = version.split('.');
  return parseInt(parts[0]) * 100 + parseInt(parts[1]) * 10 + parseInt(parts[2]);
}
```

### 6.3 固件下载实现

```javascript
// 下载固件到本地
wx.downloadFile({
  url: 'http://134.175.125.61:88/device/firmware/pTens_v1.00.05.bin',
  header: {
    'Authorization': 'Basic ' + wx.getStorageSync('firmwareAuth')
  },
  success: (res) => {
    // 保存到本地文件系统
    const fs = wx.getFileSystemManager();
    fs.saveFile({
      tempFilePath: res.tempFilePath,
      success: (saveRes) => {
        // 保存路径: saveRes.savedFilePath
      }
    });
  }
})
```

### 6.4 疗法服务器API

#### 6.4.1 获取疗法列表

```
GET /device/therapy

Authorization: Basic base64(root:miniTENS220528)

Response: HTML页面 (目录列表)
或 JSON格式

疗法文件命名规范:
therapy_{id}_{name}.bin
therapy_01_肩周炎.bin
therapy_02_颈椎病.bin
```

#### 6.4.2 获取疗法文件

```
GET /device/therapy/therapy_01_肩周炎.bin

Authorization: Basic base64(root:miniTENS220528)

Response: Binary文件流
```

#### 6.4.3 疗法下载实现

```javascript
// 1. 获取疗法列表
async function fetchTherapyList() {
  return new Promise((resolve, reject) => {
    wx.request({
      url: 'http://134.175.125.61:88/device/therapy',
      method: 'GET',
      header: {
        'Authorization': 'Basic cm9vdDptaW5pVEVOUyAyMjA1Mjg='
      },
      success: (res) => {
        // 解析HTML目录
        const list = parseTherapyList(res.data);
        resolve(list);
      },
      fail: reject
    });
  });
}

// 解析疗法列表
function parseTherapyList(html) {
  const regex = /therapy_(\d+)_([^.]+)\.bin/g;
  const list = [];
  let match;
  while ((match = regex.exec(html)) !== null) {
    list.push({
      filename: match[0],
      id: parseInt(match[1]),
      name: decodeURIComponent(match[2])
    });
  }
  return list.sort((a, b) => a.id - b.id);
}

// 2. 下载疗法文件
async function downloadTherapy(therapy) {
  return new Promise((resolve, reject) => {
    wx.downloadFile({
      url: `http://134.175.125.61:88/device/therapy/${therapy.filename}`,
      header: {
        'Authorization': 'Basic cm9vdDptaW5pVEVOUyAyMjA1Mjg='
      },
      success: (res) => {
        // 读取文件内容
        const fs = wx.getFileSystemManager();
        fs.readFile({
          filePath: res.tempFilePath,
          success: (readRes) => {
            resolve(readRes.data);
          },
          fail: reject
        });
      },
      fail: reject
    });
  });
}

// 3. 解析疗法数据结构
function parseTherapyData(buffer) {
  const view = new DataView(buffer);
  let offset = 0;
  
  // 读取疗法数量
  const therapyCount = view.getUint8(offset + 6);
  offset = 11;
  
  const therapies = [];
  for (let i = 0; i < therapyCount; i++) {
    const id = view.getUint32(offset);
    offset += 4;
    
    const len = view.getUint16(offset);
    offset += 2;
    
    const chann = view.getUint8(offset);
    offset += 1;
    
    // 疗法数据
    const data = new Uint8Array(buffer, offset, len - 7);
    offset += len - 7;
    
    therapies.push({ id, chann, data });
  }
  
  return therapies;
}

// 4. 传输到设备 (使用DATACMD_THERAPY)
async function uploadTherapyToDevice(therapyData, onProgress) {
  const PACKET_SIZE = 240;
  const totalPkg = Math.ceil(therapyData.length / PACKET_SIZE);
  
  for (let i = 0; i < totalPkg; i++) {
    const offset = i * PACKET_SIZE;
    const chunk = therapyData.slice(offset, offset + PACKET_SIZE);
    
    // 构建数据包
    const packet = [
      0x02,  // 写
      totalPkg,
      i,
      chunk.length & 0xFF,
      (chunk.length >> 8) & 0xFF,
      ...chunk
    ];
    
    // 发送并等待ACK
    const success = await sendPacketWithRetry(packet);
    if (!success) {
      throw new Error(`疗法包 ${i} 传输失败`);
    }
    
    onProgress(i + 1, totalPkg);
  }
  
  return true;
}
```

### 6.5 数据结构定义

#### 6.5.1 疗法列表项

```javascript
therapyItem: {
  filename: string,      // 文件名
  id: number,            // 疗法ID
  name: string,          // 疗法名称
  downloaded: boolean,    // 是否已下载
  localPath: string       // 本地路径
}
```

#### 6.5.2 疗法数据

```javascript
therapyData: {
  id: number,            // 疗法ID
  chann: number,         // 通道数
  data: Uint8Array,      // 疗法波形数据
  size: number           // 数据大小
}
```

---

## 7. 错误处理

### 7.1 蓝牙错误

| 错误码 | 说明 | 处理 |
|--------|------|------|
| -1 | 未开启蓝牙 | 提示开启蓝牙 |
| -2 | 未授权蓝牙 | 跳转授权 |
| -3 | 搜索超时 | 重新搜索 |
| -4 | 连接超时 | 重试连接 |
| -5 | 断开连接 | 重新连接 |

### 7.2 升级错误

| 错误码 | 说明 | 处理 |
|--------|------|------|
| 1001 | 下载失败 | 重试下载 |
| 1002 | 固件校验失败 | 重新下载 |
| 1003 | 连接断开 | 重连设备 |
| 1004 | 传输超时 | 重试当前包 |
| 1005 | 设备无响应 | 重试升级 |
| 1006 | 版本校验失败 | 反馈问题 |

### 7.3 疗法下载错误

| 错误码 | 说明 | 处理 |
|--------|------|------|
| 2001 | 获取列表失败 | 重试请求 |
| 2002 | 疗法文件不存在 | 提示用户 |
| 2003 | 下载失败 | 重试下载 |
| 2004 | 疗法数据解析失败 | 提示文件损坏 |
| 2005 | 设备连接断开 | 重连设备 |
| 2006 | 传输到设备失败 | 重试传输 |
| 2007 | 设备写入失败 | 提示设备错误 |
| 2008 | 疗法数量超限 | 提示删除旧疗法 |

---

## 8. 性能考虑

### 8.1 BLE传输优化

- 使用notify代替indicate (更快)
- 批量发送数据减少往返
- 超时重试机制 (3次)
- 进度实时更新

### 8.2 电量优化

- 升级过程保持屏幕常亮
- 后台传输使用Worklet
- 完成后及时断开连接

---

## 9. 版本历史

| 版本 | 日期 | 修改内容 |
|------|------|----------|
| V1.0 | 2024 | 初始版本 |
