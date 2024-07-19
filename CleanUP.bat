@echo off
cd %~dp0
rmdir /s /q ".idea", ".vs", "Binaries", "DerivedDataCache", "Intermediate", "Saved"
del /q /f .vsconfig, *.sln
echo All files are deleted successfully.
pause