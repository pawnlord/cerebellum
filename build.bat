@ECHO OFF
g++ *.cpp coff/*.cpp assembler/*.cpp -Icoff -I assembler -o cerebellum.exe 