#include <tice.h>

#include "board.h"

void initBoard(board *b) {
    int8_t i;

    b->color = 0;
    b->taken = 0;
    for (i = 0; i < 12; ++i) {
        b->color |= (uint32_t) RED << i;
        b->color |= (uint32_t) BLACK << (31-i);
        b->taken |= (uint32_t) 1 << i;
        b->taken |= (uint32_t) 1 << (31-i);
    }

    b->king = 0;
}

void initMove(move *m) {
    m->loc = -1;
    m->next = NULL;
}

void copyBoard(board *from, board *to) {
    to->color = from->color;
    to->king = from->king;
    to->taken = from->taken;
}

void copyMove(move *from, move *to) {
    while(from) {
        addMove(to, from->loc);
        from = from->next;
    }
}

int8_t getMovesDir(board *b, int8_t b_index, move *moves, int8_t m_index, move *source, int8_t dir) {
    int8_t temp;
    int8_t hor, vert;
    
    hor = ((dir & 1) << 1) - 1;
    vert = (dir & 2) - 1;

    if (!inBounds(getRow(b_index)+hor, getCol(b_index)+vert)) {
        return m_index;
    }

    temp = m_index & CAP_MASK;
    m_index &= ~CAP_MASK;

    if (getSpot(b->taken, getRow(b_index)+hor, getCol(b_index)+vert)) {
        move tempMove;
        board boardCopy;

        if (getSpot(b->color, getRow(b_index)+hor, getCol(b_index)+vert) == getBit(b->color, b_index)) {
            return m_index | temp;
        }

        if (!inBounds(getRow(b_index)+hor*2, getCol(b_index)+vert*2)) {
            return m_index | temp;
        }

        if (getSpot(b->taken, getRow(b_index)+hor*2, getCol(b_index)+vert*2)) {
            return m_index | temp;
        }

        m_index |= CAP_MASK;
        initMove(&tempMove);
        if (source) {
            copyMove(source, &tempMove);
        } else {
            addMove(&tempMove, b_index);
        }

        copyBoard(b, &boardCopy);
        addMove(&tempMove, getInd(getRow(b_index)+hor*2, getCol(b_index)+vert*2));
        doMove(&boardCopy, &tempMove);

        temp = getMoves(&boardCopy, getInd(getRow(b_index)+hor*2, getCol(b_index)+vert*2), moves, m_index, &tempMove);

        if (temp == m_index) {
            initMove(moves + (m_index & ~CAP_MASK));
            copyMove(&tempMove, moves + (m_index & ~CAP_MASK));
            return m_index + 1;
        }

        return temp;
    }

    if (temp) {
        return m_index | temp;
    }

    initMove(moves + m_index);
    addMove(moves + m_index, b_index);
    addMove(moves + m_index, getInd(getRow(b_index)+hor, getCol(b_index)+vert));
    return m_index + 1;
}

int8_t getMoves(board *b, int8_t b_index, move *moves, int8_t m_index, move *source) {
    bool color;

    color = getBit(b->color, b_index);
    m_index = getMovesDir(b, b_index, moves, m_index, source, color | LEFT);
    m_index = getMovesDir(b, b_index, moves, m_index, source, color | RIGHT);

    if (getBit(b->king, b_index)) {
        m_index = getMovesDir(b, b_index, moves, m_index, source, (!color) | LEFT);
        m_index = getMovesDir(b, b_index, moves, m_index, source, (!color) | RIGHT);
    }

    return m_index;
}

int8_t getAllMoves(board *b, bool color, move *moves) {
    size_t ind;
    int8_t i;

    ind = 0;
    for (i = 0; i < 32; ++i) {
        if (getBit(b->taken, i) && (color == getBit(b->color, i))) {
            ind = getMoves(b, i, moves, ind, NULL);
        }
    }

    if (ind & CAP_MASK) {
        ind &= ~CAP_MASK;
        for (i = 0; i < ind; ++i) {
            if (!(moves[i].next->loc & CAP_MASK)) {
                freeMove(moves + i);
                moves[i].loc = -1;
            }
        }
    }

    return ind;
}

void addMove(move *m, int8_t dest) {
    if (m->loc == -1) {
        m->loc = dest;
    } else if (m->next) {
        addMove(m->next, dest);
    } else {
        int8_t temp;

        temp = getCol(dest) - getCol(m->loc);
        if (temp == 2 || temp == -2) {
            dest |= CAP_MASK;
        }

        m->next = (move *) malloc(sizeof(move));
        initMove(m->next);
        m->next->loc = dest;
    }
}

bool moveContains(move *m, int8_t spot) {
    if ((m->loc & ~CAP_MASK) == spot) {
        return true;
    }

    if (m->next) {
        return moveContains(m->next, spot);
    }

    return false;
}

move *removeSpotRec(move *m, int8_t spot) {
    if ((m->loc & ~CAP_MASK) == spot) {
        freeMove(m);
        free(m);
        return NULL;
    } else if (m->next) {
        m->next = removeSpotRec(m->next, spot);
    }
    return m;
}

void removeSpot(move *m, int8_t spot) {
    if (m->loc == spot) {
        m->loc = -1;
        freeMove(m);
    } else if (m->next) {
        m->next = removeSpotRec(m->next, spot);
    }
}

bool movesEq(move *m1, move *m2) {
    if (m1 == m2) {
        return true;
    }

    return (m1->loc == m2->loc) &&
            movesEq(m1->next, m2->next);
}

bool validMove(move *m, move *valid, int8_t v_length) {
    int8_t i;
    
    if (m->loc == -1) {
        return false;
    }
    for(i = 0; i < v_length; ++i) {
        if (movesEq(m, valid + i)) {
            return true;
        }
    }

    return false;
}

void doMove(board *b, move *m) {
    int8_t temp;

    if (!(m->next)) {
        if (getRow(m->loc) == getBit(b->color, m->loc) * 7) {
            setBit(b->king, m->loc);
        }

        return;
    }

    temp = m->next->loc & CAP_MASK;
    m->next->loc &= ~CAP_MASK;
    
    invBit(b->taken, m->loc);
    invBit(b->taken, m->next->loc);
    
    if (getBit(b->color, m->loc)) {
        setBit(b->color, m->next->loc);
    } else {
        unsetBit(b->color, m->next->loc);
    }
    
    if (getBit(b->king, m->loc)) {
        setBit(b->king, m->next->loc);
    } else {
        unsetBit(b->king, m->next->loc);
    }

    if (temp) {
        unsetSpot(b->taken, (getRow(m->loc)+getRow(m->next->loc))/2, (getCol(m->loc)+getCol(m->next->loc))/2);
    }

    doMove(b, m->next);
}

bool hasValidMoves(move *moves, int8_t m_length) {
    int8_t i;

    for (i = 0; i < m_length; i++) {
        if (moves[i].loc != -1) {
            return true;
        }
    }

    return false;
}

void freeMove(move *m) {
    if (m->next) {
        freeMove(m->next);
        free(m->next);
        m->next = NULL;
    }
}

void freeMoves(move *moves, int8_t length) {
    int8_t i;

    for (i = 0; i < length; ++i) {
        freeMove(moves + i);
    }
}

