@echo off
rem Abre o War of the Monsters no PCSX2 com o ELF escolhido (padrao: halfcpp). Exemplos: jogar.bat   |   jogar.bat bis_old   |   jogar.bat control_matching
set NAME=%1
if "%NAME%"=="" set NAME=halfcpp
start "" "D:\Emulators\PCSX2\pcsx2-qt.exe" -batch -elf "%~dp0build\pcsx2_test\SCUS_971.97_%NAME%.elf" -- "%~dp0ISO\SCUS_971.97.War of the Monsters.iso"
