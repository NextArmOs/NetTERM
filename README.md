# NetTERM & NothingUI Ecosystem

Fast, lightweight, and modern **Code Editor**, **Network Utility Terminal**, and **Desktop Environment** written in pure Rust using the **egui** native graphical engine. Perfectly optimized for Linux window managers like Hyprland and Niri And Other Operating Systems.

---

## Package Manager & Versioning Strategy

We are actively developing our native package manager (**PKGS**) to power the ecosystem (currently at 12 / 400 implemented packages, or 3%) and shifting toward a stable release model.

* **Versioning Scheme:** `4.Major.Minor-Patch` (e.g., `4.0.0-1`)
* **Designation:** **Permanent LTS (Long-Term Support)** focusing on extreme stability, hardware support, and strict backward compatibility.

### Detailed Versioning Breakdown:
* **`4` (The Immutable Base):** **This first digit will never change.** It stands as a permanent lifetime guarantee for maximum hardware support and total backward compatibility across the entire future lifecycle of the project.
* **`4.x` (Second Digit - Major Update):** Represents major architectural shifts, heavy system integration milestones, or massive new core feature sets.
* **`4.0.x` (Third Digit - Minor Update):** Indicates smaller feature releases, UI improvements in NothingUI, or safe additions of new system commands.This number strictly increments with every single minor maintenance update. Each minor update guarantees:
  * Continuous CPU/memory load optimizations.
  * Seamless expansion of the native **PKGS** repository towards our 400-package milestone.
* **`4.0.0-x` (Bug Fix - Patch Release):** **The primary dynamic indicator.** Each patch update:
  * Fast delivery of stable bug fixes and immediate hotfixes

This custom strategy ensures that users receive an absolutely predictable, rock-solid environment where the core platform configuration never breaks.

---

## Core Features & Commands

* **Tools:** Advanced Code Editor with live counters, real-time ICMP ping utility, and NothingUI DE.
* **Commands:** Use built-in prompts like `:editor`, `:terminal`, `:canvas`, `:fetch`, `:calc`, `:ping`, `:settings`, and `:help`.

---

## Platform & Architecture Roadmap

NetTERM and NothingUI are designed to be ultra-fast, minimal, and keyboard-driven. Below is the current compatibility status across different operating systems and CPU architectures:

### Linux
- [x] **Linux x86_64** — **Fully Supported & Tested** 
  * Flawless optimization and window pooling for dynamic WMs like Hyprland and Niri.
  * Native high-precision ICMP ping features and fast local `fastfetch`.
- [ ] **Linux ARM64 / AArch64** — **Planned / In Progress** 
  * Target expansion for single-board computers (Raspberry Pi) and ARM laptops.
- [ ] **Linux RISC-V** — **Planned** 
  * Future-proofing the ecosystem for open-source hardware architectures.

### Android
- [ ] **Android ARM64 (AArch64)** — **Planned**
  * Exploring compatibility for terminal environments (e.g., Termux integrations) or native mobile shells.

### macOS
- [ ] **macOS ARM64 (Apple M-Series)** — **Not Supported / Investigating**
  * *Current Status:* Testing fails due to Os restrictions. (Still Works in VisualStudioCode With cargo run)

---

## ⚙️ How to Build & Run

You can start using NetTERM either by downloading a pre-built binary or compiling it directly from the source code.

### Option 1: Quick Start (Pre-built Binaries)
The easiest way is to download the ready-to-run compiled version for your system:
1. Go to the [Releases](https://github.com/NextArmOs/NetTERM/releases) page on GitHub.
2. Download the latest asset for your architecture.
3. Grant execution permissions and run it.

### Option 2: Build from Source (Rust / Cargo)
If you want to compile the project yourself, make sure you have the Rust toolchain installed, then execute the following commands:

```bash
# Clone the repository

# Compile and run
cargo run --release
#or
cargo run
```

---

## 🤝 Contributing & Bug Reports

We welcome community feedback, especially regarding cross-platform compatibility, compilation logs, and package expansion!

| Goal | How to Help / What to Do | Where to Go |
| :--- | :--- | :--- |
| **Bug Reporting** | Open an issue with your system specs (OS, WM, Rust version) and steps to reproduce. | [Open an Issue](https://github.com/) |
| **macOS / ARM64 Testing** | If you manage to compile or trace crashes on Apple Silicon or ARM64 Linux, please share your logs! | [Open an Issue / PR](https://github.com/) |
| **Adding Packages** | Help us reach our 400-package milestone by contributing new scripts or tools to **PKGS**. | [Submit a PR](https://github.com/NextArmOs) |
| **Feature Requests** | Suggest new built-in commands or UI improvements for NothingUI. | [Discussions / Issues](https://github.com) |

---

## License

This project is open-source and available under the terms of the **MIT License**.
