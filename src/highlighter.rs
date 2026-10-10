use eframe::egui;

pub fn highlight_rust_code(ctx: &egui::Context, code: &str) -> egui::text::LayoutJob {
    let mut job = egui::text::LayoutJob::default();
    let font_id = egui::FontId::new(14.5, egui::FontFamily::Monospace);
    
    let keywords = [
        "fn", "let", "mut", "match", "struct", "enum", "pub", "use", "mod", 
        "true", "false", "impl", "return", "if", "else", "for", "in", "while", 
        "loop", "break", "continue", "as", "async", "await", "const", "static", 
        "type", "where", "trait", "dyn", "unsafe", "move", "crate", "self", "Self"
    ];

    let types = [
        "i8", "i16", "i32", "i64", "i128", "isize",
        "u8", "u16", "u32", "u64", "u128", "usize",
        "f32", "f64", "bool", "char", "str", "String", "Vec", "Option", "Result"
    ];
    
    let mut current_word = String::new();
    let mut in_string = false;

    for ch in code.chars() {
        if in_string {
            current_word.push(ch);
            if ch == '"' {
                in_string = false;
                job.append(&current_word, 0.0, egui::TextFormat {
                    font_id: font_id.clone(),
                    color: egui::Color32::from_hex("#e6c875").unwrap(),
                    ..Default::default()
                });
                current_word.clear();
            }
        } else if ch == '"' {
            if !current_word.is_empty() {
                job.append(&current_word, 0.0, egui::TextFormat { font_id: font_id.clone(), color: egui::Color32::from_hex("#e4e1e9").unwrap(), ..Default::default() });
                current_word.clear();
            }
            in_string = true;
            current_word.push(ch);
        } else if ch.is_alphanumeric() || ch == '_' {
            current_word.push(ch);
        } else {
            if !current_word.is_empty() {
                let color = if keywords.contains(&current_word.as_str()) {
                    egui::Color32::from_hex("#ff757f").unwrap()
                } else if types.contains(&current_word.as_str()) {
                    egui::Color32::from_hex("#4fd6be").unwrap()
                } else if current_word.chars().next().unwrap().is_numeric() {
                    egui::Color32::from_hex("#ff966c").unwrap()
                } else {
                    egui::Color32::from_hex("#e4e1e9").unwrap()
                };
                
                job.append(&current_word, 0.0, egui::TextFormat { font_id: font_id.clone(), color, ..Default::default() });
                current_word.clear();
            }
            let mut symbol_str = String::new();
            symbol_str.push(ch);
            let symbol_color = match ch {
                '=' | '+' | '-' | '*' | '/' | '%' | '&' | '|' | '^' | '!' | '<' | '>' => egui::Color32::from_hex("#ff966c").unwrap(),
                _ => egui::Color32::from_hex("#82a1c1").unwrap(),
            };
            job.append(&symbol_str, 0.0, egui::TextFormat { font_id: font_id.clone(), color: symbol_color, ..Default::default() });
        }
    }
    
    if !current_word.is_empty() {
        job.append(&current_word, 0.0, egui::TextFormat { font_id: font_id.clone(), color: egui::Color32::from_hex("#e4e1e9").unwrap(), ..Default::default() });
    }
    
    job
}
