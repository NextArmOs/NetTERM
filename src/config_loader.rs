use serde::{Deserialize, Serialize};
use std::fs;

#[derive(Serialize, Deserialize, Clone)]
pub struct AppConfig {
    pub bg_color: String,
    pub transparent: bool,
    pub syntax_lang: String,
}

impl Default for AppConfig {
    fn default() -> Self {
        Self {
            bg_color: "#131428".to_string(),
            transparent: false,
            syntax_lang: "Rust".to_string(),
        }
    }
}

pub fn load_config() -> AppConfig {
    if let Ok(content) = fs::read_to_string("app_config.toml") {
        if let Ok(config) = toml::from_str(&content) {
            return config;
        }
    }
    AppConfig::default()
}

pub fn save_config(config: &AppConfig) {
    if let Ok(content) = toml::to_string(config) {
        let _ = fs::write("app_config.toml", content);
    }
}
