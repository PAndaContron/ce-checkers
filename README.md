# CE Checkers

This is a short program for the game of checkers on the TI-84+ CE graphing calculator.

[Rules](https://www.thesprucecrafts.com/play-checkers-using-standard-rules-409287)

## Controls

 * Arrow keys move the cursor
 * `+` adds/removes a space from the current move
 * `-` clears the current move
 * `Enter` plays the move

In order to make a move, use `+` to select the piece and each of the squares it should move to in order, then press `Enter`.

## Building from source

This program requires the [CEdev C toolchain](https://github.com/CE-Programming/toolchain) to build. Once that is installed properly, the program can be built with the `make` command.

## Debugging

This program can be built with some debug features that use [CEmu](https://ce-programming.github.io/CEmu/)'s console. In order to build the program in debug mode, edit the `makefile` and uncomment this line:

```Makefile
.DEFAULT_GOAL := debug
```

### Debugging commands

 * `×` prints the current move to the console
 * `÷` prints out all possible moves for the current player

Moves are printed using indexes starting from the first black square (0) and going from left to right, then up and down, to the last black square (31). `-1` indicates no move.
