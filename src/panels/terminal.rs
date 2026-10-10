use eframe::egui;

pub fn show(ui: &mut egui::Ui, terminal_lines: &[String]) {
    ui.heading("Network Tools Terminal");
    ui.add_space(10.0);
    ui.separator();
    ui.add_space(10.0);
    
    egui::ScrollArea::vertical().show(ui, |ui| {
        ui.vertical(|ui| {
            for line in terminal_lines {
                ui.label(egui::RichText::new(line).monospace());
            }
        });
    });
}
