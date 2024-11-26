#!/usr/bin/env python3

"""
Prototype to establish the quickdrop_row() function.
"""
gameboard0 = []

Z0_PIECE = [(0, 0), (1, 0), (1, 1), (2, 1)]
Z0_BOTTOM = [0, 2, 3]

Z1_PIECE = [(2, 0), (2, 1), (1, 1), (1, 2)]
Z1_BOTTOM = [1, 3]


def print_board():
    for i in range(len(gameboard0)):
        row = gameboard0[i]
        print(' '.join(row))


def quickdrop_row(piece, bottom, cur_row, cur_col):
    min_row = len(gameboard0) - 1
    for i in bottom:
        pos = piece[i]
        print("piece: ", pos)
        for r in range(cur_row + pos[1], len(gameboard0)):
            #print("checking row: %d col: %d" % (r, cur_col + pos[0]))
            if gameboard0[r][cur_col + pos[0]] != '0':
                row = r - 1 - pos[1]
                if row <= min_row:
                    print("replaced min_row with: ", row)
                    min_row = row
                break
    if min_row == (len(gameboard0) - 1):
        minpos = piece[bottom[-1]]
        return min_row - minpos[1]
    else:
        return min_row


def init_gameboard():
    for i in range(20):
        row = []
        for j in range(10):
            row.append('0')
        gameboard0.append(row)

def board_1_tile():
    gameboard0[19][3] = 'X'

def board_2_tiles():
    gameboard0[19][3] = 'X'
    gameboard0[19][4] = 'X'


def board_4_tiles():
    gameboard0[18][3] = 'X'
    gameboard0[18][4] = 'X'
    gameboard0[19][3] = 'X'
    gameboard0[19][4] = 'X'


def board_with_hole():
    gameboard0[18][3] = 'X'
    #gameboard0[18][4] = 'X'
    #gameboard0[18][5] = 'X'
    gameboard0[18][6] = 'X'
    gameboard0[18][7] = 'X'

    gameboard0[19][3] = 'X'
    gameboard0[19][4] = 'X'
    #gameboard0[19][5] = 'X'
    gameboard0[19][6] = 'X'
    gameboard0[19][7] = 'X'

if __name__ == '__main__':
    init_gameboard()
    board_with_hole()

    print_board()
    #r = quickdrop_row(Z0_PIECE, Z0_BOTTOM, 0, 4)
    r = quickdrop_row(Z1_PIECE, Z1_BOTTOM, 0, 3)

    print("quick draw row: ", r)


