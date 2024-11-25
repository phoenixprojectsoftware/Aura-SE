@echo off
xcopy /f /y "Debug\" "C:\Program Files (x86)\Steam\steamapps\common\Half-Life\zamnhlmp\aura"
IF ERRORLEVEL 1 (
	xcopy /f /y "Debug\" "D:\Program Files (x86)\Steam\steamapps\common\Half-Life\zamnhlmp\aura"
	pause
) ELSE (
	pause
)