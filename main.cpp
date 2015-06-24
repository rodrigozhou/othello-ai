#include <cstdio>
#include <thread>

#include "Board.h"
#include "MaxDiff.h"
#include "PVS_Endgame.h"

using namespace std;

int game(Strategy *stb, Strategy *stw, bool log = true) {
    Board board;
    Strategy *st[2] = { stb, stw };
    bool turn_passed = false;
    for (int round = 0; ; round++) {
        int player = round & 1;
        if (!stb || !stw || log) {
            if (round) puts("");
            printf("Round %d: %s\n", round, player == BLACK ? "Black" : "White");
            board.print();
        }
        if (st[player]) {
            pii move = st[player]->get_next_move(board, player);
            if (move == mp(-1, -1)) {
                if (log) printf("No possible move\n\n");
                if (turn_passed)
                    break;
                turn_passed = true;
                continue;
            }
            turn_passed = false;
            if (log) printf("Movement: %d %c\n", move.fi, 'a'+move.se);
            board = board.make_move(player, move.fi, move.se);
        }
        else {
            if (!board.has_move(player)) {
                printf("No possible move\n\n");
                if (turn_passed)
                    break;
                turn_passed = true;
                continue;
            }
            turn_passed = false;
            int x, y;
            char c;
            printf("Choose a position (row, col): ");
            scanf("%d %c", &x, &c);
            y = c - 'a';
            if (board.count_gain(player, x, y) == 0) {
                printf("Invalid move\n");
                round--;
                continue;
            }
            board = board.make_move(player, x, y);
        }
    }
    if (!stb || !stw || log) {
        printf("End of game\n");
        board.print();
    }
    return board.cnt[0] - board.cnt[1];
}

int main() {
    // Single game test
    int r = game(new MaxDiff(), new PVS_Endgame(), true);
    printf("%s\n", r > 0 ? "Black" : r < 0 ? "White" : "Draw");
}
