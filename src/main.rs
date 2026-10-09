mod network;

use eframe::egui;

#[derive(PartialEq)]
enum Tab {
    CodeEditor,
    NetworkTerminal,
}

fn main() -> eframe::Result<()> {
    let options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default().with_inner_size([700.0, 530.0]),
        ..Default::default()
    };

    let mut code_text = String::new();
    let mut current_tab = Tab::CodeEditor;
    
    let mut target_ip = String::from("8.8.8.8");
    let mut terminal_lines: Vec<String> = Vec::new();

    eframe::run_simple_native("NetTERM v0.0.4", options, move |ctx, _frame| {
        let mut visuals = egui::Visuals::dark();
        visuals.extreme_bg_color = egui::Color32::from_hex("#2e2f42").unwrap();
        visuals.panel_fill = egui::Color32::from_hex("#2e2f42").unwrap();
        ctx.set_visuals(visuals);

        egui::TopBottomPanel::top("top_bar")
            .exact_height(60.0)
            .resizable(false)
            .show(ctx, |ui| {
                ui.add_space(5.0);
                ui.horizontal(|ui| {
                    ui.selectable_value(&mut current_tab, Tab::CodeEditor, "📝 Code Editor");
                    ui.selectable_value(&mut current_tab, Tab::NetworkTerminal, "🌐 Network Terminal");
                });
                ui.add_space(5.0);
                ui.separator();
            });

        egui::CentralPanel::default().show(ctx, |ui| {
            match current_tab {
                Tab::CodeEditor => {
                    let line_count = if code_text.is_empty() { 1 } else { code_text.lines().count() };
                    let char_count = code_text.chars().count();

                    ui.horizontal(|ui| {
                        ui.label(format!("Lines: {}", line_count));
                        ui.separator();
                        ui.label(format!("Characters: {}", char_count));
                    });
                    ui.add_space(5.0);

                    egui::ScrollArea::vertical().show(ui, |ui| {
                        ui.add_sized(
                            ui.available_size(),
                            egui::TextEdit::multiline(&mut code_text)
                                .font(egui::TextStyle::Monospace)
                                .code_editor()
                                .lock_focus(true),
                        );
                    });
                }
                Tab::NetworkTerminal => {
                    ui.heading("Network Tools Terminal");
                    ui.add_space(10.0);
                    
                    ui.horizontal(|ui| {
                        ui.label("Target IP:");
                        ui.text_edit_singleline(&mut target_ip);
                        
                        if ui.button("Run Ping").clicked() {
                            let result = network::run_ping_command(&target_ip);
                            terminal_lines.clear();
                            terminal_lines.extend(result);
                        }
                    });
                    
                    ui.add_space(15.0);
                    ui.separator();
                    ui.add_space(10.0);
                    
                    egui::ScrollArea::vertical().show(ui, |ui| {
                        ui.vertical(|ui| {
                            for line in &terminal_lines {
                                ui.label(egui::RichText::new(line).monospace());
                            }
                        });
                    });
                }
            }
        });
    })
}
