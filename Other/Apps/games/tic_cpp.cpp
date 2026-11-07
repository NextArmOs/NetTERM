#include <iostream>
#include <vector>

char board[9] = {'1','2','3','4','5','6','7','8','9'};

void drawBoard() {
    std::cout << "\n " << board[0] << " | " << board[1] << " | " << board[2] << " \n";
    std::cout << "---|---|---\n";
    std::cout << " " << board[3] << " | " << board[4] << " | " << board[5] << " \n";
    std::cout << "---|---|---\n";
    std::cout << " " << board[6] << " | " << board[7] << " | " << board[8] << " \n\n";
}

int checkWin() {
    int wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
    for (int i = 0; i < 8; i++) {
        if (board[wins[i][0]] == board[wins[i][1]] && board[wins[i][1]] == board[wins[i][2]])
            return 1;
    }
    return 0;
}

int main() {
    int choice, player = 1;
    char mark;
    for (int turn = 0; turn < 9; turn++) {
        drawBoard();
        player = (turn % 2 == 0) ? 1 : 2;
        mark = (player == 1) ? 'X' : 'O';
        std::cout << "Player " << player << " (" << mark << "), enter a number (1-9): ";
        std::cin >> choice;
        if (choice < 1 || choice > 9 || board[choice-1] == 'X' || board[choice-1] == 'O') {
            std::cout << "Invalid move!\n";
            turn--;
            continue;
        }
        board[choice-1] = mark;
        if (checkWin()) {
            drawBoard();
            std::cout << "🎉 Player " << player << " wins!\n";
            return 0;
        }
    }
    drawBoard();
    std::cout << "🤝 It's a draw!\n";
    return 0;
}
