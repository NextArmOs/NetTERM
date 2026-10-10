use eframe::egui;

pub fn show(ui: &mut egui::Ui, calc_input: &mut String, calc_result: &str, evaluate_btn_clicked: &mut bool) {
    ui.heading("Math Expression Calculator");
    ui.add_space(10.0);
    
    ui.horizontal(|ui| {
        ui.label("Expression:");
        if ui.text_edit_singleline(calc_input).lost_focus() && ui.input(|i| i.key_pressed(egui::Key::Enter)) {
            *evaluate_btn_clicked = true;
        }
        
        if ui.button("🟰 Calculate").clicked() {
            *evaluate_btn_clicked = true;
        }
    });
    
    ui.add_space(15.0);
    ui.separator();
    
    let available_height = ui.available_height();
    ui.add_space(available_height / 3.0);
    
    ui.vertical_centered(|ui| {
        ui.label(
            egui::RichText::new(calc_result)
                .font(egui::FontId::new(48.0, egui::FontFamily::Monospace))
                .color(egui::Color32::LIGHT_GREEN)
        );
    });
}
