#ifndef BOARD_H
#define BOARD_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <random>

using namespace std;

#define INF 0x3f3f3f3f

#define mp make_pair
#define fi first
#define se second

typedef pair<int, int> pii;
typedef pair<int, pii> piii;

#define EMPTY -1
#define BLACK 0
#define WHITE 1

struct Board {
    const static int n = 8;
    char g[n][n];
    int cnt[2];

    Board() {
        memset(g, EMPTY, sizeof(g));
        g[3][4] = g[4][3] = BLACK;
        g[3][3] = g[4][4] = WHITE;
        cnt[0] = cnt[1] = 2;
    }

    static bool is_valid(int x, int y) {
        return 0 <= x && x < n && 0 <= y && y < n;
    }

    inline bool is_empty(int x, int y) const {
        return g[x][y] == EMPTY;
    }

    int count_gain(int player, int x, int y) const {
        if (g[x][y] != EMPTY)
            return -1;
        int ret = 0;
        for (int dx = -1; dx < 2; dx++) {
            for (int dy = -1; dy < 2; dy++) {
                if (dx == 0 && dy == 0)
                    continue;
                for (int i = x+dx, j = y+dy, cnt = 0; ; i += dx, j += dy) {
                    if (!is_valid(i, j) || g[i][j] == EMPTY)
                        break;
                    if (g[i][j] == player) {
                        ret += cnt;
                        break;
                    }
                    cnt++;
                }
            }
        }
        return ret;
    }

    bool is_valid_move(int player, int x, int y) const {
        if (g[x][y] != EMPTY)
            return false;
        for (int dx = -1; dx < 2; dx++) {
            for (int dy = -1; dy < 2; dy++) {
                if (dx == 0 && dy == 0)
                    continue;
                for (int i = x+dx, j = y+dy, cnt = 0; ; i += dx, j += dy) {
                    if (!is_valid(i, j) || g[i][j] == EMPTY)
                        break;
                    if (g[i][j] == player) {
                        if (cnt > 0)
                            return true;
                        break;
                    }
                    cnt++;
                }
            }
        }
        return false;
    }

    bool has_move(int player) const {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (is_valid_move(player, i, j))
                    return true;
        return false;
    }

    Board make_move(int player, int x, int y) const {
        Board ret = *this;
        int gain = 0;
        for (int dx = -1; dx < 2; dx++) {
            for (int dy = -1; dy < 2; dy++) {
                if (dx == 0 && dy == 0)
                    continue;
                int cnt = 0;
                for (int i = x+dx, j = y+dy; ; i += dx, j += dy) {
                    if (!is_valid(i, j) || ret.g[i][j] == EMPTY) {
                        cnt = 0;
                        break;
                    }
                    if (ret.g[i][j] == player)
                        break;
                    cnt++;
                }
                if (cnt > 0) {
                    for (int i = x+dx, j = y+dy; ; i += dx, j += dy) {
                        if (ret.g[i][j] == player)
                            break;
                        ret.g[i][j] = player;
                    }
                    gain += cnt;
                }
            }
        }
        ret.g[x][y] = player;
        ret.cnt[player] += gain + 1;
        ret.cnt[player^1] -= gain;
        return ret;
    }

    void print() const {
        printf("  ");
        for (int j = 0; j < n; j++)
            printf("%c", 'a'+j);
        puts("");
        for (int i = 0; i < n; i++) {
            printf("%d ", i);
            for (int j = 0; j < n; j++)
                printf("%c", g[i][j] == EMPTY ? '.' : g[i][j] == BLACK ? 'B' : 'W');
            puts("");
        }
        printf("Black: %d / White: %d\n", cnt[BLACK], cnt[WHITE]);
    }
};

struct Strategy {
    minstd_rand gen;

    Strategy() {
        this_thread::sleep_for(chrono::milliseconds(100));
        random_device rd;
        gen = minstd_rand(rd());
    }

    double rand01() {
        uniform_real_distribution<double> dist(0, 1);
        return dist(gen);
    }

    virtual pii get_next_move(const Board &b, int player) = 0;
};

#endif // BOARD_H
