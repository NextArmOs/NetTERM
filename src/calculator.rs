pub fn evaluate_expression(expr: &str) -> Result<f64, String> {
    let clean: String = expr.chars().filter(|c| !c.is_whitespace()).collect();
    if clean.is_empty() {
        return Err("Empty expression".to_string());
    }
    parse_addition_subtraction(&clean)
}

fn parse_addition_subtraction(expr: &str) -> Result<f64, String> {
    let mut parts = Vec::new();
    let mut ops = Vec::new();
    let mut current = String::new();

    for ch in expr.chars() {
        if ch == '+' || ch == '-' {
            parts.push(current.clone());
            ops.push(ch);
            current.clear();
        } else {
            current.push(ch);
        }
    }
    parts.push(current);

    let mut result = parse_multiplication_division(&parts[0])?;
    for i in 0..ops.len() {
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

    let mut result = parts[0].parse::<f64>().map_err(|_| "Invalid number format".to_string())?;
    for i in 0..ops.len() {
        let next_val = parts[i + 1].parse::<f64>().map_err(|_| "Invalid number format".to_string())?;
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

