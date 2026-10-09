use std::net::IpAddr;
use std::time::Duration;

pub fn run_ping_command(ip_str: &str) -> Vec<String> {
    let mut lines = Vec::new();
    lines.push(format!("PING {} ({}) 56(84) bytes of data.", ip_str, ip_str));

    let ip: IpAddr = match ip_str.parse() {
        Ok(parsed_ip) => parsed_ip,
        Err(_) => {
            lines.push("Error: Invalid IP address format".to_string());
            return lines;
        }
    };

    let timeout = Duration::from_secs(2);
    
    match ping::ping(ip, Some(timeout), None, None, None, None) {
        Ok(_) => {
            lines.push(format!("64 bytes from {}: icmp_seq=1 time=stable", ip_str));
            lines.push(format!("--- {} ping statistics ---", ip_str));
            lines.push("1 packets transmitted, 1 received, 0% packet loss".to_string());
        }
        Err(e) => {
            lines.push(format!("Ping failed: {:?}", e));
        }
    }
    
    lines
}
