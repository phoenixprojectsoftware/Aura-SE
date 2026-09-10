@echo off
xcopy /f /y "HaloRelease\Aura server" "C:\Program Files (x86)\Steam\steamapps\common\Cross Product Beta\excession_dev\aura"
IF ERRORLEVEL 1 (
	xcopy /f /y "HaloRelease\Aura server" "D:\Program Files (x86)\Steam\steamapps\common\Cross Product Beta\excession_dev\aura"
	pause
) ELSE (
	pause
)