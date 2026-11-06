#lang "fb"
#include "fbgfx.bi"

Using FB

Dim Shared As Integer mx, my, mb
Dim Shared As String currentValue
Dim Shared As Double storedValue
Dim Shared As String currentOp

ScreenRes 800, 600, 8
Width 100, 37

currentValue = ""
currentOp = ""
storedValue = 0

Sub DrawButton(x As Integer, y As Integer, w As Integer, h As Integer, text As String)
    Line (x, y)-(x + w, y + h), 7, BF
    Line (x, y)-(x + w, y + h), 15, B
    Locate (y + h / 2) / 16 + 1, (x + w / 2) / 8 - Len(text) / 2 + 1
    Print text;
End Sub

Sub UpdateDisplay()
    Line (20, 20)-(780, 90), 0, BF
    Line (20, 20)-(780, 90), 15, B
    Locate 3, 6
    If currentValue = "" Then
        Print "0";
    Else
        Print currentValue;
    End If
End Sub

Function CheckClick(cx As Integer, cy As Integer, cw As Integer, ch As Integer) As Integer
    If mb = 1 And mx >= cx And mx <= (cx + cw) And my >= cy And my <= (cy + ch) Then
        Return 1
    End If
    Return 0
End Function

Sub ApplyOp(op As String)
    If currentValue <> "" Then
        storedValue = Val(currentValue)
        currentValue = ""
        currentOp = op
        Sleep 150
    End If
End Sub

Do
    ScreenLock
    Cls
    
    UpdateDisplay()
    
    DrawButton(20,  120, 160, 90, "7"): DrawButton(200, 120, 160, 90, "8"): DrawButton(380, 120, 160, 90, "9")
    DrawButton(20,  230, 160, 90, "4"): DrawButton(200, 230, 160, 90, "5"): DrawButton(380, 230, 160, 90, "6")
    DrawButton(20,  340, 160, 90, "1"): DrawButton(200, 340, 160, 90, "2"): DrawButton(380, 340, 160, 90, "3")
    DrawButton(20,  450, 340, 90, "0"): DrawButton(380, 450, 160, 90, "C")
    
    DrawButton(560, 120, 100, 90, "+"): DrawButton(680, 120, 100, 90, "-")
    DrawButton(560, 230, 100, 90, "*"): DrawButton(680, 230, 100, 90, "/")
    DrawButton(560, 340, 220, 200, "=")
    
    GetMouse mx, my, , mb
    
    If mb = 1 Then
        If CheckClick(20, 340, 160, 90)  Then currentValue = currentValue + "1": Sleep 150
        If CheckClick(200, 340, 160, 90) Then currentValue = currentValue + "2": Sleep 150
        If CheckClick(380, 340, 160, 90) Then currentValue = currentValue + "3": Sleep 150
        If CheckClick(20, 230, 160, 90)  Then currentValue = currentValue + "4": Sleep 150
        If CheckClick(200, 230, 160, 90) Then currentValue = currentValue + "5": Sleep 150
        If CheckClick(380, 230, 160, 90) Then currentValue = currentValue + "6": Sleep 150
        if CheckClick(20, 120, 160, 90)   Then currentValue = currentValue + "7": Sleep 150
        If CheckClick(200, 120, 160, 90)  Then currentValue = currentValue + "8": Sleep 150
        If CheckClick(380, 120, 160, 90)  Then currentValue = currentValue + "9": Sleep 150
        If CheckClick(20, 450, 340, 90)  Then currentValue = currentValue + "0": Sleep 150
        
        If CheckClick(380, 450, 160, 90) Then
            currentValue = ""
            storedValue = 0
            currentOp = ""
            Sleep 150
        End If
        
        If CheckClick(560, 120, 100, 90) Then ApplyOp("+")
        If CheckClick(680, 120, 100, 90) Then ApplyOp("-")
        If CheckClick(560, 230, 100, 90) Then ApplyOp("*")
        If CheckClick(680, 230, 100, 90) Then ApplyOp("/")
        
        If CheckClick(560, 340, 220, 200) Then
            If currentOp <> "" And currentValue <> "" Then
                Dim result As Double
                If currentOp = "+" Then result = storedValue + Val(currentValue)
                If currentOp = "-" Then result = storedValue - Val(currentValue)
                If currentOp = "*" Then result = storedValue * Val(currentValue)
                If currentOp = "/" Then 
                    If Val(currentValue) <> 0 Then
                        result = storedValue / Val(currentValue)
                    Else
                        currentValue = "ERROR": currentOp = "": Sleep 150
                        Continue Do
                    End If
                End If
                currentValue = Str(result)
                currentOp = ""
                Sleep 150
            End If
        End If
    End If
    
    ScreenUnlock
    Sleep 10, 1
Loop Until MultiKey(SC_ESCAPE)
