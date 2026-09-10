# 阅星瞳 X4 Pro 编译与烧录指南

本指南介绍如何从源码编译 StarReader Pro 固件，并烧录到您的阅星瞳 X4 Pro 设备。

---

## 前置要求

### 软件需求

1. **PlatformIO Core**（或 PlatformIO IDE 扩展）
   - 通过 pip 安装：`pip install platformio`
   - 或使用 VS Code 的 PlatformIO 扩展

2. **USB 驱动程序**
   - CP210x 或 CH340 驱动（取决于磁吸适配器）
   - 现代操作系统通常自动安装

3. **esptool**（高级烧录用）
   - `pip install esptool`

### 硬件需求

- 阅星瞳 X4 Pro
- 磁吸数据适配器 / 顶针数据线
- 电脑（Windows/macOS/Linux）

---

## 编译

### 1. 克隆仓库

```bash
git clone https://github.com/SHMILYapos/starreader-firmware.git
cd starreader-firmware
```

### 2. 配置环境

根据需要编辑 `platformio.ini`：

```ini
# 根据您的系统更新上传端口
upload_port = /dev/ttyACM0       # Linux
# upload_port = COM3             # Windows
# upload_port = /dev/cu.usbserial-0001  # macOS
```

### 3. 编译固件

```bash
# 调试版本
platformio run

# 发布版本（优化）
platformio run --environment release
```

编译后的固件二进制文件位于：
```
.pio/build/esp32-c3-devkitm-1/firmware.bin
```

---

## 烧录

### 方法一：PlatformIO（推荐）

1. 将设备进入烧录模式：
   - 按住电源键 + 特定按键组合
   - 连接磁吸数据适配器
   - 2 秒后松开按键

2. 烧录：
```bash
platformio run -t upload
```

3. 监视输出：
```bash
platformio device monitor
```

### 方法二：esptool.py（手动）

供高级用户或恢复使用：

```bash
# 完整烧录（16MB）
python -m esptool --chip esp32c3 --port /dev/ttyACM0 --baud 921600 write_flash 0x0 firmware.bin

# 仅应用分区（更快）
python -m esptool --chip esp32c3 --port /dev/ttyACM0 --baud 921600 write_flash 0x10000 firmware.bin
```

### 方法三：SD 卡恢复

如果设备变砖：

1. 将固件二进制文件复制到 SD 卡，命名为 `update.bin`
2. 将 SD 卡插入设备
3. 按住恢复按键组合开机
4. 固件将自动烧录

---

## 备份与恢复

### 备份原始固件

**重要：** 烧录自定义固件前，请务必备份原始固件！

```bash
# 读取整个 16MB Flash
python -m esptool --chip esp32c3 --port /dev/ttyACM0 read_flash 0x0 0x1000000 firmware_backup.bin

# 仅读取应用分区（更快）
python -m esptool --chip esp32c3 --port /dev/ttyACM0 read_flash 0x10000 0x640000 app_backup.bin
```

### 恢复原始固件

```bash
# 恢复完整备份
python -m esptool --chip esp32c3 --port /dev/ttyACM0 write_flash 0x0 firmware_backup.bin

# 仅恢复应用
python -m esptool --chip esp32c3 --port /dev/ttyACM0 write_flash 0x10000 app_backup.bin
```

---

## OTA 升级

StarReader 支持通过 WiFi 进行空中升级（计划中）。

### 切换 OTA 分区

```bash
# 备份 OTA 数据
python -m esptool --chip esp32c3 --port /dev/ttyACM0 read_flash 0xE000 0x2000 otadata_backup.bin

# 从 app0 启动
python -m esptool --chip esp32c3 --port /dev/ttyACM0 write_flash 0xE000 otadata_boot_app0.bin

# 从 app1 启动
python -m esptool --chip esp32c3 --port /dev/ttyACM0 write_flash 0xE000 otadata_boot_app1.bin
```

---

## 故障排除

### 设备未检测到

- 确保磁吸适配器正确对齐
- 尝试调整磁吸连接
- 安装 CP210x/CH340 驱动
- Linux 系统：将用户添加到 `dialout` 组

### 烧录失败

- 降低波特率：`--baud 115200`
- 确保设备处于烧录模式（正确的按键组合）
- 检查是否选择了正确的端口

### 显示乱码

- 验证显示方向设置
- 尝试全刷模式
- 检查 GxEPD2 显示类是否与您的面板匹配

### 触控不工作

- 检查 `config.h` 中的 I2C 引脚定义
- 验证触控 IC 地址（GT911: 0x5D）
- 尝试通过 RST 引脚复位触控面板

### 前光不工作

- 验证 PWM 引脚分配
- 检查设置中是否启用了前光
- 尝试通过 `on()` / `off()` 函数切换

### SD 卡未检测到

- 检查 SD 卡是否为 FAT32 格式
- 尝试不同的 SD 卡（有些卡不兼容）
- 验证 SPI 引脚定义

---

## 开发技巧

### 调试输出

启用详细日志：
```cpp
#define CORE_DEBUG_LEVEL 3
```

### 内存分析

```cpp
Serial.printf("空闲堆内存: %d 字节\n", esp_get_free_heap_size());
```

### 触控调试

```cpp
TouchEvent event = touch->getLastEvent();
if (event.gesture != GESTURE_NONE) {
    Serial.printf("手势: %d 位置: (%d, %d)\n",
                  event.gesture, event.endPoint.x, event.endPoint.y);
}
```

### 添加新功能

1. 创建新的 `Activity` 子类
2. 实现生命周期方法（`onEnter`, `loop`, `render`）
3. 从现有 Activity 添加导航入口
4. 添加触控手势处理
5. 在真机上测试

---

## 参考资源

- [ESP32-C3 数据手册](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf)
- [PlatformIO 文档](https://docs.platformio.org/)
- [GxEPD2 库](https://github.com/ZinggJM/GxEPD2)
- [GT911 触控 IC 数据手册](https://www.goodix.com/en/product/touch/gt911)
- [esptool 文档](https://docs.espressif.com/projects/esptool/)
