@echo off
python Setup.py
copy "%~dp0..\Glamoth\DefaultUILayout.ini" "%~dp0..\Glamoth\imgui.ini"
PAUSE