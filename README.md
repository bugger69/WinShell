# WinShell

## Features
This is supposed to be a basic shell designed for windows. I will keep adding more commands/features to it from time to time.
For now it features the following shell commands : cd, echo, exit, type, pwd, help and is capable of finding executables from path variable
in type and executing them when called.

## How to set up
Clone the repo using git clone.

And then execute the following:
```bash  
g++ main.cpp shell_cmds.cpp utils.cpp -o WinShell.exe
```

And now run:
```bash
WinShell.exe 
```


