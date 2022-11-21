#include "Player.hh"
#include <map>
#include <queue>
#define PLAYER_NAME Napoleon0

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
    map<Pos, int> board;
    map<Pos, map<int, vector<Pos>>> Adj;
    int INF = 1e7;
    vector<int> sello_zombie = { 1000, 0, 100, 125, 150, 25, 10, 5, 1 };
    // vector<int> sello_zombie = { 0 };
    vector<int> sello_enemy = { 1000, 0, 100, -100, -40, 25, 10, 5, 1 };
    // vector<int> sello_enemy = { 0 };
    vector<int> sello_blanco = { 3, 2, 1 };
    vector<int> sello_friend = { -3, -3, 1, 2 };
    vector<int> sello_dead_1 = { -100, -50, -25 };
    vector<int> sello_dead_2 = { -100, -50, -25 };
    vector<int> sello_food = { 1000, 500, 250, 100, 40, 25, 10, 5, 1 };

    struct Dpd {
        Dir dir;
        Pos p;
        int dist;
    };
    struct gradient {
        Dir dir;
        Pos p;
        double mod;
        bool operator<(const gradient& a) const { return mod < a.mod; }
    };

    int BFS(Pos p1, Pos p2)
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
                if (p1 + d == p2)
                    return 1;
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
                    if (new_pos == p2)
                        return x.dist + 1;
                }
            }
        }
    }
    void build_board() //Inicialización de las distancias del tablero
    {
        for (int i = 0; i < 60; i++)
            for (int j = 0; j < 60; j++) {
                Pos p = Pos(i, j);
                Adj[p][0].push_back(p);
                if (pos_correct(p))
                    for (Dir d : dirs)
                        if (pos_correct(p + d))
                            Adj[p][1].push_back(p + d);
            }

        map<Pos, map<Pos, bool>> visited;
        for (int k = 2; k <= 11; k++)
            for (int i = 0; i < 60; i++)
                for (int j = 0; j < 60; j++) {
                    Pos p = Pos(i, j);
                    visited[p][p] = true;
                    if (pos_correct(p))
                        for (Pos p2 : Adj[p][k - 1]) {
                            visited[p][p2] = true;
                            for (Pos p3 : Adj[p2][1])
                                if (not visited[p][p3]) {
                                    visited[p][p3] = true;
                                    Adj[p][k].push_back(p3);
                                }
                        }
                }
    }

    void poner_sello(Pos p, vector<int> sello) // Función que imprime el sello pasado en la posición indicada
    {
        int n = sello.size();
        for (int dist = 0; dist < n; ++dist)
            for (Pos p2 : Adj[p][dist]) {
                board[p2] += sello[dist];
            }
    }
    void clear_board()
    {
        for (int i = 0; i < 60; i++) {
            for (int j = 0; j < 60; j++) {
                Pos p = Pos(i, j);
                if (pos_correct(p)) {
                    if (cell(p).owner != me()) {
                        board[p] = 0;
                        poner_sello(p, sello_blanco);
                    } else
                        board[p] = -1;
                } else
                    board[p] = -10;
            }
        }
        // if (cell(it*).owner != me())
        //     board[p] = 1;
        // else
        //     board[p] = 0;
    }
    void update_board() // Función que actualiza los valores del mapa de interés
    {
        for (int id : zombies()) //Ponemos sellos en los zombies
            poner_sello(unit(id).pos, sello_zombie);

        for (int pl = 0; pl < num_players(); ++pl) //Ponemos sellos en cada unidad viva de cada equipo enemigo
            if (pl != me())
                for (int id : alive_units(pl))
                    poner_sello(unit(id).pos, sello_enemy);
            else
                for (int id : alive_units(pl))
                    poner_sello(unit(id).pos, sello_friend);

        for (int pl = 0; pl < num_players(); ++pl) //Ponemos sellos en cada unidad muerta de cada equipo
            //  (EL MIO INCLUIDO YA QUE ACABAN ZOMBIES IGUAL)
            for (int id : dead_units(pl))
                if (unit(id).rounds_for_zombie < 3)
                    poner_sello(unit(id).pos, sello_dead_1);
                else
                    poner_sello(unit(id).pos, sello_dead_2);

        for (Pos food_pos : get_food()) //Ponemos sellos en cada celda donde hay comida
            poner_sello(food_pos, sello_food);
    }

    vector<bool> equal(double north, double south, double west, double east)
    {
        vector<bool> equals(4, false);
        if (north == south or north == west or north == east)
            equals[0] = true;
        if (south == north or south == west or south == east)
            equals[1] = true;
        if (west == north or south == west or west == east)
            equals[2] = true;
        if (east == north or east == west or south == east)
            equals[3] = true;
        return equals;
    }

    gradient get_gradient(Pos p) // returns the norm of the gradient and its direction
    {
        gradient max;
        max.p = p;
        double grad_long = 2;
        double north = 0, south = 0, east = 0, west = 0;
        int i = 1;
        Pos p_aux = p;
        while (i <= grad_long and pos_correct(p_aux + Pos(-1, 0))) {
            p_aux = p_aux + Up;
            north += board[p_aux];
            i++;
        }
        if (i != 1)
            north = north / (i - 1);
        north -= board[p];
        i = 1;
        p_aux = p;
        while (i <= grad_long and pos_correct(p_aux + Pos(1, 0))) {
            p_aux = p_aux + Down;
            south += board[p_aux];
            i++;
        }
        if (i != 1)
            south = south / (i - 1);
        south -= board[p];
        i = 1;
        p_aux = p;
        while (i <= grad_long and pos_correct(p_aux + Pos(0, -1))) {
            p_aux = p_aux + Left;
            west += board[p_aux];
            i++;
        }
        if (i != 1)
            west = west / (i - 1);
        west -= board[p];
        i = 1;
        p_aux = p;
        while (i <= grad_long and pos_correct(p_aux + Pos(0, 1))) {
            p_aux = p_aux + Right;
            east += board[p_aux];
            i++;
        }
        if (i != 1)
            east = east / (i - 1);
        east -= board[p];
        cerr << p << ": " << north << " " << south << " " << west << " " << east << endl;
        //
        // for (int i = 1; i <= int(grad_long); i++) {
        //     if (pos_correct(p + Pos(-i, 0)))
        //         north += board[p + Pos(-i, 0)] / grad_long;
        //     if (pos_correct(p + Pos(i, 0)))
        //         south += board[p + Pos(i, 0)] / grad_long;
        //     if (pos_correct(p + Pos(0, -i)))
        //         west += board[p + Pos(0, -i)] / grad_long;
        //     if (pos_correct(p + Pos(0, i)))
        //         east += board[p + Pos(0, i)] / grad_long;
        // }

        double horizontal = (east - west) / 2;
        double vertical = (south - north) / 2;
        if (north > south and north > west and north > east) {
            max.mod = abs(north);
            max.dir = Up;
        } else if (south > north and south > east and south > west) {
            max.mod = abs(south);
            max.dir = Down;
        } else if (east > north and east > south and east > west) {
            max.mod = abs(east);
            max.dir = Right;
        } else if (west > north and west > south and west > east) {
            max.mod = abs(west);
            max.dir = Left;
        } else {
            vector<bool> equals = equal(north, south, west, east);
            vector<double> mods = { north, south, west, east };
            for (int i : random_permutation(4)) {
                if (equals[i]) {
                    max.mod = mods[i];
                    max.dir = dirs[i];
                    return max;
                }
            }
        }
        // if (abs(horizontal) > abs(vertical)) {
        //     max.mod = abs(horizontal);
        //     if (horizontal > 0)
        //         max.dir = Right;
        //     else
        //         max.dir = Left;
        // } else {
        //     max.mod = abs(vertical);
        //     if (vertical > 0)
        //         max.dir = Down;
        //     else
        //         max.dir = Up;
        // }
        return max;
    }
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
        priority_queue<gradient> Q;
        for (int id : alive) {
            Q.push(get_gradient(unit(id).pos));
        }
        while (not Q.empty()) {
            if (Q.top().mod != 0 and pos_correct(Q.top().p + Q.top().dir))
                move(cell(Q.top().p).id, Q.top().dir);
            else {
                int dist;
                Dir opt_dir;
                BFS_enemy(dist, opt_dir, Q.top().p, -1);
                move(cell(Q.top().p).id, opt_dir);
            }
            Q.pop();
        }
    }

    void print_adj() // Función que imprime la matriz de distancias (solo para comprobar)
    {
        for (int i = 1; i < 2; i++) {
            for (int j = 0; j < 1; j++) {
                Pos p = Pos(i, j);
                cerr << p << ": " << endl;
                for (int k = 0; k < 11; k++) {
                    cerr << k << ": ";
                    for (Pos p2 : Adj[p][k])
                        cerr << p2.i << " " << p2.j << ", ";
                    cerr << endl;
                }
            }
        }
    }
    void print_board() // Imprime la tabla para checkear que esta bien el mapa de interés
    {
        fstream myfile;

        myfile.open("matrix.txt", fstream::out);
        for (int i = 0; i < 60; i++) {
            for (int j = 0; j < 60; j++)
                myfile << board[Pos(i, j)] << " ";
            myfile << endl;
        }
        myfile << endl;
        myfile.close();
    }
    virtual void play()
    {
        //move_units();
        if (round() == 0) {
            build_board();
            // //Hacer un bfs que vaya pintando la distancia a los vertices
        }
        clear_board();
        update_board();
        move_units();
        if (round() == 51) {
            // priority_queue<gradient> Q;
            // poner_sello(Pos(1, 0), sello_zombie);
            // Q.push(get_gradient(Pos(3, 0)));
            // Q.push(get_gradient(Pos(0, 0)));
            // cerr << Q.top().mod << " " << Q.top().dir << endl;
            // Q.pop();
            // cerr << Q.top().mod << " " << Q.top().dir << endl;

            print_board();
        }
    }
};

RegisterPlayer(PLAYER_NAME);
