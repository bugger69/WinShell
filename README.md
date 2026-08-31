# WinShell

## Features
This is supposed to be a basic shell designed for windows. I will keep adding more commands/features to it from time to time.
For now it features the following shell commands : cd, echo, exit, type, pwd, help and is capable of finding executables from path variable
in type and executing them when called.

## How to set up and build
You can build via two ways (I just added cmake just in case the project becomes complicated in the future.)

- Clone the repo using git clone.
```bash
git clone https://github.com/bugger69/WinShell.git
cd WinShell
```

### Using CMake
- Make sure you have cmake installed, and now create a build dir via the following commands.
```bash
mkdir build
cd build
```
- Now if you're using nmake, you can directly run the following:
```bash
cmake ..
cmake --build .
```

- If like me you're using MinGW g++, you'll have to do it this way
```bash
cmake -G "MinGW Makefiles" ..
cmake --build .
```

### Compiling directly
Now since we've added support for modules, cmake method stays the same but in this method you'll have to compile each module seperately.
- You can execute the following:
```bash  
g++ -Iinclude src/main.cpp src/shell_cmds.cpp src/utils.cpp -o WinShell.exe
cd modules
g++ hello/main.cpp -o hello.exe
```
### Running the program
- And no matter which way you compiled, run:
```bash
WinShell.exe 
```


