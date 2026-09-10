# StarReader Pro 阅星瞳 X4 Pro 开源固件

专为 **阅星瞳 X4 Pro**（Xteink X4 Pro）电子墨水屏阅读器设计的开源固件。

---

## 项目简介

StarReader Pro 是一个社区驱动的完全开源固件，专为阅星瞳 X4 Pro 打造——这款超薄便携电子墨水屏阅读器配备了电容触控屏和双色前光。

**目标设备：** 阅星瞳 X4 Pro（Xteink X4 Pro）

---

## X4 Pro 特色功能

- **4.3 英寸墨水触控屏** - 电容触控，支持点选和滑动手势
- **双色前光** - 可调亮度和冷暖色温
- **电容 Home 键** - 底部触控式 Home 键
- **磁吸充电** - 无 Type-C 接口，采用磁吸顶针充电
- **超薄设计** - 厚度仅 5.95mm，重量约 70g

---

## 功能特性

### 触控与导航
- **电容触控** - 点选选择，滑动翻页
- **电容 Home 键** - 底部触控按键
- **物理按键** - 侧边翻页按键依然支持
- **手势支持** - 左右滑动翻页，下拉快速设置，长按开关前光

### 前光控制
- **双色 LED** - 暖白和冷白双声道
- **亮度调节** - 0-100% PWM 无级调光
- **色温调节** - 冷暖混合，完美阅读光
- **快捷开关** - 一键开关前光

### 阅读体验
- **TXT 阅读器** - 纯文本阅读器，支持翻页导航
- **EPUB 支持** - EPUB 2/3 渲染（计划中）
- **自定义字体** - 从 SD 卡安装字体
- **排版控制** - 边距、行间距、对齐方式
- **阅读进度** - 自动保存阅读位置

### 显示
- **4.3 英寸墨水屏** - 800×480 单色显示（GDEQ0426T82）
- **多种刷新模式** - 全刷、局刷、快刷
- **方向支持** - 竖屏 / 横屏 / 倒置
- **状态栏** - 电量、页码、前光状态显示

### 电源管理
- **深度休眠** - 超低功耗
- **自动休眠** - 可配置超时时间
- **电池监测** - 电压和百分比显示
- **触控/电源键唤醒** - 瞬间唤醒

### 存储
- **microSD 卡** - 可扩展存储
- **设置持久化** - 配置保存到 SD 卡
- **书籍缓存** - 预渲染页面缓存

### 双语支持
- **中文界面** - 默认简体中文
- **英文界面** - 可一键切换
- **设置中切换** - 在系统设置中切换语言

---

## 硬件规格

| 组件 | 规格 |
|------|------|
| 主控 | ESP32-C3 (RISC-V, 160MHz) |
| Flash | 16MB SPI |
| RAM | 约 400KB（可用约 380KB） |
| 屏幕 | 4.3 英寸墨水屏，800×480，SSD1677 |
| 触控 | 电容触控（I2C，GT911 兼容） |
| 前光 | 双色（暖白+冷白），PWM 控制 |
| 存储 | microSD 卡槽 |
| 电池 | 锂电池（磁吸充电） |
| 厚度 | 5.95mm |
| 重量 | 约 70g |
| 磁吸 | 支持（充电 + 数据） |

详细引脚定义请参考 [docs/hardware.md](docs/hardware.md)。

---

## 快速开始

### 前置要求

- 已安装 [PlatformIO](https://platformio.org/)
- 阅星瞳 X4 Pro 设备
- 磁吸数据线 / 顶针适配器

### 编译与烧录

```bash
# 克隆仓库
git clone https://github.com/SHMILYapos/starreader-firmware.git
cd starreader-firmware

# 编译
platformio run

# 烧录（通过特殊按键组合进入下载模式）
platformio run -t upload

# 打开串口监视器
platformio device monitor
```

详细构建说明请参考 [docs/build.md](docs/build.md)。

---

## 项目结构

```
starreader-firmware/
├── src/
│   ├── main.cpp              # 入口文件
│   ├── config.h              # 硬件引脚定义
│   ├── hal/                  # 硬件抽象层 (HAL)
│   │   ├── display.h/.cpp    # 墨水屏驱动
│   │   ├── input.h/.cpp      # 按键输入处理
│   │   ├── touch.h/.cpp      # 电容触控屏
│   │   ├── frontlight.h/.cpp # 前光控制
│   │   ├── storage.h/.cpp    # SD 卡存储
│   │   └── power.h/.cpp      # 电源管理
│   ├── ui/                   # 用户界面层
│   │   ├── activity.h         # Activity 基类
│   │   ├── home_activity.h/.cpp    # 主菜单
│   │   ├── library_activity.h/.cpp  # 书库/文件浏览
│   │   ├── txt_reader_activity.h/.cpp  # TXT 阅读器
│   │   ├── settings_activity.h/.cpp    # 设置界面
│   │   └── about_activity.h/.cpp       # 关于界面
│   └── utils/                # 工具层
│       ├── settings.h/.cpp    # 设置持久化
│       └── i18n.h/.cpp       # 国际化双语支持
├── docs/
│   ├── hardware.md           # 硬件文档
│   └── build.md              # 构建说明
├── platformio.ini            # PlatformIO 配置
├── partitions.csv            # Flash 分区表
├── LICENSE                   # MIT 开源协议
└── README.md                 # 本文件
```

---

## 架构设计

StarReader Pro 采用分层架构：

1. **HAL 层** - 硬件抽象（显示、触控、按键、前光、存储、电源）
2. **输入层** - 统一输入事件处理（触控 + 按键）
3. **Activity 层** - 屏幕管理和导航
4. **应用层** - 阅读、设置、书库

基于 Activity 的导航模型提供了清晰的界面分离，便于添加新功能。

---

## 开发路线图

- [x] 显示驱动 (SSD1677)
- [x] 按键输入 (电阻梯 ADC)
- [x] SD 卡存储
- [x] 电源管理 (深度休眠)
- [x] TXT 阅读器
- [x] 设置菜单
- [x] 电容触控屏驱动
- [x] 双色前光控制
- [x] 手势导航（左右滑动翻页）
- [x] 下拉快速设置面板
- [x] 文件浏览器/书库
- [x] 双语支持（中文/英文）
- [x] 关于设备界面
- [ ] EPUB 阅读器
- [ ] WiFi Web 服务器（通过磁吸上传书籍）
- [ ] 自定义休眠画面
- [ ] OTA 升级

---

## 贡献指南

欢迎贡献代码！请随时提交 Pull Request 或创建 Issue。

---

## 致谢

本项目基于开源电子书阅读器社区的成果：

- [CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader) - 开创性的开源固件
- [open-x4-epaper](https://github.com/open-x4-epaper) - 社区 SDK
- [sunwoods/Xteink-X4](https://github.com/sunwoods/Xteink-X4) - 硬件逆向工程

---

## 开源协议

MIT License - 详见 [LICENSE](LICENSE) 文件。

**免责声明：** 这是非官方的社区固件，使用风险自负。烧录前请务必备份原始固件。
