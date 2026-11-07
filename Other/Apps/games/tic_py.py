import sys

board = [str(i) for i in range(1, 10)]

def draw_board():
    print(f"\n {board[0]} | {board[1]} | {board[2]} ")
    print("---|---|---")
    print(f" {board[3]} | {board[4]} | {board[5]} ")
    print("---|---|---")
    print(f" {board[6]} | {board[7]} | {board[8]} \n")

def check_win():
    wins = [[0,1,2],[3,4,5],[6,7,8],[0,3,6],[1,4,7],[2,5,8],[0,4,8],[2,4,6]]
    for w in wins:
        if board[w[0]] == board[w[1]] == board[w[2]]:
            return True
    return False

turn = 0
while turn < 9:
    draw_board()
    player = 1 if turn % 2 == 0 else 2
    mark = 'X' if player == 1 else 'O'
    try:
        choice = int(input(f"Player {player} ({mark}), enter a number (1-9): "))
        if choice < 1 or choice > 9 or board[choice-1] in ['X', 'O']:
            print("Invalid move!")
            continue
    except ValueError:
        print("Invalid move!")
        continue
    
    board[choice-1] = mark
    if check_win():
        draw_board()
        print(f"🎉 Player {player} wins!")
        sys.exit(0)
    turn += 1

draw_board()
print("🤝 It's a draw!")
