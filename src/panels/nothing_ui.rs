use eframe::egui;

pub fn show(ui: &mut egui::Ui) {
    ui.heading("NothingUI Window Manager");
    ui.add_space(10.0);
    ui.separator();
    ui.label("nothingUI 0.0.1");
}
