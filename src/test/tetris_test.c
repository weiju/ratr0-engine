#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tetris/game_data.h"
#include "../../chibi_test/chibi.h"

int gameboard[BOARD_HEIGHT][BOARD_WIDTH];
struct MoveRegions move_regions;

void init_move_regions(struct MoveRegions *regions)
{
    regions->count = 0;
}

void tetris_test_setup(void *userdata)
{
    clear_board(&gameboard);
    init_move_regions(&move_regions);
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
    // completed rows are returned in increasing order
    chibi_assert_eq_int(BOARD_HEIGHT - 4, res.rows[0]);
    chibi_assert_eq_int(BOARD_HEIGHT - 3, res.rows[1]);
    chibi_assert_eq_int(BOARD_HEIGHT - 1, res.rows[2]);
}

/**
 * 1 completed row, which is at the top, so no move ranges
 */
CHIBI_TEST(TestGetMoveRegions_1RowAtTop)
{
    struct CompletedRows completed_rows;
    completed_rows.count = 1;
    completed_rows.rows[0] = BOARD_HEIGHT - 1;
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 1][i] = 1;
    }
    BOOL result = get_move_regions(&move_regions, &completed_rows, &gameboard);
    chibi_assert(result);
    chibi_assert_eq_int(move_regions.count, 0);
}

CHIBI_TEST(TestGetMoveRegions_1Row2RowsToMove)
{
    struct CompletedRows completed_rows;
    completed_rows.count = 1;
    completed_rows.rows[0] = BOARD_HEIGHT - 1;
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 1][i] = 1;
    }
    for (int i = 1; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 2][i] = 1;
        gameboard[BOARD_HEIGHT - 3][i] = 1;
    }
    BOOL result = get_move_regions(&move_regions, &completed_rows, &gameboard);
    chibi_assert(result);
    chibi_assert_eq_int(move_regions.count, 1);
    chibi_assert_eq_int(move_regions.regions[0].end, BOARD_HEIGHT - 2);
    chibi_assert_eq_int(move_regions.regions[0].start, BOARD_HEIGHT - 3);
    chibi_assert_eq_int(move_regions.regions[0].move_by, 1);
}

CHIBI_TEST(TestGetMoveRegions_4Rows2RowsToMove)
{
    struct CompletedRows completed_rows;
    completed_rows.count = 4;
    completed_rows.rows[0] = BOARD_HEIGHT - 4;
    completed_rows.rows[1] = BOARD_HEIGHT - 3;
    completed_rows.rows[2] = BOARD_HEIGHT - 2;
    completed_rows.rows[3] = BOARD_HEIGHT - 1;
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 1][i] = 1;
        gameboard[BOARD_HEIGHT - 2][i] = 1;
        gameboard[BOARD_HEIGHT - 3][i] = 1;
        gameboard[BOARD_HEIGHT - 4][i] = 1;
    }

    for (int i = 1; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 5][i] = 1;
        gameboard[BOARD_HEIGHT - 6][i] = 1;
    }
    BOOL result = get_move_regions(&move_regions, &completed_rows, &gameboard);
    chibi_assert(result);
    chibi_assert_eq_int(move_regions.count, 1);
    chibi_assert_eq_int(move_regions.regions[0].start, BOARD_HEIGHT - 6);
    chibi_assert_eq_int(move_regions.regions[0].end, BOARD_HEIGHT - 5);
    chibi_assert_eq_int(move_regions.regions[0].move_by, 4);
}

CHIBI_TEST(TestGetMoveRegions_2Rows1RegionToMove)
{
    struct CompletedRows completed_rows;
    completed_rows.count = 2;
    completed_rows.rows[0] = BOARD_HEIGHT - 4;
    completed_rows.rows[1] = BOARD_HEIGHT - 3;
    // complete rows
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 3][i] = 1;
        gameboard[BOARD_HEIGHT - 4][i] = 1;
    }
    // incomplete rows
    for (int i = 1; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 1][i] = 1;
        gameboard[BOARD_HEIGHT - 2][i] = 1;
        gameboard[BOARD_HEIGHT - 5][i] = 1;
        gameboard[BOARD_HEIGHT - 6][i] = 1;
    }
    BOOL result = get_move_regions(&move_regions, &completed_rows, &gameboard);
    chibi_assert(result);
    chibi_assert_eq_int(move_regions.count, 1);
    chibi_assert_eq_int(move_regions.regions[0].start, BOARD_HEIGHT - 6);
    chibi_assert_eq_int(move_regions.regions[0].end, BOARD_HEIGHT - 5);
    chibi_assert_eq_int(move_regions.regions[0].move_by, 2);
}

CHIBI_TEST(TestGetMoveRegions_2Rows2RegionsToMove)
{
    struct CompletedRows completed_rows;
    completed_rows.count = 2;
    completed_rows.rows[0] = BOARD_HEIGHT - 5;
    completed_rows.rows[1] = BOARD_HEIGHT - 2;

    // complete rows
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 2][i] = 1;
        gameboard[BOARD_HEIGHT - 5][i] = 1;
    }
    // incomplete rows
    for (int i = 1; i < BOARD_WIDTH; i++) {
        // bottom
        gameboard[BOARD_HEIGHT - 1][i] = 1;
        // gap
        gameboard[BOARD_HEIGHT - 3][i] = 1;
        gameboard[BOARD_HEIGHT - 4][i] = 1;
        // above
        gameboard[BOARD_HEIGHT - 6][i] = 1;
        gameboard[BOARD_HEIGHT - 7][i] = 1;
    }
    BOOL result = get_move_regions(&move_regions, &completed_rows, &gameboard);
    chibi_assert(result);
    chibi_assert_eq_int(move_regions.count, 2);
    // check that the gap is included
    chibi_assert_eq_int(move_regions.regions[0].start, BOARD_HEIGHT - 4);
    chibi_assert_eq_int(move_regions.regions[0].end, BOARD_HEIGHT - 3);
    chibi_assert_eq_int(move_regions.regions[0].move_by, 2);
    // check the "above" region is included
    chibi_assert_eq_int(move_regions.regions[1].start, BOARD_HEIGHT - 7);
    chibi_assert_eq_int(move_regions.regions[1].end, BOARD_HEIGHT - 6);
    chibi_assert_eq_int(move_regions.regions[1].move_by, 3);
}

/**
 * The simplest 3 connected row case: all of them are connected
 */
CHIBI_TEST(TestGetMoveRegions_3ConnectedRows1RegionToMove)
{
    struct CompletedRows completed_rows;
    completed_rows.count = 3;
    completed_rows.rows[0] = BOARD_HEIGHT - 4;
    completed_rows.rows[1] = BOARD_HEIGHT - 3;
    completed_rows.rows[2] = BOARD_HEIGHT - 2;

    // complete rows
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 2][i] = 1;
        gameboard[BOARD_HEIGHT - 3][i] = 1;
        gameboard[BOARD_HEIGHT - 4][i] = 1;
    }
    // incomplete rows
    for (int i = 1; i < BOARD_WIDTH; i++) {
        // bottom
        gameboard[BOARD_HEIGHT - 1][i] = 1;
        // above
        gameboard[BOARD_HEIGHT - 5][i] = 1;
        gameboard[BOARD_HEIGHT - 6][i] = 1;
    }
    BOOL result = get_move_regions(&move_regions, &completed_rows, &gameboard);
    chibi_assert(result);
    chibi_assert_eq_int(move_regions.count, 1);
    // check the "above" region is included
    chibi_assert_eq_int(move_regions.regions[0].start, BOARD_HEIGHT - 6);
    chibi_assert_eq_int(move_regions.regions[0].end, BOARD_HEIGHT - 5);
    chibi_assert_eq_int(move_regions.regions[0].move_by, 3);
}

/**
 * Rows 1 and 2 connected with a gap to the 3rd
 */
CHIBI_TEST(TestGetMoveRegions_12_3Rows2RegionsToMove)
{
    struct CompletedRows completed_rows;
    completed_rows.count = 3;
    completed_rows.rows[0] = BOARD_HEIGHT - 5;
    completed_rows.rows[1] = BOARD_HEIGHT - 4;
    completed_rows.rows[2] = BOARD_HEIGHT - 2;

    // complete rows
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 2][i] = 1;
        gameboard[BOARD_HEIGHT - 4][i] = 1;
        gameboard[BOARD_HEIGHT - 5][i] = 1;
    }
    // incomplete rows
    for (int i = 1; i < BOARD_WIDTH; i++) {
        // bottom
        gameboard[BOARD_HEIGHT - 1][i] = 1;
        // gap
        gameboard[BOARD_HEIGHT - 3][i] = 1;
        // above
        gameboard[BOARD_HEIGHT - 6][i] = 1;
        gameboard[BOARD_HEIGHT - 7][i] = 1;
    }
    BOOL result = get_move_regions(&move_regions, &completed_rows, &gameboard);
    chibi_assert(result);
    chibi_assert_eq_int(move_regions.count, 2);
    // check that the gap is included
    chibi_assert_eq_int(move_regions.regions[0].start, BOARD_HEIGHT - 3);
    chibi_assert_eq_int(move_regions.regions[0].end, BOARD_HEIGHT - 3);
    chibi_assert_eq_int(move_regions.regions[0].move_by, 1);

    // check the "above" region is included
    chibi_assert_eq_int(move_regions.regions[1].start, BOARD_HEIGHT - 7);
    chibi_assert_eq_int(move_regions.regions[1].end, BOARD_HEIGHT - 6);
    chibi_assert_eq_int(move_regions.regions[1].move_by, 3);
}


CHIBI_TEST(TestGetMoveRegions_1_23Rows2RegionsToMove)
{
    struct CompletedRows completed_rows;
    completed_rows.count = 3;
    completed_rows.rows[0] = BOARD_HEIGHT - 5;
    completed_rows.rows[1] = BOARD_HEIGHT - 3;
    completed_rows.rows[2] = BOARD_HEIGHT - 2;

    // complete rows
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 2][i] = 1;
        gameboard[BOARD_HEIGHT - 3][i] = 1;
        gameboard[BOARD_HEIGHT - 5][i] = 1;
    }
    // incomplete rows
    for (int i = 1; i < BOARD_WIDTH; i++) {
        // bottom
        gameboard[BOARD_HEIGHT - 1][i] = 1;
        // gap
        gameboard[BOARD_HEIGHT - 4][i] = 1;
        // above
        gameboard[BOARD_HEIGHT - 6][i] = 1;
        gameboard[BOARD_HEIGHT - 7][i] = 1;
    }
    BOOL result = get_move_regions(&move_regions, &completed_rows, &gameboard);
    chibi_assert(result);
    chibi_assert_eq_int(move_regions.count, 2);
    // check that the gap is included
    chibi_assert_eq_int(move_regions.regions[0].start, BOARD_HEIGHT - 4);
    chibi_assert_eq_int(move_regions.regions[0].end, BOARD_HEIGHT - 4);
    chibi_assert_eq_int(move_regions.regions[0].move_by, 2);

    // check the "above" region is included
    chibi_assert_eq_int(move_regions.regions[1].start, BOARD_HEIGHT - 7);
    chibi_assert_eq_int(move_regions.regions[1].end, BOARD_HEIGHT - 6);
    chibi_assert_eq_int(move_regions.regions[1].move_by, 3);
}

CHIBI_TEST(TestDeleteRowsFromBoard_BottomRow)
{
    struct CompletedRows completed_rows;
    struct MoveRegions move_regions;
    completed_rows.count = 1;
    completed_rows.rows[0] = BOARD_HEIGHT - 1;
    move_regions.count = 1;
    move_regions.regions[0].start = BOARD_HEIGHT - 4;
    move_regions.regions[0].end = BOARD_HEIGHT - 2;
    move_regions.regions[0].move_by = 1;

    // Fill the bottom row
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 1][i] = 1;
    }

    // and just build a few rows on top
    for (int i = 1; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 2][i] = 1;
        gameboard[BOARD_HEIGHT - 3][i] = 1;
        gameboard[BOARD_HEIGHT - 4][i] = 1;
    }
    BOOL result = delete_rows_from_board(&move_regions, &completed_rows,
                                         &gameboard);
    chibi_assert(result);
    for (int i = 1; i < BOARD_WIDTH; i++) {
        chibi_assert_eq_int(1, gameboard[BOARD_HEIGHT - 1][i]);
        chibi_assert_eq_int(1, gameboard[BOARD_HEIGHT - 2][i]);
        chibi_assert_eq_int(1, gameboard[BOARD_HEIGHT - 3][i]);
    }
    chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 1][0]);
    chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 2][0]);
    chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 3][0]);
    for (int i = 0; i < BOARD_WIDTH; i++) {
        chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 4][i]);
    }
}

CHIBI_TEST(TestDeleteRowsFromBoard_2TopRows)
{
    struct CompletedRows completed_rows;
    struct MoveRegions move_regions;
    completed_rows.count = 2;
    completed_rows.rows[0] = BOARD_HEIGHT - 5;
    completed_rows.rows[1] = BOARD_HEIGHT - 4;
    move_regions.count = 0;

    // Fill the 2 top rows
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 5][i] = 1;
        gameboard[BOARD_HEIGHT - 4][i] = 1;
    }

    // and just build a few rows at the bottom
    for (int i = 1; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 1][i] = 1;
        gameboard[BOARD_HEIGHT - 2][i] = 1;
        gameboard[BOARD_HEIGHT - 3][i] = 1;
    }
    BOOL result = delete_rows_from_board(&move_regions, &completed_rows,
                                         &gameboard);
    chibi_assert(result);

    // check for untouched bottom rows
    for (int i = 1; i < BOARD_WIDTH; i++) {
        chibi_assert_eq_int(1, gameboard[BOARD_HEIGHT - 1][i]);
        chibi_assert_eq_int(1, gameboard[BOARD_HEIGHT - 2][i]);
        chibi_assert_eq_int(1, gameboard[BOARD_HEIGHT - 3][i]);
    }
    chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 1][0]);
    chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 2][0]);
    chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 3][0]);

    // check for cleared top rows
    for (int i = 0; i < BOARD_WIDTH; i++) {
        chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 4][i]);
        chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 5][i]);
    }
}

CHIBI_TEST(TestDeleteRowsFromBoard_3Rows2Regions)
{
    struct CompletedRows completed_rows;
    struct MoveRegions move_regions;
    completed_rows.count = 3;
    completed_rows.rows[0] = BOARD_HEIGHT - 5;
    completed_rows.rows[1] = BOARD_HEIGHT - 4;
    completed_rows.rows[2] = BOARD_HEIGHT - 2;
    move_regions.count = 2;
    move_regions.regions[0].start = BOARD_HEIGHT - 3;
    move_regions.regions[0].end = BOARD_HEIGHT - 3;
    move_regions.regions[0].move_by = 1;
    move_regions.regions[1].start = BOARD_HEIGHT - 7;
    move_regions.regions[1].end = BOARD_HEIGHT - 6;
    move_regions.regions[1].move_by = 3;

    // Fill the completed rows
    for (int i = 0; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 5][i] = 1;
        gameboard[BOARD_HEIGHT - 4][i] = 1;
        gameboard[BOARD_HEIGHT - 2][i] = 1;
    }

    // and put in uncompleted rows
    for (int i = 1; i < BOARD_WIDTH; i++) {
        gameboard[BOARD_HEIGHT - 6][i] = 1;
        gameboard[BOARD_HEIGHT - 7][i] = 1;
        gameboard[BOARD_HEIGHT - 3][i] = 1;
        gameboard[BOARD_HEIGHT - 1][i] = 1;
    }
    BOOL result = delete_rows_from_board(&move_regions, &completed_rows,
                                         &gameboard);
    chibi_assert(result);

    // compacted bottom rows
    for (int i = 1; i < BOARD_WIDTH; i++) {
        chibi_assert_eq_int(1, gameboard[BOARD_HEIGHT - 1][i]);
        chibi_assert_eq_int(1, gameboard[BOARD_HEIGHT - 2][i]);
        chibi_assert_eq_int(1, gameboard[BOARD_HEIGHT - 3][i]);
        chibi_assert_eq_int(1, gameboard[BOARD_HEIGHT - 4][i]);
    }
    chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 1][0]);
    chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 2][0]);
    chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 3][0]);
    chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 4][0]);

    // clear top rows
    for (int i = 0; i < BOARD_WIDTH; i++) {
        chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 5][i]);
        chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 6][i]);
        chibi_assert_eq_int(0, gameboard[BOARD_HEIGHT - 7][i]);
    }
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
    chibi_suite_add_test(suite, TestGetMoveRegions_1RowAtTop);
    chibi_suite_add_test(suite, TestGetMoveRegions_1Row2RowsToMove);
    chibi_suite_add_test(suite, TestGetMoveRegions_4Rows2RowsToMove);
    chibi_suite_add_test(suite, TestGetMoveRegions_2Rows1RegionToMove);
    chibi_suite_add_test(suite, TestGetMoveRegions_2Rows2RegionsToMove);
    chibi_suite_add_test(suite, TestGetMoveRegions_3ConnectedRows1RegionToMove);
    chibi_suite_add_test(suite, TestGetMoveRegions_12_3Rows2RegionsToMove);
    chibi_suite_add_test(suite, TestGetMoveRegions_1_23Rows2RegionsToMove);

    chibi_suite_add_test(suite, TestDeleteRowsFromBoard_BottomRow);
    chibi_suite_add_test(suite, TestDeleteRowsFromBoard_2TopRows);
    chibi_suite_add_test(suite, TestDeleteRowsFromBoard_3Rows2Regions);
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
