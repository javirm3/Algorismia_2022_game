#include "Player.hh"

#define PLAYER_NAME Rick

struct PLAYER_NAME : public Player {
    static Player* factory()
    {
        return new PLAYER_NAME;
    }

    const vector<Dir> dirs = { Up, Down, Left, Right };

    virtual void play()
    {
        vector<int> alive = alive_units(me());
        for (int id : alive) {
            Dir d = dirs[random(0, dirs.size() - 1)];
            Pos new_pos = unit(id).pos + d;
            if (pos_ok(new_pos) and cell(new_pos.i, new_pos.j).type != Waste)
                move(id, d);
        }
    }
};

RegisterPlayer(PLAYER_NAME);
