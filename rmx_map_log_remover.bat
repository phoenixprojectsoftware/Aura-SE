:: Settings
@echo off
cd maps
title Scrap File Remover B)
color 0e

:: Initial start
echo This batch process removes all RMX,
echo JMX, LOG and MAP files from your SDK
echo maps folder and its subdirectories.
pause
cls
echo PLEASE NOTE: THIS ONLY removes
echo FILES FROM THE MAPS FOLDER AND 
echo SUBDIRECTORIES THAT ALREADY 
echo CAME WITH THE AURA SERVER SDK.
pause
cls

:: Remove files from maps
echo Removing files from maps
pause
del /f *.rmx
del /f *.jmx 
del /f *.map 
del /f *.log 
del /f *.max
echo Process complete.
pause
cls

:: Remove files from maps\credits
echo Removing files from maps\credits.
pause
cls
cd credits
del /f *.rmx
del /f *.jmx 
del /f *.map 
del /f *.log 
del /f *.max
echo Process complete.
pause
cls

:: Remove files from maps\op4_test
cd..
echo Removing files from maps\op4_test.
pause
cls
cd op4_test
del /f *.rmx
del /f *.jmx 
del /f *.map 
del /f *.log 
del /f *.max
echo Process complete.
pause
cls

:: Finish
echo All files have been removed.
pause