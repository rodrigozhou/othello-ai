#ifndef PVS_Endgame_H
#define PVS_Endgame_H

#include <algorithm>
#include <vector>

#include "Board.h"
#include "Score.h"

using namespace std;

struct PVS_Endgame : Strategy {
    const static int cnt_tiles = Board::n * Board::n;
    const static int MAX_DEPTH = 7;
    const static int MAX_DEPTH_ENDGAME = 16;
    const static int MAX_SHALLOW_DEPTH = 2;

    double shallow_search(const Board &b, int player, int depth = 0,
            double alpha = -INF, double beta = INF) {
        if (depth >= MAX_SHALLOW_DEPTH)
            return score(b, player);
        Board newb;
        bool has_empty = false;
        double ret = -INF;
        for (int i = 0; i < b.n; i++) {
            for (int j = 0; j < b.n; j++) {
                if (b.is_empty(i, j))
                    has_empty = true;
                if (b.is_valid_move(player, i, j)) {
                    newb = b.make_move(player, i, j);
                    double aux = -shallow_search(newb, player ^ 1, depth + 1, -beta, -alpha);
                    ret = max(ret, aux);
                    alpha = max(alpha, aux);
                    if (alpha >= beta)
                        return alpha;
                }
            }
        }
        if (ret == -INF && has_empty)
            return -shallow_search(b, player ^ 1, depth + 1, -beta, -alpha);
        return ret;
    }

    int dfs_endgame(const Board &b, int player, int depth = 0,
            int alpha = -INF, int beta = INF) {
        if (depth >= MAX_DEPTH_ENDGAME)
            return b.cnt[player] - b.cnt[player^1];
        Board newb;
        bool has_empty = false;
        int ret = -INF;
        bool first_child = true;
        for (int i = 0; i < b.n; i++) {
            for (int j = 0; j < b.n; j++) {
                if (b.is_empty(i, j))
                    has_empty = true;
                if (b.is_valid_move(player, i, j)) {
                    newb = b.make_move(player, i, j);
                    int val;
                    if (!first_child) {
                        val = -dfs_endgame(newb, player ^ 1, depth + 1, -alpha - 1, -alpha);
                        if (alpha < val && val < beta) // Check for failure.
                            val = -dfs_endgame(newb, player ^ 1, depth + 1, -beta, -val);
                    }
                    else {
                        val = -dfs_endgame(newb, player ^ 1, depth + 1, -beta, -alpha);
                        first_child = false;
                    }
                    alpha = max(alpha, val);
                    ret = max(ret, val);
                    if (alpha >= beta)
                        return alpha;
                }
            }
        }
        if (ret == -INF && has_empty)
            return -dfs_endgame(b, player ^ 1, depth + 1, -beta, -alpha);
        return ret;
    }

    double dfs(const Board &b, int player, int depth = 0,
            double alpha = -INF, double beta = INF) {
        if (depth >= MAX_DEPTH)
            return score(b, player);
        Board newb;
        bool has_empty = false, first_child = true;
        double ret = -INF;
        for (int i = 0; i < b.n; i++) {
            for (int j = 0; j < b.n; j++) {
                if (b.is_empty(i, j))
                    has_empty = true;
                if (b.is_valid_move(player, i, j)) {
                    newb = b.make_move(player, i, j);
                    double val;
                    if (!first_child) {
                        val = -dfs(newb, player ^ 1, depth + 1, -alpha - 1, -alpha);
                        if (alpha < val && val < beta) // Check for failure.
                            val = -dfs(newb, player ^ 1, depth + 1, -beta, -val);
                    }
                    else {
                        val = -dfs(newb, player ^ 1, depth + 1, -beta, -alpha);
                        first_child = false;
                    }
                    alpha = max(alpha, val);
                    ret = max(ret, val);
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
        vector< pair<double, pii> > pq;
        pii ret = mp(-1, -1);
        Board newb;
        double best = -INF;
        bool endgame = cnt_tiles - b.cnt[0] - b.cnt[1] <= MAX_DEPTH_ENDGAME;
        for (int i = 0; i < b.n; i++) {
            for (int j = 0; j < b.n; j++) {
                if (b.is_valid_move(player, i, j)) {
                    newb = b.make_move(player, i, j);
                    if (!endgame) {
                        pq.push_back(mp(-shallow_search(newb, player ^ 1, 0, -INF, -best),
                                    mp(i, j)));
                    } else {
                        int aux = -dfs_endgame(newb, player ^ 1, 0, -INF, -best);
                        if (aux > best) {
                            best = aux;
                            ret = mp(i, j);
                        }
                    }
                }
            }
        }

        if (!endgame) {
            sort(pq.begin(), pq.end());
            reverse(pq.begin(), pq.end());
            for (int i = 0; i < (int) pq.size(); i++) {
                newb = b.make_move(player, pq[i].second.first, pq[i].second.second);
                double aux = -dfs(newb, player ^ 1, 0, -INF, -best);
                if (aux > best) {
                    best = aux;
                    ret = pq[i].second;
                }
            }
        }

        return ret;
    }

};

#endif // PVS_Endgame_H
