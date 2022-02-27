# cerebellum
A brainf*** compiler built in C++ for windows to learn the basics of compiling code  
version: 1.1.2

## usage
```
cerebellum [-o/--option=arg] <program.bf>
Options:
[-v/--verbose]
[-V/--version]
[-o/--output=<output_file>]
```
Currently, only object building is supported. In the future, it will automatically link the file to the needed dlls.  
The only dll currently used is `kernel32.dll`  
I have been testing with golink. The go link command is in the `tests/run_tests.bat` file.  

## Current Limitations
The largest problem it has is lack good automatic memory initialization. Because of this, you should manually go through all the memory your program will possibly use before the beginning of the program proper. This is something I want to look into, but I also want to move on to other projects and it's a limitation that you can work around.  
The other problem is with input. For whatever reason, the `kernel32.dll` `FlushConsoleInputBuffer` function just does not want to work. This seems to be a problem other people have had, and I have not seen a fix. If I had to guess, support for manually accessing the command line API is limited, at best.  
