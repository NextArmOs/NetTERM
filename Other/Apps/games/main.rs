use std::io::{self, Write};

fn draw_board(board: &[char; 9]) {
    println!("\n {} | {} | {} ", board[0], board[1], board[2]);
    println!("---|---|---");
    println!(" {} | {} | {} ", board[3], board[4], board[5]);
    println!("---|---|---");
    println!(" {} | {} | {} \n", board[6], board[7], board[8]);
}

fn check_win(board: &[char; 9]) -> bool {
    let wins = [[0,1,2],[3,4,5],[6,7,8],[0,3,6],[1,4,7],[2,5,8],[0,4,8],[2,4,6]];
    for w in wins.iter() {
        if board[w[0]] == board[w[1]] && board[w[1]] == board[w[2]] {
            return true;
        }
    }
    false
}

fn main() {
    let mut board = ['1','2','3','4','5','6','7','8','9'];
    let mut turn = 0;
    while turn < 9 {
        draw_board(&board);
        let player = if turn % 2 == 0 { 1 } else { 2 };
        let mark = if player == 1 { 'X' } else { 'O' };
        print!("Player {} ({}), enter a number (1-9): ", player, mark);
        io::stdout().flush().unwrap();
        let mut input = String::new();
        io::stdin().read_line(&mut input).unwrap();
        let choice: usize = match input.trim().parse() {
            Ok(num) if num >= 1 && num <= 9 => num,
            _ => {
                println!("Invalid move!");
                continue;
            }
        };
        if board[choice-1] == 'X' || board[choice-1] == 'O' {
            println!("Invalid move!");
            continue;
        }
        board[choice-1] = mark;
        if check_win(&board) {
            draw_board(&board);
            println!("🎉 Player {} wins!", player);
            return;
        }
        turn += 1;
    }
    draw_board(&board);
    println!("🤝 It's a draw!");
}
