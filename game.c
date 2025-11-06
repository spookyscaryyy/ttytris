#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#include "input.h"
#include "game.h"
#include "termwrapper.h"
#include "assets.h"


typedef enum block_type_impl
{
    BLOCK_NULL = -1,
    BLOCK_I = 0,
    BLOCK_O = 1,
    BLOCK_T = 2,
    BLOCK_S = 3,
    BLOCK_Z = 4,
    BLOCK_J = 5,
    BLOCK_L = 6
} block_type;

typedef enum block_orient_impl
{
    BLOCK_UP = 0,
    BLOCK_RIGHT = 1,
    BLOCK_DOWN = 2,
    BLOCK_LEFT = 3

} block_orient;

typedef enum t_spin_state_impl
{
    TSPIN_NONE = 0,
    TSPIN_MINI = 1,
    TSPIN_NORM = 2
} t_spin_state;

#define ROW_CNT 23
#define COL_CNT 12

#define BLOCK_CNT 7
#define MINO_CNT 4

#define KICK_CNT 5
#define ROT_CNT 4

#define SET_BLOCK_GRACE_PERIOD 30
#define GRACE_MOVES 15

#define KICK_BLOCK_CW 1
#define KICK_BLOCK_CCW -1

#define NEXT_BLOCK_ROW 12
#define HOLD_BLOCK_ROW 6

#define DAS_STARTUP 16
#define DAS_DELAY 2

// cw rotate = swap row and col, invert col
// ccw rotate = invert col, swap row and col

static const int8_t BLOCK_O_KICKS[KICK_CNT][ROT_CNT][2] =
{
    {
        {0,0},
        {1,0},
        {1,-1},
        {0,-1}
    }
};

static const int8_t BLOCK_I_KICKS[KICK_CNT][ROT_CNT][2] =
{
    {
        {0,0},
        {0,-1},
        {-1,-1},
        {-1,0}
    },
    {
        {0,-1},
        {0,0},
        {-1,1},
        {-1,0}
    },
    {
        {0,-1},
        {-1,0},
        {0,1},
        {1,0}
    },
    {
        {0,2},
        {2,0},
        {0,-2},
        {-2,0}
    }
};

static const int8_t BLOCK_JLSTZ_KICKS[KICK_CNT][ROT_CNT][2] =
{
    {
        {0,0},
        {0,0},
        {0,0},
        {0,0}
    },
    {
        {0,0},
        {0,1},
        {0,0},
        {0,-1}
    },
    {
        {0,0},
        {1,1},
        {0,0},
        {1,-1}
    },
    {
        {0,0},
        {-2,0},
        {0,0},
        {-2,0}
    },
    {
        {0,0},
        {-2,1},
        {0,0},
        {-2,-1}
    }
};

static const int8_t BLOCK_OFFSETS[BLOCK_CNT][MINO_CNT][2] = 
{
    // BLOCK_I
    {
        {0,-1},
        {0,0},
        {0,1},
        {0,2}
    },

    // BLOCK_O
    {
        {-1,0},
        {0,0},
        {0,1},
        {-1,1}
    },

    // BLOCK_T
    {
        {0,-1},
        {0,0},
        {0,1},
        {-1,0}
    },

    // BLOCK_S
    {
        {0,-1},
        {0,0},
        {-1,0},
        {-1,1}
    },

    // BLOCK_Z
    {
        {-1,0},
        {-1,-1},
        {0,0},
        {0,1}
    },

    // BLOCK_J
    {
        {0,-1},
        {-1,-1},
        {0,0},
        {0,1}
    },

    // BLOCK_L
    {
        {0,-1},
        {-1,1},
        {0,0},
        {0,1}
    }
};


typedef struct block_impl
{
    block_type type;
    block_orient orient;
    bool set;
    int8_t row;
    int8_t col; 
    uint8_t color;
    int8_t offsets[MINO_CNT][2];

} block;

typedef struct field_unit_impl
{
    bool has_block;
    bool is_falling;
    bool is_wall;
    bool is_ghost;
    uint8_t color;
} field_unit;

typedef struct game_state_impl
{
    field_unit playfield[ROW_CNT][COL_CNT];
    block* falling;
    block* ghost;
    block_type block_bag[BLOCK_CNT];
    block_type hold;
    bool just_held;
    bool b2b;
    bool last_rotate;
    bool lost;
    bool paused;
    int8_t bag_idx;
    uint8_t level;
    int8_t last_kick;
    int32_t combo;
    t_spin_state tspin_state;
    int8_t grace_moves;
    int8_t step_count;
    int8_t set_grace_count;
    int8_t das_startup;
    int8_t das;
    uint32_t lines;
    uint32_t score;
} game_state;
static game_state state;

static uint8_t calc_step(uint8_t slevel);
static void spawn_block(block_type type);
static void set_block();
static void calc_score(int32_t lines);
static t_spin_state detect_tspin();

static bool check_shift(block* falling, int8_t row_s, int8_t col_s);
static void shift_block(block* falling, int8_t row_s, int8_t col_s);
static void block_state(bool enable);

static block init_block(block_type type);
static void draw_display_block(int8_t row_origin, block_type type);

static void rotate_block(int8_t change);
static bool kick_block(block* falling, const int8_t block_kicks[KICK_CNT][ROT_CNT][2], int8_t change);

static int8_t calc_ghost_offset();
static void move_ghost();

static void hard_drop();
static void soft_drop();
static void grab_block();

static void load_7bag();
static block_type query_7bag();
static block_type pop_7bag();

static bool check_line(int8_t row);
static uint32_t clear_lines();

static void clear_dyn_mem();
static void swap(int8_t* a, int8_t* b);


static void hard_drop()
{
    if (state.falling == NULL)
    {
        return;
    }
    int8_t drop_amt = calc_ghost_offset();
    shift_block(state.falling, drop_amt, 0);
    state.score += 2 * drop_amt;
    set_block();
}

static void soft_drop()
{
    if (state.falling == NULL)
    {
        return;
    }
    state.step_count -= calc_step(state.level + 5);

}

static void grab_block()
{
    if (state.just_held || state.falling == NULL)
    {
        return;
    }
    block_type tmp = state.hold;
    state.hold = state.falling->type;
    block_state(false);
    clear_dyn_mem();
    spawn_block(tmp);
    state.just_held = true;
}

static void clear_dyn_mem()
{
    free(state.falling);
    state.falling = NULL;
    free(state.ghost);
    state.ghost = NULL;
}

static void load_7bag()
{
    for (block_type i = 0; i < BLOCK_CNT; i++)
    {
        state.block_bag[i] = i;
    }
    for (uint8_t i = BLOCK_CNT - 1; i >= 1; i--)
    {
        int8_t j = rand() % i;
        block_type tmp = state.block_bag[i];
        state.block_bag[i] = state.block_bag[j];
        state.block_bag[j] = tmp;
    }
    state.bag_idx = BLOCK_CNT - 1;
}

static block_type query_7bag()
{
    if (state.bag_idx <= -1)
    {
        load_7bag();
    }
    return state.block_bag[state.bag_idx];
}

static block_type pop_7bag()
{
    block_type ret = query_7bag();
    state.bag_idx--;
    return ret;
}

static void rotate_block(int8_t change)
{
    if (state.falling == NULL)
    {
        return;
    }
    block falling = *state.falling;

    // perform op on fake block first
    for (uint8_t i = 0; i < MINO_CNT; i++)
    {
        if (change == KICK_BLOCK_CW)
        {
            swap(&falling.offsets[i][0], &falling.offsets[i][1]);
            falling.offsets[i][1] *= -1;
        }
        else
        {
            falling.offsets[i][1] *= -1;
            swap(&falling.offsets[i][0], &falling.offsets[i][1]);
        }
    }
    bool rotated = false;
    switch(falling.type)
    {
        case(BLOCK_O):
            rotated = kick_block(&falling, BLOCK_O_KICKS, change);
            break;
        case(BLOCK_I):
            rotated = kick_block(&falling, BLOCK_I_KICKS, change);
            break;
        case(BLOCK_J):
        case(BLOCK_L):
        case(BLOCK_S):
        case(BLOCK_T):
        case(BLOCK_Z):
        default:
            rotated = kick_block(&falling, BLOCK_JLSTZ_KICKS, change);
            break;
    }

    // rotation failed
    if (!rotated)
    {
        return;
    }
    // apply changes to block
    block_state(false);
    *state.falling = falling;
    move_ghost();
    block_state(true);
}

static void move_ghost()
{
    *state.ghost = *state.falling;
    state.ghost->row += calc_ghost_offset();
}

static bool kick_block(block* falling, const int8_t block_kicks[KICK_CNT][ROT_CNT][2], int8_t change)
{
    int8_t adj_orient = (falling->orient + ROT_CNT + change) % ROT_CNT;
    for (uint8_t i = 0; i < KICK_CNT; i++)
    {
        int8_t row_off = block_kicks[i][falling->orient][0] - block_kicks[i][adj_orient][0];
        int8_t col_off = block_kicks[i][falling->orient][1] - block_kicks[i][adj_orient][1];
        if (check_shift(falling, row_off, col_off))
        {
            shift_block(falling, row_off, col_off);
            falling->orient = adj_orient;
            state.last_kick = i + 1;
            return true;
        }
    }
    state.last_kick = -1;
    return false;
}

static void swap(int8_t* a, int8_t* b)
{
    int8_t tmp = *a;
    *a = *b;
    *b = tmp;
}

static void block_state(bool enable)
{
    block* falling = state.falling;
    block* ghost = state.ghost;
    for (uint8_t i = 0; i < MINO_CNT; i++)
    {
        field_unit* space = &state.playfield[falling->row + falling->offsets[i][0]][falling->col + falling->offsets[i][1]];
        field_unit* gspace = &state.playfield[ghost->row + ghost->offsets[i][0]][ghost->col + ghost->offsets[i][1]];
        if (enable)
        {
            space->color = falling->color;
            space->has_block = true;
            space->is_falling = true;

            gspace->is_ghost = true;
            gspace->color = ghost->color;
        }
        else
        {
            space->color = 0;
            space->has_block = false;
            space->is_falling = false;

            gspace->is_ghost = false;
            gspace->color = 0;
        }
    }
}

static bool check_shift(block* falling, int8_t row_s, int8_t col_s)
{
    for (int8_t i = 0; i < MINO_CNT; i++)
    {
        int8_t row_sel = falling->row + falling->offsets[i][0] + row_s;
        int8_t col_sel = falling->col + falling->offsets[i][1] + col_s;
        if (row_sel < 0 || row_sel >= ROW_CNT || col_sel < 0 || col_sel >= COL_CNT)
        {
            return false;
        }
        field_unit unit = state.playfield[row_sel][col_sel];

        if (unit.is_wall)
        {
            return false;
        }
        if (unit.has_block && !unit.is_falling)
        {
            return false;
        }
    }
    return true;
}

static void shift_block(block* falling, int8_t row_s, int8_t col_s)
{
    if (falling == NULL || !check_shift(falling, row_s, col_s))
    {
        return;
    }
    block_state(false);
    falling->row += row_s;
    falling->col += col_s;
    if (falling == state.falling)
    {
        move_ghost();
    } 
    block_state(true);
    state.set_grace_count = SET_BLOCK_GRACE_PERIOD;
}

void game_init(uint8_t level_select)
{
    srand((unsigned int)time(NULL));
    state.falling = NULL;
    state.ghost = NULL;
    state.level = level_select;
    state.step_count = calc_step(state.level);
    state.set_grace_count = SET_BLOCK_GRACE_PERIOD;
    state.bag_idx = -1;
    state.lines = 0;
    state.score = 0;
    state.hold = BLOCK_NULL;
    state.grace_moves = GRACE_MOVES;
    state.lost = false;
    state.combo = -1;
    state.b2b = false;
    state.tspin_state = TSPIN_NONE;
    state.last_rotate = false;
    state.paused = false;
    state.das = DAS_DELAY;
    state.das_startup = DAS_STARTUP;
    for (uint8_t i = 0; i < ROW_CNT; i++)
    {
        for(uint8_t j = 0; j < COL_CNT; j++)
        {
            state.playfield[i][j].has_block = false;
            state.playfield[i][j].is_falling = false;
            state.playfield[i][j].is_ghost = false;
            state.playfield[i][j].color = 0;
            if (j == 0 || j == COL_CNT - 1 || i == ROW_CNT - 1)
            {
                state.playfield[i][j].is_wall = true;
            }
            else
            {
                state.playfield[i][j].is_wall = false;
            }
        }
    }
}

static uint8_t calc_step(uint8_t slevel)
{
    double level = (double)slevel;
    double lpf = pow((0.8 - ((level - 1) * 0.007)),level - 1.0);
    return (uint8_t)(lpf * 60);
}

static bool check_line(int8_t row)
{
    for (uint8_t i = 1; i < COL_CNT - 1; i++)
    {
        if (!state.playfield[row][i].has_block)
        {
            return false;
        }
    }
    return true;
}

static uint32_t clear_lines()
{
    int8_t bdx = 0;
    int8_t edx = -1;
    for (int8_t i = 0; i < ROW_CNT - 1; i++)
    {
        if (bdx == 0 && check_line(i))
        {
            bdx = i;
            edx = i;
        }
        else if (check_line(i) && bdx != 0)
        {
            edx = i;
        }
    }
    uint32_t cleared = 1 + edx - bdx;
    for (int8_t i = bdx-1; i >= 0; i--)
    {
        for (int8_t j = 1; j < COL_CNT-1; j++)
        {
            state.playfield[edx][j] = state.playfield[i][j];
        }
        edx--;
    }
    return cleared;
}

static void calc_score(int32_t lines)
{
    int32_t score_add = 0;
    bool b2b_prev = state.b2b;
    switch (lines)
    {
        case(4):
        score_add = 800;
        state.b2b = true;
        state.combo++;
        break;
        case(3):
        if (state.tspin_state == TSPIN_NORM)
        {
            score_add = 1600;
            state.b2b = true;
        }
        else
        {
            score_add = 500;
            state.b2b = false;
        }
        state.combo++;
        break;
        case(2):
        if (state.tspin_state == TSPIN_NORM)
        {
            score_add = 1200;
            state.b2b = true;
        }
        else if (state.tspin_state == TSPIN_MINI)
        {
            score_add = 400;
            state.b2b = true;
        }
        else
        {
            score_add = 300;
            state.b2b = false;
        }
        state.combo++;
        break;
        case(1):
        if (state.tspin_state == TSPIN_NORM)
        {
            score_add = 800;
            state.b2b = true;
        }
        else if (state.tspin_state == TSPIN_MINI)
        {
            score_add = 200;
            state.b2b = true;
        }
        else
        {
            score_add = 100;
            state.b2b = false;
        }
        state.combo++;
        break;
        default:
        if (state.tspin_state == TSPIN_NORM)
        {
            score_add = 400;
        }
        else if (state.tspin_state == TSPIN_MINI)
        {
            score_add = 100;
        }
        else
        {
            score_add = 100;
            state.b2b = false;
        }
        state.combo = -1;
        break;
    }
    score_add *= state.level;
    if (state.combo > 0)
    {
        score_add += 50 * state.combo * state.level;
    }
    if (b2b_prev)
    {
        score_add = (int32_t)((double)score_add * 1.5);
    }
    state.score += score_add;
}

static t_spin_state detect_tspin()
{
    if (!state.last_rotate || state.falling->type != BLOCK_T)
    {
        return TSPIN_NONE;
    }

    // t block corners, front corners are first two
    int8_t corners[4][2] = {{-1,-1},{-1,1},{1,1},{1,-1}};

    // make sure front lines up with falling t block
    for (block_orient i = 0; i < state.falling->orient; i++)
    {
        for (uint8_t j = 0; j < 4; j++)
        {
            swap(&corners[j][0], &corners[j][1]);
            corners[j][1] *= -1;
        }
    }

    uint8_t fronts = 0;
    uint8_t backs = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        field_unit unit = state.playfield[state.falling->row + corners[i][0]][state.falling->col + corners[i][1]];
        if (unit.has_block || unit.is_wall)
        {
            if (i < 2)
            {
                fronts++;
            }
            else
            {
                backs++;
            }
        }
    }

    if (fronts + backs >= 3)
    {
        if (fronts == 2 || state.last_kick == 5)
        {
            return TSPIN_NORM;
        }
        return TSPIN_MINI;
    }

    return TSPIN_NONE;
}

static void set_block()
{
    block* falling = state.falling;
    int8_t drop_amt = calc_ghost_offset();
    shift_block(state.falling, drop_amt, 0);
    for (uint8_t i = 0; i < MINO_CNT; i++)
    {
        field_unit* unit = &state.playfield[falling->row + falling->offsets[i][0]][falling->col + falling->offsets[i][1]];
        unit->is_falling = false;
    }
    state.tspin_state = detect_tspin();
    uint32_t lines = clear_lines();
    calc_score(lines);
    state.lines += lines;

    if (state.lines >= state.level * 10)
    {
        state.level++;
    }
    clear_dyn_mem();
    state.just_held = false;
    spawn_block(BLOCK_NULL);
}

static int8_t calc_ghost_offset()
{
    for(int8_t i = 1; i < ROW_CNT; i++)
    {
        if (!check_shift(state.falling, i, 0))
        {
            return i - 1;
        }
    }
    return 0;
}

static void spawn_block(block_type type)
{
    if (state.falling != NULL)
    {
        return;
    }
    block* faller = malloc(sizeof(*faller));
    state.falling = faller;
    block* ghost = malloc(sizeof(*ghost));
    state.ghost = ghost;
    if (type == BLOCK_NULL)
    {
        type = pop_7bag();
    }
    *state.falling = init_block(type);
    move_ghost();
    if (!check_shift(state.falling, 0, 0) && !check_shift(state.falling, 0, 1) && !check_shift(state.falling, 0, -1))
    {
        state.lost = true;
    }
    block_state(true);
}

bool game_loop()
{
    // parse controls
    if (key_pressed(KEY_P))
    {
        TSW_DrawTextYX("PAUSED", 13, 20);
        state.paused = !state.paused;
    }
    if (state.paused)
    {
        return false;
    }
    if (key_pressed(KEY_LEFT))
    {
        shift_block(state.falling, 0, -1);
        state.last_rotate = false;
        if (state.falling != NULL && state.falling->set)
        {
            state.grace_moves--;
        }
    }
    if (key_pressed(KEY_RIGHT))
    {
        shift_block(state.falling, 0, 1);
        state.last_rotate = false;
        if (state.falling != NULL && state.falling->set)
        {
            state.grace_moves--;
        }
    }
    if (key_held(KEY_LEFT) || key_held(KEY_RIGHT))
    {
        state.das_startup--;
        if (state.das_startup <= 0)
        {
            state.das--;
            if (state.das <= 0)
            {
                shift_block(state.falling, 0, key_held(KEY_LEFT) ? -1:1);
                state.das = DAS_DELAY;
            }
        }
    }
    if ((key_held(KEY_RIGHT) && key_pressed(KEY_LEFT)) || 
    (key_held(KEY_LEFT) && key_pressed(KEY_RIGHT)) || 
    (!key_held(KEY_LEFT) && !key_held(KEY_RIGHT)))
    {
        state.das_startup = DAS_STARTUP;
        state.das = DAS_DELAY;
    }
    if (key_pressed(KEY_UP))
    {
        rotate_block(KICK_BLOCK_CW);
        state.last_rotate = true;
        if (state.falling != NULL && state.falling->set)
        {
            state.grace_moves--;
        }
    }
    if (key_pressed(KEY_Z))
    {
        rotate_block(KICK_BLOCK_CCW);
        state.last_rotate = true;
        if (state.falling != NULL && state.falling->set)
        {
            state.grace_moves--;
        }
    }
    if (key_pressed(KEY_SPACE))
    {
        hard_drop();
    }
    if (key_pressed(KEY_C))
    {
        grab_block();
    }
    if (key_held(KEY_DOWN))
    {
        soft_drop();
    }

    // move game forward
    spawn_block(BLOCK_NULL);
    state.step_count--;
    if (state.falling != NULL && state.step_count <= 0)
    {
        if (!check_shift(state.falling, 1, 0))
        {
            state.grace_moves = 15;
            state.falling->set = true;
        }
        else
        {
            // add score for soft drop
            if (key_held(KEY_DOWN))
            {
                state.score++;
            }
            shift_block(state.falling, 1, 0);
        }
    }
    if (state.falling != NULL && state.falling->set)
    {
        state.set_grace_count--;
        if (state.set_grace_count <= 0 || state.grace_moves <= 0)
        {
            set_block();
        }
    }
    
    // reset frames between drops
    if (state.step_count <= 0)
    {
        state.step_count = calc_step(state.level);
    }

    // draw screen
    TSW_DrawTextYX("", 1, 13);
    for (uint8_t i = 0; i < ROW_CNT - 1; i++)
    {
        for (uint8_t j = 1; j < COL_CNT - 1; j++)
        {
            if (state.playfield[i][j].has_block)
            {
                TSW_ChangeBGColor(state.playfield[i][j].color);
                TSW_DrawBlock();
                TSW_ClearColor();
            }
            else if (state.playfield[i][j].is_ghost)
            {
                TSW_ChangeFGColor(state.playfield[i][j].color);
                TSW_DrawGhost();
                TSW_ClearColor();
            }
            else
            {
                // draw the top board instead of blank
                if (i != 1)
                {
                    TSW_DrawSpacer();
                }
                else
                {
                    TSW_DrawBoardHorizontal();
                }
            }
        }
        TSW_NextLine();
    }

    char buf[8];
    snprintf(buf, 8, "%7d", state.score);
    TSW_DrawTextYX(buf, 5, 35);
    snprintf(buf, 8, "%5d", state.lines);
    TSW_DrawTextYX(buf, 10, 35);
    snprintf(buf, 8, "%5d", state.level);
    TSW_DrawTextYX(buf, 15, 35);

    draw_display_block(HOLD_BLOCK_ROW, state.hold);
    draw_display_block(NEXT_BLOCK_ROW, query_7bag());

    return state.lost;
}

static block init_block(block_type type)
{
    block new_block;
    new_block.orient = BLOCK_UP;
    new_block.row = 1;
    new_block.col = 5;
    new_block.set = false;
    new_block.type = type;
    switch (new_block.type)
    {
        case(BLOCK_I):
            new_block.color = 45;
            break;
        case(BLOCK_J):
            new_block.color = 27;
            break;
        case(BLOCK_L):
            new_block.color = 208;
            break;
        case(BLOCK_T):
            new_block.color = 165;
            break;
        case(BLOCK_S):
            new_block.color = 76;
            break;
        case(BLOCK_Z):
            new_block.color = 9;
            break;
        case(BLOCK_O):
        default:
            new_block.color = 11;
            break; 
    }
    memcpy(&new_block.offsets[0][0], &BLOCK_OFFSETS[new_block.type], sizeof(int8_t) * MINO_CNT * 2);
    return new_block;
}

static void draw_display_block(int8_t row_origin, block_type type)
{
    TSW_DrawTextYX("         ", row_origin - 1, 3);
    TSW_DrawTextYX("         ", row_origin, 3);
    TSW_DrawTextYX("         ", row_origin + 1, 3);
    if (type == BLOCK_NULL)
    {
        return;
    }
    block n = init_block(type);
    TSW_ChangeBGColor(n.color);
    for (uint8_t i = 0; i < MINO_CNT; i++)
    {
        if (type == BLOCK_I || type == BLOCK_O)
        {
            TSW_DrawTextYX("", row_origin, 5);
        }
        else
        {
            TSW_DrawTextYX("", row_origin, 6);
        }
        TSW_ShiftCursor(n.offsets[i][0], n.offsets[i][1]);
        TSW_DrawBlock();
    }
    TSW_ClearColor();
}

void game_shutdown()
{
    if (state.falling != NULL)
    {
        free(state.falling);
        state.falling = NULL;
    }
    if (state.ghost != NULL)
    {
        free(state.ghost);
        state.ghost = NULL;
    }
}

void draw_gamescreen()
{
    TSW_DrawText(gamescreen);
}