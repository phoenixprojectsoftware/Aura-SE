@echo off
xcopy /f /y "HaloDebug\" "C:\Program Files (x86)\Steam\steamapps\common\Half-Life\HaloGS\aura"
IF ERRORLEVEL 1 (
	xcopy /f /y "HaloDebug\" "D:\Program Files (x86)\Steam\steamapps\common\Half-Life\HaloGS\aura"
	pause
) ELSE (
	pause
)