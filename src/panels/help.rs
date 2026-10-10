use eframe::egui;

pub fn show(ui: &mut egui::Ui) {
    ui.heading("NetTERM System Help");
    ui.add_space(15.0);
    ui.separator();
    ui.add_space(15.0);
    
    ui.vertical(|ui| {
        ui.label(egui::RichText::new("Available System Commands:").strong().font(egui::FontId::new(16.0, egui::FontFamily::Proportional)));
        ui.add_space(10.0);
        
        ui.label(egui::RichText::new("  :editor  or  :edit     ->  Switch to the primary Code Editing panel").monospace());
        ui.label(egui::RichText::new("  :terminal or :net      ->  Open the low-level Network Diagnostics logs").monospace());
        ui.label(egui::RichText::new("  :canvas   or :paint    ->  Open the multi-color mouse drawing canvas").monospace());
        ui.label(egui::RichText::new("  :fetch                 ->  Trigger local fastfetch telemetry directly").monospace());
        ui.label(egui::RichText::new("  :calc <expression>     ->  Instantly solve formulas (e.g. :calc 5+(2*3))").monospace());
        ui.label(egui::RichText::new("  :ping <target_ip>      ->  Execute a live network ICMP diagnostic query").monospace());
        ui.label(egui::RichText::new("  :settings or :config   ->  Open theme profiles and transparency hooks").monospace());
        ui.label(egui::RichText::new("  :help                  ->  Display this core structural instruction panel").monospace());
    });
}
