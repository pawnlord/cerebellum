@ECHO OFF
echo building (command: g++ test_main.cpp ../filereader.cpp ../assembler/*.cpp ../coff/*.cpp -o test.exe -I.. -I../coff -I../assembler)
g++ ../cerebellum.cpp ../filereader.cpp ../assembler/*.cpp ../coff/*.cpp -o test.exe -I.. -I../coff -I../assembler
echo running test.exe
test.exe
echo linking test_build.obj (command: golink /console /entry main .\test_build.obj kernel32.dll)
golink /console /entry main .\test_build.obj kernel32.dll
echo ----------Final Result----------
test_build.exe