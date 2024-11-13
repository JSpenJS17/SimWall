// Struct to store ant information
typedef struct {
    int x;
    int y;
    int direction; // 0:UP, 1:RIGHT, 2:DOWN, 3:LEFT
} Ant;

typedef enum {
    UP,
    RIGHT,
    DOWN,
    LEFT
} Direction;

int* ant_gen_next(int* grid, int width, int height);
void ant_add_life(int* pattern, int width, int height, int percent_alive);
int* ant_gen_random(int width, int height, int percent_alive);
void init_ants(Ant* inp_ants, int num_ants, char* inp_ruleset);