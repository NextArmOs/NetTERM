use eframe::egui;

pub fn show(ui: &mut egui::Ui, fetch_output: &str) {
    ui.heading("System Information Telemetry");
    ui.add_space(10.0);
    ui.separator();
    ui.add_space(5.0);

    egui::ScrollArea::both().show(ui, |ui| {
        let clean_text = fetch_output
            .replace("\x1b[1m", "")
            .replace("\x1b[0m", "")
            .replace("\x1b[31m", "")
            .replace("\x1b[32m", "")
            .replace("\x1b[33m", "")
            .replace("\x1b[34m", "")
            .replace("\x1b[35m", "")
            .replace("\x1b[36m", "");

        ui.vertical(|ui| {
            for line in clean_text.lines() {
                if line.contains("OS:") || line.contains("Kernel:") || line.contains("WM:") || line.contains("Uptime:") {
                    ui.label(egui::RichText::new(line).monospace().color(egui::Color32::LIGHT_GREEN));
                } else if line.contains("Memory:") || line.contains("CPU:") || line.contains("GPU:") {
                    ui.label(egui::RichText::new(line).monospace().color(egui::Color32::LIGHT_BLUE));
                } else {
                    ui.label(egui::RichText::new(line).monospace());
                }
            }
        });
    });
}
