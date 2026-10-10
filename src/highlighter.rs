use eframe::egui;

pub fn highlight_code(ctx: &egui::Context, code: &str, lang: &str, plain_color_hex: &str) -> egui::text::LayoutJob {
    let mut job = egui::text::LayoutJob::default();
    let font_id = egui::FontId::new(14.5, egui::FontFamily::Monospace);
    let base_color = egui::Color32::from_hex(plain_color_hex).unwrap_or(egui::Color32::from_hex("#e4e1e9").unwrap());

    if lang == "Plain Text" {
        job.append(code, 0.0, egui::TextFormat { font_id, color: base_color, ..Default::default() });
        return job;
    }

    let control_flow = ["while", "for", "loop", "if", "else", "match", "case", "elif", "switch", "break", "continue"];
    let booleans = ["true", "false", "True", "False"];

    let keywords = match lang {
        "Rust" => vec![
            "fn", "let", "mut", "struct", "enum", "pub", "use", "mod", 
            "impl", "return", "as", "async", "await", "const", "static", "type", "where", 
            "trait", "dyn", "unsafe", "move", "crate", "self", "Self", "ref", 
            "extern", "macro", "super", "yield"
        ],
        "Python" => vec![
            "None", "and", "as", "assert", "async", "await", "class", "def", "del", 
            "except", "finally", "from", "global", "import", "in", "is", 
            "lambda", "nonlocal", "not", "or", "pass", "raise", "return", "try", 
            "with", "yield"
        ],
        "C++" => vec![
            "auto", "catch", "class", "const", "consteval", 
            "constexpr", "constinit", "co_await", "co_return", "co_yield", 
            "decltype", "default", "delete", "do", "enum", "explicit", 
            "export", "extern", "friend", "goto", "inline", "mutable", 
            "namespace", "new", "noexcept", "operator", "private", "protected", 
            "public", "register", "reinterpret_cast", "requires", "return", "sizeof", 
            "static", "static_assert", "static_cast", "struct", "template", 
            "this", "thread_local", "throw", "try", "typedef", "typeid", "typename", 
            "union", "using", "virtual", "void", "volatile"
        ],
        _ => vec!["and", "or", "not"],
    };

    let builtin_functions = match lang {
        "Rust" => vec!["println!", "print!", "format!", "vec!", "panic!", "assert_eq!", "unreachable!"],
        "Python" => vec!["print", "len", "range", "str", "int", "float", "list", "dict", "set", "open", "type", "append"],
        "C++" => vec!["vector", "string", "map", "set", "push_back", "size", "length", "main", "cout", "cin", "endl"],
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
                let color = if control_flow.contains(&current_word.as_str()) {
                    egui::Color32::from_hex("#aa75ad").unwrap() // Purple for loops & control flow
                } else if booleans.contains(&current_word.as_str()) {
                    egui::Color32::from_hex("#9eccf5").unwrap() // Cyan-Blue for booleans
                } else if keywords.contains(&current_word.as_str()) {
                    egui::Color32::from_hex("#ff757f").unwrap() // Default Pink for core keywords
                } else if builtin_functions.contains(&current_word.as_str()) {
                    egui::Color32::from_hex("#00ffff").unwrap()
                } else if current_word.chars().next().unwrap().is_numeric() {
                    egui::Color32::from_hex("#ff966c").unwrap()
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
        let color = if control_flow.contains(&current_word.as_str()) {
            egui::Color32::from_hex("#aa75ad").unwrap()
        } else if booleans.contains(&current_word.as_str()) {
            egui::Color32::from_hex("#9eccf5").unwrap()
        } else if keywords.contains(&current_word.as_str()) {
            egui::Color32::from_hex("#ff757f").unwrap()
        } else {
            base_color
        };
        job.append(&current_word, 0.0, egui::TextFormat { font_id: font_id.clone(), color, ..Default::default() });
    }
    job
}
