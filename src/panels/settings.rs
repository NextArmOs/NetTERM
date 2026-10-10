use eframe::egui;

pub fn show(
    ui: &mut egui::Ui,
    ctx: &egui::Context,
    syntax_lang: &mut String,
    session_plain_color: &mut String,
) {
    ui.heading("Application Settings");
    ui.add_space(10.0);
    ui.separator();
    ui.add_space(15.0);

    ui.label("Syntax Highlighting Mode:");
    ui.horizontal(|ui| {
        if ui.selectable_value(syntax_lang, "Rust".to_string(), "Rust").changed() { ctx.request_repaint(); }
        if ui.selectable_value(syntax_lang, "Python".to_string(), "Python").changed() { ctx.request_repaint(); }
        if ui.selectable_value(syntax_lang, "C++".to_string(), "C++").changed() { ctx.request_repaint(); }
        if ui.selectable_value(syntax_lang, "Plain Text".to_string(), "Plain Text").changed() { ctx.request_repaint(); }
    });

    if syntax_lang == "Plain Text" {
        ui.add_space(15.0);
        ui.label("Temporary Plain Text Font Color (Resets after close):");
        ui.horizontal(|ui| {
            if ui.selectable_value(session_plain_color, "#e4e1e9".to_string(), "Default White").changed() { ctx.request_repaint(); }
            if ui.selectable_value(session_plain_color, "#00ff00".to_string(), "Neon Green").changed() { ctx.request_repaint(); }
            if ui.selectable_value(session_plain_color, "#00ffff".to_string(), "Cyber Blue").changed() { ctx.request_repaint(); }
            if ui.selectable_value(session_plain_color, "#ff966c".to_string(), "Retro Orange").changed() { ctx.request_repaint(); }
        });
    }
}
