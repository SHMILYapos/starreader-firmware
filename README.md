<div align="center">

# StarReader Pro

### 阅星瞳 X4 Pro 开源墨水屏阅读器固件

![Version](https://img.shields.io/badge/version-0.2.0-blue)
![License](https://img.shields.io/badge/license-MIT-green)
![Platform](https://img.shields.io/badge/platform-ESP32--C3-red)
![Language](https://img.shields.io/badge/language-C%2B%2B-orange)

**简洁 · 高效 · 完全开源 · 社区驱动**

[功能特性](#-功能特性) · [快速开始](#-快速开始) · [编译烧录](#-编译烧录) · [项目结构](#-项目结构) · [开发指南](#-开发指南) · [贡献指南](#-贡献指南)

</div>

---

## 📖 项目简介

**StarReader Pro** 是专为 **阅星瞳 X4 Pro**（Xteink X4 Pro）电子墨水屏阅读器打造的完全开源固件。

我们相信，你的设备应该做你想让它做的事——而不是制造商决定的那样。本项目由开源社区驱动，目标是提供一个**功能完整、界面美观、完全可定制**的阅读体验。

> ⚠️ **注意**：这是非官方社区固件，与阅星瞳/Xteink 公司无关。使用风险自负，烧录前请务必备份原始固件。

---

## ✨ 功能特性

### 📚 阅读体验
- ✅ **TXT 阅读器** - 纯文本阅读，支持翻页、行间距调节
- ✅ **阅读进度保存** - 自动记录上次阅读位置，一键继续阅读
- 🚧 **EPUB 支持** - EPUB 2/3 格式支持（开发中）
- 🚧 **PDF 支持** - PDF 文档阅读（计划中）
- 🚧 **自定义字体** - 从 SD 卡加载字体文件（计划中）

### 🎨 显示优化
- ✅ **多种刷新模式** - 全刷 / 局刷 / 快刷，适配不同场景
- ✅ **屏幕方向** - 支持竖屏 / 横屏切换
- ✅ **自动全刷** - 每 N 页自动全刷，消除残影
- ✅ **状态栏** - 实时显示电量、前光状态、充电状态

### 💡 前光控制
- ✅ **双色温前光** - 暖白 + 冷白，无级调光
- ✅ **亮度调节** - 0-100% PWM 无级调光
- ✅ **色温调节** - 冷暖自由混合，打造完美阅读光
- ✅ **预设模式** - 阅读 / 夜间 / 白天三种预设
- ✅ **快捷开关** - 长按快速开关前光

### 📡 网络功能
- 🚧 **WiFi 连接** - 无线联网（开发中）
- 🚧 **Web 传书** - 浏览器直接上传书籍（计划中）
- 🚧 **OTA 升级** - 在线无线升级固件（计划中）
- 🚧 **蓝牙同步** - 手机蓝牙同步阅读进度（计划中）

### 🎮 交互方式
- ✅ **电容触控** - 点按选择，滑动翻页
- ✅ **物理按键** - 侧边翻页按键支持
- ✅ **手势支持** - 上下左右滑动、长按
- ✅ **电容 Home 键** - 底部触控 Home 键

### ⚡ 电源管理
- ✅ **深度休眠** - 超低功耗，超长续航
- ✅ **自动休眠** - 可配置超时时间
- ✅ **电池监测** - 电压检测与百分比显示
- ✅ **磁吸充电检测** - 自动识别充电状态

### 🌐 双语支持
- ✅ **简体中文** - 默认界面语言
- ✅ **English** - 英文界面
- ✅ **一键切换** - 在设置中随时切换语言

---

## 📋 硬件规格

| 项目 | 参数 |
|------|------|
| **设备型号** | 阅星瞳 X4 Pro (Xteink X4 Pro) |
| **主控芯片** | ESP32-C3 (RISC-V, 160MHz) |
| **Flash** | 16MB SPI |
| **RAM** | 约 400KB（可用约 380KB） |
| **屏幕** | 4.3 英寸墨水屏，800×480 |
| **驱动 IC** | SSD1677 (GDEQ0426T82) |
| **触控** | 电容式多点触控 (GT911, I2C) |
| **前光** | 双色温 LED（暖白+冷白），PWM 控制 |
| **存储** | microSD 卡槽 (FAT32) |
| **电池** | 锂聚合物电池（磁吸充电） |
| **厚度** | 5.95mm |
| **重量** | 约 70g |
| **连接方式** | 磁吸顶针（充电 + 数据） |

详细引脚定义请查看 [docs/hardware.md](docs/hardware.md)。

---

## 🚀 快速开始

### 前置要求

1. **硬件**
   - 阅星瞳 X4 Pro 阅读器
   - 磁吸数据适配器 / 顶针数据线
   - 电脑（Windows / macOS / Linux）

2. **软件**
   - [PlatformIO](https://platformio.org/) - 推荐使用 VS Code + PlatformIO 插件
   - 或者使用命令行：`pip install platformio`

### 一键烧录（推荐新手）

1. **下载固件**
   - 前往 [Releases](https://github.com/SHMILYapos/starreader-firmware/releases) 页面
   - 下载最新的 `starreader-pro-vX.X.X-factory.bin` 完整固件

2. **进入下载模式**
   - 按住电源键 + 特定按键组合
   - 连接磁吸数据适配器
   - 2 秒后松开按键

3. **烧录固件**
   ```bash
   # 使用 esptool 烧录完整固件
   python -m esptool --chip esp32c3 --port /dev/ttyUSB0 --baud 921600 write_flash 0x0 starreader-pro-v0.2.0-factory.bin
   ```

   > **Windows 用户**：端口号类似 `COM3`，在设备管理器中查看

### 从源码编译

```bash
# 克隆仓库
git clone https://github.com/SHMILYapos/starreader-firmware.git
cd starreader-firmware

# 编译
platformio run

# 烧录
platformio run -t upload

# 串口监视
platformio device monitor
```

详细编译说明请查看 [docs/build.md](docs/build.md)。

---

## 📁 项目结构

```
starreader-firmware/
├── src/
│   ├── main.cpp              # 程序入口
│   ├── config.h              # 硬件引脚配置
│   ├── hal/                  # 硬件抽象层
│   │   ├── display.h/.cpp    # 墨水屏驱动
│   │   ├── input.h/.cpp      # 按键输入
│   │   ├── touch.h/.cpp      # 电容触控
│   │   ├── frontlight.h/.cpp # 前光控制
│   │   ├── storage.h/.cpp    # SD 卡存储
│   │   └── power.h/.cpp      # 电源管理
│   ├── ui/                   # 用户界面层
│   │   ├── activity.h        # Activity 基类
│   │   ├── home_activity.h/.cpp    # 主菜单
│   │   ├── library_activity.h/.cpp # 书库/文件浏览
│   │   ├── reader_activity.h/.cpp  # TXT 阅读器
│   │   ├── settings_activity.h/.cpp # 设置界面
│   │   ├── wifi_activity.h/.cpp    # WiFi 配置
│   │   └── about_activity.h/.cpp  # 关于设备
│   └── utils/                # 工具层
│       ├── settings.h/.cpp    # 设置持久化
│       └── i18n.h/.cpp       # 国际化双语
├── docs/
│   ├── hardware.md           # 硬件文档
│   └── build.md              # 编译指南
├── releases/                 # 固件发布包
├── platformio.ini            # PlatformIO 配置
├── partitions.csv            # Flash 分区表
├── LICENSE                   # MIT 开源协议
└── README.md                 # 本文件
```

---

## 🏗️ 架构设计

StarReader Pro 采用经典的分层架构，便于维护和扩展：

```
┌─────────────────────────────────┐
│         应用层 (UI)             │  ← 界面、交互、业务逻辑
├─────────────────────────────────┤
│       活动管理器 (Manager)      │  ← 页面导航、栈管理
├─────────────────────────────────┤
│       工具层 (Utils)             │  ← 设置、国际化、工具函数
├─────────────────────────────────┤
│       硬件抽象层 (HAL)           │  ← 屏幕、触控、按键、前光、电源
├─────────────────────────────────┤
│       硬件平台 (ESP32-C3)        │  ← MCU、外设、驱动
└─────────────────────────────────┘
```

**设计理念**：
- **关注点分离**：硬件操作和 UI 逻辑完全分离
- **可移植性**：HAL 层抽象后，可轻松移植到其他硬件平台
- **可扩展性**：新增功能只需添加新的 Activity，无需修改核心代码

---

## 🗺️ 开发路线图

### v0.2.0 - 当前版本
- [x] 基础 HAL 层（屏幕、按键、前光、SD卡）
- [x] 电容触控屏驱动
- [x] 主菜单界面
- [x] TXT 阅读器
- [x] 设置界面
- [x] 书库/文件浏览器
- [x] 关于设备界面
- [x] 中英双语支持
- [x] 双色前光控制

### v0.3.0 - 网络功能
- [ ] WiFi 连接配置
- [ ] Web 服务器（无线传书）
- [ ] OTA 在线升级
- [ ] 蓝牙低功耗同步

### v0.4.0 - 阅读增强
- [ ] EPUB 2/3 解析与渲染
- [ ] 书签管理
- [ ] 目录跳转
- [ ] 搜索功能
- [ ] 自定义字体支持

### v0.5.0 - 体验优化
- [ ] 睡眠屏幕自定义
- [ ] 屏保功能
- [ ] 手势自定义
- [ ] 主题系统
- [ ] 性能优化

### v1.0.0 - 正式版
- [ ] 完整的 EPUB 支持
- [ ] PDF 基础支持
- [ ] 插件系统
- [ ] 更多设备适配
- [ ] 稳定版本发布

---

## 🤝 贡献指南

我们欢迎任何形式的贡献！无论是代码、文档、翻译还是建议。

### 如何贡献

1. Fork 本仓库
2. 创建你的特性分支：`git checkout -b feature/amazing-feature`
3. 提交你的修改：`git commit -m 'feat: add some amazing feature'`
4. 推送到分支：`git push origin feature/amazing-feature`
5. 提交 Pull Request

### 代码规范

- 遵循现有的代码风格
- 提交信息使用 [Conventional Commits](https://www.conventionalcommits.org/) 格式
- 新功能需要添加相应的注释
- 保持界面的简洁美观，符合墨水屏阅读设备的特性

### 报告问题

如果你发现了 bug 或者有新的功能建议，请[创建 Issue](https://github.com/SHMILYapos/starreader-firmware/issues/new)。

---

## 🙏 致谢

本项目的诞生离不开以下开源项目和社区的贡献：

- **[CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader)** - 开创性的墨水屏开源固件项目
- **[open-x4-epaper](https://github.com/open-x4-epaper)** - 社区逆向工程与 SDK
- **[sunwoods/Xteink-X4](https://github.com/sunwoods/Xteink-X4)** - 硬件逆向分析
- **[GxEPD2](https://github.com/ZinggJM/GxEPD2)** - 优秀的墨水屏驱动库
- **PlatformIO** - 强大的嵌入式开发平台

感谢所有为开源墨水屏生态做出贡献的开发者们！

---

## 📄 开源协议

本项目采用 **MIT License** 开源协议 - 详见 [LICENSE](LICENSE) 文件。

你可以自由地使用、修改、分发本项目代码，只需保留原作者版权声明即可。

---

<div align="center">

**如果这个项目对你有帮助，请给它一个 ⭐ Star 支持一下！**

Made with ❤️ by the StarReader Community

</div>
