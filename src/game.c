/* This file handles the game and the game loop */

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "dog.h"

Dog player;
Hydrant hydrant;
int jump_timer = 0; // Intitiate jump timer

int game_state = 1; // 0 (Menu), 1(Game), 2(Pause), 3(highscore)
const int DOG_SPAWN_Y = GROUND_LEVEL - DOG_HEIGHT;
const int HYDRANT_SPAWN_Y = GROUND_LEVEL - FH_HEIGHT;
const int JUMP_HEIGHT = 6; // Jump vertical boundary

/* Turns on the display pixels for the static ground (which is a line of pixels)*/
void show_ground()
{
    int c;
    for (c = 0; c < COLUMNS; c++)
    {
        display[GROUND_LEVEL][c] = 1;
    }
}

// void show_box()
// {

//     int i;
//     int j;

//     for (i = 0; i < BOX_SIZE; i++)
//     {
//         for (j = 0; j < BOX_SIZE; j++)
//         {
//             display[player.box_y[j]][player.box_x[i]] = 1;
//         }
//     }
// }

/* Takes in start posotion of box, top left corner (x,y)*/
// void preload_box(struct Box *box)
// {
//     int i;

//     for (i = 0; i < BOX_SIZE; i++)
//     {
//         box->box_x[i] = i + box->start_x;
//         box->box_y[i] = i + box->start_y;
//     }
// }

/* Changes the players position  */
void update_player()
{
    if (!player.is_grounded)
    {

        player.y += player.vel_y; // Decrement the y position
        jump_timer++;

        // Turning point
        if (jump_timer == JUMP_HEIGHT)
        {
            player.vel_y *= -1;
        }
        // Check if character has landed
        if (player.y >= DOG_SPAWN_Y)
        {
            player.y = DOG_SPAWN_Y;
            player.is_grounded = 1; // Set is_grounded to true
            player.vel_y = 0;
            jump_timer = 0; // Reset jump timer so that dog doesnt fly away
        }
    }
}

void update_hydrant()
{
    hydrant.y = HYDRANT_SPAWN_Y;
    hydrant.x += hydrant.vel_x; // Move x-position
}
/* Test function for spawning a hydrant*/
void spawn_hydrant()
{
    hydrant.x = 128 - FH_WIDTH;
    hydrant.y = HYDRANT_SPAWN_Y;
    hydrant.vel_x = -2; // Set test speed of 2 pixels
}
/* Function for spawning character and environment on game start*/
void spawn_player()
{
    player.is_grounded = 1; // Set on ground
    player.x = 20;
    player.y = DOG_SPAWN_Y;
    player.vel_y = 0;
}

void player_jump()
{
    if (player.is_grounded == 1)
    {
        player.is_grounded = 0;
        player.vel_y = JUMP_FORCE;
    }
}
/* This function check for button presses and handles it accordingly */
void check_buttons()
{
    int btn = getbtns();

    // Button 2 makes the player jump when pressed
    if ((btn & 0x1) == 1)
    {
        player_jump();
    }
}

void update()
{
    check_buttons();
    update_player();
    update_hydrant();
    display_figure(player.x, player.y, DOG_HEIGHT, DOG_WIDTH, dogPixels);
    display_figure(hydrant.x, hydrant.y, FH_HEIGHT, FH_WIDTH, hydrantPixels);
    show_ground();
}

void highscore()
{
    display_string(1, "Highscore");
    display_update();
}
/*  Menu screen (static display)*/
void menu()
{
    display_string(1, "MENU");
    display_string(2, "Toggle Switch 1 To Start Game");
    display_update();
}

/* This function check for switch toggles and handles it accordingly */

int check_switches()
{
    int sw = getsw();
    if (sw == 0x1)
    {
        return 1;
    }

    {
        return 0;
    }
}

void select_screen()
{
    // Clear all current pixels on the display
    display_clear();
    display_image(0, oled_display);
    while ((game_state >= 0 && game_state < 16))
    {
        switch (game_state)
        {
        case 0:
            // Menu
            menu();
            break;
        case 1:
            // Game loop
            // Set the intitial spawn locations of the characters
            spawn_player();
            spawn_hydrant();
            game_loop();
            break;
        case 2:
            // Pause
            break;
        case 3:
            // Highscore
            highscore();
            break;
        default:
            menu();
            break;
        }

        // Check switches on every iteration (4 bit value meaning 0-15)
        int temp = game_state; // temporary value for comparing switch combinations
        game_state = getsw();
        if (temp != game_state)
        {
            // Clear display only if there is another switch combination
            display_clear();
            display_image(0, oled_display);
        }
    }
}
/* Main game function */
void game_loop()
{

    while (game_state == 1)
    {
        display_clear();
        update();
        display_change();
        delay(FRAME_SPEED);

        game_state = check_switches();
    }

    // If game loop is broken, go the screen select
    select_screen();
}