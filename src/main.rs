mod network;
mod highlighter;
mod calculator;
mod panels;

use eframe::egui;

#[derive(PartialEq, Clone, Copy)]
pub enum Tab {
    CodeEditor,
    NetworkTerminal,
    Canvas,
    Fetch,
    Calculator,
    Settings,
    Help,
}

pub struct PaintLine {
    pub points: Vec<egui::Pos2>,
    pub color: egui::Color32,
}

fn format_number(num: usize) -> String {
    let s = num.to_string();
    let mut result = String::new();
    let chars: Vec<char> = s.chars().collect();
    let len = chars.len();
    for (i, &ch) in chars.iter().enumerate() {
        result.push(ch);
        let remaining = len - 1 - i;
        if remaining > 0 && remaining % 3 == 0 { result.push(','); }
    }
    result
}

fn format_size(bytes: usize) -> String {
    if bytes < 1024 { format!("{} B", format_number(bytes)) }
    else if bytes < 1024 * 1024 { format!("{:.2} KB", bytes as f32 / 1024.0) }
    else { format!("{:.2} MB", bytes as f32 / (1024.0 * 1024.0)) }
}

fn main() -> eframe::Result<()> {
    let options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default()
            .with_inner_size([800.0, 600.0])
            .with_transparent(true),
        ..Default::default()
    };

    let mut code_text = String::new();
    let mut current_tab = Tab::CodeEditor;
    let mut target_ip = String::from("8.8.8.8");
    let mut terminal_lines: Vec<String> = Vec::new();
    let mut paint_lines: Vec<PaintLine> = Vec::new();
    let mut current_brush_color = egui::Color32::from_hex("#82a1c1").unwrap();
    let mut fetch_output = String::from("Type :fetch in the prompt below to load system metrics.");
    let mut calc_input = String::new();
    let mut calc_result = String::from("Enter expression");
    let mut command_buffer = String::new();
    let mut command_feedback = String::from("Active: Code Editor. Type :help for commands.");
    let mut active_syntax_lang = String::from("Rust");
    let mut session_plain_color = String::from("#e4e1e9");

    eframe::run_simple_native("NetTERM v0.5.0", options, move |ctx, _frame| {
        ctx.request_repaint_after(std::time::Duration::from_secs(1));

        let current_lang = active_syntax_lang.clone();
        let current_plain_color = session_plain_color.clone();

        let mut style = (*ctx.style()).clone();
        if style.text_styles.get(&egui::TextStyle::Body).map_or(true, |f| f.size != 14.5) {
            style.text_styles.insert(egui::TextStyle::Heading, egui::FontId::new(18.0, egui::FontFamily::Proportional));
            style.text_styles.insert(egui::TextStyle::Body, egui::FontId::new(14.5, egui::FontFamily::Proportional));
            style.text_styles.insert(egui::TextStyle::Monospace, egui::FontId::new(14.5, egui::FontFamily::Monospace));
            style.text_styles.insert(egui::TextStyle::Button, egui::FontId::new(14.5, egui::FontFamily::Proportional));
            ctx.set_style(style);
        }

        let final_color = egui::Color32::from_rgba_unmultiplied(19, 20, 40, 165);

        let mut visuals = egui::Visuals::dark();
        visuals.extreme_bg_color = final_color;
        visuals.panel_fill = final_color;
        visuals.window_fill = final_color;
        ctx.set_visuals(visuals);

        egui::TopBottomPanel::bottom("command_bar").exact_height(45.0).resizable(false).show(ctx, |ui| {
            ui.add_space(4.0);
            ui.horizontal(|ui| {
                ui.colored_label(egui::Color32::from_hex("#ff757f").unwrap(), egui::RichText::new("❯").font(egui::FontId::new(16.0, egui::FontFamily::Proportional)));
                let prompt = ui.add_sized([ui.available_width() - 350.0, 24.0], egui::TextEdit::singleline(&mut command_buffer).font(egui::TextStyle::Monospace).hint_text("Type command (e.g. :editor, :lang py, :calc 2+2) ..."));
                ui.separator();
                ui.label(egui::RichText::new(&command_feedback).small().color(egui::Color32::from_gray(140)));

                if prompt.lost_focus() && ui.input(|i| i.key_pressed(egui::Key::Enter)) {
                    let trimmed = command_buffer.trim();
                    if trimmed.starts_with(':') {
                        let parts: Vec<&str> = trimmed.splitn(2, ' ').collect();
                        let cmd = parts[0];
                        match cmd {
                            ":editor" | ":edit" => { current_tab = Tab::CodeEditor; command_feedback = "Switched to Code Editor.".to_string(); }
                            ":terminal" | ":net" => { current_tab = Tab::NetworkTerminal; command_feedback = "Switched to Network Terminal.".to_string(); }
                            ":canvas" | ":paint" => { current_tab = Tab::Canvas; command_feedback = "Switched to Canvas Paint Area.".to_string(); }
                            ":settings" | ":config" => { current_tab = Tab::Settings; command_feedback = "Switched to Application Settings.".to_string(); }
                            ":help" => { current_tab = Tab::Help; command_feedback = "Displaying command help summary.".to_string(); }
                            ":calculator" => { current_tab = Tab::Calculator; command_feedback = "Opened full screen calculator view.".to_string(); }
                            ":fetch" => { current_tab = Tab::Fetch; fetch_output = network::get_fastfetch_output(); command_feedback = "Executed system fetch.".to_string(); }
                            ":q" | ":exit" => { ctx.send_viewport_cmd(egui::ViewportCommand::Close); }
                            ":ping" => {
                                current_tab = Tab::NetworkTerminal;
                                if parts.len() > 1 {
                                    target_ip = parts[1].trim().to_string();
                                    terminal_lines.clear();
                                    terminal_lines.extend(network::run_ping_command(&target_ip));
                                    command_feedback = format!("Ping targeted: {}", target_ip);
                                } else { command_feedback = "Error: Use `:ping 8.8.8.8`".to_string(); }
                            }
                            ":calc" => {
                                if parts.len() > 1 {
                                    calc_input = parts[1].trim().to_string();
                                    calc_result = match calculator::evaluate_expression(&calc_input) {
                                        Ok(res_str) => res_str,
                                        Err(err) => format!("Error: {}", err),
                                    };
                                    command_feedback = format!("Evaluated: {} -> {}", calc_input, calc_result);
                                } else {
                                    current_tab = Tab::Calculator;
                                    command_feedback = "Switched to calculator tab.".to_string();
                                }
                            }
                            ":lang" => {
                                if parts.len() > 1 {
                                    let chosen_lang = parts[1].trim().to_lowercase();
                                    match chosen_lang.as_str() {
                                        "rust" | "rs" => { active_syntax_lang = "Rust".to_string(); command_feedback = "Syntax set to Rust".to_string(); }
                                        "python" | "py" => { active_syntax_lang = "Python".to_string(); command_feedback = "Syntax set to Python".to_string(); }
                                        "cpp" | "c++" | "cc" => { active_syntax_lang = "C++".to_string(); command_feedback = "Syntax set to C++".to_string(); }
                                        "plain" | "txt" | "text" => { active_syntax_lang = "Plain Text".to_string(); command_feedback = "Syntax highlighting disabled".to_string(); }
                                        _ => { command_feedback = format!("Unknown lang: '{}'", parts[1]); }
                                    }
                                    ctx.request_repaint();
                                } else {
                                    command_feedback = format!("Usage Error: Try `:lang py` (Active: {})", active_syntax_lang);
                                }
                            }
                            _ => { command_feedback = format!("Unknown command: {}", cmd); }
                        }
                    } else { command_feedback = "Command must begin with an explicit `:` prefix.".to_string(); }
                    command_buffer.clear();
                    prompt.request_focus();
                }
            });
        });

        egui::CentralPanel::default()
            .frame(egui::Frame::none().fill(final_color))
            .show(ctx, |ui| {
                match current_tab {
                    Tab::CodeEditor => panels::editor::show(
                        ui, 
                        ctx, 
                        &mut code_text, 
                        &current_lang, 
                        &current_plain_color, 
                        format_number as fn(usize) -> String, 
                        format_size as fn(usize) -> String
                    ),
                    Tab::NetworkTerminal => panels::terminal::show(ui, &terminal_lines),
                    Tab::Canvas => panels::canvas::show(ui, &mut paint_lines, &mut current_brush_color),
                    Tab::Fetch => panels::fetch::show(ui, &fetch_output),
                    Tab::Calculator => {
                        let mut evaluate_clicked = false;
                        panels::calculator::show(ui, &mut calc_input, &calc_result, &mut evaluate_clicked);
                        if evaluate_clicked {
                            calc_result = match calculator::evaluate_expression(&calc_input) {
                                Ok(res_str) => res_str,
                                Err(err) => format!("Error: {}", err),
                            };
                        }
                    }
                    Tab::Settings => panels::settings::show(ui, ctx, &mut active_syntax_lang, &mut session_plain_color),
                    Tab::Help => panels::help::show(ui),
                }
            });
    })
}
