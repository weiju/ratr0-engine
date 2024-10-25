#pragma once
/* Module for rendering the Tetrazone display */
#ifndef __RENDER_DISPLAY_H__
#define __RENDER_DISPLAY_H__

#include <ratr0/datastructs/queue.h>

#define DRAW_PIECE_QUEUE_LEN (10)
#define CLEAR_ROW_QUEUE_LEN (10)
#define MOVE_QUEUE_LEN (10)
#define DRAW_NEXT_QUEUE_LEN (4)
#define DRAW_HOLD_QUEUE_LEN (2)
#define SCORE_QUEUE_LEN (10)

#define NUM_DISPLAY_BUFFERS (2)


// DRAW, CLEAR AND MOVE QUEUES

struct PieceQueueItem {
    UINT8 piece, rotation, row;
    INT8 col;  // needs to be able to draw negative !!!
    BOOL clear; // if TRUE, clear after draw
};
struct RowQueueItem {
    UINT8 row, num_rows;
};

RATR0_QUEUE_ARR_DEF(draw_piece_queue, struct PieceQueueItem,
                    DRAW_PIECE_QUEUE_LEN,
                    NUM_DISPLAY_BUFFERS)

RATR0_QUEUE_ARR_DEF(clear_piece_queue, struct PieceQueueItem,
                    DRAW_PIECE_QUEUE_LEN,
                    NUM_DISPLAY_BUFFERS)

RATR0_QUEUE_ARR_DEF(clear_row_queue, struct RowQueueItem,
                    CLEAR_ROW_QUEUE_LEN,
                    NUM_DISPLAY_BUFFERS)

/**
 * The Move queue is to store the actions to move regions of block rows
 * after lines where deleted
 */
struct MoveQueueItem {
    int from, to, num_rows;
};

RATR0_QUEUE_ARR_DEF(move_queue, struct MoveQueueItem, MOVE_QUEUE_LEN,
                    NUM_DISPLAY_BUFFERS)


struct NextQueueItem {
    UINT8 piece, position;
};

RATR0_QUEUE_ARR_DEF(next_queue, struct NextQueueItem, DRAW_NEXT_QUEUE_LEN,
                    NUM_DISPLAY_BUFFERS)

struct HoldQueueItem {
    UINT8 piece;
};

RATR0_QUEUE_ARR_DEF(hold_queue, struct HoldQueueItem, DRAW_HOLD_QUEUE_LEN,
                    NUM_DISPLAY_BUFFERS)

struct DigitQueueItem {
    UINT8 digit;
    UINT8 rpos;
};

RATR0_QUEUE_ARR_DEF(score_queue, struct DigitQueueItem, SCORE_QUEUE_LEN,
                    NUM_DISPLAY_BUFFERS)

RATR0_QUEUE_ARR_DEF(level_queue, struct DigitQueueItem, 2, NUM_DISPLAY_BUFFERS)
RATR0_QUEUE_ARR_DEF(lines_queue, struct DigitQueueItem, 2, NUM_DISPLAY_BUFFERS)

extern void draw_hold_piece(struct Ratr0DisplayBuffer *backbuffer,
                            struct Ratr0Surface *preview_surface,
                            UINT8 piece);
extern void draw_next_piece(struct Ratr0DisplayBuffer *backbuffer,
                            struct Ratr0Surface *preview_surface,
                            UINT8 pos, UINT8 piece);
extern void draw_score_digit(struct Ratr0DisplayBuffer *backbuffer,
                             struct Ratr0Surface *digits_surface,
                             UINT8 rpos, UINT8 digit);

extern void enqueue_next3(void);
extern void render_preview_queues(struct Ratr0DisplayBuffer *backbuffer,
                                  struct Ratr0Surface *preview_surface,
                                  int cur_buffer);
extern void process_blit_queues(struct Ratr0DisplayBuffer *backbuffer,
                                struct Ratr0Surface *tiles_surface,
                                struct Ratr0Surface *preview_surface);


#endif // __RENDER_DISPLAY_H__
