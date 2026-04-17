**多通道主机通信协议**

**（技术保密文件，仅供授权人员使用）**

**广州健领信息科技有限公司**

<span id="_Toc28081" class="anchor"></span>目 录

[目 录 [2](#_Toc28081)](#_Toc28081)

[版本变更 [4](#_Toc8330)](#_Toc8330)

[名词解释 [5](#_Toc24064)](#_Toc24064)

[1. 通信说明 [6](#通信说明)](#通信说明)

[1.1 通讯链路层(Link layer) [6](#通讯链路层link-layer)](#通讯链路层link-layer)

[2. 协议格式 [7](#协议格式)](#协议格式)

[2.1 Payload业务数据 [7](#payload业务数据)](#payload业务数据)

[2.2 系统命令 [9](#系统命令)](#系统命令)

[2.2.1 握手 [9](#握手)](#握手)

[2.2.2 获取设备信息 [9](#获取设备信息)](#获取设备信息)

[2.2.3 获取设备版本 [10](#获取设备版本)](#获取设备版本)

[2.2.4 心跳包 [10](#心跳包)](#心跳包)

[2.2.5 故障码 [10](#故障码)](#故障码)

[2.2.6 固件升级 [11](#固件升级)](#固件升级)

[2.3 控制命令 [11](#控制命令)](#控制命令)

[2.3.1 输出时间 [11](#输出时间)](#输出时间)

[2.3.2 输出控制 [12](#输出控制)](#输出控制)

[2.3.3 输出强度 [12](#输出强度)](#输出强度)

[2.3.4 通道设置 [12](#疗法设置)](#疗法设置)

[2.4 数据命令 [12](#数据命令)](#数据命令)

[2.4.1 疗法数据传输 [13](#疗法数据传输)](#疗法数据传输)

[3. 附录 [14](#_Toc15694)](#_Toc15694)

[3.1 数据加密规则——未定，供讨论 [14](#_Toc4461)](#_Toc4461)

<span id="_Toc8330" class="anchor"></span>版本变更

<table>
<colgroup>
<col style="width: 10%" />
<col style="width: 70%" />
<col style="width: 18%" />
</colgroup>
<tbody>
<tr>
<td style="text-align: center;"><strong>版本</strong></td>
<td style="text-align: center;"><strong>主要修订内容说明</strong></td>
<td style="text-align: center;"><strong>修订人/日期</strong></td>
</tr>
<tr>
<td style="text-align: center;">V1.0a~g</td>
<td style="text-align: left;">核心围绕TENS制定协议，完成了原型机（痛经腰带等）开发和系统核心算法验证，未完成产品样机</td>
<td style="text-align: center;">刘锡锦、林展2022-10-11</td>
</tr>
<tr>
<td style="text-align: center;">V1.1a</td>
<td style="text-align: left;">增加指令分集协议：系统指令、控制指令和数据指令等</td>
<td style="text-align: center;">李晓锋2024-03-22</td>
</tr>
<tr>
<td rowspan="3" style="text-align: center;">V2.0a</td>
<td style="text-align: left;"><p>在V1.0基础上进行了重大升版：</p>
<ol type="1">
<li><p>完善指令的分层：系统指令、控制指令和数据指令及详细定义</p></li>
<li><p>新增了疗法思路，支持控制所有物理通道的治疗处方</p></li>
<li><p>完善所有物理治疗采用统一定义</p></li>
</ol></td>
<td style="text-align: center;"><p>刘锡锦</p>
<p>2024-04-30</p></td>
</tr>
<tr>
<td style="text-align: left;">采用公司模板重新排版</td>
<td style="text-align: center;"><p>李晓锋</p>
<p>2024-05-01</p></td>
</tr>
<tr>
<td style="text-align: left;">修正部分错误，补充完善大部分描述（协议用词一一对应），排版增强可读性</td>
<td style="text-align: center;"><p>林展</p>
<p>2024-05-02</p></td>
</tr>
<tr>
<td style="text-align: center;"></td>
<td style="text-align: left;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td colspan="3" style="text-align: left;"><p>注：</p>
<ol type="1">
<li><p>红色字体代表最后一次修订内容。灰色字体表示未确定内容。</p></li>
<li><p>升版规则：协议（如V1.1a）一旦在样板中使用（固件），再修改则升级为V1.1b；一旦生产提供给客户则为正式版去掉字母后缀如V1.1；重大功能升级（如较大的重构，医疗器械涉及注册变更）则升级大版本如V2.0a。</p></li>
</ol></td>
</tr>
</tbody>
</table>

<span id="_Toc24064" class="anchor"></span>名词解释

|          |                                           |          |
|:--------:|:-----------------------------------------:|:--------:|
| **名词** |                 **解释**                  | **备注** |
|  1Byte   |              8位（8Bit）数据              |          |
| Tens1~n  |     电刺激输出通道序号，最大支持n通道     |          |
|  上位机  | 电脑PC或手机端App、小程序，也可指主控方。 |          |
|  下位机  |        多通道设备，也可指受控方。         |          |
|   疗法   |   一个或多个物理通道的联合输出治疗方法    |          |

# 通信说明

## 通讯链路层(Link layer)

1.  **通讯端点：**

- 上位机：PC/App应用终端

- 下位机：MiniTens设备

2.  **通讯接口：可使用通讯接口包括“蓝牙”“串口(或模拟串口方式)”“WIFI透传”等通信：**

- 上位机(PC或移动终端)通过通过蓝牙BLE或RS232串口与下位机(MiniTens主机)通信

- 通信模块：具备BLE/SPP(串口透传)双模蓝牙模块

- 串口波特率：115200 bps(数据位8位；停止位1位；无校验位)

3.  **通讯接口工作模式：**

- 蓝牙：4.1以上，下位机提供可被搜索可被连接，上位机必须搜索下位机建立连接，蓝牙设备名：“MiniTens+出厂编码”，如需配对密码，默认使用主机类型码如：EEHV

- 串行口： 波特率115200，1位起始位，8位数据位，无校验位，1位停止位

# 协议格式

<table>
<colgroup>
<col style="width: 12%" />
<col style="width: 12%" />
<col style="width: 12%" />
<col style="width: 12%" />
<col style="width: 12%" />
<col style="width: 12%" />
<col style="width: 12%" />
<col style="width: 12%" />
</colgroup>
<tbody>
<tr>
<td style="text-align: center;"><strong>协议头</strong></td>
<td style="text-align: center;"><strong>长度</strong></td>
<td style="text-align: center;"><strong>版本</strong></td>
<td style="text-align: center;"><strong>序列号</strong></td>
<td style="text-align: center;"><strong>类型</strong></td>
<td style="text-align: center;"><strong>Payload</strong></td>
<td style="text-align: center;"><strong>校验码</strong></td>
<td style="text-align: center;"><strong>协议尾</strong></td>
</tr>
<tr>
<td style="text-align: center;">1字节</td>
<td style="text-align: center;">2字节</td>
<td style="text-align: center;">1字节</td>
<td style="text-align: center;">1字节</td>
<td style="text-align: center;">1字节</td>
<td style="text-align: center;">n字节</td>
<td style="text-align: center;">1字节</td>
<td style="text-align: center;">1字节</td>
</tr>
<tr>
<td colspan="8" style="text-align: left;"><p>协议头：0xAA</p>
<p>长度：除协议头、长度、协议尾外的所有字节总，最大长度不超过2048Byte</p>
<p>版本：本通信协议的版本号，1字节，高4位对应V0.x~9.x，低4位对应Vx.0~Vx.9</p>
<p>序列号：通信帧序列号，自增1</p>
<p>类型：数据帧类型，0x01-上位机数据帧，0x02-下位机数据帧，0x00-回应数据帧</p>
<p>Payload：业务数据</p>
<p>校验码：除协议头、检验、协议尾外，其它所有字节和序列异或计算所得值</p>
<p>协议尾：0x55</p></td>
</tr>
</tbody>
</table>

校验码实例：

```c
unsigned char checksum(unsigned char *data, unsigned int len)
{
    unsigned int i;
    unsigned char crc = 0x55;

    for (i = 0; i < len; i++) {
        crc ^= data[i];
    }

    return crc;
}
```

## Payload业务数据

|            |          |                  |          |
|:----------:|:--------:|:----------------:|:--------:|
| **命令组** | **命令** | **读/写/返回值** | **参数** |
|   1字节    |  1字节   |      1字节       |  n字节   |

读/写/返回值：0x01-读，0x02-写，返回值：0x00-正确，其它有错误

<table>
<colgroup>
<col style="width: 12%" />
<col style="width: 14%" />
<col style="width: 12%" />
<col style="width: 12%" />
<col style="width: 48%" />
</colgroup>
<tbody>
<tr>
<td style="text-align: center;"><strong>命令组</strong></td>
<td style="text-align: center;"><strong>命令组编号</strong></td>
<td style="text-align: center;"><strong>命令</strong></td>
<td style="text-align: center;"><strong>命令编号</strong></td>
<td style="text-align: center;"><strong>说明</strong></td>
</tr>
<tr>
<td rowspan="6" style="text-align: center;">系统命令</td>
<td rowspan="6" style="text-align: center;">0x01</td>
<td style="text-align: center;">握手</td>
<td style="text-align: center;">0x01</td>
<td style="text-align: left;">上位机与下位机握手</td>
</tr>
<tr>
<td style="text-align: center;">设备信息</td>
<td style="text-align: center;">0x02</td>
<td style="text-align: left;">获取设备信息</td>
</tr>
<tr>
<td style="text-align: center;">版本</td>
<td style="text-align: center;">0x03</td>
<td style="text-align: left;">获取设备硬件版本、固件版本</td>
</tr>
<tr>
<td style="text-align: center;">心跳</td>
<td style="text-align: center;">0x04</td>
<td style="text-align: left;">下位机返回设备状态、电量等</td>
</tr>
<tr>
<td style="text-align: center;">故障码</td>
<td style="text-align: center;">0x05</td>
<td style="text-align: left;">下位机上报故障码</td>
</tr>
<tr>
<td style="text-align: center;">固件升级</td>
<td style="text-align: center;">0x06</td>
<td style="text-align: left;">升级固件</td>
</tr>
<tr>
<td rowspan="4" style="text-align: center;">控制命令</td>
<td rowspan="4" style="text-align: center;">0x02</td>
<td style="text-align: center;">输出时间</td>
<td style="text-align: center;">0x01</td>
<td style="text-align: left;">设置/读取输出时间</td>
</tr>
<tr>
<td style="text-align: center;">输出控制</td>
<td style="text-align: center;">0x02</td>
<td style="text-align: left;">启动/停止/暂停/重启/关机</td>
</tr>
<tr>
<td style="text-align: center;">输出强度</td>
<td style="text-align: center;">0x03</td>
<td style="text-align: left;">设置/读取输出强度</td>
</tr>
<tr>
<td style="text-align: center;">通道设置</td>
<td style="text-align: center;">0x04</td>
<td style="text-align: left;">输出通道设置</td>
</tr>
<tr>
<td style="text-align: center;">数据命令</td>
<td style="text-align: center;">0x03</td>
<td style="text-align: center;">处方数据</td>
<td style="text-align: center;">0x01</td>
<td style="text-align: left;">上位机向下位机下发处方数据</td>
</tr>
</tbody>
</table>

## 系统命令

### 握手

|        |            |          |                  |                   |
|:------:|:----------:|:--------:|:----------------:|:-----------------:|
|        | **命令组** | **命令** | **读/写/返回值** | **参数（2字节）** |
| 上位机 |    0x01    |   0x01   |       0x01       |     MARK字节      |
| 下位机 |    0x01    |   0x01   |       0x00       |     MARK反数      |

- 只有握手成功后设备才进入通信状态

- MARK为随机数2字节（增加一个字节，方便以后扩展加密用）

### 获取设备信息

|  |  |  |  |  |
|:--:|:--:|:--:|:--:|:--:|
|  | **命令组** | **命令** | **读/写/返回值** | **参数** |
| 上位机 | 0x01 | 0x02 | 0x01 | 无 |
| 下位机 | 0x01 | 0x02 | 0x00 | 设备类型（1字节）、通道描述、设备描述（最大30字节） |

**参数**

<table>
<colgroup>
<col style="width: 12%" />
<col style="width: 12%" />
<col style="width: 16%" />
<col style="width: 15%" />
<col style="width: 15%" />
<col style="width: 14%" />
<col style="width: 13%" />
</colgroup>
<tbody>
<tr>
<td rowspan="2" style="text-align: center;"><strong>设备类型</strong></td>
<td colspan="5" style="text-align: center;"><strong>通道描述</strong></td>
<td rowspan="2" style="text-align: center;"><strong>设备描述</strong></td>
</tr>
<tr>
<td style="text-align: center;"><strong>总通道数</strong></td>
<td style="text-align: center;"><strong>通道1类型</strong></td>
<td style="text-align: center;"><strong>通道2类型</strong></td>
<td style="text-align: center;"><strong>通道3类型</strong></td>
<td style="text-align: center;"><strong>通道n类型</strong></td>
</tr>
<tr>
<td style="text-align: center;">0xXX</td>
<td style="text-align: center;">0x0n</td>
<td style="text-align: center;">0x01</td>
<td style="text-align: center;">0x01</td>
<td style="text-align: center;">0x02</td>
<td style="text-align: center;">0x03</td>
<td style="text-align: center;">字符串</td>
</tr>
</tbody>
</table>

- **设备类型**

|          |                  |          |
|:--------:|:----------------:|:--------:|
| **编号** |   **设备类型**   | **说明** |
|   0x01   | 中医馆通用理疗仪 |          |
|   0x02   |  家庭通用理疗仪  |          |
|   0x03   |  女用康美理疗仪  |          |
|   0x04   |  人流康复治疗仪  |          |
|   0x05   |    盆底治疗仪    |          |
|   0x06   |    痛经治疗仪    |          |
|    ……    |                  |          |

- **通道类型**

|          |              |          |
|:--------:|:------------:|:--------:|
| **编号** | **通道类型** | **说明** |
|   0x01   |     TENS     | Tens输出 |
|   0x02   |     加热     | 加热输出 |
|   0x03   |     振动     | 振动输出 |
|   0x04   |      光      |          |
|   0x05   |     超声     | 磁疗输出 |
|   0x06   |     磁疗     | 超声输出 |
|    ……    |              |          |

- **设备描述：产品型号+出厂编号+备用码**

<table style="width:100%;">
<colgroup>
<col style="width: 49%" />
<col style="width: 17%" />
<col style="width: 33%" />
</colgroup>
<tbody>
<tr>
<td style="text-align: center;"><strong>出厂编号</strong></td>
<td style="text-align: center;"><strong>产品型号</strong></td>
<td style="text-align: center;"><strong>备用码</strong></td>
</tr>
<tr>
<td style="text-align: center;"><p>共16字节，Pxxxxxxxx（8字节料号）-xxx（1字节年份后两位，1字节月，1字节日）-xx（2字节流水号）</p>
<p>例：P12160213-180502-10012</p></td>
<td style="text-align: center;"><p>共4字节</p>
<p>(待定)</p></td>
<td style="text-align: left;">16字节，待定</td>
</tr>
</tbody>
</table>

### 获取设备版本

<table style="width:100%;">
<colgroup>
<col style="width: 10%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 18%" />
<col style="width: 51%" />
</colgroup>
<tbody>
<tr>
<td style="text-align: center;"></td>
<td style="text-align: center;"><strong>命令组</strong></td>
<td style="text-align: center;"><strong>命令</strong></td>
<td style="text-align: center;"><strong>读/写/返回值</strong></td>
<td style="text-align: center;"><strong>参数</strong></td>
</tr>
<tr>
<td style="text-align: center;">上位机</td>
<td style="text-align: center;">0x01</td>
<td style="text-align: center;">0x03</td>
<td style="text-align: center;">0x01</td>
<td style="text-align: left;">无</td>
</tr>
<tr>
<td style="text-align: center;">下位机</td>
<td style="text-align: center;">0x01</td>
<td style="text-align: center;">0x03</td>
<td style="text-align: center;">0x00</td>
<td style="text-align: left;"><p>硬件版本（5字节）+固件版本（5字节）</p>
<p>版本说明：Vx.x-年月日</p>
<p>1字节位大版本序号V1.x~V9.x</p>
<p>1字节小版本Vx.0~Vx.9</p>
<p>3字节位为日期（1字节年份后两位，1字节月，1字节日）</p></td>
</tr>
</tbody>
</table>

### 心跳包

|  |  |  |  |  |
|:--:|:--:|:--:|:--:|:--:|
|  | **命令组** | **命令** | **读/写/返回值** | **参数** |
| 上位机（读） | 0x01 | 0x04 | 0x01 | 无 |
| 下位机（回） | 0x01 | 0x04 | 0x00 | 电量（1字节）、设备状态（1字节）、运行剩余时间（2字节） |

- 握手成功后上位机间隔3秒发一次心跳（也可随时发送查询）

- 上位机连续3次没有收到下位机回应，则认为设备离线，需要重新握手

- 下位机连续3个间隔没有收到上位机心跳命令则认为上位机离线，需要重新握手

### 故障码

|  |  |  |  |  |
|:--:|:--:|:--:|:--:|:--:|
|  | **命令组** | **命令** | **读/写/返回值** | **参数** |
| 下位机（读） | 0x01 | 0x05 | 0x02 | 故障码（1字节），故障描述（n字节） |

|            |                   |                                               |
|:----------:|:-----------------:|:---------------------------------------------:|
| **故障码** |   **故障描述**    |                   **说明**                    |
|    0x00    |       0x00        |                    无故障                     |
|    0x01    |  故障码（1字节）  | 系统异常（1自检、MCU电压异常、3设备温度过高） |
|    0x02    |  故障码（1字节）  |      电池故障（1断开、2短路、3充电故障）      |
|    0x03    |  故障码（1字节）  |          通信异常（1、串口、2蓝牙）           |
|    0x04    | 通道编号（1字节） |                   电极脱落                    |
|    0x05    | 通道编号（1字节） |                   电极短路                    |
|    0x06    | 通道编号（1字节） |                   温度过高                    |
|    0x07    | 通道编号（1字节） |                   温度过低                    |
|    0x08    | 通道编号（1字节） |              温度传感器自检错误               |
|            |                   |                                               |
|     ……     |                   |                                               |

### 固件升级

|  |  |  |  |  |
|:--:|:--:|:--:|:--:|:--:|
|  | **命令组** | **命令** | **读/写/返回值** | **参数** |
| 上位机（写） | 0x01 | 0x06 | 0x01 | 总包数（1字节）、当前包序号（1字节，自增）、本包数据长度（2字节，最大1024）、本包数据（固件BIN数据分包，最后一分包须含CRC16检验) |
| 下位机（回） | 0x01 | 0x06 | 0x00 | 当前包序号（1字节） |

固件BIN文件校验码示例（CRC16）：（参考www.ip33.com/crc.html，采用CRC-16/MODBUS，多项式公式x16 + x15 + x2 + 1，8005）

```c
unsigned char checksum(unsigned char *data, unsigned int len)
{
    unsigned int i;
    unsigned char crc = 0x55;

    for (i = 0; i < len; i++) {
        crc ^= data[i];
    }

    return crc;
}
```

## 控制命令

### 输出时间

<table>
<colgroup>
<col style="width: 20%" />
<col style="width: 10%" />
<col style="width: 9%" />
<col style="width: 18%" />
<col style="width: 40%" />
</colgroup>
<tbody>
<tr>
<td style="text-align: center;"></td>
<td style="text-align: center;"><strong>命令组</strong></td>
<td style="text-align: center;"><strong>命令</strong></td>
<td style="text-align: center;"><strong>读/写/返回值</strong></td>
<td style="text-align: center;"><strong>参数</strong></td>
</tr>
<tr>
<td style="text-align: center;">上位机（读）</td>
<td style="text-align: center;">0x02</td>
<td style="text-align: center;">0x01</td>
<td style="text-align: center;">0x01</td>
<td style="text-align: left;">无</td>
</tr>
<tr>
<td style="text-align: center;">下位机</td>
<td style="text-align: center;">0x02</td>
<td style="text-align: center;">0x01</td>
<td style="text-align: center;">0x00</td>
<td style="text-align: left;">输出时间（2字节）+剩余时间（2字节），单位：秒</td>
</tr>
<tr>
<td style="text-align: center;">上位机（写）</td>
<td style="text-align: center;">0x02</td>
<td style="text-align: center;">0x01</td>
<td style="text-align: center;">0x02</td>
<td style="text-align: left;"><p>输出时间（2字节）</p>
<p>单位：秒</p></td>
</tr>
<tr>
<td style="text-align: center;">下位机</td>
<td style="text-align: center;">0x02</td>
<td style="text-align: center;">0x01</td>
<td style="text-align: center;">0x00</td>
<td style="text-align: left;">输出时间（2字节）+剩余时间（2字节），单位：秒</td>
</tr>
<tr>
<td style="text-align: center;">下位机上报（写）</td>
<td style="text-align: center;">0x02</td>
<td style="text-align: center;">0x01</td>
<td style="text-align: center;">0x02</td>
<td style="text-align: left;">输出时间（2字节）+剩余时间（2字节），单位：秒</td>
</tr>
</tbody>
</table>

### 输出控制

|                  |            |          |                  |                   |
|:----------------:|:----------:|:--------:|:----------------:|:-----------------:|
|                  | **命令组** | **命令** | **读/写/返回值** |     **参数**      |
|   上位机（读）   |    0x02    |   0x02   |       0x01       |        无         |
|   下位机（回）   |    0x02    |   0x02   |       0x00       | 运行状态（1字节） |
|   上位机（写）   |    0x02    |   0x02   |       0x02       | 运行状态（1字节） |
|   下位机（回）   |    0x02    |   0x02   |       0x00       | 运行状态（1字节） |
| 上位机上报（写） |    0x02    |   0x02   |       0x02       | 运行状态（1字节） |

运行状态：0x01~启动、0x02~暂停、0x03~停止、0x04~重启、0x05~设备关机

### 输出强度

|  |  |  |  |  |
|:--:|:--:|:--:|:--:|:--:|
|  | **命令组** | **命令** | **读/写/返回值** | **参数** |
| 上位机（读） | 0x02 | 0x03 | 0x01 |  |
| 下位机（回） | 0x02 | 0x03 | 0x00 | 通道数（1字节）、通道1强度、通道2强度...通道n强度 |
| 上位机（写） | 0x02 | 0x03 | 0x02 | 通道数（1字节）、通道1强度、通道2强度...通道n强度 |
| 下位机（回） | 0x02 | 0x03 | 0x00 | 通道数（1字节）、通道1强度、通道2强度...通道n强度 |
| 下位机上报（写） | 0x02 | 0x03 | 0x02 | 通道数（1字节）、通道1强度、通道2强度...通道n强度 |

### 疗法设置

|              |            |          |                  |              |
|:------------:|:----------:|:--------:|:----------------:|:------------:|
|              | **命令组** | **命令** | **读/写/返回值** |   **参数**   |
| 上位机（读） |    0x02    |   0x04   |       0x01       |      无      |
| 下位机（回） |    0x02    |   0x04   |       0x00       | 疗法位置编号 |
| 上位机（写） |    0x02    |   0x04   |       0x02       | 疗法位置编号 |
| 下位机（回） |    0x02    |   0x04   |       0x00       | 疗法位置编号 |

## 数据命令

### 疗法数据传输

|  |  |  |  |  |
|:--:|:--:|:--:|:--:|:--:|
|  | **命令组** | **命令** | **读/写/返回值** | **参数** |
| 上位机（读） | 0x03 | 0x01 | 0x01 | 位置 |
| 下位机（回） | 0x03 | 0x01 | 0x00 | **总包数（1字节）、当前包编号（1字节）、当前包长度（2字节）、疗法数据** |
| 上位机（写） | 0x03 | 0x01 | 0x02 | **总包数、当前包编号、当前包长度（2字节）、疗法数据** |
| 下位机（回） | 0x03 | 0x01 | 0x00 | **当前包编号** |

- **疗法数据**

<table>
<colgroup>
<col style="width: 11%" />
<col style="width: 10%" />
<col style="width: 10%" />
<col style="width: 11%" />
<col style="width: 17%" />
<col style="width: 14%" />
<col style="width: 17%" />
<col style="width: 6%" />
</colgroup>
<tbody>
<tr>
<td style="text-align: center;"><strong>疗法编号</strong></td>
<td style="text-align: center;"><strong>数据长度</strong></td>
<td style="text-align: center;"><strong>通道数</strong></td>
<td style="text-align: center;"><strong>通道1类型</strong></td>
<td style="text-align: center;"><strong>通道1处方数据</strong></td>
<td style="text-align: center;"><strong>通道2类型</strong></td>
<td style="text-align: center;"><strong>通道2处方数据</strong></td>
<td style="text-align: center;"><strong>…</strong></td>
</tr>
<tr>
<td style="text-align: center;"><p>4字节</p>
<p>（对应处方数据库的唯一编号）</p></td>
<td style="text-align: center;">2字节</td>
<td style="text-align: center;">1字节</td>
<td style="text-align: center;">1字节</td>
<td style="text-align: center;">n字节</td>
<td style="text-align: center;">1字节</td>
<td style="text-align: center;">n字节</td>
<td style="text-align: center;"></td>
</tr>
</tbody>
</table>

- **通道类型：同上**

- **处方描述欠缺，执行逻辑欠缺，处方描述会将位置也作为参数，但目前无处方位置定义**

- **010107005805012a00000510F419a009c409c4000014F419a0004200420000ffF419a000c800c8000063F419a0003200320000c8F419a000640064000032021300000310F20fa00064006400000aF219a000fa00fa000032F219a0003200320000c6032000000510F40aa009c409c400001eF40aa0004200420000ffF40aa000c800c8000063F40aa0003200320000ffF40aa00064006400005a040c00000510F219a003e803e800000fF219a000320032000050F219a009c409c400000aF219a000320032000050F219a001f401f4000014052000000510F20aa009c409c400000aF20aa0004200420000c0F20aa000640064000080F20aa000c800c8000040F20aa0003200320000ff3b**

  **疗法数据包结构**

  **\[数据包长度(4字节)\] \[疗法数量(1字节)\] \[疗法1数据\] \[疗法2数据\]...**

  **疗法数据结构**

  **\[疗法编号(4字节)\] \[数据长度(2字节)\] \[通道数(1字节)\] \[通道1类型(1字节)\] \[通道1处方数据\] \[通道2类型\] \[通道2处方数据\]...**

  **TENS处方数据结构**

  **\[数据长度(2字节)\] \[处方编号(2字节)\] \[波群数量(1字节)\] \[波群1序号(1字节)\] \[波群循环次数(1字节)\] \[波群1间隔时间(2字节)\] \[波群1数据\]...**

### Tens处方描述数据格式

处方有两种格式：①基础波形处方；②音乐包络处方。数据格式分别定义如下：

1)  **基础波形处方数据格式**

    ![](media/image1.emf)

    刺激处方=波群是波形+载波的各种组合

- K1~Kn为波形的循环次数(个数)

- M1~Mn为波群的循环次数(个数)

  1.  **基础波形描述数据**

      **格式为：波形编号(1Byte)+波形描述数据(nByte)**

<table>
<colgroup>
<col style="width: 10%" />
<col style="width: 8%" />
<col style="width: 67%" />
<col style="width: 13%" />
</colgroup>
<tbody>
<tr>
<td><strong>名称</strong></td>
<td><strong>波形编号</strong></td>
<td><strong>波形描述数据</strong></td>
<td><strong>总字节数</strong>Byte</td>
</tr>
<tr>
<td>间隔波</td>
<td>0xFF</td>
<td>暂停输出时间2Byte，单位0.1ms，范围0.1mS~6.5532S</td>
<td>1+2=3</td>
</tr>
<tr>
<td>方波</td>
<td>0xF2</td>
<td>载波频率1Byte+调制深度&amp;抖动比例1Byte+高电平时间2Byte(╳0.1ms)+低电平时间2Byte(╳0.1ms)+停止时间2Byte(╳0.1ms)+循环次数1Byte</td>
<td>1+9=10</td>
</tr>
<tr>
<td>梯形波</td>
<td>0xF3</td>
<td>载波频率1Byte+调制深度&amp;抖动比例1Byte+上升时间2Byte(╳0.1ms)+高电平时间2Byte(╳0.1ms)+下降时间2Byte(╳0.1ms)+停止时间2Byte(╳0.1ms)+循环次数1Byte</td>
<td>1+11=12</td>
</tr>
<tr>
<td><p>三角波</p>
<p>(锯齿波)</p></td>
<td>0xF4</td>
<td>载波频率1Byte+调制深度&amp;抖动比例1Byte+上升时间2Byte(╳0.1ms)+下降时间2Byte(╳0.1ms)+停止时间2Byte(╳0.1ms)+循环次数1Byte</td>
<td>1+9=10</td>
</tr>
<tr>
<td>球形波</td>
<td>0xF5</td>
<td>载波频率1Byte+调制深度&amp;抖动比例1Byte+周期时间2Byte(╳0.1ms)+停止时间2Byte(╳0.1ms)+循环次数1Byte</td>
<td>1+7=8</td>
</tr>
<tr>
<td>正弦波</td>
<td>0xF6</td>
<td>载波频率1Byte+调制深度&amp;抖动比例1Byte+周期时间2Byte(╳0.1ms)+停止时间2Byte(╳0.1ms)+循环次数1Byte</td>
<td>1+7=8</td>
</tr>
<tr>
<td>指数波</td>
<td>0xF7</td>
<td>载波频率1Byte+调制深度&amp;抖动比例1Byte+周期时间2Byte(╳0.1ms)+停止时间2Byte(╳0.1ms)+循环次数1Byte</td>
<td>1+7=8</td>
</tr>
<tr>
<td>对数波</td>
<td>0xF8</td>
<td>载波频率1Byte+调制深度&amp;抖动比例1Byte+周期时间2Byte(╳0.1ms)+停止时间2Byte(╳0.1ms)+循环次数1Byte</td>
<td>1+7=8</td>
</tr>
<tr>
<td><p>手绘波1</p>
<p>~</p>
<p>手绘波15</p></td>
<td><p>0xE1</p>
<p>~</p>
<p>0xEF</p></td>
<td><p><strong>数据生成原理：</strong>通过上位机处方编辑工具可手绘任意波形，再采样转为200Byte的数组，再由中频载波调制(斩波)作为治疗波输出。手绘波数据由上位机生成下传给下位机存储。</p>
<p><strong>数组输出格式：</strong>载波频率1Byte+调制深度1Byte+周期时间2Byte（╳0.1ms）+停止时间2Byte(╳0.1ms)+循环次数1Byte</p></td>
<td>1+200=201</td>
</tr>
<tr>
<td colspan="4"><ol>
<li><p><strong>波形编号：</strong>1Byte，即波形ID号，用于识别和存储波形数据</p></li>
<li><p><strong>载波频率：</strong>1Byte╳100Hz，对应100Hz~25500Hz；设为0x00则关闭载波。举例“0x0A”则载波频率为10╳100Hz=1KHz。</p></li>
<li><p><strong>调制深度&amp;抖动比例：</strong>1Byte，高四位和低四位分别定义如下：</p></li>
</ol>
<ol type="a">
<li><p><strong>高4位：</strong>为调制波包络的“调制深度”，0~A对应范围为0%、10%、20%、30%、40%、50%、60%、70%、80%、90%、100%（默认100%）；</p></li>
<li><p><strong>低4位：</strong>为本波形循环输出（含停止时间）次数的<strong>抖动比例</strong>，1~A对应范围为10%~100%（步长10%）变量，默认0%（固定次数）。假设循环次数为10次本值设置为2，则随机次数为10*（1<u>+</u>20%）=8~12次。——此参数利于降低耐药性</p>
<p><strong>举例：</strong>假设调制深度设置为80%，抖动比例设置为20%（假设波形“循环次数”为10次），则本Byte数值为：0x82。</p></li>
</ol>
<ol start="4">
<li><p><strong>周期时间：</strong>2Byte，即波长时间（单位步长0.1ms），周期时间T为频率f的倒数：T=1/f。</p></li>
<li><p><strong>停止时间：</strong>2Byte，本波形暂停输出的时间（单位步长0.1ms）</p></li>
<li><p><strong>循环次数：</strong>1Byte，本波形循环输出（含停止时间）的次数（上位机可转为“波形输出时间”，每单位0.1s）</p></li>
<li><p><strong>周期时间：</strong>2Byte，一个波形周期T时长，单位0.1ms，范围0.1mS~6.5532S。</p></li>
</ol></td>
</tr>
</tbody>
</table>

1.  **波群描述数据：**

    各种基础波形的集合，通常表达一个**“动作”（手法）**，如“敲”“捏”“推”“揉捏”“麻痹”等，波群格式如下：

<table>
<colgroup>
<col style="width: 18%" />
<col style="width: 21%" />
<col style="width: 59%" />
</colgroup>
<tbody>
<tr>
<td><strong>波形数量</strong></td>
<td><strong>幅值模式&amp;抖动比例</strong></td>
<td><strong>波形描述数据(不超过1KHz)</strong></td>
</tr>
<tr>
<td style="text-align: center;">1Byte</td>
<td style="text-align: center;">1Byte</td>
<td style="text-align: center;">波形1(nByte)+波形2(nByte)+波形n(nByte)</td>
</tr>
<tr>
<td colspan="3" style="text-align: left;"><p><strong>波形数量：</strong>1Byte，本波群所包含的基础波形总数量，范围为1~20个。</p>
<p><strong>幅值模式&amp;抖动比例：</strong>1Byte，高四位和低四位分别定义如下</p>
<ol type="a">
<li><p><strong>高4位：</strong>本波群幅值（强度）输出的模式设置，控制波群循环中每次幅值的变化规则，数值1~5对应五种循环输出幅值模式：1恒定模式（波群输出固定幅值为100%）；2渐强模式（波群幅值输出10%~100%线性增强）；3渐弱模式（波群幅值输出100%~10%线性减弱）；4加速渐强模式（波群幅值输出10%~100%加速增强）；5加速渐弱模式（波群幅值输出100%~10%加速减弱）。</p></li>
</ol>
<ol start="3" type="a">
<li><p><strong>低4位：</strong>为本波群循环输出次数的<strong>抖动比例</strong>，1~A对应范围为10%~100%（步长10%）次数变量，默认0%（固定次数）。假设循环次数为10次本值设置为20%，则随机次数为10*（1<u>+</u>20%）=8~12次。——此参数利于降低耐药性</p>
<p><strong>举例：</strong>假设幅值模式设置“恒定模式”，抖动比例设置为20%（假设波群“循环次数”为10次），则本Byte数值为：0x12。</p></li>
</ol></td>
</tr>
</tbody>
</table>

假设波群M1=(方波1+梯形波1+三角波1)，则波群描述数据为：

**波群M1=方波1**(载波4K+高电平时间2ms+低电平时间4.2ms+循环次数12)+**梯形波1**(载波1KHzHz+上升时间1.5ms+高电平时间1.2ms+下降时间2ms+循环次数6)+**三角波1**(载波3KHz+上升时间120ms+下降时间112.8ms+循环次数8)

**波群M1数据(共23Byte)：**(方波)0xF2+0x04+0x00 0x14+0x00 0x2A+0x0C+(梯形波)0xF3+0x01+0x00 0x0F+0x00 0x0C+0x00 0x14+0x06+(三角波)0xF4+0x03+0x04 0xB0+0x04 0x68+0x08

1.  **处方描述数据：**

    多种波群的集合，通常由一系列动作组成的**治疗功能**，如“镇痛”“推拿”“减肥”等。处方描述数据举例如下表：

<table>
<colgroup>
<col style="width: 9%" />
<col style="width: 8%" />
<col style="width: 9%" />
<col style="width: 8%" />
<col style="width: 7%" />
<col style="width: 7%" />
<col style="width: 7%" />
<col style="width: 7%" />
<col style="width: 7%" />
<col style="width: 7%" />
<col style="width: 7%" />
<col style="width: 7%" />
</colgroup>
<tbody>
<tr>
<td style="text-align: center;"><strong>数据长度</strong></td>
<td style="text-align: center;"><strong>处方编号</strong></td>
<td style="text-align: center;"><strong>波群数量</strong></td>
<td style="text-align: center;"><strong>波群1序号</strong></td>
<td style="text-align: center;"><strong>波群1循环次数</strong></td>
<td style="text-align: center;"><strong>波群1间隔时间</strong></td>
<td style="text-align: center;"><strong>波群1号数据</strong></td>
<td style="text-align: center;"><strong>波群n序号</strong></td>
<td style="text-align: center;"><strong>波群n循环次数</strong></td>
<td style="text-align: center;"><strong>波群n间隔时间</strong></td>
<td style="text-align: center;"><strong>波群n数据</strong></td>
<td style="text-align: center;"><strong>校验值</strong></td>
</tr>
<tr>
<td style="text-align: center;"><strong>2Byte</strong></td>
<td style="text-align: center;"><strong>2Byte</strong></td>
<td style="text-align: center;"><strong>1Byte</strong></td>
<td style="text-align: center;"><strong>1Byte</strong></td>
<td style="text-align: center;"><strong>1Byte</strong></td>
<td style="text-align: center;"><strong>2Byte</strong></td>
<td style="text-align: center;"><strong>nByte</strong></td>
<td style="text-align: center;"><strong>1Byte</strong></td>
<td style="text-align: center;"><strong>1Byte</strong></td>
<td style="text-align: center;"><strong>2Byte</strong></td>
<td style="text-align: center;"><strong>nByte</strong></td>
<td style="text-align: center;"><strong>1Byte</strong></td>
</tr>
<tr>
<td colspan="12" style="text-align: left;"><p><strong>数据长度：</strong>除“数据长度”“校验值”外字节数，整个处方数据的字节数(Byte)，<strong>最大不大于1024Byte。</strong></p>
<p><strong>处方编号：2Byte，</strong>由上位机定义生成下传，此编号为处方唯一ID识别号，也用于上位机调用输出，其中0x0000号为临时处方编号（可用于上位机灵活控制变化的处方），下位机接收后不保存到FLASH，仅存于内存用于临时输出调用，直至其它处方调用后，临时处方所占内存将被清除。处方编号范围0~65532。</p>
<p><strong>波群数量：1Byte，</strong>本处方包含的波群总数量，可用于处方完整性校验。波群数量范围1~20个。</p>
<p><strong>波群序号：1Byte，</strong>本波群的序号（如某处方共10个波群，波群序号则为1~10），可用于处方完整性校验。波群序号的范围1~20个。</p>
<p><strong>循环次数：1Byte，</strong>本序号波群的循环输出次数，默认值为0x01(即不循环)，范围1~255次。</p>
<p><strong>间隔时间：2Byte，</strong>本序号波群每次循环输出后暂停输出的时间，单位0.1ms，范围0.1mS~6.5532S</p>
<p><strong>波群数据：nByte，</strong>一系列波形的组合，详见上述“波群描述数据”</p>
<p><strong>校验值：1Byte，</strong>每帧数据所有字节(除去帧头和帧尾)和固定值0x55异或。</p>
<p>例如：收到数据段“0xAA+0x00+0x0A+0x43+0x01+0x25+检验值+0x55”</p>
<p>则：检验值= 0x00^0x0A^0x43^0x01^0x25^0x55 = 0x38</p></td>
</tr>
<tr>
<td colspan="12" style="text-align: center;"><p>备注：</p>
<ol type="1">
<li><p>每个处方的最大数据量不大于1KHz（上位机生成数据时，应有超限提示）</p></li>
<li><p>每个处方的波群数量限制不大于20个（上位机生成数据时，应有超限提示）</p></li>
<li><p>每个波群的波形数量限制不大于20个（上位机生成数据时，应有超限提示）</p></li>
</ol></td>
</tr>
</tbody>
</table>

2)  **音乐包络处方数据：**

    1.  **生成原理：**通过ADC或MP3数据提取音乐包络(节律)，转为一系列实时数据，加上1~10K载波后输出。

    2.  **处方数据格式：**载波频率1Byte+调制深度1Byte+实时包络数据nByte(音乐节律)
