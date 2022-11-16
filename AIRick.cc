#include "Player.hh"

#define PLAYER_NAME Rick

typedef vector<int> VI;
typedef vector<VI> VVI;
typedef vector<bool> VB;
typedef vector<VB> VVB;

struct PLAYER_NAME : public Player {
    static Player* factory()
    {
        return new PLAYER_NAME;
    }

    const vector<Dir> dirs = { Up, Down, Left, Right };
    using VB = vector<bool>;
    using VVB = vector<VB>;

    VVB get_board()
    {
        VVB table(60, VB(60));
        for (int i = 0; i < 60; i++) {
            for (int j = 0; j < 60; j++) {
                if (cell(i, j).type == Waste)
                    table[i][j] = true;
            }
        }
        return table;
    }

    VVI board_prop()
    {
        VVI proportions(3, VI(3, 0));
        for (int i = 0; i < 60; i++) {
            for (int j = 0; j < 60; j++) {
                if (pos_correct(Pos(i, j)) and cell(i, j).owner != me())
                    proportions[i / 20][j / 20]++;
            }
        }
        return proportions;
    }
    Pos best_quad(Pos p, VVI proportions)
    {
        int max = 0;
        Pos quad;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int distance_center = 1 + distance(p, Pos(20 * i + 10, 20 * j + 10));
                int weight = proportions[i][j] / distance_center;
                if (weight > max) {
                    max = weight;
                    quad = Pos(20 * i + 10, 20 * j + 10);
                }
            }
        }
        return quad;
    }

    bool pos_correct(Pos p)
    {
        return (pos_ok(p) and cell(p).type != Waste);
    }
    int distance(Pos p1, Pos p2)
    {
        return (abs(p2.i - p1.i) + abs(p2.j - p1.j));
    }

    virtual void play()
    {
        VVB table(60, VB(60));
        if (round() == 1)
            table = get_board();
        vector<int> alive = alive_units(me());
        for (int id : alive) {
            bool enter = false;
            for (Dir d : dirs) {
                Pos new_pos = unit(id).pos + d;
                if (pos_correct(new_pos) and cell(new_pos).food) {
                    move(id, d);
                    enter = true;
                }
            }
            for (Dir d : dirs) {
                Pos new_pos = unit(id).pos + d;
                if (pos_correct(new_pos) and cell(new_pos).owner != me() and not enter) {
                    move(id, d);
                    enter = true;
                }
            }
            for (Dir d : dirs) {
                Pos new_pos = unit(id).pos + d;
                if (pos_correct(new_pos) and unit(cell(new_pos).id).player != me() and not enter) {
                    move(id, d);
                    enter = true;
                }
            }
            Pos quad = best_quad(unit(id).pos, board_prop());
            for (Dir d : dirs) {
                Pos new_pos = unit(id).pos + d;
                if (pos_correct(new_pos)) {
                    if (distance(quad, new_pos) < distance(quad, unit(id).pos)) {
                        move(id, d);
                        enter = true;
                    }
                }
            }
        }
    }
};

RegisterPlayer(PLAYER_NAME);
