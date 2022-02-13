# cerebellum
A brainf*** compiler built in C++ for windows to learn the basics of compiling code

## dependencies

## usage
```
cerebellum [-o/--option=arg] <program.bf>
Options:
[-v/--verbose]
[-V/--version]
[-o/--output=<output_file>]
```
Currently, only object building is supported. In the future, it will automatically link the file to the needed dlls  
The only dll currently used is `kernel32.dll`  
## TODO
 - Write assembly generator
 - Find a way to convert each instruction into assembly