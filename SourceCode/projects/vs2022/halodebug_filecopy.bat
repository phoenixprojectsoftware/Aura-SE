@echo off
xcopy /f /y "HaloDebug\" "C:\Program Files (x86)\Steam\steamapps\common\Cross Product Beta\excession_dev\aura"
IF ERRORLEVEL 1 (
	xcopy /f /y "HaloDebug\" "D:\Program Files (x86)\Steam\steamapps\common\Cross Product Beta\excession_dev\aura"
	pause
) ELSE (
	pause
)