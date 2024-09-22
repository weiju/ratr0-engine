#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tetris/game_data.h"
#include "../../chibi_test/chibi.h"

int gameboard[BOARD_HEIGHT][BOARD_WIDTH];

void tetris_test_setup(void *userdata)
{
    clear_board(&gameboard);
}
void tetris_test_teardown(void *userdata) { }

/*
 * TEST CASES
 */

/**
 * get_srs_translation() will always return (0,0) for the O piece
 */
CHIBI_TEST(TestGetSRSTranslation_O)
{
    struct Translate *translation = get_srs_translation(PIECE_O, 0, 1,
                                                        10, 5,
                                                        &gameboard);
    chibi_assert_eq_int(0, translation->x);
    chibi_assert_eq_int(0, translation->y);
}

void print_wallkick_data(int from, int to)
{
    to = (to - 1) - from % 2;
    printf("WALLKICK DATA (%d>>%d)\n", from, to);
    for (int i = 0; i < NUM_WALLKICK_TESTS; i++) {
        printf("(%d, %d)\n", WALLKICK_JLTSZ[from][to][i].x,
               WALLKICK_JLTSZ[from][to][i].y);
    }
}

CHIBI_TEST(TestGetSRSTranslation_Z)
{
    struct Translate *translation1 = get_srs_translation(PIECE_Z, 0, 1,
                                                         3, 0,
                                                         &gameboard);
    chibi_assert_eq_int(0, translation1->x);
    chibi_assert_eq_int(0, translation1->y);

    // Test 2
    struct Translate *translation2 = get_srs_translation(PIECE_Z, 1, 2,
                                                         7, -1,
                                                         &gameboard);
    chibi_assert_not_null(translation2);
    chibi_assert_eq_int(1, translation2->x);
    chibi_assert_eq_int(0, translation2->y);
}

CHIBI_TEST(TestCanMoveLeft_LeftBorder)
{
    chibi_assert(!can_move_left(PIECE_T, 0, 10, 0, &gameboard));
}

CHIBI_TEST(TestGetCompletedRows_BottomRow)
{
    struct CompletedRows res;
    BOOL status = get_completed_rows(&res,
                                     PIECE_I, 0, BOARD_HEIGHT - 2,
                                     &gameboard);
    // first is empty
    chibi_assert(!status);

    // fill the bottom row
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 1][i] = 1;
    }
    status = get_completed_rows(&res,
                                PIECE_I, 0, BOARD_HEIGHT - 2,
                                &gameboard);
    // now the bottom row is detected as completed
    chibi_assert(status);
    chibi_assert_eq_int(1, res.count);
    chibi_assert_eq_int(BOARD_HEIGHT - 1, res.rows[0]);
}

CHIBI_TEST(TestGetCompletedRows_ThreeRows)
{
    struct CompletedRows res;
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 1][i] = 1;
    }
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 3][i] = 1;
    }
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 4][i] = 1;
    }

    BOOL status = get_completed_rows(&res,
                                     PIECE_I, 1, BOARD_HEIGHT - 4,
                                     &gameboard);
    // now the bottom row is detected as completed
    chibi_assert(status);
    chibi_assert_eq_int(3, res.count);
    chibi_assert_eq_int(BOARD_HEIGHT - 4, res.rows[0]);
    chibi_assert_eq_int(BOARD_HEIGHT - 3, res.rows[1]);
    chibi_assert_eq_int(BOARD_HEIGHT - 1, res.rows[2]);
}

/*
 * SUITE DEFINITION
 */

chibi_suite *CoreSuite(void)
{
    chibi_suite *suite = chibi_suite_new_fixture("ratr0.TetrisSuite",
                                                 tetris_test_setup,
                                                 tetris_test_teardown, NULL);
    chibi_suite_add_test(suite, TestGetSRSTranslation_O);
    chibi_suite_add_test(suite, TestGetSRSTranslation_Z);
    chibi_suite_add_test(suite, TestCanMoveLeft_LeftBorder);
    chibi_suite_add_test(suite, TestGetCompletedRows_BottomRow);
    chibi_suite_add_test(suite, TestGetCompletedRows_ThreeRows);

    return suite;
}

int main(int argc, char **argv)
{
    chibi_summary_data summary;
    chibi_suite *suite = CoreSuite();

    chibi_suite_run(suite, &summary);
    chibi_suite_delete(suite);
    return summary.num_failures;
}
