# GMonitor 📊

![C](https://img.shields.io/badge/Language-Pure%20C-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%2010%2F11-lightgrey.svg)
![Size](https://img.shields.io/badge/Size-~30%20KB-success.svg)
![RAM](https://img.shields.io/badge/RAM_Usage-<2%20MB-success.svg)
![License](https://img.shields.io/badge/License-MIT-orange.svg)

A hyper-optimized, zero-dependency, bare-metal Windows hardware monitor overlay written in pure C.

> **Why build this?** Modern desktop widgets are often built on heavy frameworks like Electron or .NET, consuming 150MB+ of RAM just to display simple text. **GMonitor** was built from the ground up using the native Win32 API. It consumes **under 2MB of RAM**, uses roughly **0% CPU**, and requires zero external drivers or background apps to run.


## 📑 Table of Contents
- [Features](#-features)
- [Under the Hood](#-under-the-hood)
- [Installation & Usage](#-installation--usage)
- [Build from Source](#-build-from-source)
- [Controls](#-controls)
- [License](#-license)

## ✨ Features
* **Zero Dependencies:** Compiles to a single standalone executable. No installers, no frameworks, no third-party software required.
* **Native OS Telemetry:** Taps directly into the Windows Kernel to pull real-time hardware data.
* **Click-Through Overlay:** Renders an invisible, borderless window layer that lets you click right through it to the desktop behind.
* **Dynamic Hardware Detection:** Automatically identifies primary GPU adapters, VRAM sizes, and Laptop Battery states.

**Currently Tracking:**
* 🧠 CPU Usage (%)
* 💾 RAM Usage (Used GB / Total GB / %)
* 💿 Disk Activity (%)
* 🎮 GPU Name & Total VRAM
* 🔋 Battery Percentage (If applicable)

## ⚙️ Under the Hood
This project is a demonstration of low-level Win32 systems programming. It utilizes:
* **`windows.h` (Win32 API):** Manages the asynchronous message loop, global hotkey registration, and native memory calculation (`GlobalMemoryStatusEx`).
* **`GDI` (Graphics Device Interface):** Handles double-buffering and rendering layered (`WS_EX_LAYERED`), transparent (`LWA_COLORKEY`) text overlays.
* **`pdh.h` (Performance Data Helper):** Queries Windows performance counters for CPU and Disk IO telemetry without needing Ring-0 drivers.
* **`dxgi.h` (DirectX Graphics Infrastructure):** Safely spins up a minimal DirectX factory to enumerate physical PCI-e graphics adapters and calculate dedicated video memory.

## 🚀 Installation & Usage
1. Go to the **[Releases](../../releases)** page.
2. Download `GMonitor.exe`.
3. Double-click the executable. The overlay will instantly appear in the top-left corner of your primary monitor. 

*(Note: Because this is a custom unsigned C binary, Windows SmartScreen may flag it. Click `More Info` -> `Run Anyway` to launch it).*

## ⌨️ Controls
Because GMonitor is a transparent, click-through background process, it cannot be closed with a standard "X" button. 
* To instantly terminate the application, press the global kill-switch: 
  **`Ctrl + Shift + Y`**

## 💻 Build from Source
To compile this project yourself, you will need a C compiler (`gcc`) installed via MinGW-w64 or MSYS2.

Run the following command to compile. This command includes flags to optimize for size (`-Os`), strip debug symbols for privacy and size reduction (`-s`), hide the background terminal (`-mwindows`), and link the required Windows libraries:

```bash
gcc Main.c -o GMonitor.exe -Os -s -mwindows -luser32 -lgdi32 -lpdh -ldxgi -ldxguid
