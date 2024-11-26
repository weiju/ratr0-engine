#!/usr/bin/env python3

"""
prototype for piece_landed() function
"""

gameboard0 = []

Z0_PIECE = [(0, 0), (1, 0), (1, 1), (2, 1)]
Z0_BOTTOM = [0, 2, 3]

def print_board():
    for i in range(len(gameboard0)):
        row = gameboard0[i]
        print(' '.join(row))


def piece_landed(piece, bottom, cur_row, cur_col):
    result = False
    for i in bottom:
        pos = piece[i]
        print("piece: ", pos)
        # 1. at bottom
        if cur_row + pos[1] + 1 >= len(gameboard0):
            return True
        # 2. The space below is occupied
        if gameboard0[pos[1] + cur_row + 1][pos[0] + cur_col] != '0':
            return True
    return result

def establish_piece(piece, cur_row, cur_col):
    for pos in piece:
        gameboard0[pos[1] + cur_row][pos[0] + cur_col] = 'X'


def init_gameboard():
    for i in range(20):
        row = []
        for j in range(10):
            row.append('0')
        gameboard0.append(row)


if __name__ == "__main__":
    init_gameboard()

    print_board()
    #result = piece_landed(Z0_PIECE, Z0_BOTTOM, 18, 3)
    #print("piece_landed: ", result)

    establish_piece(Z0_PIECE, 18, 3)
    print_board()

    result = piece_landed(Z0_PIECE, Z0_BOTTOM, 17, 4)
    print("piece_landed: ", result)
