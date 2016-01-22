@echo off
rmdir sln /S/Q
mkdir sln
cd sln
cmake -G "Visual Studio 14 2015 Win64" ..

pause