@echo off

if not exist Release mkdir Release

echo(
echo Copying dll's...
echo(
rem make sure no trash dll will left
del /q /f Release\*.dll
rem copy dll
xcopy /y 3rd\Release-Bin32\*.dll Release\*.dll

echo(
echo Copying Data...
echo(
rem make sure no trash Data will left
if exist Release\Data rmdir /q /s Release\Data
rem copy Data
xcopy /s /y /c /i Game\Data Release\Data

echo(
echo Copying launcher...
echo(
xcopy /s /y /c /i "../Windows Launcher/Bin-Release-32" "Release"

echo(
echo Done.
echo(

pause