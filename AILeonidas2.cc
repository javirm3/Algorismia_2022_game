#include "Player.hh"
#include <queue>
#define PLAYER_NAME Leonidas2

typedef vector<bool> VB;
typedef vector<VB> VVB;

struct PLAYER_NAME : public Player
{
    static Player *factory()
    {
        return new PLAYER_NAME;
    }

    const vector<Dir> dirs = {Up, Down, Left, Right};
    int INF = 1e7;
    struct Dpd
    {
        Dir dir;
        Pos p;
        int dist;
    };
    struct Dpdu
    {
        Dir dir;
        Pos p;
        int dist;
        int unit;
    };
    struct movement
    {
        Dir dir;
        int id;
        int priority;
        bool operator<(const movement &a) const { return priority < a.priority; }
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
        for (int i = 0; i < 60; i++)
        {
            for (int j = 0; j < 60; j++)
            {
                if (cell(i, j).food)
                    food_pos.push_back(Pos(i, j));
            }
        }
        return food_pos;
    }

    void BFS(int &dist, Dir &opt_dir, Pos p1, Pos p2)
    {
        VVB visited(60, VB(60, false));
        queue<Dpd> Q;
        for (Dir d : dirs)
        {
            if (pos_correct(p1 + d))
            {
                Q.push({d,
                        p1 + d,
                        1});
                visited[(p1 + d).i][(p1 + d).j] = true;
                if (p1 + d == p2)
                {
                    opt_dir = d;
                    dist = 1;
                    return;
                }
            }
        }
        while (not Q.empty())
        {
            Dpd x = Q.front();
            Q.pop();
            for (Dir d : dirs)
            {
                Pos new_pos = x.p + d;
                if (pos_correct(new_pos) and not visited[new_pos.i][new_pos.j])
                {
                    visited[new_pos.i][new_pos.j] = true;
                    Q.push({x.dir,
                            new_pos,
                            x.dist + 1});
                    if (new_pos == p2)
                    {
                        opt_dir = x.dir;
                        dist = x.dist + 1;
                        return;
                    }
                }
            }
        }
    }
    void BFS_enemy_no_zombie(int &dist, Dir &opt_dir, Pos p1, int team)
    {
        VVB visited(60, VB(60, false));
        queue<Dpd> Q;
        for (int i : random_permutation(4))
        {
            Dir d = dirs[i];
            if (pos_correct(p1 + d))
            {
                Q.push({d,
                        p1 + d,
                        1});
                visited[(p1 + d).i][(p1 + d).j] = true;
                if (thereis_enemy_no_zombie(p1 + d, team, 1))
                {
                    opt_dir = d;
                    dist = 1;
                    return;
                }
            }
        }
        while (not Q.empty())
        {
            Dpd x = Q.front();
            Q.pop();
            for (Dir d : dirs)
            {
                Pos new_pos = x.p + d;
                if (pos_correct(new_pos) and not visited[new_pos.i][new_pos.j])
                {
                    visited[new_pos.i][new_pos.j] = true;
                    Q.push({x.dir,
                            new_pos,
                            x.dist + 1});
                    int id = (cell(new_pos).id);
                    if (thereis_enemy_no_zombie(new_pos, team, x.dist + 1))
                    {
                        opt_dir = x.dir;
                        dist = x.dist + 1;
                        return;
                    }
                }
            }
        }
        opt_dir = Up;
        dist = INF;
    }
    void BFS_zombie(int &dist, Dir &opt_dir, Pos p1)
    {
        VVB visited(60, VB(60, false));
        queue<Dpd> Q;
        for (int i : random_permutation(4))
        {
            Dir d = dirs[i];
            if (pos_correct(p1 + d))
            {
                Q.push({d,
                        p1 + d,
                        1});
                visited[(p1 + d).i][(p1 + d).j] = true;
                if (thereis_zombie(p1 + d, 1))
                {
                    opt_dir = d;
                    dist = 1;
                    return;
                }
            }
        }
        while (not Q.empty())
        {
            Dpd x = Q.front();
            Q.pop();
            for (Dir d : dirs)
            {
                Pos new_pos = x.p + d;
                if (pos_correct(new_pos) and not visited[new_pos.i][new_pos.j])
                {
                    visited[new_pos.i][new_pos.j] = true;
                    Q.push({x.dir,
                            new_pos,
                            x.dist + 1});
                    if (thereis_zombie(new_pos, x.dist + 1))
                    {
                        opt_dir = x.dir;
                        dist = x.dist + 1;
                        return;
                    }
                }
            }
        }
        opt_dir = Up;
        dist = INF;
    }
    void BFS_dead(int &dist, Dir &opt_dir, int &rounds, Pos p1)
    {
        VVB visited(60, VB(60, false));
        queue<Dpd> Q;
        for (int i : random_permutation(4))
        {
            Dir d = dirs[i];
            if (pos_correct(p1 + d))
            {
                Q.push({d,
                        p1 + d,
                        1});
                visited[(p1 + d).i][(p1 + d).j] = true;
                if (thereis_dead(p1 + d))
                {
                    opt_dir = d;
                    dist = 1;
                    rounds = unit(cell(p1 + d).id).rounds_for_zombie;
                    return;
                }
            }
        }
        while (not Q.empty())
        {
            Dpd x = Q.front();
            Q.pop();
            for (Dir d : dirs)
            {
                Pos new_pos = x.p + d;
                if (pos_correct(new_pos) and not visited[new_pos.i][new_pos.j])
                {
                    visited[new_pos.i][new_pos.j] = true;
                    Q.push({x.dir,
                            new_pos,
                            x.dist + 1});
                    if (thereis_dead(new_pos))
                    {
                        opt_dir = x.dir;
                        dist = x.dist + 1;
                        rounds = unit(cell(p1 + d).id).rounds_for_zombie;
                        return;
                    }
                }
            }
        }
    }
    void BFS_empty(Dir &opt_dir, Pos p1)
    {
        VVB visited(60, VB(60, false));
        queue<Dpd> Q;
        for (int i : random_permutation(4))
        {
            Dir d = dirs[i];
            if (pos_correct(p1 + d))
            {
                Q.push({d,
                        p1 + d,
                        1});
                visited[(p1 + d).i][(p1 + d).j] = true;
                if (cell(p1 + d).owner != me() and cell(p1 + d).id == -1)
                {
                    opt_dir = d;
                    return;
                }
            }
        }
        while (not Q.empty())
        {
            Dpd x = Q.front();
            Q.pop();
            for (Dir d : dirs)
            {
                Pos new_pos = x.p + d;
                if (pos_correct(new_pos) and not visited[new_pos.i][new_pos.j])
                {
                    visited[new_pos.i][new_pos.j] = true;
                    Q.push({x.dir,
                            new_pos,
                            x.dist + 1});
                    if (cell(new_pos).owner != me())
                    {
                        opt_dir = x.dir;
                        return;
                    }
                }
            }
        }
        opt_dir = Up;
        return;
    }

    bool thereis_enemy(Pos p, int team)
    {
        if (cell(p).id != -1 and unit(cell(p).id).type == Alive)
        {
            if (team == -1)
            {
                return (unit(cell(p).id).player != me());
            }
            else
                return (unit(cell(p).id).player == team);
        }
        return false;
    }
    bool thereis_enemy_no_zombie(Pos p, int team, int dist)
    {
        if (cell(p).id != -1 and unit(cell(p).id).type == Alive)
        {
            if (unit(cell(p).id).rounds_for_zombie == -1 or unit(cell(p).id).rounds_for_zombie >= dist)
            {
                if (strength(unit(cell(p).id).player) <= 2.7 * strength(me()))
                {
                    if (team == -1)
                    {
                        return (unit(cell(p).id).player != me());
                    }
                    else
                        return (unit(cell(p).id).player == team);
                }
            }
        }
        return false;
    }
    bool thereis_zombie(Pos p, int dist)
    {
        if (cell(p).id != -1)
            if (unit(cell(p).id).type == Zombie or (unit(cell(p).id).rounds_for_zombie != -1 and unit(cell(p).id).rounds_for_zombie != dist))
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

    Dir dir_alternative_zombie(Pos p, Dir d)
    {
        // cerr << p << " " << d;
        for (int i : random_permutation(4))
        {
            Dir d1 = dirs[i];
            if (d1 != d and pos_correct(p + d1))
            {
                int dist_zombie;
                Dir dir_zombie;
                BFS_zombie(dist_zombie, dir_zombie, p + d1);
                if (dist_zombie > 1 and dir_zombie != d1)
                    return d1;
            }
        }
        return d;
    }
    Dir dir_alternative_dead(Pos p, Dir d)
    {
        // cerr << p << " " << d;
        for (int i : random_permutation(4))
        {
            Dir d1 = dirs[i];
            priority_queue<pair<int, Dir>> possible_dirs;
            if (d1 != d and pos_correct(p + d1))
            {
                int dist_dead, dist_enemy, rounds;
                Dir dir_dead, dir_enemy;
                BFS_dead(dist_dead, dir_dead, rounds, p + d1);
                BFS_enemy_no_zombie(dist_enemy, dir_enemy, p + d1, -1);
                if (dist_dead > 1 and dir_dead != d1)
                    possible_dirs.push({dist_enemy, d1});
            }
            if (not possible_dirs.empty())
            {
                return possible_dirs.top().second;
            }
        }
        return d;
    }
    Dir dir_alternative_enemy(Pos p, Dir d)
    {
        // cerr << p << " " << d;
        for (int i : random_permutation(4))
        {
            Dir d1 = dirs[i];
            if (d1 != d and pos_correct(p + d1))
            {
                int dist_dead, rounds;
                Dir dir_dead;
                BFS_enemy_no_zombie(dist_dead, dir_dead, p + d1, -1);
                if (dist_dead > 1 and dir_dead != d1)
                    return d1;
            }
        }
        return d;
    }

    void multiBFS_food(Pos p, vector<bool> &used, map<Pos, bool> &catched_food, int &dist, Dir &opt_dir, int &mov_unit, int &enemy_dist)
    {
        VVB visited(60, VB(60, false));
        queue<Dpdu> Q;
        for (int act_unit : alive_units(me()))
        {
            if (not used[act_unit])
            {
                Pos p1 = unit(act_unit).pos;
                for (int i : random_permutation(4))
                {
                    Dir d = dirs[i];
                    if (pos_correct(p1 + d))
                    {
                        Q.push({d,
                                p1 + d,
                                1,
                                act_unit});
                        visited[(p1 + d).i][(p1 + d).j] = true;
                        if (thereis_enemy(p1 + d, -1))
                            enemy_dist = 1;
                        if (cell(p1 + d).food and not catched_food[p1 + d])
                        {
                            opt_dir = d;
                            dist = 1;
                            mov_unit = act_unit;
                            catched_food[p1 + d] = true;
                            return;
                        }
                    }
                }
            }
        }
        while (not Q.empty())
        {
            Dpdu x = Q.front();
            Q.pop();
            for (Dir d : dirs)
            {
                Pos new_pos = x.p + d;
                if (pos_correct(new_pos) and not visited[new_pos.i][new_pos.j])
                {
                    visited[new_pos.i][new_pos.j] = true;
                    Q.push({x.dir,
                            new_pos,
                            x.dist + 1,
                            x.unit});
                    if (thereis_enemy(new_pos, -1))
                        enemy_dist = x.dist + 1;
                    if (cell(new_pos).food and not catched_food[new_pos])
                    {
                        opt_dir = x.dir;
                        dist = x.dist + 1;
                        mov_unit = x.unit;
                        catched_food[new_pos] = true;
                        return;
                    }
                }
            }
        }
    }
    priority_queue<movement> catch_food(vector<bool> &used, map<int, map<string, int>> &distances)
    {
        int mov_unit, dist, count = 0;
        int enemy_dist = INF;
        Dir opt_dir;
        map<Pos, bool> catched_food;
        priority_queue<movement> Q;
        for (Pos p : get_food())
        {
            count = 0;
            while (not catched_food[p] and count < alive_units(me()).size())
            {
                multiBFS_food(p, used, catched_food, dist, opt_dir, mov_unit, enemy_dist);
                if (dist <= 12) // and (unit(mov_unit).rounds_for_zombie == -1 or unit(mov_unit).rounds_for_zombie >= dist))
                {
                    used[mov_unit] = true;
                    Q.push({opt_dir,
                            mov_unit,
                            2});
                }
                count++;
            }
        }
        return Q;
    }
    priority_queue<movement> kill_prox(vector<bool> &used, map<int, map<string, int>> &distances, map<int, map<string, Dir>> &directions, map<int, int> &rounds_dead)
    {
        vector<int> alive = alive_units(me());
        int mov_unit, dist;
        Dir opt_dir;
        priority_queue<movement> Q;
        for (int id : alive)
        {
            if (distances[id]["enemy"] == 1)
            {
                Q.push({directions[id]["enemy"],
                        id,
                        5});
                used[id] = true;
            }
            else if (distances[id]["dead"] == 1)
            {
                if (rounds_dead[id] == 1)
                    Q.push({directions[id]["dead"],
                            id,
                            2});
                else
                    Q.push({dir_alternative_dead(unit(id).pos, directions[id]["dead"]),
                            id,
                            2});
                used[id] = true;
            }
            else if (distances[id]["zombie"] == 1)
            {
                Q.push({directions[id]["zombie"],
                        id,
                        2});
                used[id] = true;
            }
        }
        return Q;
    }

    bool zombies_diagonal(Pos p)
    {
        vector<Dir> dirs_diagonal = {UL, RU, LD, DR};
        bool found = false;
        for (Dir d : dirs_diagonal)
        {
            found = found or (pos_correct(p + d) and thereis_zombie(p + d, 0));
        }
        return found;
    }

    priority_queue<movement> merge(priority_queue<movement> Q1, priority_queue<movement> Q2)
    {
        while (not Q1.empty())
        {
            Q2.push(Q1.top());
            Q1.pop();
        }
        return Q2;
    }

    movement catch_zombie(Pos p)
    {
        movement move = {Up, cell(p).id, 2};
        for (int i : random_permutation(4))
        {
            Dir d = dirs[i];
            if (pos_correct(p + d) and not zombies_diagonal(p + d))
            {
                move.dir = d;
            }
        }
        return move;
    }
    movement best_dir(Pos p, map<int, map<string, int>> &distances, map<int, map<string, Dir>> &directions)
    {
        int id = cell(p).id;
        movement act_move;
        act_move.id = id;
        if (distances[id]["dead"] == 2)
        {
            act_move.priority = 2;
            act_move.dir = directions[id]["dead"];
        }
        else if (distances[id]["enemy"] == 2)
        {
            act_move.priority = 1;
            act_move.dir = directions[id]["enemy"];
        }
        else if (distances[id]["zombie"] == 2)
        {
            act_move.priority = 2;
            act_move.dir = dir_alternative_zombie(p, directions[id]["zombie"]);
        }
        else if (distances[id]["zombie"] == 3)
        {
            act_move = catch_zombie(p);
        }
        else
        {

            if (distances[id]["enemy"] <= distances[id]["zombie"] and distances[id]["enemy"] <= 12)
            {
                act_move.priority = 2;
                act_move.dir = directions[id]["enemy"];
            }
            else if (distances[id]["zombie"] <= 12)
            {
                act_move.priority = 2;
                act_move.dir = directions[id]["zombie"];
            }
            else
            {
                act_move.priority = 2;
                BFS_empty(act_move.dir, p);
            }
        }
        return act_move;
    }

    priority_queue<movement> zombiefied(vector<bool> &used)
    {
        vector<int> alive = alive_units(me());
        priority_queue<movement> Q;
        int dist_zombie;
        Dir dir_zombie;
        for (int id : alive)
        {
            if (unit(id).rounds_for_zombie != -1)
            {
                BFS_zombie(dist_zombie, dir_zombie, unit(id).pos);
                if (dist_zombie <= unit(id).rounds_for_zombie)
                    Q.push({dir_zombie,
                            id,
                            2});
                used[id] = true;
            }
        }
        return Q;
    }
    void get_distances_dirs(map<int, map<string, int>> &distances, map<int, map<string, Dir>> &directions, map<int, int> &rounds_dead)
    {
        for (int id : alive_units(me()))
        {
            int rounds;
            Pos p = unit(id).pos;
            BFS_enemy_no_zombie(distances[id]["enemy"], directions[id]["enemy"], p, -1);
            BFS_zombie(distances[id]["zombie"], directions[id]["zombie"], p);
            BFS_dead(distances[id]["dead"], directions[id]["dead"], rounds_dead[id], p);
        }
    }

    void move_units(map<int, map<string, int>> &distances, map<int, map<string, Dir>> &directions, map<int, int> &rounds_dead)
    {
        vector<int> alive = alive_units(me());
        vector<bool> used(100, false);
        priority_queue<movement> moves;
        priority_queue<movement> moves_1 = kill_prox(used, distances, directions, rounds_dead);
        // priority_queue<movement> moves_3 = zombiefied(used);
        priority_queue<movement> moves_2 = catch_food(used, distances);
        moves = merge(moves_1, moves_2);
        for (int id : alive)
        {
            if (not used[id])
            {
                moves.push(best_dir(unit(id).pos, distances, directions));
            }
        }
        while (not moves.empty())
        {
            move(moves.top().id, moves.top().dir);
            moves.pop();
        }
    }

    virtual void play()
    {
        vector<bool> used(alive_units(me()).size(), false);
        map<int, map<string, int>> distances;
        map<int, map<string, Dir>> directions;
        map<int, int> rounds_dead;
        get_distances_dirs(distances, directions, rounds_dead);
        move_units(distances, directions, rounds_dead);
    }
};

RegisterPlayer(PLAYER_NAME);
