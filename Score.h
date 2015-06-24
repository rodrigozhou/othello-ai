#ifndef SCORE_H
#define SCORE_H

#include <algorithm>

#include "Board.h"

using namespace std;

int g[Board::n][Board::n], aux[Board::n][Board::n];
int pos_stable[Board::n][Board::n], pos_unstable[Board::n][Board::n];

void expand_corner_stability() {
    if (g[0][0] == EMPTY) return;
    int color = g[0][0];
    int maxi = Board::n, maxj = Board::n;
    for (int k = 0; k < min(maxi, maxj); k++) {
        if (g[k][k] != color) break;
        int i = k, j = k;
        pos_stable[k][k] = 1;
        while (i < maxi && g[i][k] == color)
            pos_stable[i++][k] = 1;
        while (j < maxj && g[k][j] == color)
            pos_stable[k][j++] = 1;
        if (i > j)
            i--;
        else if (j > i)
            j--;
        else if (i == j)
            i--, j--;
        maxi = i;
        maxj = j;
    }
}

void get_stability(const Board &b) {
    memcpy(g, b.g, sizeof(g));
    memset(pos_stable, 0, sizeof(pos_stable));
    for (int k = 0; k < 4; k++) {
        expand_corner_stability();
        // rotate counter-clockwise
        for (int i = 0; i < Board::n; i++)
            for (int j = 0; j < Board::n; j++)
                aux[-j+Board::n-1][i] = g[i][j];
        memcpy(g, aux, sizeof(g));
        for (int i = 0; i < Board::n; i++)
            for (int j = 0; j < Board::n; j++)
                aux[-j+Board::n-1][i] = pos_stable[i][j];
        memcpy(pos_stable, aux, sizeof(pos_stable));
    }
}

void expand_unstable(const Board &b, int player, int x, int y) {
    if (g[x][y] != EMPTY)
        return;
    for (int dx = -1; dx < 2; dx++) {
        for (int dy = -1; dy < 2; dy++) {
            if (dx == 0 && dy == 0)
                continue;
            int cnt = 0;
            for (int i = x+dx, j = y+dy; ; i += dx, j += dy) {
                if (!b.is_valid(i, j) || b.g[i][j] == EMPTY) {
                    cnt = 0;
                    break;
                }
                if (b.g[i][j] == player)
                    break;
                cnt++;
            }
            if (cnt > 0) {
                for (int i = x+dx, j = y+dy; ; i += dx, j += dy) {
                    if (b.g[i][j] == player)
                        break;
                    pos_unstable[i][j] = 1;
                }
            }
        }
    }
}

void get_instability(const Board &b) {
    memset(pos_unstable, 0, sizeof(pos_unstable));
    for (int i = 0; i < Board::n; i++)
        for (int j = 0; j < Board::n; j++)
            for (int k = 0; k < 2; k++)
                expand_unstable(b, k, i, j);
}

double score(const Board &b, int player) {
    int cnt_tiles[2];

    // Coin parity
    double parity = 100.0 * (b.cnt[player] - b.cnt[player ^ 1]) / (b.cnt[0] + b.cnt[1]);

    // Mobility
    // Counting the number of available moves to each player
    cnt_tiles[0] = cnt_tiles[1] = 0;
    for (int i = 0; i < Board::n; i++)
        for (int j = 0; j < Board::n; j++)
            for (int k = 0; k < 2; k++)
                if (b.is_valid_move(k, i, j))
                    cnt_tiles[k]++;
    double mobility = 0;
    if (cnt_tiles[0] + cnt_tiles[1] != 0)
        mobility = 100.0 * (cnt_tiles[player] - cnt_tiles[player^1]) /
            (cnt_tiles[0] + cnt_tiles[1]);;

    // Corner occupancy
    // Count the number of captured, potencial and unlikely corners
    static int corners[4][2] = {
        {0, 0}, {0, Board::n-1}, {Board::n-1, 0}, {Board::n-1, Board::n-1},
    };

    int p_corner[2] = {0, 0};

    // captured corners
    static int weight_corner_captured = 10;
    for (int k = 0; k < 4; k++)
        if (!b.is_empty(corners[k][0], corners[k][1]))
            p_corner[(int) b.g[corners[k][0]][corners[k][1]]] += weight_corner_captured;

    // potencial and unlikely corners
    static int weight_corner_potencial = 4;
    static int weight_corner_unlikely = -1;
    for (int k = 0; k < 4; k++) {
        int i = corners[k][0], j = corners[k][1];
        for (int player = 0; player < 2; player++) {
            if (b.is_valid_move(player, i, j))
                p_corner[k] += weight_corner_potencial;
            else
                p_corner[k] += weight_corner_unlikely;
        }
    }

    // Corner occupancy final value
    double corner_value = 0;
    if (p_corner[0] + p_corner[1] != 0)
        corner_value = 100.0 * (p_corner[player] - p_corner[player ^ 1]) /
            (p_corner[0] + p_corner[1]);

    // Stability
    // Count the number of stable, unstable and semi-stable coins
    // Stable   = can't be flipped anymore
    // Unstable = can be flipped in the next move
    int p_stability[2] = {0, 0};

    get_stability(b);
    get_instability(b);
    for (int i = 0; i < Board::n; i++) {
        for (int j = 0; j < Board::n; j++) {
            if (b.is_empty(i, j)) continue;
            // stability
            if (pos_stable[i][j])
                p_stability[(int) b.g[i][j]]++;
            // instability
            if (pos_unstable[i][j])
                p_stability[(int) b.g[i][j]]--;
        }
    }

    double stability = 0;
    if (p_stability[0] + p_stability[1] != 0)
        stability = 100.0 * (p_stability[player] - p_stability[player ^ 1]) /
            (p_stability[0] + p_stability[1]);

    // Final weighted score
    double score = 10 * parity + 200 * corner_value + 60 * mobility + 180 * stability;
    return score;
}

#endif // SCORE_H
