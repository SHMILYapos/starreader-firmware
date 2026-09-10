# StarReader Pro v0.2.0 - 首个测试版本

## 版本说明

这是阅星瞳 X4 Pro 开源固件的首个可编译测试版本，基于 ESP32-C3 + 4.3英寸墨水屏硬件平台开发。

## 固件包说明

| 文件 | 大小 | 说明 |
|------|------|------|
| starreader-pro-v0.2.0-x4pro.bin | 478KB | 应用固件（刷到 0x10000 偏移） |
| starreader-pro-v0.2.0-factory.bin | 544KB | 完整出厂固件（包含 bootloader + 分区表） |

## 已实现功能

### 硬件抽象层 (HAL)
- 墨水屏驱动 (SSD1677, GDEQ0426T82)
- 电容触控屏驱动 (GT911, I2C)
- 双色前光控制 (PWM 无级调光)
- 按键输入 (电阻梯 ADC)
- SD 卡存储
- 电源管理 (电池监测、深度休眠)

### 用户界面
- 主菜单 (中英文双语)
- TXT 文本阅读器
- 设置界面 (前光、显示、阅读、电源、系统设置)
- 书库/文件浏览器
- 关于设备界面

### 特色功能
- 中英双语切换 (在设置中一键切换)
- 简洁美观的黑白 UI 设计
- 触控手势支持
- 阅读进度自动保存

## 刷入说明

### 方法一：使用 esptool 刷入应用固件
```bash
python -m esptool --chip esp32c3 --port /dev/ttyACM0 --baud 921600 write_flash 0x10000 starreader-pro-v0.2.0-x4pro.bin
```

### 方法二：刷入完整出厂固件
```bash
python -m esptool --chip esp32c3 --port /dev/ttyACM0 --baud 921600 write_flash 0x0 starreader-pro-v0.2.0-factory.bin
```

## 已知限制

1. **硬件引脚为社区逆向推测值**，实际硬件引脚可能需要调整
2. **休眠唤醒功能** 待完善 (ESP32-C3 GPIO 唤醒配置)
3. **文件读取** 暂不支持随机访问，仅读取文件开头
4. **触控手势** 基础实现，灵敏度待调校
5. **EPUB 支持** 计划中，当前仅支持 TXT

## 开发计划

- [ ] 完善 EPUB 阅读器支持
- [ ] WiFi Web 服务器 (无线传书)
- [ ] 自定义字体支持
- [ ] OTA 在线升级
- [ ] 更多手势和快捷操作

## 致谢

本项目基于开源社区的成果：
- [CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader)
- [open-x4-epaper](https://github.com/open-x4-epaper)
- [sunwoods/Xteink-X4](https://github.com/sunwoods/Xteink-X4)

## 免责声明

这是非官方的社区固件，使用风险自负。烧录前请务必备份原始固件。
