#include "Player.hh"
#include <queue>
#define PLAYER_NAME Zarathustra4

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
    int INF = 1e7;
    struct Dpd {
        Dir dir;
        Pos p;
        int dist;
    };
    struct Dpdu {
        Dir dir;
        Pos p;
        int dist;
        int unit;
    };
    struct movement {
        Dir dir;
        int id;
        int priority;
        bool operator<(const movement& a) const { return priority < a.priority; }
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
    void BFS_enemy_no_zombie(int& dist, Dir& opt_dir, Pos p1, int team)
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
                if (thereis_enemy_no_zombie(p1 + d, team, 1)) {
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
                    if (thereis_enemy_no_zombie(new_pos, team, x.dist + 1)) {
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
    void BFS_empty(Dir& opt_dir, Pos p1)
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
                if (cell(p1 + d).owner != me()) {
                    opt_dir = d;
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
                    if (cell(new_pos).owner != me()) {
                        opt_dir = x.dir;
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
    bool thereis_enemy_no_zombie(Pos p, int team, int dist)
    {
        if (cell(p).id != -1 and unit(cell(p).id).type == Alive) {
            if (unit(cell(p).id).rounds_for_zombie == -1 or unit(cell(p).id).rounds_for_zombie >= dist) {
                if (team == -1) {
                    return (unit(cell(p).id).player != me());
                } else
                    return (unit(cell(p).id).player == team);
            }
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
    Dir dir_alternative_zombie(Pos p, Dir d)
    {
        //cerr << p << " " << d;
        for (int i : random_permutation(4)) {
            Dir d1 = dirs[i];
            if (d1 != d and pos_correct(p + d1)) {
                int dist_zombie;
                Dir dir_zombie;
                BFS_zombie(dist_zombie, dir_zombie, p + d1);
                if (dist_zombie > 1 and dir_zombie != d1)
                    return d1;
            }
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

    void multiBFS_food(Pos p, vector<bool>& used, int& dist, Dir& opt_dir, int& mov_unit)
    {
        VVB visited(60, VB(60, false));
        queue<Dpdu> Q;
        for (int act_unit : alive_units(me())) {
            if (not used[act_unit]) {
                p1 = unit(act_unit).pos;
                for (int i : random_permutation(4)) {
                    Dir d = dirs[i];
                    if (pos_correct(p1 + d)) {
                        Q.push({ d,
                            p1 + d,
                            1,
                            act_unit });
                        visited[(p1 + d).i][(p1 + d).j] = true;
                        if (p1 + d == p) {
                            opt_dir = d;
                            dist = 1;
                            mov_unit = act_unit;
                            return;
                        }
                    }
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
                        x.dist + 1,
                        x.unit });
                    if (cell(new_pos).food) {
                        opt_dir = x.dir;
                        dist = x.dist + 1;
                        mov_unit = x.unit;
                        return;
                    }
                }
            }
        }
    }
    void catch_food(vector<bool>& used)
    {
        int mov_unit, dist;
        Dir opt_dir;
        for (Pos p : get_food()) {
            multiBFS_food(p, used, dist, opt_dir, mov_unit);
            used[mov_unit] == true;
            // PROBLEMA: Cuando miro primero una comida que tiene una unidad a 3
            //y luego esa unidad tiene una comida a 2. Se movera a la mirada primero.
            // Posible sol: Multi BFS buscando cualquier comida desde todas las unidades
            // Y voy tachando la comida y las unidades conforme hago (vector de unidades, map de comida)
        }
    }

    priority_queue<movement> merge(priority_queue<movement> Q1, priority_queue<movement> Q2)
    {
        while (not Q1.empty()) {
            Q2.push(Q1.top());
            Q1.pop();
        }
        return Q2;
    }

    movement best_dir(Pos p)
    {
        int id = cell(p).id;
        movement act_move;
        int dist_food = INF, dist_enemy = INF, dist_enemy_flojo = INF, dist_zombie = INF, dist_dead = INF;
        Dir dir_food, dir_enemy, dir_zombie, dir_dead, opt_dir;
        BFS_food(dist_food, dir_food, p);
        int team_flojo = 0, team_fuerte = 0, rounds_dead = 0;
        BFS_enemy_no_zombie(dist_enemy, dir_enemy, p, -1);
        BFS_zombie(dist_zombie, dir_zombie, p);
        //BFS_dead(dist_dead, dir_dead, rounds_dead, unit(id).pos);
        act_move.id = id;
        if (dist_enemy == 1) {
            act_move.dir = dir_enemy;
            act_move.priority = 5;
        } else {
            if (dist_zombie == 1) {
                act_move.dir = dir_zombie;
                act_move.priority = 2;
            } else if (dist_food < dist_enemy) {
                //cerr << p << " FOOD < ENEMY " << dist_food << dir_food << " " << dist_enemy << dir_enemy << endl;
                act_move.dir = dir_food;
                act_move.priority = 2;
            } else {
                if (dist_enemy <= 4) {
                    if (dist_enemy == 2)
                        act_move.priority = 1;
                    else
                        act_move.priority = 2;
                    act_move.dir = dir_enemy;
                } else {
                    if (dist_zombie == 2) {
                        act_move.priority = 2;
                        act_move.dir = dir_alternative_zombie(p, dir_zombie);
                    } else if (dist_food <= 7) {
                        act_move.priority = 2;
                        act_move.dir = dir_food;
                    } else {
                        act_move.priority = 2;
                        //BFS_empty(act_move.dir, p);
                    }
                }
            }
        }
        return act_move;
    }

    void move_units()
    {
        vector<int> alive = alive_units(me());
        vector<bool>& used;
        priority_queue<movement> moves;
        for (int id : alive) {
            moves.push(best_dir(unit(id).pos));
        }
        while (not moves.empty()) {
            move(moves.top().id, moves.top().dir);
            moves.pop();
        }
    }

    virtual void play()
    {
        move_units();
    }
};

RegisterPlayer(PLAYER_NAME);
