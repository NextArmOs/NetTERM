mod network;
mod highlighter;
mod calculator;
mod config_loader;

use eframe::egui;
use std::fs::File;
use std::io::{Read, Write};

#[derive(PartialEq)]
enum Tab {
    CodeEditor,
    NetworkTerminal,
    Canvas,
    Fetch,
    Calculator,
    Settings,
}

struct PaintLine {
    points: Vec<egui::Pos2>,
    color: egui::Color32,
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

fn format_calculator_result(val: f64) -> String {
    let integral = val.trunc() as i64;
    let fraction = val.fract().abs();
    
    let sign = if integral < 0 || (integral == 0 && val < 0.0) { "-" } else { "" };
    let abs_integral = integral.abs().to_string();
    
    let mut formatted_integral = String::new();
    let chars: Vec<char> = abs_integral.chars().collect();
    let len = chars.len();
    
    for (i, &ch) in chars.iter().enumerate() {
        formatted_integral.push(ch);
        let remaining = len - 1 - i;
        if remaining > 0 && remaining % 3 == 0 {
            formatted_integral.push(',');
        }
    }
    
    if fraction > 0.00001 {
        let frac_str = format!("{:.4}", fraction);
        format!("{}{}{}", sign, formatted_integral, &frac_str[1..])
    } else {
        format!("{}{}", sign, formatted_integral)
    }
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

fn parse_hex_color(hex_str: &str) -> egui::Color32 {
    let clean_hex = hex_str.trim_start_matches('#');
    if let Ok(color) = egui::Color32::from_hex(clean_hex) {
        color
    } else if let Ok(color) = egui::Color32::from_hex(&format!("#{}", clean_hex)) {
        color
    } else {
        egui::Color32::from_rgb(19, 20, 40)
    }
}

fn main() -> eframe::Result<()> {
    let app_config = config_loader::load_config();
    
    let options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default()
            .with_inner_size([800.0, 600.0])
            .with_transparent(app_config.transparent),
        ..Default::default()
    };

    let mut code_text = String::new();
    let mut current_tab = Tab::CodeEditor;
    
    let mut target_ip = String::from("8.8.8.8");
    let mut terminal_lines: Vec<String> = Vec::new();
    
    let mut paint_lines: Vec<PaintLine> = Vec::new();
    let mut current_brush_color = egui::Color32::from_hex("#82a1c1").unwrap();
    
    let mut fetch_output = String::from("Click 'Run Fetch' to load system information.");
    
    let mut calc_input = String::new();
    let mut calc_result = String::from("Enter expression");
    
    let mut mutable_config = app_config;
    let mut session_plain_color = String::from("#e4e1e9");

    eframe::run_simple_native("NetTERM v0.4.0", options, move |ctx, _frame| {
        let current_lang = mutable_config.syntax_lang.clone();
        let current_plain_color = session_plain_color.clone();

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
        let base_color = parse_hex_color(&mutable_config.bg_color);
        
        let final_color = if mutable_config.transparent {
            base_color.gamma_multiply(0.6)
        } else {
            base_color
        };

        visuals.extreme_bg_color = final_color;
        visuals.panel_fill = final_color;
        ctx.set_visuals(visuals);

        egui::TopBottomPanel::top("top_bar")
            .exact_height(60.0)
            .resizable(false)
            .show(ctx, |ui| {
                ui.add_space(5.0);
                ui.horizontal(|ui| {
                    ui.selectable_value(&mut current_tab, Tab::CodeEditor, "📝 Code Editor");
                    ui.selectable_value(&mut current_tab, Tab::NetworkTerminal, "🌐 Network Terminal");
                    ui.selectable_value(&mut current_tab, Tab::Canvas, "🎨 Paint");
                    ui.selectable_value(&mut current_tab, Tab::Fetch, "💻 Fetch");
                    ui.selectable_value(&mut current_tab, Tab::Calculator, "Calculator");
                    ui.selectable_value(&mut current_tab, Tab::Settings, "Settings");
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
                        
                        if ui.button("Open File").clicked() {
                            if let Some(path) = rfd::FileDialog::new()
                                .set_title("Open Code File")
                                .pick_file() 
                            {
                                if let Ok(mut file) = File::open(path) {
                                    let mut contents = String::new();
                                    if file.read_to_string(&mut contents).is_ok() {
                                        code_text = contents;
                                    }
                                }
                            }
                        }

                        if ui.button("Save File").clicked() {
                            if let Some(path) = rfd::FileDialog::new()
                                .set_title("Save Your Code File")
                                .save_file() 
                            {
                                if let Ok(mut file) = File::create(path) {
                                    let _ = file.write_all(code_text.as_bytes());
                                }
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
                                for _ in 0..line_count {
                                    ui.label(egui::RichText::new(" ").monospace());
                                }
                                if visible_lines > line_count {
                                    for _ in 0..(visible_lines - line_count) {
                                        ui.label(egui::RichText::new("~").color(egui::Color32::from_gray(80)).monospace());
                                    }
                                }
                            });

                            let mut layouter = move |ui: &egui::Ui, string: &str, _wrap_width: f32| {
                                let layout_job = highlighter::highlight_code(ui.ctx(), string, &current_lang, &current_plain_color);
                                ui.fonts(|f| f.layout_job(layout_job))
                            };

                            let edit_area = egui::TextEdit::multiline(&mut code_text)
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
                                    "NetTERM v0.4.0",
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
                                    "🔗 https://github.com/NextArmOs/NetTERM/tree/main",
                                    egui::FontId::new(14.5, egui::FontFamily::Monospace),
                                    egui::Color32::from_hex("#82a1c1").unwrap()
                                );
                            }
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
                Tab::Canvas => {
                    ui.horizontal(|ui| {
                        ui.heading("Paint Area");
                        ui.separator();
                        if ui.button("Clear Canvas").clicked() {
                            paint_lines.clear();
                        }
                        ui.separator();
                        ui.label("Brush Color:");
                        ui.selectable_value(&mut current_brush_color, egui::Color32::from_hex("#82a1c1").unwrap(), "Default");
                        ui.selectable_value(&mut current_brush_color, egui::Color32::from_hex("#00ff00").unwrap(), "Green");
                        ui.selectable_value(&mut current_brush_color, egui::Color32::from_hex("#00ffff").unwrap(), "Blue");
                        ui.selectable_value(&mut current_brush_color, egui::Color32::from_hex("#ff966c").unwrap(), "Orange");
                        ui.selectable_value(&mut current_brush_color, egui::Color32::from_hex("#ff757f").unwrap(), "Pink");
                    });
                    ui.add_space(5.0);
                    ui.separator();

                    let (response, painter) = ui.allocate_painter(ui.available_size(), egui::Sense::drag());
                    if let Some(pointer_pos) = response.interact_pointer_pos() {
                        if response.dragged_by(egui::PointerButton::Primary) {
                            if response.drag_started_by(egui::PointerButton::Primary) {
                                paint_lines.push(PaintLine {
                                    points: vec![pointer_pos],
                                    color: current_brush_color,
                                });
                            } else if let Some(last_line) = paint_lines.last_mut() {
                                if last_line.points.last() != Some(&pointer_pos) {
                                    last_line.points.push(pointer_pos);
                                }
                            }
                        }
                    }

                    for line in &paint_lines {
                        if line.points.len() >= 2 {
                            painter.add(egui::Shape::line(line.points.clone(), egui::Stroke::new(2.5, line.color)));
                        }
                    }
                }
                Tab::Fetch => {
                    ui.horizontal(|ui| {
                        ui.heading("System Information Fetch");
                        if ui.button("Run Fetch").clicked() {
                            fetch_output = network::get_fastfetch_output();
                        }
                    });
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
                Tab::Calculator => {
                    ui.heading("Math Calculator");
                    ui.add_space(10.0);
                    ui.horizontal(|ui| {
                        ui.label("Expression:");
                        ui.text_edit_singleline(&mut calc_input);
                    if ui.button("🟰 Calculate").clicked() {
                        calc_result = match calculator::evaluate_expression(&calc_input) {
                            Ok(res_str) => res_str,
                            Err(err) => format!("Error: {}", err),
                        };
                    }

                    });
                    ui.add_space(15.0);
                    ui.separator();
                    let available_height = ui.available_height();
                    ui.add_space(available_height / 3.0);
                    ui.vertical_centered(|ui| {
                        ui.label(
                            egui::RichText::new(&calc_result)
                                .font(egui::FontId::new(48.0, egui::FontFamily::Monospace))
                                .color(egui::Color32::LIGHT_GREEN)
                        );
                    });
                }
                Tab::Settings => {
                    ui.heading("Application Settings");
                    ui.add_space(10.0);
                    ui.separator();
                    ui.add_space(10.0);
                    let mut changed = false;
                    if ui.checkbox(&mut mutable_config.transparent, "Enable Window Transparency").changed() {
                        changed = true;
                    }
                    ui.add_space(15.0);
                    ui.label("Select Window Background Theme Color:");
                    ui.horizontal(|ui| {
                        if ui.selectable_value(&mut mutable_config.bg_color, "#131428".to_string(), "Deep Navy (#131428)").changed() { changed = true; }
                        if ui.selectable_value(&mut mutable_config.bg_color, "#2e2f42".to_string(), "Tokyo Dark (#2e2f42)").changed() { changed = true; }
                        if ui.selectable_value(&mut mutable_config.bg_color, "#1e1e1e".to_string(), "Dark (#1e1e1e)").changed() { changed = true; }
                    });
                    ui.add_space(20.0);
                    ui.label("Syntax Highlighting Mode:");
                    ui.horizontal(|ui| {
                        if ui.selectable_value(&mut mutable_config.syntax_lang, "Rust".to_string(), "Rust").changed() { changed = true; }
                        if ui.selectable_value(&mut mutable_config.syntax_lang, "Python".to_string(), "Python").changed() { changed = true; }
                        if ui.selectable_value(&mut mutable_config.syntax_lang, "C++".to_string(), "C++").changed() { changed = true; }
                        if ui.selectable_value(&mut mutable_config.syntax_lang, "Plain Text".to_string(), "Plain Text").changed() { changed = true; }
                    });
                    if mutable_config.syntax_lang == "Plain Text" {
                        ui.add_space(10.0);
                        ui.label("Temporary Plain Text Font Color (Resets after close):");
                        ui.horizontal(|ui| {
                            ui.selectable_value(&mut session_plain_color, "#e4e1e9".to_string(), "Default White");
                            ui.selectable_value(&mut session_plain_color, "#00ff00".to_string(), "Green");
                            ui.selectable_value(&mut session_plain_color, "#00ffff".to_string(), "Blue");
                            ui.selectable_value(&mut session_plain_color, "#ff966c".to_string(), "Orange");
                            }
                        );
                        }
                        if changed {config_loader::save_config(&mutable_config);
                        }
                        }
                        }
                        }
                    );
                    }
                )
                }
