Set objShell = CreateObject("Shell.Application")
Set FSO = CreateObject("Scripting.FileSystemObject")
strPath = WScript.Arguments.Item(0)
If FSO.FileExists(strPath) Then
    objShell.ShellExecute strPath, "", "", "runas", 0

    While FSO.FileExists(strPath)
        WScript.Sleep 2000
        FSO.DeleteFile(strPath)
        On Error Resume Next
    WEnd
Else
    MsgBox strPath & " not found"
End If