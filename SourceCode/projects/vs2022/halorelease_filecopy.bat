@echo off
xcopy /f /y "HaloRelease\Aura server" "C:\Program Files (x86)\Steam\steamapps\common\Half-Life\HaloGS\aura"
IF ERRORLEVEL 1 (
	xcopy /f /y "HaloRelease\Aura server" "D:\Program Files (x86)\Steam\steamapps\common\Half-Life\HaloGS\aura"
	pause
) ELSE (
	pause
)