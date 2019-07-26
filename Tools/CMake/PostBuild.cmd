@echo off

set batDir=%~dp0
shift & set OutDir=%1


cd /d %batDir%
copy /y  %OutDir%\nsCurl.dll      ..\..\publish
copy /y  %OutDir%\nsDM.dll        ..\..\publish
copy /y  %OutDir%\nsUtil.dll      ..\..\publish