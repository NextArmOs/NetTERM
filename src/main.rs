mod network;

use eframe::egui;
use std::fs::File;
use std::io::Write;

#[derive(PartialEq)]
enum Tab {
    CodeEditor,
    NetworkTerminal,
}

fn format_number(num: usize) -> String {
    let s = num.to_string();
    let mut result = String::new();
    let chars: Vec<char> = s.chars().collect();
    let len = chars.len();
    
    for (i, &ch) in chars.iter().enumerate() {
        result.push(ch);
        let remaining = len - 1 - i;
        if remaining > 0 && remaining % 3 == 0 {
            result.push(',');
        }
    }
    result
}

fn format_size(bytes: usize) -> String {
    if bytes < 1024 {
        format!("{} B", format_number(bytes))
    } else if bytes < 1024 * 1024 {
        format!("{:.2} KB", bytes as f32 / 1024.0)
    } else {
        format!("{:.2} MB", bytes as f32 / (1024.0 * 1024.0))
    }
}

fn main() -> eframe::Result<()> {
    let options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default().with_inner_size([750.0, 550.0]),
        ..Default::default()
    };

    let mut code_text = String::new();
    let mut current_tab = Tab::CodeEditor;
    
    let mut target_ip = String::from("8.8.8.8");
    let mut terminal_lines: Vec<String> = Vec::new();

    eframe::run_simple_native("NetTERM v0.1.5", options, move |ctx, _frame| {
        let mut style = (*ctx.style()).clone();
        let needs_update = style.text_styles.get(&egui::TextStyle::Body)
            .map_or(true, |font| font.size != 14.5);

        if needs_update {
            style.text_styles.insert(egui::TextStyle::Heading, egui::FontId::new(18.0, egui::FontFamily::Proportional));
            style.text_styles.insert(egui::TextStyle::Body, egui::FontId::new(14.5, egui::FontFamily::Proportional));
            style.text_styles.insert(egui::TextStyle::Monospace, egui::FontId::new(14.5, egui::FontFamily::Monospace));
            style.text_styles.insert(egui::TextStyle::Button, egui::FontId::new(14.5, egui::FontFamily::Proportional));
            ctx.set_style(style);
        }

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
                    let byte_size = code_text.len();

                    ui.horizontal(|ui| {
                        ui.label(format!("Lines: {}", format_number(line_count)));
                        ui.separator();
                        ui.label(format!("Characters: {}", format_number(char_count)));
                        ui.separator();
                        ui.label(format!("Size: {}", format_size(byte_size)));
                        ui.separator();
                        
                        if ui.button("💾 Save File").clicked() {
                            if let Some(path) = rfd::FileDialog::new()
                                .set_title("Save Your Code File")
                                .save_file() 
                            {
                                if let Ok(mut file) = File::create(path) {
                                    let _ = file.write_all(code_text.as_bytes());
                                }
                            }
                        }
                    });
                    ui.add_space(5.0);

                    let available_height = ui.available_height();
                    let font_height = 18.0; 
                    let visible_lines = (available_height / font_height).floor() as usize;

                    egui::ScrollArea::vertical().show(ui, |ui| {
                        ui.horizontal(|ui| {
                            ui.vertical(|ui| {
                                for _ in 0..line_count {
                                    ui.label(egui::RichText::new(" ").monospace());
                                }
                                if visible_lines > line_count {
                                    for _ in 0..(visible_lines - line_count) {
                                        ui.label(egui::RichText::new("~").color(egui::Color32::from_gray(80)).monospace());
                                    }
                                }
                            });

                            ui.add_sized(
                                ui.available_size(),
                                egui::TextEdit::multiline(&mut code_text)
                                    .font(egui::TextStyle::Monospace)
                                    .code_editor()
                                    .lock_focus(true),
                            );
                        });
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
