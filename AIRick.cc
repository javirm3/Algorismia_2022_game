#include "Player.hh"
#include <queue>
#define PLAYER_NAME Rick

typedef vector<int> VI;
typedef vector<VI> VVI;
typedef vector<bool> VB;
typedef vector<VB> VVB;
typedef vector<Pos> VP;
typedef vector<VP> VVP;
typedef pair<int, Pos> tri;
typedef vector<tri> VT;
typedef vector<VT> VVT;

struct PLAYER_NAME : public Player {
    static Player* factory()
    {
        return new PLAYER_NAME;
    }

    const vector<Dir> dirs = { Up, Down, Left, Right };
    using VB = vector<bool>;
    using VVB = vector<VB>;

    struct Dpd {
        Dir dir;
        Pos p;
        int dist;
    };

    bool pos_correct(Pos p)
    {
        return (pos_ok(p) and cell(p).type != Waste);
    }

    vector<Pos> get_food()
    {
        vector<Pos> food_pos;
        for (int i = 0; i < 60; i++) {
            for (int j = 0; j < 60; j++) {
                if (cell(i, j).food)
                    food_pos.push_back(Pos(i, j));
            }
        }
        return food_pos;
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

    int distance(Pos p1, Pos p2)
    {
        return (abs(p2.i - p1.i) + abs(p2.j - p1.j));
    }
    void BFS(int& dist, Dir& opt_dir, Pos p1, Pos p2)
    {
        VVB visited(60, VB(60, false));
        queue<Dpd> Q;
        for (Dir d : dirs) {
            if (pos_correct(p1 + d)) {
                Q.push({ d,
                    p1 + d,
                    1 });
                visited[(p1 + d).i][(p1 + d).j] = true;
                if (p1 + d == p2) {
                    opt_dir = d;
                    dist = 1;
                    return;
                }
            }
        }
        while (not Q.empty()) {
            Dpd x = Q.front();
            Q.pop();
            for (Dir d : dirs) {
                Pos new_pos = x.p + d;
                if (pos_correct(new_pos) and not visited[new_pos.i][new_pos.j]) {
                    visited[new_pos.i][new_pos.j] = true;
                    Q.push({ x.dir,
                        new_pos,
                        x.dist + 1 });
                    if (new_pos == p2) {
                        opt_dir = x.dir;
                        dist = x.dist + 1;
                        return;
                    }
                }
            }
        }
    }

    void BFS_food(int& dist, Dir& opt_dir, Pos p1)
    {
        VVB visited(60, VB(60, false));
        queue<Dpd> Q;
        for (Dir d : dirs) {
            if (pos_correct(p1 + d)) {
                Q.push({ d,
                    p1 + d,
                    1 });
                visited[(p1 + d).i][(p1 + d).j] = true;
                if (cell(p1 + d).food) {
                    opt_dir = d;
                    dist = 1;
                    return;
                }
            }
        }
        while (not Q.empty()) {
            Dpd x = Q.front();
            Q.pop();
            for (Dir d : dirs) {
                Pos new_pos = x.p + d;
                if (pos_correct(new_pos) and not visited[new_pos.i][new_pos.j]) {
                    visited[new_pos.i][new_pos.j] = true;
                    Q.push({ x.dir,
                        new_pos,
                        x.dist + 1 });
                    if (cell(new_pos).food) {
                        opt_dir = x.dir;
                        dist = x.dist + 1;
                        return;
                    }
                }
            }
        }
    }
    virtual void play()
    {
        vector<int> alive = alive_units(me());
        vector<Pos> food_positions = get_food();
        int dist_min = 1000;
        Dir dir_min;
        for (int id : alive) {
            int dist = 0;
            Dir opt_dir = Up;
            for (Pos food_pos : food_positions) {
                BFS(dist, opt_dir, unit(id).pos, food_pos);
                if (dist < dist_min) {
                    dir_min = opt_dir;
                    dist_min = dist;
                }
            }
            move(id, opt_dir);

            // int dist = 0;
            // Dir opt_dir;
            // BFS_food(dist, opt_dir, unit(id).pos);
            // move(id, opt_dir);
        }
    }
};

RegisterPlayer(PLAYER_NAME);
