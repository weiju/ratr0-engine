#include <ratr0/ratr0.h>
#include <clib/graphics_protos.h>
#include "render_display.h"
#include "draw_primitives.h"

/**
 * For each one of the double buffers, create a draw and clear queue
 */
RATR0_QUEUE_ARR(draw_piece_queue, struct PieceQueueItem, DRAW_PIECE_QUEUE_LEN,
                NUM_DISPLAY_BUFFERS)

RATR0_QUEUE_ARR(clear_piece_queue, struct PieceQueueItem, DRAW_PIECE_QUEUE_LEN,
                NUM_DISPLAY_BUFFERS)

RATR0_QUEUE_ARR(clear_row_queue, struct RowQueueItem, CLEAR_ROW_QUEUE_LEN,
                NUM_DISPLAY_BUFFERS)

RATR0_QUEUE_ARR(move_queue, struct MoveQueueItem, MOVE_QUEUE_LEN,
                NUM_DISPLAY_BUFFERS)

RATR0_QUEUE_ARR(next_queue, struct NextQueueItem, DRAW_NEXT_QUEUE_LEN,
                NUM_DISPLAY_BUFFERS)

RATR0_QUEUE_ARR(hold_queue, struct HoldQueueItem, DRAW_HOLD_QUEUE_LEN,
                NUM_DISPLAY_BUFFERS)

RATR0_QUEUE_ARR(score_queue, struct DigitQueueItem, SCORE_QUEUE_LEN,
                NUM_DISPLAY_BUFFERS)
RATR0_QUEUE_ARR(level_queue, struct DigitQueueItem, 2, NUM_DISPLAY_BUFFERS)
RATR0_QUEUE_ARR(lines_queue, struct DigitQueueItem, 2, NUM_DISPLAY_BUFFERS)


void draw_hold_piece(struct Ratr0DisplayBuffer *backbuffer,
                     struct Ratr0Surface *preview_surface,
                     UINT8 piece)
{
    draw_preview_piece(&backbuffer->surface, preview_surface, piece, 64, 47);
}

void draw_next_piece(struct Ratr0DisplayBuffer *backbuffer,
                     struct Ratr0Surface *preview_surface,
                     UINT8 pos, UINT8 piece)
{
    draw_preview_piece(&backbuffer->surface, preview_surface, piece, 208,
                       48 + 20 * pos);
}

void draw_level_digit(struct Ratr0DisplayBuffer *backbuffer,
                      struct Ratr0Surface *digits_surface,
                      UINT8 rpos, UINT8 digit)
{
    draw_digit16(&backbuffer->surface, digits_surface, digit, 88 - 16 * rpos, 116);
}

void draw_lines_digit(struct Ratr0DisplayBuffer *backbuffer,
                      struct Ratr0Surface *digits_surface,
                      UINT8 rpos, UINT8 digit)
{
    draw_digit16(&backbuffer->surface, digits_surface, digit, 88 - 16 * rpos, 89);
}

void draw_score_digit(struct Ratr0DisplayBuffer *backbuffer,
                      struct Ratr0Surface *digits_surface,
                      UINT8 rpos, UINT8 digit)
{
    draw_digit8(&backbuffer->surface, digits_surface, digit, 72 - 8 * rpos, 164);
}

void enqueue_next3(void)
{
    int pq_index;
    struct NextQueueItem next_item;
    for (int i = 0; i < 3; i++) {
        pq_index = (piece_queue_idx + i) % PIECE_QUEUE_LEN;
        next_item.piece = piece_queue[pq_index];
        next_item.position = i;
        // Enqueue in both buffers
        RATR0_ENQUEUE_ARR(next_queue, 0, next_item);
        RATR0_ENQUEUE_ARR(next_queue, 1, next_item);
    }
}

/**
 * This is the drawing section:
 * 1. process all queued up clear commands for the current buffer
 * 2. then draw all queued up draw command for the current buffer
 */
void process_blit_queues(struct Ratr0DisplayBuffer *backbuffer,
                         struct Ratr0Surface *tiles_surface,
                         struct Ratr0Surface *preview_surface,
                         struct Ratr0Surface *digits16_surface,
                         struct Ratr0Surface *digits_surface)
{
    struct PieceQueueItem piece_queue_item;
    struct RowQueueItem row_queue_item;
    struct RotationSpec *queued_spec;
    struct Ratr0Surface *backbuffer_surface = &backbuffer->surface;
    int cur_buffer = backbuffer->buffernum;
    OwnBlitter();
    // 1. Clear queue to clean up pieces from the previous render pass
    while (clear_piece_queue_num_elems[cur_buffer] > 0) {
        RATR0_DEQUEUE_ARR(piece_queue_item, clear_piece_queue, cur_buffer);
        queued_spec = &PIECE_SPECS[piece_queue_item.piece].rotations[piece_queue_item.rotation];
        clear_piece(backbuffer_surface,
                    &queued_spec->draw_spec, piece_queue_item.row,
                    piece_queue_item.col);
    }
    while (clear_row_queue_num_elems[cur_buffer] > 0) {
        RATR0_DEQUEUE_ARR(row_queue_item, clear_row_queue, cur_buffer);
        clear_rect(backbuffer_surface, row_queue_item.row,
                   0, row_queue_item.num_rows, BOARD_WIDTH);
    }

    // 2. draw all enqueued items
    while (draw_piece_queue_num_elems[cur_buffer] > 0) {
        RATR0_DEQUEUE_ARR(piece_queue_item, draw_piece_queue, cur_buffer);
        queued_spec = &PIECE_SPECS[piece_queue_item.piece].rotations[piece_queue_item.rotation];
        draw_piece(backbuffer_surface, tiles_surface,
                   &queued_spec->draw_spec,
                   piece_queue_item.piece,
                   piece_queue_item.row,
                   piece_queue_item.col);

        // put this piece in the clear buffer for next time this
        // frame gets drawn
        if (piece_queue_item.clear) {
            RATR0_ENQUEUE_ARR(clear_piece_queue, cur_buffer, piece_queue_item);
        }
    }
    render_preview_queues(backbuffer, preview_surface, cur_buffer);
    render_score_queues(backbuffer, digits16_surface, digits_surface, cur_buffer);
    DisownBlitter();
}

void render_preview_queues(struct Ratr0DisplayBuffer *backbuffer,
                           struct Ratr0Surface *preview_surface,
                           int cur_buffer)
{
    // PREVIEW (NEXT and HOLD)
    struct NextQueueItem next_item;
    struct HoldQueueItem hold_item;

    while (next_queue_num_elems[cur_buffer] > 0) {
        RATR0_DEQUEUE_ARR(next_item, next_queue, cur_buffer);
        draw_next_piece(backbuffer, preview_surface,
                        next_item.position, next_item.piece);
    }
    while (hold_queue_num_elems[cur_buffer] > 0) {
        RATR0_DEQUEUE_ARR(hold_item, hold_queue, cur_buffer);
        draw_hold_piece(backbuffer, preview_surface,
                        hold_item.piece);
    }
}

void render_score_queues(struct Ratr0DisplayBuffer *backbuffer,
                         struct Ratr0Surface *digits16_surface,
                         struct Ratr0Surface *digits_surface,
                         int cur_buffer)
{
    struct DigitQueueItem digit_item;

    while (level_queue_num_elems[cur_buffer] > 0) {
        RATR0_DEQUEUE_ARR(digit_item, level_queue, cur_buffer);
        draw_level_digit(backbuffer, digits16_surface,
                         digit_item.rpos, digit_item.digit);
    }
    while (lines_queue_num_elems[cur_buffer] > 0) {
        RATR0_DEQUEUE_ARR(digit_item, lines_queue, cur_buffer);
        draw_lines_digit(backbuffer, digits16_surface,
                         digit_item.rpos, digit_item.digit);
    }
    while (score_queue_num_elems[cur_buffer] > 0) {
        RATR0_DEQUEUE_ARR(digit_item, score_queue, cur_buffer);
        draw_score_digit(backbuffer, digits_surface,
                         digit_item.rpos, digit_item.digit);
    }
}

/**
 * Move the specified rectangular region
 */
void _move_board_rect(struct Ratr0Surface *backbuffer_surface,
                      int from_row, int to_row, int num_rows)
{
    int srcx = BOARD_X0, srcy = BOARD_Y0 + from_row * 8,
        dstx = BOARD_X0, dsty = BOARD_Y0 + to_row * 8;
    int blit_width_pixels = BOARD_WIDTH * 8;
    int blit_height_pixels = num_rows * 8;
    if (num_rows == 0 > num_rows > 15) {
#ifdef DEBUG_TETRIS
        fprintf(debug_fp,
                "ERROR: _move_board_rect(), sketchy num_rows value: %d\n", num_rows);
        fflush(debug_fp);
#endif
    }

    // this is most likely overlapping, ratr0_blit_rect_simple()
    // will perform reverse copying if that is the case
    ratr0_blit_rect_simple(backbuffer_surface,
                           backbuffer_surface,
                           dstx, dsty,
                           srcx, srcy,
                           blit_width_pixels,
                           blit_height_pixels);
}

void process_move_queue(struct Ratr0DisplayBuffer *backbuffer)
{
    struct MoveQueueItem item;
    struct Ratr0Surface *backbuffer_surface = &backbuffer->surface;
    int cur_buffer = backbuffer->buffernum;
    OwnBlitter();
    while (move_queue_num_elems[cur_buffer] > 0) {
        RATR0_DEQUEUE_ARR(item, move_queue, cur_buffer);
        _move_board_rect(backbuffer_surface, item.from, item.to, item.num_rows);
    }
    DisownBlitter();
}

void clear_render_queues(void)
{
    for (int i = 0; i < 2; i++) {
        RATR0_CLEAR_QUEUE_ARR(draw_piece_queue, i)
        RATR0_CLEAR_QUEUE_ARR(clear_piece_queue, i)
        RATR0_CLEAR_QUEUE_ARR(clear_row_queue, i)
        RATR0_CLEAR_QUEUE_ARR(move_queue, i)
        RATR0_CLEAR_QUEUE_ARR(next_queue, i)
        RATR0_CLEAR_QUEUE_ARR(hold_queue, i)
        RATR0_CLEAR_QUEUE_ARR(score_queue, i)
        RATR0_CLEAR_QUEUE_ARR(level_queue, i)
        RATR0_CLEAR_QUEUE_ARR(lines_queue, i)
    }
}
