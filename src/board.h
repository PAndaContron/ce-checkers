#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define getInd(row, col) (4*(row) + (col)/2)
#define getRow(index) ((index) / 4)
#define getCol(index) (((((index) & 7) << 1) + 1) % 9)
#define inBounds(row, col) (!(((row) >> 3) || ((col) >> 3)))

#define getBit(bitfield, index) (((bitfield) >> (index)) & 1)
#define setBit(bitfield, index) {(bitfield) |= ((uint32_t) 1 << (index));}
#define invBit(bitfield, index) {(bitfield) ^= ((uint32_t) 1 << (index));}
#define unsetBit(bitfield, index) {(bitfield) &= ~((uint32_t) 1 << (index));}

#define getSpot(bitfield, row, col) (getBit(bitfield, getInd(row, col)))
#define setSpot(bitfield, row, col) {setBit(bitfield, getInd(row, col));}
#define invSpot(bitfield, row, col) {invBit(bitfield, getInd(row, col));}
#define unsetSpot(bitfield, row, col) {unsetBit(bitfield, getInd(row, col));}

#define BLACK 0
#define RED 1

#define UP BLACK
#define DOWN RED
#define LEFT 0
#define RIGHT 2

#define CAP_MASK 64

typedef struct {
	uint32_t color;
	uint32_t king;
	uint32_t taken;
} board;

struct move_s {
	int8_t loc;
	struct move_s *next;
};

typedef struct move_s move;

void initBoard(board *b);
void initMove(move *m);
void copyBoard(board *from, board *to);
void copyMove(move *from, move *to);
int8_t getMoves(board *b, int8_t b_index, move *moves, int8_t m_index, move *source);
int8_t getAllMoves(board *b, bool color, move *moves);
void addMove(move *m, int8_t dest);
bool moveContains(move *m, int8_t spot);
void removeSpot(move *m, int8_t spot);
bool movesEq(move *m1, move *m2);
bool validMove(move *m, move *valid, int8_t v_length);
void doMove(board *b, move *m);
bool hasValidMoves(move *moves, int8_t m_length);
void freeMove(move *m);
void freeMoves(move *moves, int8_t length);
