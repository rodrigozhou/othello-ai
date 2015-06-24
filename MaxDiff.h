#ifndef MAXDIFF_H
#define MAXDIFF_H

#include <algorithm>

#include "Board.h"

using namespace std;

struct MaxDiff : Strategy {
    const static int MAX_DEPTH = 10;

    int dfs(const Board &b, int player, int depth = 0,
            int alpha = -INF, int beta = INF) {
        if (depth >= MAX_DEPTH)
            return b.cnt[player] - b.cnt[player^1];
        Board newb;
        bool has_empty = false;
        int ret = -INF;
        for (int i = 0; i < b.n; i++) {
            for (int j = 0; j < b.n; j++) {
                if (b.is_empty(i, j))
                    has_empty = true;
                if (b.is_valid_move(player, i, j)) {
                    newb = b.make_move(player, i, j);
                    int aux = -dfs(newb, player ^ 1, depth + 1, -beta, -alpha);
                    ret = max(ret, aux);
                    alpha = max(alpha, aux);
                    if (alpha >= beta)
                        return alpha;
                }
            }
        }
        if (ret == -INF && has_empty)
            return -dfs(b, player ^ 1, depth + 1, -beta, -alpha);
        return ret;
    }

    pii get_next_move(const Board &b, int player) {
        pii ret = mp(-1, -1);
        Board newb;
        int best = -INF, cnt = 0;
        for (int i = 0; i < b.n; i++) {
            for (int j = 0; j < b.n; j++) {
                if (b.is_valid_move(player, i, j)) {
                    newb = b.make_move(player, i, j);
                    int aux = -dfs(newb, player ^ 1, 0, -INF, -best);
                    if (aux > best) {
                        best = aux;
                        cnt = 1;
                        ret = mp(i, j);
                    }
                }
            }
        }
        return ret;
    }

};

#endif // MAXDIFF_H
