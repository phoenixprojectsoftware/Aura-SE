@echo off
xcopy /f /y "Release\Aura server\" "C:\Program Files (x86)\Steam\steamapps\common\Cross Product Beta\zamnhlmp_dev\aura"
IF ERRORLEVEL 1 (
	xcopy /f /y "Release\Aura server\" "D:\Program Files (x86)\Steam\steamapps\common\Cross Product Beta\zamnhlmp_dev\aura"
	pause
) ELSE (
	pause
)