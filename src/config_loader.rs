use serde::{Deserialize, Serialize};
use std::fs;

#[derive(Serialize, Deserialize, Clone)]
pub struct AppConfig {
    pub bg_color: String,
    pub transparent: bool,
}

impl Default for AppConfig {
    fn default() -> Self {
        Self {
            bg_color: "#131428".to_string(),
            transparent: false,
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
