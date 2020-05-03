# ----------------------------
# Set NAME to the program name
# Set ICON to the png icon file name
# Set DESCRIPTION to display within a compatible shell
# Set COMPRESSED to "YES" to create a compressed program
# ----------------------------

NAME        ?= CHECKERS
COMPRESSED  ?= YES
ICON        ?= iconc.png
DESCRIPTION ?= "C SDK Demo"

# ----------------------------

#.DEFAULT_GOAL := debug

include $(CEDEV)/include/.makefile
