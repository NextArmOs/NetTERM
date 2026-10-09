use sysinfo::{Pid, ProcessRefreshKind, System, MINIMUM_CPU_UPDATE_INTERVAL};

pub struct ResourceMonitor {
    sys: System,
    pid: Pid,
    pub cpu_usage: f32,
    pub memory_mb: f32,
}

impl ResourceMonitor {
    pub fn new() -> Self {
        let mut sys = System::new();
        let pid = Pid::from(std::process::id() as usize);
        
        let refresh_kind = ProcessRefreshKind::nothing()
            .with_cpu()
            .with_memory();

        sys.refresh_process_specifics(pid, refresh_kind);
        std::thread::sleep(MINIMUM_CPU_UPDATE_INTERVAL);
        sys.refresh_process_specifics(pid, refresh_kind);

        let mut monitor = Self {
            sys,
            pid,
            cpu_usage: 0.0,
            memory_mb: 0.0,
        };
        
        monitor.update();
        monitor
    }

    pub fn update(&mut self) {
        let refresh_kind = ProcessRefreshKind::nothing()
            .with_cpu()
            .with_memory();

        self.sys.refresh_process_specifics(self.pid, refresh_kind);
        
        if let Some(process) = self.sys.process(self.pid) {
            self.cpu_usage = process.cpu_usage();
            self.memory_mb = (process.memory() as f32) / 1024.0 / 1024.0;
        }
    }
}
