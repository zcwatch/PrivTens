# Bootloader 功能概览

## 内存布局
- `APP_ADDR (0x08001800)`：主应用程序起始地址，IAP 跳转时从这里读取堆栈指针和复位向量。
- `FW_ADDR (0x08010800)`：升级固件包区域，开头 2 字节保存 `FW_MARK`，后续依次是文件长度、固件数据与 CRC。
- `SECTOR_SIZE` 与 `FW_SIZE` 定义擦除主程序所需扇区数量（30 个 2 KB 扇区，覆盖整个应用空间）。

## 启动流程
- `main()` 先调用 `hardware_init()` 初始化 GPIO 与电源保持，再读取升级标志。
- `fw_read_update_flag()` 检查 `FW_ADDR` 起始处的 `FW_MARK`（0xaa55）；若标志存在则执行 `fw_update()` 并在更新结束后通过 `NVIC_SystemReset()` 复位。
- 若无更新标志则直接调用 `iap_load_app()` 跳转到主程序。

## 应用跳转（iap_load_app）
- 从 `APP_ADDR` 读取栈顶（第 1 个字）与复位向量（第 2 个字），通过 `0x2FFE0000` 掩码判断栈顶是否落在 RAM 区间。
- 调用 `rcu_deinit()` 清理时钟、设定 MSP 与 VTOR，然后把应用入口地址转换成函数指针 `JumpToApp` 并跳转执行。

## 固件更新流程（fw_update）
- 固件包结构为：`FW_MARK` + `FileLen（固件 + CRC）` + 固件数据 + CRC。
- 计算 `len = FileLen - 4` 取得固件长度，遍历 `FW_ADDR + 6` 开始的数据求和，与尾部 CRC 比对，同时确保长度不超过 `FW_ADDR - APP_ADDR` 以免越界。
- 校验通过后解锁 Flash，按 `FW_SIZE` 个扇区从 `APP_ADDR` 开始擦除，每次擦除前调用 `feed_dog()` 喂狗；然后按半字写入 `len` 个字节的固件数据，写入过程中也会喂狗。
- 写完后清除 `FW_ADDR` 处的标志、锁定 Flash，返回 `1` 表示更新成功。

## 硬件相关支持
- `hardware_init()` 打开 GPIOB 时钟、把 `GPIO_PIN_8` 拉高作为电源保持输出，`RELEASE` 模式下启用看门狗以防止系统挂起。
- `wdt_init()` 打开 40 kHz 低速振荡器、配置独立看门狗的预分频和重装值，然后使能。
- `feed_dog()` 调用 `fwdgt_counter_reload()` 在擦写与写入期间保持看门狗不复位。
- `systick_config()` 存在但被注释，未在 `hardware_init()` 中启用 SysTick。

## 关键宏说明
- `FW_MARK`：16 位 0xaa55 标记表示闪存中有有效的升级包。
- `FW_SIZE` / `SECTOR_SIZE`：用于计算要擦除的 Flash 区域大小，确保覆盖应用空间。
- `pFunction` 定义成 `typedef void (*pFunction)(void)`，用于把复位向量转换成可调用函数指针。

## 备注
- `gd32f30x_libopt.h` 提供芯片底层接口（`fmc_*`、`rcu_*`、`gpio_*`、`NVIC_SystemReset()` 等），是自动生成的 SDK 头文件。
- 更新完成后并不会立即跳到新固件，而是通过 `NVIC_SystemReset()` 复位，bootloader 重新判断标志再跳转，以保证初始化阶段正确执行。
