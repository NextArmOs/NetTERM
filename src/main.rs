use eframe::egui;

fn main() -> eframe::Result<()> {
    let options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default().with_inner_size([700.0, 500.0]),
        ..Default::default()
    };

    let mut code_text = String::new();

    eframe::run_simple_native("NetTERM - Code Editor 0.0.3", options, move |ctx, _frame| {
        let mut visuals = egui::Visuals::dark();
        visuals.extreme_bg_color = egui::Color32::from_hex("#2e2f42").unwrap();
        visuals.panel_fill = egui::Color32::from_hex("#2e2f42").unwrap();
        ctx.set_visuals(visuals);

        egui::CentralPanel::default().show(ctx, |ui| {
            ui.heading("NetTERM Code Editor");
            ui.separator();

            let line_count = if code_text.is_empty() { 
                1 
            } else { 
                code_text.lines().count() 
            };
            
            let char_count = code_text.chars().count();

            ui.horizontal(|ui| {
                ui.label(format!("Lines: {}", line_count));
                ui.separator();
                ui.label(format!("Characters: {}", char_count));
            });

            ui.separator();

            egui::ScrollArea::vertical().show(ui, |ui| {
                ui.add_sized(
                    ui.available_size(),
                    egui::TextEdit::multiline(&mut code_text)
                        .font(egui::TextStyle::Monospace)
                        .code_editor()
                        .desired_rows(20)
                        .lock_focus(true),
                );
            });
        });
    })
}
