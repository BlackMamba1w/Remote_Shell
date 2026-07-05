[![progress-banner](https://github.com/user-attachments/assets/daa948af-b82f-4fc8-8ffd-327c08b3b139
)](https://app.codecrafters.io/users/BlackMamba1w?r=2qF)

This is my solution and implementation to the ["Build Your Own Shell" Challenge](https://app.codecrafters.io/courses/shell/overview).

In this challenge, I built my own POSIX compliant shell, that is capable of
interpreting shell commands, running external programs and builtin commands such as
cd, pwd, echo. This project is mostly for learning purposes, but a portal/gateway to future projects to come!


# Programme structure

The entry point for my `shell` implementation is in `src/main.cpp`.
All functions that I use in `src/main.cpp` can be found in `src/funcs.cpp` and are defined in `funcs.hpp`.
One can run the shell via *WSL* or *Terminal* by travelling to the directory and running:

```sh
./QShell
```

# Set-up instructions:

1. Ensure you have `cmake` installed locally
2. Run `./QShell.sh` to run your program, which is implemented in
   `src/main.cpp`.
