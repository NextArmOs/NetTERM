use eframe::egui;

pub fn highlight_code(ctx: &egui::Context, code: &str, lang: &str, plain_color_hex: &str) -> egui::text::LayoutJob {
    let mut job = egui::text::LayoutJob::default();
    let font_id = egui::FontId::new(14.5, egui::FontFamily::Monospace);
    let base_color = egui::Color32::from_hex(plain_color_hex).unwrap_or(egui::Color32::from_hex("#e4e1e9").unwrap());

    if lang == "Plain Text" {
        job.append(code, 0.0, egui::TextFormat { font_id, color: base_color, ..Default::default() });
        return job;
    }

    let keywords = match lang {
        "Rust" => vec![
            "fn", "let", "mut", "match", "struct", "enum", "pub", "use", "mod", 
            "true", "false", "impl", "return", "if", "else", "for", "in", "while",
            "loop", "break", "continue", "as", "async", "await", "const", "static",
            "type", "where", "trait", "dyn", "unsafe", "move", "crate", "self", "Self"
        ],
        "Python" => vec![
            "def", "import", "from", "as", "return", "if", "elif", "else", 
            "for", "while", "in", "is", "not", "and", "or", "True", "False", "pass",
            "try", "except", "finally", "raise", "assert", "class", "with", "lambda", "global"
        ],
        "C++" => vec![
            "int", "char", "float", "double", "void", "if", "else", "for", "while",
            "switch", "case", "return", "class", "public", "private", "namespace", "include",
            "std", "cout", "cin", "endl", "using", "template", "typename", "bool", "true", "false"
        ],
        _ => vec![],
    };

    let builtin_functions = match lang {
        "Rust" => vec!["println!", "print!", "format!", "vec!", "panic!", "assert_eq!", "unreachable!"],
        "Python" => vec!["print", "len", "range", "str", "int", "float", "list", "dict", "set", "open", "type", "append"],
        "C++" => vec!["vector", "string", "map", "set", "push_back", "size", "length", "main"],
        _ => vec![],
    };

    let mut current_word = String::new();
    let mut in_string = false;

    for ch in code.chars() {
        if in_string {
            current_word.push(ch);
            if ch == '"' || ch == '\'' {
                in_string = false;
                job.append(&current_word, 0.0, egui::TextFormat {
                    font_id: font_id.clone(),
                    color: egui::Color32::from_hex("#e6c875").unwrap(),
                    ..Default::default()
                });
                current_word.clear();
            }
        } else if ch == '"' || ch == '\'' {
            if !current_word.is_empty() {
                job.append(&current_word, 0.0, egui::TextFormat { font_id: font_id.clone(), color: base_color, ..Default::default() });
                current_word.clear();
            }
            in_string = true;
            current_word.push(ch);
        } else if ch.is_alphanumeric() || ch == '_' || ch == '!' {
            current_word.push(ch);
        } else {
            if !current_word.is_empty() {
                let color = if keywords.contains(&current_word.as_str()) {
                    egui::Color32::from_hex("#ff757f").unwrap() // Розовый для ключевых слов
                } else if builtin_functions.contains(&current_word.as_str()) {
                    egui::Color32::from_hex("#00ffff").unwrap() // Циановый для встроенных функций
                } else if current_word.chars().next().unwrap().is_numeric() {
                    egui::Color32::from_hex("#ff966c").unwrap() // Оранжевый для чисел
                } else {
                    base_color
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
        job.append(&current_word, 0.0, egui::TextFormat { font_id: font_id.clone(), color: base_color, ..Default::default() });
    }
    job
}
