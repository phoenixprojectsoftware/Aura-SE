@echo off
xcopy /f /y "Release\" "C:\Program Files (x86)\Steam\steamapps\common\Half-Life\zamnhlmp_dev\aura"
IF ERRORLEVEL 1 (
	xcopy /f /y "Release\" "D:\Program Files (x86)\Steam\steamapps\common\Half-Life\zamnhlmp_dev\aura"
	pause
) ELSE (
	pause
)