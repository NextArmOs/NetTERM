#lang "fb"

Dim Shared As Byte board(0 To 8) = {49,50,51,52,53,54,55,56,57}

Sub DrawBoard()
    Print ""
    Print " " & Chr(board(0)) & " | " & Chr(board(1)) & " | " & Chr(board(2))
    Print "---|---|---"
    Print " " & Chr(board(3)) & " | " & Chr(board(4)) & " | " & Chr(board(5))
    Print "---|---|---"
    Print " " & Chr(board(6)) & " | " & Chr(board(7)) & " | " & Chr(board(8))
    Print ""
End Sub

Function CheckWin() As Integer
    Dim As Integer wins(0 To 7, 0 To 2) = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}}
    For i As Integer = 0 To 7
        If board(wins(i,0)) = board(wins(i,1)) And board(wins(i,1)) = board(wins(i,2)) Then Return 1
    Next
    Return 0
End Function

Dim As Integer choice, player, turn = 0
Dim As Byte mark

Do While turn < 9
    DrawBoard()
    If turn Mod 2 = 0 Then player = 1 Else player = 2
    If player = 1 Then mark = 88 Else mark = 79
    Input "Player " & player & " (" & Chr(mark) & "), enter a number (1-9): ", choice
    If choice < 1 Or choice > 9 Or board(choice-1) = 88 Or board(choice-1) = 79 Then
        Print "Invalid move!"
    Else
        board(choice-1) = mark
        If CheckWin() = 1 Then
            DrawBoard()
            Print "🎉 Player " & player & " wins!"
            End
        End If
        turn += 1
    End If
Loop
DrawBoard()
Print "🤝 It's a draw!"
