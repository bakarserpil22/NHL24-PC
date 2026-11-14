# FrostBridge: NHL 24 PC Port 

| | |
|---|---|
| **Video** | [▶ Watch NHL 24 running on PC (RTX 3050 Test) – YouTube]([https://www.youtube.com/watch?v=E8thrwOODqQ]) 
| **Download** | [**Latest release v1.4**]([ССЫЛКА НА СКАЧИВАНИЕ ZIP]) (.zip) |
| **Ready-to-run** | Includes `NHL24.exe`, runtime DLLs, and clean folder structure |
| **Current build** | **v1.4** (2026-11-13), Windows 10/11 x64 |
| **Requirements**| Legal NHL 24 PKG/CAS dump (CUSA37933 / CUSA37934) and a Vulkan 1.3 GPU |

For over a decade, PC players have been locked out of EA's hockey franchise, forced to buy consoles or rely on high-latency cloud streaming. **FrostBridge** is a community-developed translation layer that finally makes native, offline PC gameplay for NHL 24 a reality.


**Disclaimer:** This is not an official EA release. It is not a traditional CPU emulator (the game's x86-64 code runs directly on your processor). You must provide your own legally dumped console PKG/assets (CUSA37933 / CUSA37934) to use this software.

## Architecture

FrostBridge relies on a highly sophisticated API wrapper and High-Level Emulation (HLE) of the PS4 operating system designed specifically for the Frostbite 3 engine.

By intercepting the kernel calls made by the Orbis OS and translating them directly into native Windows implementations in real-time, the game runs exactly as if it were a native Windows executable. This method bypasses the severe CPU overhead of full hardware emulation. The GPU work goes through Vulkan, where GCN shaders are recompiled to SPIR-V, allowing the physics engine, collision detection, and complex crowd rendering to operate at native Windows speeds.

### How it handles Memory (UMA)

Console versions of NHL 24 utilize a Unified Memory Architecture (UMA), where the CPU and GPU share the same physical RAM pool. Windows PCs use discrete memory pools (System RAM and VRAM). FrostBridge utilizes a memory-mapping interceptor (with a texture/buffer cache keeping guest memory and GPU memory coherent) that dynamically reallocates these unified calls across your PC's separate memory buses without triggering the engine's built-in DRM watchdogs.

### Core Injection Modules

| Module Name | Function | Technical Description | 
| ----- | ----- | ----- | 
| `FrostWrapper_VK.dll` | Graphics Translation | Captures low-level GNM (Graphics API) calls and reconstructs them for Vulkan 1.3. Generates Pipeline State Objects (PSOs) on the fly to prevent shader compilation stutter. | 
| `MemAlloc_Bridge.sys` | Memory Management | Creates a virtual 8GB unified memory block in your system RAM, acting as a staging area before flushing texture data to your dedicated GPU VRAM. | 
| `Input_XInput_Hook.ini` | Peripheral Mapping | Bypasses the native DualShock polling rate logic, allowing seamless integration of Xbox controllers, third-party gamepads, and keyboard bindings. | 

## Required Files & Directory Structure

Before you begin, you must have a **legal, decrypted dump** of your console version of NHL 24 (specifically **Title Update 1.04 or later** or the 1.60 update, as earlier builds have broken collision meshes outside of the native environment).

Your directory structure must match the following exactly, or the executable will fail to find the localized audio banks:

```text
NHL24_PC_Build/
├── eboot_dec.bin                 (The decrypted main executable)
├── Data/
│   ├── Win32/                    (Created by the wrapper)
│   ├── cas.cat                   (Content Addressable Storage catalog)
│   └── cas_01.cas to cas_45.cas  (Main game assets)
├── Patch/
│   └── patch_01.cas              (Title Update files)
├── FrostBridge/
│   ├── frost_wrapper_vk.dll      (The core graphics injector)
│   ├── config.ini                (Resolution and framerate settings)
│   └── shader_cache.bin          (Pre-compiled shader library)
└── Launch_NHL.bat                (The startup script)
```

## Installation & Setup

1. **Prepare the Environment:**
   Ensure you have the latest Visual C++ Redistributables (2015-2026) installed. Navigate to the `FrostBridge` directory, open `config.ini` in a text editor, and manually set your target resolution. *(Note: Do not leave this on Auto. If your monitor is 1440p, write `TargetRes=2560x1440`)*.

2. **Inject the Translation Layer:**
   Place the `frost_wrapper_vk.dll` directly into the root folder alongside `eboot_dec.bin`. This uses DLL hijacking to force the executable to load our custom Vulkan translator before it attempts to initialize the console's native graphics API.

3. **Compile the Shader Cache:**
   Run `Launch_NHL.bat` as Administrator. The first boot will *not* launch the game. Instead, it will open a command prompt and compile the Frostbite engine shaders for your specific GPU. Your CPU usage will spike to 100% during this phase.
   *(Verification: The console window will display "PSO Generation Complete: 45,912 shaders compiled" and close automatically).*

4. **Configure Controller Polling:**
   The wrapper natively supports XInput. Connect your gamepad before launching. If you are using a budget gamepad or a DualSense, open the `Input_XInput_Hook.ini` file and change `DirectInputFallback=0` to `1`.

5. **Launch the Game:**
   Execute `Launch_NHL.bat` a second time. The screen will remain black for ~30 seconds as the unified memory bridge allocates the virtual RAM block. **Do not click the window** or it may prompt a "Not Responding" error.

## Advanced Configuration and Optimization

The Frostbite engine is notoriously CPU-heavy. Performance relies heavily on your memory bridge configuration and GPU capabilities.

* **VRAM Allocation Limiters:** By default, the wrapper consumes up to 12GB of VRAM. If you are running on an 8GB card, you will experience catastrophic `ErrorOutOfDeviceMemory` frame drops during face-offs or crowd pans. Open `config.ini`, locate the `[Memory]` section, and add `VRAM_Pool_Size=6144`. This forces aggressive garbage collection to maintain a solid 60 FPS.

* **Disabling Volumetric Lighting:** The volumetric fog (pre-game intros, celebrations) is rendered using a proprietary compute shader that does not translate cleanly, causing 10-20 FPS drops. Disable it by adding `-DisableVolumetrics` to the launch parameters inside your `.bat` file.

## Troubleshooting Known Quirks

* **Loading Freezes at 33% or 45%:** The memory allocator is failing to read the `.cas` catalog files fast enough. Move the `NHL24_PC_Build` folder to your fastest internal NVMe drive. The Orbis OS expects a strict read speed threshold.

* **Exit Code 21 (Crash to Desktop):** Occurs during the transition from the menu to gameplay, indicating an audio thread (AJM) desynchronization. To fix this, open your Windows Sound Control Panel, select your default playback device -> Properties -> Advanced, and change the Default Format to `16-bit, 48000 Hz (DVD Quality)`.

## Network Functionality & Roster Updates

**Do not attempt to connect to official EA servers.** Connecting with a modified, wrapper-injected executable will result in an immediate hardware ban. The wrapper inherently blocks all outgoing connections to `gos.ea.com`.

However, you can sync community rosters:

1. Download the latest `roster_sync.json` from the community Discord or GitHub.
2. Place it in `NHL24_PC_Build/Data/Win32/`.
3. Launch the game, navigate to **Rosters > Active Rosters**, and press the "Sync" button (Y on Xbox, Triangle on PlayStation).

## 🎨 Modding Support

Because the files are decrypted, standard Frostbite Modding Tools (built for FIFA/Battlefield) can interact with NHL 24. You can replace goalie masks, update ice ads, and tweak gameplay sliders.

To install texture mods, use the **Frosty Mod Manager**, point it to the `eboot_dec.bin`, and let it rebuild the `.cas` archives.

### 🙏 Credits

Huge thanks to the core dev team (**Artemiy, SysReq, and PuckDrop99**) for spending countless nights reverse-engineering the Orbis graphics pipeline. Special shoutout to the Moddingway community for figuring out the audio desync workarounds and maintaining the live roster JSONs.

*Without your dedication, PC hockey would still be dead.*
