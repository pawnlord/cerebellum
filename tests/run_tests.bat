@ECHO OFF
echo building (command: g++ test_main.cpp ../filereader.cpp ../assembler/*.cpp ../coff/*.cpp -o test.exe -I.. -I../coff -I../assembler)
g++ ../cerebellum.cpp ../filereader.cpp ../assembler/*.cpp ../coff/*.cpp -o test.exe -I.. -I../coff -I../assembler
echo running test.exe
test.exe