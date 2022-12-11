#include "Player.hh"
#include <queue>
#define PLAYER_NAME Zarathustra2

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
    int INF = 1e7;
    struct Dpd {
        Dir dir;
        Pos p;
        int dist;
    };
    int min3(int x, int y, int z)
    {
        return min(min(x, y), z);
    }

    bool pos_correct(Pos p)
    {
        return (pos_ok(p) and cell(p).type == Street);
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
        for (int i : random_permutation(4)) {
            Dir d = dirs[i];
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
    bool thereis_enemy(Pos p, int team)
    {
        if (cell(p).id != -1) {
            if (team == -1) {
                return (unit(cell(p).id).player != me());
            } else
                return (unit(cell(p).id).player == team);
        }
        return false;
    }
    bool thereis_zombie(Pos p)
    {
        if (cell(p).id != -1)
            if (unit(cell(p).id).type == Zombie)
                return true;
        return false;
    }
    bool thereis_dead(Pos p)
    {
        if (cell(p).id != -1)
            if (unit(cell(p).id).type == Dead)
                return true;
        return false;
    }
    void BFS_enemy(int& dist, Dir& opt_dir, Pos p1, int team)
    {
        VVB visited(60, VB(60, false));
        queue<Dpd> Q;
        for (int i : random_permutation(4)) {
            Dir d = dirs[i];
            if (pos_correct(p1 + d)) {
                Q.push({ d,
                    p1 + d,
                    1 });
                visited[(p1 + d).i][(p1 + d).j] = true;
                if (thereis_enemy(p1 + d, team)) {
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
                    int id = (cell(new_pos).id);
                    if (thereis_enemy(new_pos, team)) {
                        opt_dir = x.dir;
                        dist = x.dist + 1;
                        return;
                    }
                }
            }
        }
    }
    void BFS_zombie(int& dist, Dir& opt_dir, Pos p1)
    {
        VVB visited(60, VB(60, false));
        queue<Dpd> Q;
        for (int i : random_permutation(4)) {
            Dir d = dirs[i];
            if (pos_correct(p1 + d)) {
                Q.push({ d,
                    p1 + d,
                    1 });
                visited[(p1 + d).i][(p1 + d).j] = true;
                if (thereis_zombie(p1 + d)) {
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
                    if (thereis_zombie(new_pos)) {
                        opt_dir = x.dir;
                        dist = x.dist + 1;
                        return;
                    }
                }
            }
        }
    }
    void BFS_dead(int& dist, Dir& opt_dir, int& rounds, Pos p1)
    {
        VVB visited(60, VB(60, false));
        queue<Dpd> Q;
        for (int i : random_permutation(4)) {
            Dir d = dirs[i];
            if (pos_correct(p1 + d)) {
                Q.push({ d,
                    p1 + d,
                    1 });
                visited[(p1 + d).i][(p1 + d).j] = true;
                if (thereis_dead(p1 + d)) {
                    opt_dir = d;
                    dist = 1;
                    rounds = unit(cell(p1 + d).id).rounds_for_zombie;
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
                    if (thereis_dead(new_pos)) {
                        opt_dir = x.dir;
                        dist = x.dist + 1;
                        rounds = unit(cell(p1 + d).id).rounds_for_zombie;
                        return;
                    }
                }
            }
        }
    }
    Dir dir_cont(Dir d)
    {
        int i = 0;
        int s;
        for (Dir dir : dirs) {
            if (d == dir)
                s = i;
            i++;
        }
        return dirs[(s + 2) % 4];
    }
    Dir dir_alternative(Pos p, Dir d)
    {
        for (int i : random_permutation(4)) {
            Dir d1 = dirs[i];
            if (d1 != d and pos_correct(p + d1))
                return d1;
        }
        return d;
    }

    void flojo_fuerte(int& min_player, int& max_player)
    {
        int min_strength = INF;
        int max_strength = 0;
        for (int i = 0; i < num_players(); i++) {
            if (strength(i) < min_strength) {
                min_player = i;
                min_strength = strength(i);
            }
            if (strength(i) > max_strength) {
                max_player = i;
                max_strength = strength(i);
            }
        }
        return;
    }

    void move_units()
    {
        vector<int> alive = alive_units(me());
        vector<Pos> food_positions = get_food();
        for (int id : alive) {
            int dist_food = INF, dist_enemy = INF, dist_enemy_flojo = INF, dist_zombie = INF, dist_dead = INF;
            Dir dir_food, dir_enemy, dir_enemy_flojo, dir_zombie, dir_dead, opt_dir;
            BFS_food(dist_food, dir_food, unit(id).pos);
            int team_flojo = 0, team_fuerte = 0, rounds_dead = 0;
            flojo_fuerte(team_flojo, team_fuerte);
            if (team_flojo != me())
                BFS_enemy(dist_enemy_flojo, dir_enemy_flojo, unit(id).pos, team_flojo);
            else
                BFS_enemy(dist_enemy, dir_enemy, unit(id).pos, -1);
            BFS_zombie(dist_zombie, dir_zombie, unit(id).pos);
            BFS_dead(dist_dead, dir_dead, rounds_dead, unit(id).pos);

            // if (dist_dead < 2 and rounds_dead < dist_dead)
            //     opt_dir = dir_alternative(unit(id).pos, dir_dead);
            // else
            if (team_flojo == me())
                if (dist_food < dist_enemy + 3)
                    opt_dir = dir_food;
                else
                    opt_dir = dir_enemy;
            else {
                if ((dist_enemy_flojo < dist_zombie) and (dist_enemy_flojo < dist_enemy + 2) and (dist_enemy_flojo < dist_food))
                    opt_dir = dir_enemy_flojo;
                else if (dist_enemy < 3 + dist_zombie)
                    opt_dir = dir_enemy;
                else if (dist_food < dist_zombie)
                    opt_dir = dir_food;
                else
                    opt_dir = dir_zombie;
            }
            // if (unit(id).rounds_for_zombie != -1) {
            //     if (dist_food <= unit(id).rounds_for_zombie)
            //         opt_dir = dir_food;
            //     else if (dist_zombie <= unit(id).rounds_for_zombie)
            //         opt_dir = dir_zombie;
            //     else if (dist_enemy < dist_zombie)
            //         opt_dir = dir_enemy;
            //     else
            //         opt_dir = dir_zombie;
            // }
            move(id, opt_dir);
        }
    }
    virtual void play()
    {
        move_units();
    }
};

RegisterPlayer(PLAYER_NAME);
