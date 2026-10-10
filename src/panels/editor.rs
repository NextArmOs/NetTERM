use eframe::egui;

pub fn show(
    ui: &mut egui::Ui,
    ctx: &egui::Context,
    code_text: &mut String,
    syntax_lang: &str,
    plain_color_hex: &str,
    format_number: fn(usize) -> String,
    format_size: fn(usize) -> String,
) {
    let line_count = if code_text.is_empty() { 1 } else { code_text.lines().count() };
    let char_count = code_text.chars().count();
    let byte_size = code_text.len();

    ui.horizontal(|ui| {
        ui.label(format!("Lines: {}", format_number(line_count)));
        ui.separator();
        ui.label(format!("Characters: {}", format_number(char_count)));
        ui.separator();
        ui.label(format!("Size: {}", format_size(byte_size)));
        ui.separator();
        
        if ui.button("Open File").clicked() {
            if let Some(path) = rfd::FileDialog::new().set_title("Open Code File").pick_file() {
                if let Ok(contents) = std::fs::read_to_string(path) {
                    *code_text = contents;
                }
            }
        }
        if ui.button("Save File").clicked() {
            if let Some(path) = rfd::FileDialog::new().set_title("Save Code File").save_file() {
                let _ = std::fs::write(path, code_text.as_bytes());
            }
        }
        if ui.button("Clear").clicked() {
            code_text.clear();
        }
    });
    ui.add_space(5.0);

    let available_height = ui.available_height();
    let font_height = 18.0; 
    let visible_lines = (available_height / font_height).floor() as usize;

    egui::ScrollArea::vertical().show(ui, |ui| {
        ui.horizontal(|ui| {
            ui.vertical(|ui| {
                let lines_to_render = std::cmp::min(line_count, visible_lines + 50);
                for _ in 0..lines_to_render {
                    ui.label(egui::RichText::new(" ").monospace());
                }
                if visible_lines > line_count {
                    for _ in 0..(visible_lines - line_count) {
                        ui.label(egui::RichText::new("~").color(egui::Color32::from_gray(80)).monospace());
                    }
                }
            });

            let lang_ref = syntax_lang.to_string();
            let plain_color_owned = plain_color_hex.to_string();
            let mut layouter = move |ui: &egui::Ui, string: &str, _wrap_width: f32| {
                let layout_job = crate::highlighter::highlight_code(ui.ctx(), string, &lang_ref, &plain_color_owned);
                ui.fonts(|f| f.layout_job(layout_job))
            };

            let edit_area = egui::TextEdit::multiline(code_text)
                .font(egui::TextStyle::Monospace)
                .code_editor()
                .lock_focus(true)
                .layouter(&mut layouter);

            let response = ui.add_sized(ui.available_size(), edit_area);
            
            if code_text.is_empty() {
                let center_x = response.rect.center().x;
                let center_y = response.rect.center().y;
                
                ctx.debug_painter().text(
                    egui::pos2(center_x, center_y - 45.0),
                    egui::Align2::CENTER_CENTER,
                    "NetTERM v0.5.0",
                    egui::FontId::new(20.0, egui::FontFamily::Proportional),
                    egui::Color32::from_gray(140)
                );
                ctx.debug_painter().text(
                    egui::pos2(center_x, center_y - 15.0),
                    egui::Align2::CENTER_CENTER,
                    "Open-source text editor & net terminal\nLicensed under the MIT License",
                    egui::FontId::new(14.5, egui::FontFamily::Proportional),
                    egui::Color32::from_gray(100)
                );
                ctx.debug_painter().text(
                    egui::pos2(center_x, center_y + 25.0),
                    egui::Align2::CENTER_CENTER,
                    "Use bottom command line prompt to route applications.",
                    egui::FontId::new(14.5, egui::FontFamily::Monospace),
                    egui::Color32::from_hex("#aa75ad").unwrap()
                );
            }
        });
    });
}
