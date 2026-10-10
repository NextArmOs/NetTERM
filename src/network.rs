use std::net::IpAddr;
use std::time::{Duration, Instant};
use std::process::Command;

pub fn run_ping_command(ip_str: &str) -> Vec<String> {
    let mut lines = Vec::new();
    let ip: IpAddr = match ip_str.parse() {
        Ok(parsed_ip) => parsed_ip,
        Err(_) => {
            lines.push("Error: Invalid IP address format".to_string());
            return lines;
        }
    };

    lines.push(format!("PING {} ({}) 56(84) bytes of data.", ip_str, ip));
    let timeout = Duration::from_secs(2);
    let start_time = Instant::now();
    
    match ping::ping(ip, Some(timeout), None, None, None, None) {
        Ok(_) => {
            let duration = start_time.elapsed();
            lines.push(format!("64 bytes from {}: icmp_seq=1 time={:.2?}", ip_str, duration));
            lines.push(format!("--- {} ping statistics ---", ip_str));
            lines.push("1 packets transmitted, 1 received, 0% packet loss".to_string());
        }
        Err(e) => {
            lines.push(format!("Ping failed: {:?}", e));
            lines.push(format!("--- {} ping statistics ---", ip_str));
            lines.push("1 packets transmitted, 0 received, 100% packet loss".to_string());
        }
    }
    lines
}

pub fn get_fastfetch_output() -> String {
    let output = Command::new("fastfetch")
        .arg("--pipe")
        .arg("true")
        .output();

    match output {
        Ok(out) => {
            let stdout_str = String::from_utf8_lossy(&out.stdout).to_string();
            if stdout_str.is_empty() {
                String::from_utf8_lossy(&out.stderr).to_string()
            } else {
                stdout_str
            }
        }
        Err(_) => "Error: fastfetch command not found. Please install it using pacman.".to_string(),
    }
}

