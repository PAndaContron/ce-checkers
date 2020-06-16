#include <tice.h>
#include <keypadc.h>

#include <graphx.h>

#ifdef DEBUG
#include <debug.h>
#include <assert.h>
#endif

#include "board.h"

void drawBoard(char *note, board *b, move *selection, int8_t cursor, bool turn);

#ifdef DEBUG
void printMove(move *sp);
#endif

void main() {
    board b;
    move m_buff[48];
    move selection;
    int8_t moveLen;
    int8_t cursor;
    uint16_t prevDir;
    uint16_t prevAct;
    bool turn;
    bool gameover;
    char *msg;

    initBoard(&b);
    initMove(&selection);
    cursor = 0;
    turn = BLACK;
    moveLen = getAllMoves(&b, turn, m_buff);

    gameover = false;
    msg = NULL;
    
    gfx_Begin();
    gfx_SetDrawBuffer();
    
    prevDir = 0;
    prevAct = 0;

    do {
        drawBoard(msg, &b, &selection, cursor, turn);
        gfx_SwapDraw();
        kb_Scan();
        
        if (msg) {
            if (!(prevAct || prevDir) && (kb_Data[6] || kb_Data[7])) {
                msg = NULL;
                if (gameover) {
                    initBoard(&b);
                    gameover = false;
                }
            }
        } else {
            if (!(prevDir & kb_Up) && (kb_Data[7] & kb_Up) && ((cursor >> 3) > 0)) {
                cursor -= 8;
            }
            if (!(prevDir & kb_Down) && (kb_Data[7] & kb_Down) && ((cursor >> 3) < 7)) {
                cursor += 8;
            }
            if (!(prevDir & kb_Left) && (kb_Data[7] & kb_Left) && ((cursor & 7) > 0)) {
                --cursor;
            }
            if (!(prevDir & kb_Right) && (kb_Data[7] & kb_Right) && ((cursor & 7) < 7)) {
                ++cursor;
            }
    
            if (!(prevAct & kb_Add) && (kb_Data[6] & kb_Add)) {
                if ((cursor >> 3) + (cursor & 7) & 1) {
                    if (moveContains(&selection, cursor >> 1)) {
                        removeSpot(&selection, cursor >> 1);
                    } else {
                        addMove(&selection, cursor >> 1);
                    }
                }
            }
            if (!(prevAct & kb_Sub) && (kb_Data[6] & kb_Sub)) {
                freeMove(&selection);
                selection.loc = -1;
            }
            if (!(prevAct & kb_Enter) && (kb_Data[6] & kb_Enter)) {
                if (validMove(&selection, m_buff, moveLen)) {
                    doMove(&b, &selection);
                    freeMoves(m_buff, moveLen);
                    turn = !turn;
                    moveLen = getAllMoves(&b, turn, m_buff);
    
                    freeMove(&selection);
                    selection.loc = -1;

                    if (!hasValidMoves(m_buff, moveLen)) {
                        msg = (turn == RED) ? "Black wins!" : "Red wins!";
                        gameover = true;
                    }
                } else {
                    msg = "Invalid Move";
                }
            }

#ifdef DEBUG
            if (!(prevAct & kb_Mul) && (kb_Data[6] & kb_Mul)) {
                dbg_sprintf(dbgout, "Current move: ");
                printMove(&selection);
            }
            if (!(prevAct & kb_Div) && (kb_Data[6] & kb_Div)) {
                int8_t i;
    
                dbg_sprintf(dbgout, "Current moves:\n");
                for (i = 0; i < moveLen; ++i) {
                    printMove(m_buff + i);
                }
            }
#endif
        }

        prevDir = kb_Data[7];
        prevAct = kb_Data[6];
    } while (!(kb_Data[6] & kb_Clear));
    
    freeMove(&selection);
    freeMoves(m_buff, moveLen);
    gfx_End();
}

void drawBoard(char *note, board *b, move *selection, int8_t cursor, bool turn) {
    int8_t i;
    int8_t j;
    
    gfx_SetColor(224);
    for (i = 0; i < 8; ++i) {
        for (j = (i & 1); j < 8; j += 2) {
            gfx_FillRectangle(40 + 30*j, 30*i, 30, 30);
        }
    }
    gfx_SetColor(0);
    for (i = 0; i < 8; ++i) {
        for (j = ((~i) & 1); j < 8; j += 2) {
            gfx_FillRectangle(40 + 30*j, 30*i, 30, 30);
        }
    }

    gfx_SetColor(24);
    while (selection) {
        if (selection->loc == -1) {
            break;
        }

        gfx_FillRectangle(40 + getCol(selection->loc & ~CAP_MASK)*30, getRow(selection->loc & ~CAP_MASK)*30, 30, 30);
        selection = selection->next;
    }

    gfx_SetColor((turn == RED) ? 160 : 65);
    gfx_FillRectangle(0, 0, 40, 240);
    gfx_FillRectangle(280, 0, 40, 240);
    gfx_SetTextFGColor(231);
    gfx_PrintStringXY((turn == RED) ? "Red" : "Black", 2, 230);
    gfx_PrintStringXY("Turn", 285, 230);

    gfx_SetColor(65);
    for (i = 0; i < 32; ++i) {
        if (getBit(b->taken, i) && (getBit(b->color, i) == BLACK)) {
            gfx_FillCircle(55 + getCol(i)*30, 15 + getRow(i)*30, 10);
        }
    }

    gfx_SetColor(160);
    for (i = 0; i < 32; ++i) {
        if (getBit(b->taken, i) && (getBit(b->color, i) == RED)) {
            gfx_FillCircle(55 + getCol(i)*30, 15 + getRow(i)*30, 10);
        }
    }

    gfx_SetTextFGColor(231);
    for (i = 0; i < 32; ++i) {
        if (getBit(b->taken, i) && getBit(b->king, i)) {
            gfx_PrintStringXY("K", 40 + getCol(i)*30, getRow(i)*30);
        }
    }

    gfx_SetColor(255);
    gfx_VertLine(54 + (cursor & 7)*30, 10 + (cursor >> 3)*30, 10);
    gfx_VertLine(55 + (cursor & 7)*30, 10 + (cursor >> 3)*30, 10);
    gfx_HorizLine(50 + (cursor & 7)*30, 14 + (cursor >> 3)*30, 10);
    gfx_HorizLine(50 + (cursor & 7)*30, 15 + (cursor >> 3)*30, 10);

    if (note) {
        gfx_SetColor(231);
        gfx_FillRectangle(0, 0, 320, 16);
        gfx_SetTextFGColor(224);
        gfx_PrintStringXY(note, 2, 2);
    }
}

#ifdef DEBUG
void printMove(move *sp) {
    dbg_sprintf(dbgout, "%d", sp->loc);
    while (sp->next) {
        sp = sp->next;
        if (sp->loc & CAP_MASK) {
            dbg_sprintf(dbgout, "->%d(C)", sp->loc & ~CAP_MASK);
        } else {
            dbg_sprintf(dbgout, "->%d", sp->loc);
        }
    }
    dbg_sprintf(dbgout, "\n", sp->loc);
}
#endif
