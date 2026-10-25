# 📊 NetTERM Package Ecosystem Progress 

Target Milestone: **400 System Packages**  
Current Status: **12 / 400 Implemented** [▓░░░░░░░░░░░░░░░░░░░] 3%

## NetTERM Progress

| Category | Implemented | Target | Completion | Status |
| :--- | :--- | :--- | :--- | :--- |
| **System Utilities** | 5 | 150 | 3.3% | 🟢 Stable |
| **Tools** | 4 | 100 | 4.0% | 🟡 In Progress |
| **Graphical Apps & Games** | 3 | 100 | 3.0% | 🟡 In Progress |
| **Other Apps** | 0 | 50 | 0.0% | 🔴 Planned |
| **TOTAL** | **12** | **400** | **3.0%** |  |


Fast, lightweight, and modern **Code Editor** and **Network Utility Terminal** **With its own PKGS** written in pure Rust using the **egui** native graphical engine. Perfectly optimized for Linux window managers like Hyprland and Niri. With NothingUI Desktop Environment.

FEATURES
- **Real-time counter for Lines**, Characters, and dynamic file size conversion (Bytes / KB / MB).
- **Live Ping Tool:** Real-time ICMP ping tool with high-precision microsecond RTT duration tracking.


**How to Build & Run** -- 
download NetTERM in releases on github


## Project Roadmap (v0.5.0 → v1.0.0 → ???)

Track the development progress of NetTERM. You can check the boxes below as milestones are achieved!

### v0.5.0
- [x] Create core multi-tab engine architecture
- [x] Remove top tab bar menu buttons entirely to clean up the screen
- [x] Implement persistent bottom command input prompt bar
- [x] Add basic for `:editor` (switch to code panel)
- [x] Add command for `:fetch` (switch to system telemetry)
- [x] Add command for `:calc <expression>` (switch to calculator)
- [x] Add `:help` command
### v0.6.0 - v0.7.0
- [x] Reduce app memory usage and cpu load
      
### Available System Commands: - v0.5.0
  - :editor  or  :edit     ->  Switch to the primary Code Editing panel
  - :terminal or :net      ->  Open Network Diagnostics logs
  - :canvas   or :paint    ->  Open the multi-color mouse drawing canvas
  - :fetch                 ->  Trigger local fastfetch telemetry directly
  -  :calc <expression>     ->  Instantly solve formulas (e.g. :calc 5+(2*3))
  -  :ping <target_ip>      ->  Execute a live network ICMP diagnostic query
  -  :settings or :config   ->  Open settings
  -  :help                  ->  Display this list
  -  :nothingUI             ->  Desktop Environment
 
  
  
## NothingUI v0.0.2
fast desktop environment for NetTERM. 


