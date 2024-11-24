@echo off
xcopy /f /y "Release\Aura engine\" "C:\Program Files (x86)\Steam\steamapps\common\Half-Life\zamnhlmp\aura"
IF ERRORLEVEL 1 (
	xcopy /f /y "Release\Aura engine\" "D:\Program Files (x86)\Steam\steamapps\common\Half-Life\zamnhlmp\aura"
	pause
) ELSE (
	pause
)