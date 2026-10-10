pub fn evaluate_expression(expr: &str) -> Result<String, String> {
    let clean: String = expr.chars().filter(|c| !c.is_whitespace()).collect();
    if clean.is_empty() {
        return Err("Empty expression".to_string());
    }
    let val = parse_parentheses(&clean)?;
    Ok(format_calculator_result(val))
}

fn parse_parentheses(expr: &str) -> Result<f64, String> {
    let mut current = expr.to_string();
    
    while let Some(open_idx) = current.rfind('(') {
        let close_idx = current[open_idx..].find(')');
        if let Some(offset_close) = close_idx {
            let actual_close_idx = open_idx + offset_close;
            let sub_expr = &current[open_idx + 1..actual_close_idx];
            let sub_result = parse_addition_subtraction(sub_expr)?;
            
            current = format!(
                "{}{}{}",
                &current[..open_idx],
                sub_result,
                &current[actual_close_idx + 1..]
            );
        } else {
            return Err("Mismatched parentheses".to_string());
        }
    }
    
    if current.contains(')') {
        return Err("Mismatched parentheses".to_string());
    }
    
    parse_addition_subtraction(&current)
}

fn parse_addition_subtraction(expr: &str) -> Result<f64, String> {
    if expr.is_empty() {
        return Ok(0.0);
    }
    
    let mut parts = Vec::new();
    let mut ops = Vec::new();
    let mut current = String::new();

    let mut chars = expr.chars().peekable();
    while let Some(ch) = chars.next() {
        if (ch == '+' || ch == '-') && !current.is_empty() && current != "-" {
            parts.push(current.clone());
            ops.push(ch);
            current.clear();
        } else {
            current.push(ch);
        }
    }
    parts.push(current);

    if parts.is_empty() {
        return Err("Invalid syntax".to_string());
    }

    let mut result = parse_multiplication_division(&parts[0])?;
    for i in 0..ops.len() {
        if i + 1 >= parts.len() {
            return Err("Invalid sequence".to_string());
        }
        let next_val = parse_multiplication_division(&parts[i + 1])?;
        if ops[i] == '+' {
            result += next_val;
        } else {
            result -= next_val;
        }
    }
    Ok(result)
}

fn parse_multiplication_division(expr: &str) -> Result<f64, String> {
    let mut parts = Vec::new();
    let mut ops = Vec::new();
    let mut current = String::new();

    for ch in expr.chars() {
        if ch == '*' || ch == '/' {
            parts.push(current.clone());
            ops.push(ch);
            current.clear();
        } else {
            current.push(ch);
        }
    }
    parts.push(current);

    if parts.is_empty() || parts[0].is_empty() {
        return Err("Missing operand".to_string());
    }
    
    let mut result = parts[0].parse::<f64>().map_err(|_| "Invalid number".to_string())?;
    
    for i in 0..ops.len() {
        if parts[i + 1].is_empty() {
            return Err("Missing operand".to_string());
        }
        let next_val = parts[i + 1].parse::<f64>().map_err(|_| "Invalid number".to_string())?;
        if ops[i] == '*' {
            result *= next_val;
        } else {
            if next_val == 0.0 {
                return Err("Division by zero".to_string());
            }
            result /= next_val;
        }
    }
    Ok(result)
}

fn format_calculator_result(val: f64) -> String {
    if val.is_nan() || val.is_infinite() {
        return "Error".to_string();
    }

    let sign = if val < 0.0 { "-" } else { "" };
    let abs_val = val.abs();
    
    let integral = abs_val.trunc() as i64;
    let fraction = abs_val.fract();
    
    let abs_integral_str = integral.to_string();
    let mut formatted_integral = String::new();
    let chars: Vec<char> = abs_integral_str.chars().collect();
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
