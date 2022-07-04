// [main.c]
// this template is provided for the 2D shooter game.

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>

// If defined, logs will be shown on console and written to file.
// If commented out, logs will not be shown nor be saved.
#define LOG_ENABLED

/* Constants. */

// Frame rate (frame per second)
const int FPS = 60;
// Display (screen) width.
const int SCREEN_W = 800;
// Display (screen) height.
const int SCREEN_H = 600;
// At most 10 audios can be played at a time.
const int RESERVE_SAMPLES = 10;
// Same as:
// const int SCENE_MENU = 1;
// const int SCENE_START = 2;
enum {
    SCENE_MENU = 1,
    SCENE_START = 2,
    // [HACKATHON 3-7]
    // TODO: Declare a new scene id.
    SCENE_SETTINGS = 3,
    SCENE_CHAR = 4,
    SCENE_CHAR2 = 5,
    SCENE_OVER = 6
};

/* Input states */

// The active scene id.
int active_scene;
// Keyboard state, whether the key is down or not.
bool key_state[ALLEGRO_KEY_MAX];
// Mouse state, whether the key is down or not.
// 1 is for left, 2 is for right, 3 is for middle.
bool* mouse_state;
// Mouse position.
int mouse_x, mouse_y;
// TODO: More variables to store input states such as joysticks, ...

/* Variables for allegro basic routines. */

ALLEGRO_DISPLAY* game_display;
ALLEGRO_EVENT_QUEUE* game_event_queue;
ALLEGRO_TIMER* game_update_timer;

/* Shared resources*/

ALLEGRO_FONT* font_pirulen_32;
ALLEGRO_FONT* font_pirulen_24;
// TODO: More shared resources or data that needed to be accessed
// across different scenes.

/* Menu Scene resources*/
ALLEGRO_BITMAP* main_img_background;
// [HACKATHON 3-1]
// TODO: Declare 2 variables for storing settings images.
// Uncomment and fill in the code below.
ALLEGRO_BITMAP* img_settings;
ALLEGRO_BITMAP* img_settings2;
ALLEGRO_SAMPLE* main_bgm;
ALLEGRO_SAMPLE_ID main_bgm_id;
ALLEGRO_SAMPLE* click;
ALLEGRO_SAMPLE_ID click_id;

ALLEGRO_BITMAP* img_set_background;
ALLEGRO_BITMAP* img_char_background;

/* Start Scene resources*/
ALLEGRO_BITMAP* start_img_background;
ALLEGRO_BITMAP* start_img_plane;
ALLEGRO_BITMAP* start_img_plane2;
ALLEGRO_BITMAP* start_img_enemy;
ALLEGRO_SAMPLE* start_bgm;
ALLEGRO_SAMPLE_ID start_bgm_id;
ALLEGRO_SAMPLE* fire;
ALLEGRO_SAMPLE_ID fire_id;
ALLEGRO_SAMPLE* laser;
ALLEGRO_SAMPLE_ID laser_id;
ALLEGRO_SAMPLE* char_scene;
ALLEGRO_SAMPLE_ID char_scene_id;
ALLEGRO_SAMPLE* setting_bg;
ALLEGRO_SAMPLE_ID setting_bg_id;
ALLEGRO_BITMAP* img_pause;
ALLEGRO_BITMAP* img_pause2;
ALLEGRO_BITMAP* img_explode[23];
// [HACKATHON 2-1]
// TODO: Declare a variable to store your bullet's image.
// Uncomment and fill in the code below.
ALLEGRO_BITMAP* img_bullet;
ALLEGRO_BITMAP* img_bullet2;
ALLEGRO_BITMAP* img_enemy_bullet;

ALLEGRO_BITMAP* over_img_background;
ALLEGRO_SAMPLE* lose;
ALLEGRO_SAMPLE_ID lose_id;

typedef struct {
    // The center coordinate of the image.
    float x, y;
    // The width and height of the object.
    float w, h;
    // The velocity in x, y axes.
    float vx, vy;
    // Should we draw this object on the screen.
    bool hidden;
    // The pointer to the object’s image.
    float hp;
    ALLEGRO_BITMAP* img;
} MovableObject;
void draw_movable_object(MovableObject obj);
#define MAX_ENEMY 4
// [HACKATHON 2-2]
// TODO: Declare the max bullet count that will show on screen.
// You can try max 4 bullets here and see if you needed more.
// Uncomment and fill in the code below.
#define MAX_BULLET 10
MovableObject plane;
MovableObject plane2;
MovableObject enemies[MAX_ENEMY];
// [HACKATHON 2-3]
// TODO: Declare an array to store bullets with size of max bullet count.
// Uncomment and fill in the code below.
MovableObject bullets[MAX_BULLET];
MovableObject bullets2[MAX_BULLET];
MovableObject enemies_bullets[MAX_ENEMY][MAX_BULLET];
// [HACKATHON 2-4]
// TODO: Set up bullet shooting cool-down variables.
// 1) Declare your shooting cool-down time as constant. (0.2f will be nice)
// 2) Declare your last shoot timestamp.
// Uncomment and fill in the code below.
const float MAX_COOLDOWN = 0.2f;
const float MAX_COOLDOWN_ENEMY = 2.5f;
double last_shoot_timestamp;
double last_shoot_timestamp2;
double last_respawn;
double last_shoot_timestamp_enemy;
int sfx = 1;
int bgm = 1;
int dx[MAX_ENEMY] = { 0 };
int dy[MAX_ENEMY] = { 0 };
int p2 = 0;
int first_plane = 1;
int second_plane = 0;
int score = 0;
int temp;
int last_frame = 0;
char highscore[50];
bool pause = false;

/* Declare function prototypes. */

// Initialize allegro5 library
void allegro5_init(void);
// Initialize variables and resources.
// Allows the game to perform any initialization it needs before
// starting to run.
void game_init(void);
// Process events inside the event queue using an infinity loop.
void game_start_event_loop(void);
// Run game logic such as updating the world, checking for collision,
// switching scenes and so on.
// This is called when the game should update its logic.
void game_update(void);
// Draw to display.
// This is called when the game should draw itself.
void game_draw(void);
// Release resources.
// Free the pointers we allocated.
void game_destroy(void);
// Function to change from one scene to another.
void game_change_scene(int next_scene);
// Load resized bitmap and check if failed.
ALLEGRO_BITMAP* load_bitmap_resized(const char* filename, int w, int h);
// [HACKATHON 3-2]
// TODO: Declare a function.
// Determines whether the point (px, py) is in rect (x, y, w, h).
// Uncomment the code below.
bool pnt_in_rect(int px, int py, int x, int y, int w, int h);
bool check(float x, float y, float w, float h, float a, float b, float n, float m);

/* Event callbacks. */
void on_key_down(int keycode);
void on_mouse_down(int btn, int x, int y);

/* Declare function prototypes for debugging. */

// Display error message and exit the program, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// If the program crashes unexpectedly, you can inspect "log.txt" for
// further information.
void game_abort(const char* format, ...);
// Log events for later debugging, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// You can inspect "log.txt" for logs in the last run.
void game_log(const char* format, ...);
// Log using va_list.
void game_vlog(const char* format, va_list arg);

int main(int argc, char** argv) {
    // Set random seed for better random outcome.
    srand(time(NULL));
    allegro5_init();
    game_log("Allegro5 initialized");
    game_log("Game begin");
    // Initialize game variables.
    game_init();
    game_log("Game initialized");
    // Draw the first frame.
    game_draw();
    game_log("Game start event loop");
    // This call blocks until the game is finished.
    game_start_event_loop();
    game_log("Game end");
    game_destroy();
    return 0;
}

void allegro5_init(void) {
    if (!al_init())
        game_abort("failed to initialize allegro");

    // Initialize add-ons.
    if (!al_init_primitives_addon())
        game_abort("failed to initialize primitives add-on");
    if (!al_init_font_addon())
        game_abort("failed to initialize font add-on");
    if (!al_init_ttf_addon())
        game_abort("failed to initialize ttf add-on");
    if (!al_init_image_addon())
        game_abort("failed to initialize image add-on");
    if (!al_install_audio())
        game_abort("failed to initialize audio add-on");
    if (!al_init_acodec_addon())
        game_abort("failed to initialize audio codec add-on");
    if (!al_reserve_samples(RESERVE_SAMPLES))
        game_abort("failed to reserve samples");
    if (!al_install_keyboard())
        game_abort("failed to install keyboard");
    if (!al_install_mouse())
        game_abort("failed to install mouse");
    // TODO: Initialize other addons such as video, ...

    // Setup game display.
    game_display = al_create_display(SCREEN_W, SCREEN_H);
    if (!game_display)
        game_abort("failed to create display");
    al_set_window_title(game_display, "I2P(I)_2020 Final Project <109000168>");

    // Setup update timer.
    game_update_timer = al_create_timer(1.0f / FPS);
    if (!game_update_timer)
        game_abort("failed to create timer");

    // Setup event queue.
    game_event_queue = al_create_event_queue();
    if (!game_event_queue)
        game_abort("failed to create event queue");

    // Malloc mouse buttons state according to button counts.
    const unsigned m_buttons = al_get_mouse_num_buttons();
    game_log("There are total %u supported mouse buttons", m_buttons);
    // mouse_state[0] will not be used.
    mouse_state = malloc((m_buttons + 1) * sizeof(bool));
    memset(mouse_state, false, (m_buttons + 1) * sizeof(bool));

    // Register display, timer, keyboard, mouse events to the event queue.
    al_register_event_source(game_event_queue, al_get_display_event_source(game_display));
    al_register_event_source(game_event_queue, al_get_timer_event_source(game_update_timer));
    al_register_event_source(game_event_queue, al_get_keyboard_event_source());
    al_register_event_source(game_event_queue, al_get_mouse_event_source());
    // TODO: Register other event sources such as timer, video, ...

    // Start the timer to update and draw the game.
    al_start_timer(game_update_timer);
}

void game_init(void) {
    /* Shared resources*/
    font_pirulen_32 = al_load_font("pirulen.ttf", 32, 0);
    if (!font_pirulen_32)
        game_abort("failed to load font: pirulen.ttf with size 32");

    font_pirulen_24 = al_load_font("pirulen.ttf", 24, 0);
    if (!font_pirulen_24)
        game_abort("failed to load font: pirulen.ttf with size 24");

    /* Menu Scene resources*/
    main_img_background = load_bitmap_resized("main-bg.jpg", SCREEN_W, SCREEN_H);

    main_bgm = al_load_sample("bgm.ogg");
    if (!main_bgm)
        game_abort("failed to load audio: bgm.ogg");

    click = al_load_sample("click.ogg");
    if (!click)
        game_abort("failed to load audio: click.ogg");

    // [HACKATHON 3-4]
    // TODO: Load settings images.
    // Don't forget to check their return values.
    // Uncomment and fill in the code below.
    img_settings = al_load_bitmap("settings.png");
    if (!img_settings)
        game_abort("failed to load image: settings.png");
    img_settings2 = al_load_bitmap("settings2.png");
    if (!img_settings2)
        game_abort("failed to load image: settings2.png");

    img_set_background = load_bitmap_resized("settings-bg1.png", SCREEN_W, SCREEN_H);
    if (!img_set_background)
        game_abort("failed to load image: settings-bg1.png");

    img_char_background = load_bitmap_resized("char-bg.jpg", SCREEN_W, SCREEN_H);
    if (!img_char_background)
        game_abort("failed to load image: char-bg.jpg");

    /* Start Scene resources*/
    start_img_background = load_bitmap_resized("start-bg.jpg", SCREEN_W, SCREEN_H);

    start_img_plane = al_load_bitmap("plane.png");
    if (!start_img_plane)
        game_abort("failed to load image: plane.png");

    start_img_plane2 = al_load_bitmap("plane2.png");
    if (!start_img_plane2)
        game_abort("failed to load image: plane2.png");

    start_img_enemy = al_load_bitmap("smallfighter0006.png");
    if (!start_img_enemy)
        game_abort("failed to load image: smallfighter0006.png");

    img_pause = al_load_bitmap("pause.png");
    if (!img_pause)
        game_abort("failed to load image: pause.png");

    img_pause2 = al_load_bitmap("pause2.png");
    if (!img_pause2)
        game_abort("failed to load image: pause2.png");

    img_explode[0] = al_load_bitmap("explosion/0.png");
    img_explode[1] = al_load_bitmap("explosion/1.png");
    img_explode[2] = al_load_bitmap("explosion/2.png");
    img_explode[3] = al_load_bitmap("explosion/3.png");
    img_explode[4] = al_load_bitmap("explosion/4.png");
    img_explode[5] = al_load_bitmap("explosion/5.png");
    img_explode[6] = al_load_bitmap("explosion/6.png");
    img_explode[7] = al_load_bitmap("explosion/7.png");
    img_explode[8] = al_load_bitmap("explosion/8.png");
    img_explode[9] = al_load_bitmap("explosion/9.png");
    img_explode[10] = al_load_bitmap("explosion/10.png");
    img_explode[11] = al_load_bitmap("explosion/11.png");
    img_explode[12] = al_load_bitmap("explosion/12.png");
    img_explode[13] = al_load_bitmap("explosion/13.png");
    img_explode[14] = al_load_bitmap("explosion/14.png");
    img_explode[15] = al_load_bitmap("explosion/15.png");
    img_explode[16] = al_load_bitmap("explosion/16.png");
    img_explode[17] = al_load_bitmap("explosion/17.png");
    img_explode[18] = al_load_bitmap("explosion/18.png");
    img_explode[19] = al_load_bitmap("explosion/19.png");
    img_explode[20] = al_load_bitmap("explosion/20.png");
    img_explode[21] = al_load_bitmap("explosion/21.png");
    img_explode[22] = al_load_bitmap("explosion/22.png");
    if (!img_explode[0])
        game_abort("failed to load image: explosion/0.png");

    start_bgm = al_load_sample("boss.ogg");
    if (!start_bgm)
        game_abort("failed to load audio: boss.ogg");

    char_scene = al_load_sample("char-scene-bg.mp3");
    if (!char_scene)
        game_abort("failed to load audio: char-scene-bg.mp3");

    fire = al_load_sample("fire.wav");
    if (!fire)
        game_abort("failed to load audio: fire.wav");

    laser = al_load_sample("laser.ogg");
    if (!laser)
        game_abort("failed to load audio: laser.ogg");

    setting_bg = al_load_sample("setting-bg.ogg");
    if (!setting_bg)
        game_abort("failed to load audio: setting-bg.ogg");

    lose = al_load_sample("lose.ogg");
    if (!lose) {
        game_abort("failed to load audio: lose.ogg");
    }

    over_img_background = load_bitmap_resized("game_over2.jpg", SCREEN_W, SCREEN_H);
    if (!over_img_background)
        game_abort("failed to load image: game_over.png");


    // [HACKATHON 2-5]
    // TODO: Initialize bullets.
    // 1) Search for a bullet image online and put it in your project.
    //    You can use the image we provided.
    // 2) Load it in by 'al_load_bitmap' or 'load_bitmap_resized'.
    // 3) If you use 'al_load_bitmap', don't forget to check its return value.
    // Uncomment and fill in the code below.
    img_bullet = al_load_bitmap("image12.png");
    if (!img_bullet)
        game_abort("failed to load image: image12.png");

    img_bullet2 = al_load_bitmap("image13.png");
    if (!img_bullet2)
        game_abort("failed to load image: image13.png");

    img_enemy_bullet = al_load_bitmap("image14.png");
    if (!img_enemy_bullet)
        game_abort("failed to load image: image14.png");

    // Change to first scene.
    game_change_scene(SCENE_MENU);
}

void game_start_event_loop(void) {
    bool done = false;
    ALLEGRO_EVENT event;
    int redraws = 0;
    while (!done) {
        al_wait_for_event(game_event_queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            // Event for clicking the window close button.
            game_log("Window close button clicked");
            done = true;
        }
        else if (event.type == ALLEGRO_EVENT_TIMER) {
            // Event for redrawing the display.
            if (event.timer.source == game_update_timer)
                // The redraw timer has ticked.
                redraws++;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            // Event for keyboard key down.
            game_log("Key with keycode %d down", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = true;
            on_key_down(event.keyboard.keycode);
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            // Event for keyboard key up.
            game_log("Key with keycode %d up", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = false;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            // Event for mouse key down.
            game_log("Mouse button %d down at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
            mouse_state[event.mouse.button] = true;
            on_mouse_down(event.mouse.button, event.mouse.x, event.mouse.y);
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            // Event for mouse key up.
            game_log("Mouse button %d up at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
            mouse_state[event.mouse.button] = false;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            if (event.mouse.dx != 0 || event.mouse.dy != 0) {
                // Event for mouse move.
                // game_log("Mouse move to (%d, %d)", event.mouse.x, event.mouse.y);
                mouse_x = event.mouse.x;
                mouse_y = event.mouse.y;
            }
            else if (event.mouse.dz != 0) {
                // Event for mouse scroll.
                game_log("Mouse scroll at (%d, %d) with delta %d", event.mouse.x, event.mouse.y, event.mouse.dz);
            }
        }
        // TODO: Process more events and call callbacks by adding more
        // entries inside Scene.

        // Redraw
        if (redraws > 0 && al_is_event_queue_empty(game_event_queue)) {
            // if (redraws > 1)
            //     game_log("%d frame(s) dropped", redraws - 1);
            // Update and draw the next frame.
            game_update();
            game_draw();
            redraws = 0;
        }
    }
}

void game_update(void) {
    if (!pause) {
        int i, j;
        if (active_scene == SCENE_START) {
            /*or (i = 0; i < MAX_ENEMY; i++) {
                if (enemies[i].hp > 0) {    
                    if (dx[i] == 0) {
                        if (enemies[i].x > SCREEN_W - 14) {
                            enemies[i].x = SCREEN_W - 14;
                            dx[i] = 1;
                        }
                        enemies[i].x += enemies[i].vx;
                    }
                    else if (dx[i] == 1) {
                        if (enemies[i].x < 14) {
                            enemies[i].x = 14;
                            dx[i] = 0;
                        }
                        enemies[i].x -= enemies[i].vx;
                    }
                    if (dy[i] == 0) {
                        if (enemies[i].y > SCREEN_H - 350) {
                            enemies[i].y = SCREEN_H - 350;
                            dy[i] = 1;
                        }
                        enemies[i].y += enemies[i].vy;
                    }
                    else if (dy[i] == 1) {
                        if (enemies[i].y < 34) {
                            enemies[i].y = 34;
                            dy[i] = 0;
                        }
                        enemies[i].y -= enemies[i].vy;
                    }
                }
                else {
                    enemies[i].hidden = true;
                    enemies[i].x = -30;
                    enemies[i].y = -30;
                }
            }*/
            if (first_plane) {
                plane.vx = plane.vy = 0;
                if (p2) {
                    if (key_state[ALLEGRO_KEY_W])
                        plane.vy -= 1;
                    if (key_state[ALLEGRO_KEY_S])
                        plane.vy += 1;
                    if (key_state[ALLEGRO_KEY_A])
                        plane.vx -= 1;
                    if (key_state[ALLEGRO_KEY_D])
                        plane.vx += 1;

                    double now = al_get_time();
                    if (key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN) {
                        for (i = 0; i < MAX_BULLET; i++) {
                            if (bullets[i].hidden) {
                                last_shoot_timestamp = now;
                                bullets[i].hidden = false;
                                bullets[i].x = plane.x;
                                bullets[i].y = plane.y - (plane.h / 2);
                                if (sfx)
                                    if (!al_play_sample(fire, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &fire_id));
                                break;
                            }
                        }
                    }
                }
                else {
                    double now = al_get_time();
                    if (key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN) {
                        for (i = 0; i < MAX_BULLET; i++) {
                            if (bullets[i].hidden) {
                                last_shoot_timestamp = now;
                                bullets[i].hidden = false;
                                bullets[i].x = plane.x;
                                bullets[i].y = plane.y - (plane.h / 2);
                                if (sfx)
                                    if (!al_play_sample(fire, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &fire_id));
                                break;
                            }
                        }
                    }

                    if (key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W])
                        plane.vy -= 1;
                    if (key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S])
                        plane.vy += 1;
                    if (key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A])
                        plane.vx -= 1;
                    if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])
                        plane.vx += 1;
                }
                // 0.71 is (1/sqrt(2)).
                plane.y += plane.vy * 4 * (plane.vx ? 0.71f : 1);
                plane.x += plane.vx * 4 * (plane.vy ? 0.71f : 1);
                // [HACKATHON 1-1]
                // TODO: Limit the plane's collision box inside the frame.
                //       (x, y axes can be separated.)
                // Uncomment and fill in the code below.
                if (plane.x - (plane.w / 2) < 0)
                    plane.x = plane.w / 2;
                else if (plane.x + (plane.w / 2) > SCREEN_W)
                    plane.x = SCREEN_W - (plane.w / 2);
                if (plane.y - (plane.h / 2) < 0)
                    plane.y = plane.h / 2;
                else if (plane.y + (plane.h / 2) > SCREEN_H)
                    plane.y = SCREEN_H - (plane.h / 2);

                // [HACKATHON 2-7]
                // TODO: Update bullet coordinates.
                // 1) For each bullets, if it's not hidden, update x, y
                // according to vx, vy.
                // 2) If the bullet is out of the screen, hide it.
                // Uncomment and fill in the code below.
                for (i = 0; i < MAX_ENEMY; i++) {
                    if (check(plane.x, plane.y, plane.w, plane.h, enemies[i].x, enemies[i].y, enemies->w, enemies->h)) {
                        FILE* f = fopen("highscore.txt", "w+");
                        if (!f) {
                            printf("Error opening highscore\n");
                            exit(1);
                        }
                        temp = atoi(highscore);
                        if (temp < score)
                            fprintf(f, "%d", score);
                        else
                            fprintf(f, "%d", temp);
                        fclose(f);
                        temp = score;
                        score = 0;
                        plane.hp = 0;
                        if (bgm)
                            if (!al_play_sample(lose, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &lose_id));
                        game_change_scene(SCENE_OVER);
                    }
                    for (j = 0; j < MAX_BULLET; j++) {
                        if (check(plane.x, plane.y, plane.w, plane.h, enemies_bullets[i][j].x, enemies_bullets[i][j].y, enemies_bullets[i]->w, enemies_bullets[i]->h)) {
                            enemies_bullets[i][j].hidden = true;
                            enemies_bullets[i][j].x = SCREEN_W + 100;
                            enemies_bullets[i][j].y = SCREEN_H + 300;
                            plane.hp -= 1.0;
                            if (plane.hp <= 0) {
                                plane.hp = 0;
                                FILE* f = fopen("highscore.txt", "w+");
                                if (!f) {
                                    printf("Error opening highscore\n");
                                    exit(1);
                                }
                                temp = atoi(highscore);
                                if (temp < score)
                                    fprintf(f, "%d", score);
                                else
                                    fprintf(f, "%d", temp);
                                fclose(f);
                                temp = score;
                                score = 0;
                                if (bgm)
                                    if (!al_play_sample(lose, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &lose_id));
                                game_change_scene(SCENE_OVER);
                            }
                        }
                    }
                }
                for (i = 0; i < MAX_BULLET; i++) {
                    if (bullets[i].hidden)
                        continue;
                    bullets[i].x += bullets[i].vx;
                    bullets[i].y += bullets[i].vy;
                    if (bullets[i].y + (bullets->h / 2) < 0)
                        bullets[i].hidden = true;

                    for (j = 0; j < MAX_ENEMY; j++) {
                        if (enemies[j].y - (enemies->h / 2) > SCREEN_H)
                            enemies[j].hidden = true;
                        else if (check(bullets[i].x, bullets[i].y, bullets->w, bullets->h, enemies[j].x, enemies[j].y, enemies->w, enemies->h)) {
                            bullets[i].hidden = true;
                            bullets[i].x = SCREEN_W + 500;
                            bullets[i].y = -400;
                            enemies[j].hp -= 1.5;
                            printf("HP %f\n", enemies[j].hp);
                        }

                        if (enemies[j].hp <= 0) {
                            enemies[j].hp = 5;
                            enemies[j].hidden = true;
                            score += 1;
                        }

                        for (int k = 0; k < MAX_BULLET; k++) {
                            if (check(bullets[i].x, bullets[i].y, bullets->w, bullets->h, enemies_bullets[j][k].x, enemies_bullets[j][k].y, enemies_bullets[j]->w, enemies_bullets[j]->h)) {
                                bullets[i].hidden = true;
                                bullets[i].x = SCREEN_W + 200;
                                bullets[i].y = -400;
                                enemies_bullets[j][k].hidden = true;
                                enemies_bullets[j][k].x = SCREEN_W + 300;
                                enemies_bullets[j][k].y = SCREEN_H + 500;
                            }
                        }
                    }
                }
            }
            if (second_plane) {
                plane2.vx = plane2.vy = 0;
                if (p2) {
                    if (key_state[ALLEGRO_KEY_UP])
                        plane2.vy -= 1;
                    if (key_state[ALLEGRO_KEY_DOWN])
                        plane2.vy += 1;
                    if (key_state[ALLEGRO_KEY_LEFT])
                        plane2.vx -= 1;
                    if (key_state[ALLEGRO_KEY_RIGHT])
                        plane2.vx += 1;

                    double now2 = al_get_time();
                    if (key_state[ALLEGRO_KEY_RCTRL] && now2 - last_shoot_timestamp2 >= MAX_COOLDOWN) {
                        for (int i = 0; i < MAX_BULLET; i++) {
                            if (bullets2[i].hidden) {
                                last_shoot_timestamp2 = now2;
                                bullets2[i].hidden = false;
                                bullets2[i].x = plane2.x;
                                bullets2[i].y = plane2.y - (plane2.h / 2);
                                if (sfx)
                                    if (!al_play_sample(laser, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &laser_id));
                                break;
                            }
                        }
                    }
                }
                else {
                    double now2 = al_get_time();
                    if (key_state[ALLEGRO_KEY_SPACE] && now2 - last_shoot_timestamp2 >= MAX_COOLDOWN) {
                        for (i = 0; i < MAX_BULLET; i++) {
                            if (bullets2[i].hidden) {
                                last_shoot_timestamp2 = now2;
                                bullets2[i].hidden = false;
                                bullets2[i].x = plane2.x;
                                bullets2[i].y = plane2.y - (plane2.h / 2);
                                if (sfx)
                                    if (!al_play_sample(laser, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &laser_id));
                                break;
                            }
                        }
                    }

                    if (key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W])
                        plane2.vy -= 1;
                    if (key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S])
                        plane2.vy += 1;
                    if (key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A])
                        plane2.vx -= 1;
                    if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])
                        plane2.vx += 1;
                }
                // 0.71 is (1/sqrt(2)).
                plane2.y += plane2.vy * 4 * (plane2.vx ? 0.71f : 1);
                plane2.x += plane2.vx * 4 * (plane2.vy ? 0.71f : 1);
                // [HACKATHON 1-1]
                // TODO: Limit the plane's collision box inside the frame.
                //       (x, y axes can be separated.)
                // Uncomment and fill in the code below.
                if (plane2.x - (plane2.w / 2) < 0)
                    plane2.x = plane2.w / 2;
                else if (plane2.x + (plane2.w / 2) > SCREEN_W)
                    plane2.x = SCREEN_W - (plane2.w / 2);
                if (plane2.y - (plane2.h / 2) < 0)
                    plane2.y = plane2.h / 2;
                else if (plane2.y + (plane2.h / 2) > SCREEN_H)
                    plane2.y = SCREEN_H - (plane2.h / 2);

                // [HACKATHON 2-7]
                // TODO: Update bullet coordinates.
                // 1) For each bullets, if it's not hidden, update x, y
                // according to vx, vy.
                // 2) If the bullet is out of the screen, hide it.
                // Uncomment and fill in the code below.
                for (i = 0; i < MAX_ENEMY; i++) {
                    if (check(plane2.x, plane2.y, plane2.w, plane2.h, enemies[i].x, enemies[i].y, enemies->w, enemies->h)) {
                        FILE* f = fopen("highscore.txt", "w+");
                        if (!f) {
                            printf("Error opening highscore\n");
                            exit(1);
                        }
                        temp = atoi(highscore);
                        if (temp < score)
                            fprintf(f, "%d", score);
                        else
                            fprintf(f, "%d", temp);
                        fclose(f);
                        temp = score;
                        score = 0;
                        plane2.hp = 0;
                        if (bgm)
                            if (!al_play_sample(lose, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &lose_id));
                        game_change_scene(SCENE_OVER);
                    }
                    for (j = 0; j < MAX_BULLET; j++) {
                        if (check(plane2.x, plane2.y, plane2.w, plane2.h, enemies_bullets[i][j].x, enemies_bullets[i][j].y, enemies_bullets[i]->w, enemies_bullets[i]->h)) {
                            enemies_bullets[i][j].hidden = true;
                            enemies_bullets[i][j].x = SCREEN_W + 450;
                            enemies_bullets[i][j].y = SCREEN_H + 250;
                            plane2.hp -= 1.0;
                            if (plane2.hp <= 0) {
                                plane2.hp = 0;
                                FILE* f = fopen("highscore.txt", "w+");
                                if (!f) {
                                    printf("Error opening highscore\n");
                                    exit(1);
                                }
                                temp = atoi(highscore);
                                if (temp < score)
                                    fprintf(f, "%d", score);
                                else
                                    fprintf(f, "%d", temp);
                                fclose(f);
                                temp = score;
                                score = 0;
                                if (bgm)
                                    if (!al_play_sample(lose, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &lose_id));
                                game_change_scene(SCENE_OVER);
                            }
                        }
                    }
                }
                for (i = 0; i < MAX_BULLET; i++) {
                    if (bullets2[i].hidden)
                        continue;
                    bullets2[i].x += bullets2[i].vx;
                    bullets2[i].y += bullets2[i].vy;
                    if (bullets2[i].y + (bullets2->h / 2) < 0)
                        bullets2[i].hidden = true;

                    for (j = 0; j < MAX_ENEMY; j++) {
                        if (enemies[j].y - (enemies->h / 2) > SCREEN_H)
                            enemies[j].hidden = true;
                        else if (check(bullets2[i].x, bullets2[i].y, bullets2->w, bullets2->h, enemies[j].x, enemies[j].y, enemies->w, enemies->h)) {
                            bullets2[i].hidden = true;
                            bullets2[i].x = SCREEN_W + 200;
                            bullets2[i].y = -400;
                            enemies[j].hp -= 0.75;
                            printf("HP %f\n", enemies[j].hp);
                        }
                        if (enemies[j].hp <= 0) {
                            enemies[j].hp = 5;
                            enemies[j].hidden = true;
                            score += 1;
                        }

                        for (int k = 0; k < MAX_BULLET; k++) {
                            if (check(bullets2[i].x, bullets2[i].y, bullets2->w, bullets2->h, enemies_bullets[j][k].x, enemies_bullets[j][k].y, enemies_bullets[j]->w, enemies_bullets[j]->h)) {
                                bullets2[i].hidden = true;
                                bullets2[i].x = SCREEN_W + 200;
                                bullets2[i].y = -300;
                                enemies_bullets[j][k].hidden = true;
                                enemies_bullets[j][k].x = SCREEN_W + 300;
                                enemies_bullets[j][k].y = SCREEN_H + 500;
                            }
                        }
                    }
                }
            }
            // [HACKATHON 2-8]
            // TODO: Shoot if key is down and cool-down is over.
            // 1) Get the time now using 'al_get_time'.
            // 2) If Space key is down in 'key_state' and the time
            //    between now and last shoot is not less that cool
            //    down time.
            // 3) Loop through the bullet array and find one that is hidden.
            //    (This part can be optimized.)
            // 4) The bullet will be found if your array is large enough.
            // 5) Set the last shoot time to now.
            // 6) Set hidden to false (recycle the bullet) and set its x, y to the
            //    front part of your plane.
            // Uncomment and fill in the code below.


            for (i = 0; i < MAX_ENEMY; i++) {
                for (j = 0; j < MAX_BULLET; j++) {
                    if (enemies_bullets[i][j].hidden)
                        continue;
                    enemies_bullets[i][j].x += enemies_bullets[i][j].vx;
                    enemies_bullets[i][j].y += enemies_bullets[i][j].vy;
                    float x = enemies_bullets[i][j].x;
                    float y = enemies_bullets[i][j].y;
                    float w = enemies_bullets[i][j].w;
                    float h = enemies_bullets[i][j].h;
                    if (enemies_bullets[i][j].y - (enemies_bullets[i]->h / 2) > SCREEN_H)
                        enemies_bullets[i][j].hidden = true;

                }
            }

            for (i = 0; i < MAX_ENEMY; i++) {
                if (enemies[i].hidden) {
                    enemies[i].hidden = false;
                    enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
                    enemies[i].y = 0 - enemies[i].h / 2;
                }
            }

            for (i = 0; i < MAX_ENEMY; i++) {
                if (enemies[i].hidden)
                    continue;
                enemies[i].x += 5 * sin(enemies[i].y / 6);
                enemies[i].y += enemies[i].vy;

                if (enemies[i].x - (enemies[i].w / 2) < 0)
                    enemies[i].x = enemies[i].w / 2;
                else if (enemies[i].x + (enemies[i].w / 2) > SCREEN_W)
                    enemies[i].x = SCREEN_W - (enemies[i].w / 2);
            }

            double now_enemy = al_get_time();
            if (now_enemy - last_shoot_timestamp_enemy >= MAX_COOLDOWN_ENEMY) {
                for (i = 0; i < MAX_ENEMY; i++) {
                    for (j = 0; j < MAX_BULLET; j++) {
                        if (enemies_bullets[i][j].hidden)
                            break;
                    }
                    if (j == MAX_BULLET)
                        return;
                    last_shoot_timestamp_enemy = now_enemy;
                    enemies_bullets[i][j].hidden = false;
                    enemies_bullets[i][j].x = enemies[i].x;
                    enemies_bullets[i][j].y = enemies[i].y + (enemies->h / 2);
                }
            }
        }
    }
}

void game_draw(void) {
    if (active_scene == SCENE_MENU) {
        FILE* f = fopen("highscore.txt", "r");
        if (!f) {
            printf("Error opening highscore\n");
            exit(1);
        }
        fgets(highscore, sizeof(highscore), f);
        al_draw_bitmap(main_img_background, 0, 0, 0);
        al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 30, ALLEGRO_ALIGN_CENTER, "Space Shooter");
        al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), 20, SCREEN_H - 50, 0, "Press enter key to start");
        al_draw_textf(font_pirulen_32, al_map_rgb(255, 255, 0), 20, SCREEN_H - 100, 0, "HIGHSCORE : %05d", atoi(highscore));
        // [HACKATHON 3-5]
        // TODO: Draw settings images.
        // The settings icon should be located at (x, y, w, h) =
        // (SCREEN_W - 48, 10, 38, 38).
        // Change its image according to your mouse position.
        // Uncomment and fill in the code below.
        if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W - 48, 10, 38, 38))
            al_draw_bitmap(img_settings2, SCREEN_W - 48, 10, 0);
        else
            al_draw_bitmap(img_settings, SCREEN_W - 48, 10, 0);
        fclose(f);
    }

    else if (active_scene == SCENE_START) {
        int i, j;
        al_draw_bitmap(start_img_background, 0, 0, 0);
        al_draw_textf(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W - 225, SCREEN_H - 590, ALLEGRO_ALIGN_CENTER, "SCORE : %05d", score);

        if (p2) {
            al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), 12, 7, 0, "HP");
            al_draw_filled_rectangle(70, 13, 170, 33, al_map_rgb(128, 128, 128));
            al_draw_filled_rectangle(70, 13, 70 + (10 * plane.hp), 33, al_map_rgb(255, 0, 0));
            al_draw_rectangle(70, 13, 170, 33, al_map_rgb(192, 192, 192), 3);

            al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), 12, 37, 0, "HP");
            al_draw_filled_rectangle(70, 43, 270, 63, al_map_rgb(128, 128, 128));
            al_draw_filled_rectangle(70, 43, 70 + (10 * plane2.hp), 63, al_map_rgb(255, 0, 0));
            al_draw_rectangle(70, 43, 270, 63, al_map_rgb(192, 192, 192), 3);
        }
        else {
            if (first_plane) {
                al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), 12, 7, 0, "HP");
                al_draw_filled_rectangle(82, 17, 220, 37, al_map_rgb(128, 128, 128));
                al_draw_filled_rectangle(82, 17, 70 + (15 * plane.hp), 37, al_map_rgb(255, 0, 0));
                al_draw_rectangle(82, 17, 220, 37, al_map_rgb(192, 192, 192), 3);
            }
            else if (second_plane) {
                al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), 12, 7, 0, "HP");
                al_draw_filled_rectangle(82, 17, 370, 37, al_map_rgb(128, 128, 128));
                al_draw_filled_rectangle(82, 17, 70 + (15 * plane2.hp), 37, al_map_rgb(255, 0, 0));
                al_draw_rectangle(82, 17, 370, 37, al_map_rgb(192, 192, 192), 3);
            }
        }

        if (first_plane) {
            draw_movable_object(plane);
            for (i = 0; i < MAX_BULLET; i++) {
                draw_movable_object(bullets[i]);
                for (j = 0; j < MAX_ENEMY; j++) {
                    if (check(bullets[i].x, bullets[i].y, bullets->w, bullets->h, enemies[j].x, enemies[j].y, enemies->w, enemies->h)) {
                        bullets[i].hidden = true;
                        enemies[j].hp -= 1.5;
                        if (game_update_timer) {
                            al_draw_bitmap_region(img_explode[last_frame], 0, 0, 64, 64, bullets[i].x, bullets[i].y, 0);
                            printf("frame %d\n", last_frame);
                            last_frame++;
                        }
                        if (last_frame == 23) {
                            bullets[i].x = SCREEN_W + 500;
                            bullets[i].y = -300;
                            last_frame = 0;
                        }
                    }
                    /*for (int k = 0; k < MAX_BULLET; k++) {
                        if (check(bullets[i].x, bullets[i].y, bullets->w, bullets->h, enemies_bullets[j][k].x, enemies_bullets[j][k].y, enemies_bullets[j]->w, enemies_bullets[j]->h)) {
                            if (game_update_timer) {
                                al_draw_bitmap_region(img_explode[last_frame], 0, 0, 64, 64, bullets[i].x, bullets[i].y, 0);
                                enemies_bullets[j][k].x = SCREEN_W + 300;
                                enemies_bullets[j][k].y = SCREEN_H + 300;
                                bullets[i].x = SCREEN_W + 500;
                                bullets[i].y = -300;
                                printf("frame %d\n", last_frame);
                                last_frame++;
                            }
                            if (last_frame == 23) {
                                last_frame = 0;
                            }
                        }
                    }*/
                }
            }
        }

        if (second_plane) {
            draw_movable_object(plane2);
            for (i = 0; i < MAX_BULLET; i++) {
                draw_movable_object(bullets2[i]);
                for (j = 0; j < MAX_ENEMY; j++) {
                    if (check(bullets2[i].x, bullets2[i].y, bullets2->w, bullets2->h, enemies[j].x, enemies[j].y, enemies->w, enemies->h)) {
                        bullets2[i].hidden = true;
                        enemies[j].hp -= 0.75;
                        if (game_update_timer) {
                            al_draw_bitmap_region(img_explode[last_frame], 0, 0, 64, 64, bullets2[i].x, bullets2[i].y, 0);
                            last_frame++;
                        }
                        if (last_frame == 23) {
                            bullets2[i].x = SCREEN_W + 500;
                            bullets2[i].y = -300;
                            last_frame = 0;
                        }
                    }
                }
            }
        }

        if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W - 48, 10, 38, 38))
            al_draw_bitmap(img_pause2, SCREEN_W - 48, 10, 0);
        else
            al_draw_bitmap(img_pause, SCREEN_W - 48, 10, 0);

        if (pause) {
            al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H - 500, ALLEGRO_ALIGN_CENTER, "PAUSED");

            if (pnt_in_rect(mouse_x, mouse_y, 332, 205, 136, 24)) {
                //al_draw_rectangle(320, 205, 477, 237, al_map_rgb(255, 255, 255), 3);
                al_draw_text(font_pirulen_24, al_map_rgb(0, 255, 255), SCREEN_W / 2, 205, ALLEGRO_ALIGN_CENTER, "RESUME");
            }
            else {
                //al_draw_rectangle(320, 205, 477, 237, al_map_rgb(0, 0, 255), 3);
                al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W / 2, 205, ALLEGRO_ALIGN_CENTER, "RESUME");
            }
            if (pnt_in_rect(mouse_x, mouse_y, 325, 260, 150, 24)) {
                //al_draw_rectangle(315, 260, 490, 292, al_map_rgb(255, 255, 255), 3);
                al_draw_text(font_pirulen_24, al_map_rgb(0, 255, 255), SCREEN_W / 2, 260, ALLEGRO_ALIGN_CENTER, "RESTART");
            }
            else {
                //al_draw_rectangle(315, 260, 490, 292, al_map_rgb(0, 0, 255), 3);
                al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W / 2, 260, ALLEGRO_ALIGN_CENTER, "RESTART");
            }

            if (pnt_in_rect(mouse_x, mouse_y, 273, 315, 255, 24)) {
                //al_draw_rectangle(262, 315, 537, 347, al_map_rgb(255, 255, 255), 3);
                al_draw_text(font_pirulen_24, al_map_rgb(0, 255, 255), SCREEN_W / 2, 315, ALLEGRO_ALIGN_CENTER, "BACK TO MENU");
            }
            else {
                //al_draw_rectangle(262, 315, 537, 347, al_map_rgb(0, 0, 255), 3);
                al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W / 2, 315, ALLEGRO_ALIGN_CENTER, "BACK TO MENU");
            }

            if (pnt_in_rect(mouse_x, mouse_y, 365, 370, 71, 24)) {
                //al_draw_rectangle(352, 370, 445, 402, al_map_rgb(255, 255, 255), 3);
                al_draw_text(font_pirulen_24, al_map_rgb(0, 255, 255), SCREEN_W / 2, 370, ALLEGRO_ALIGN_CENTER, "EXIT");
            }
            else {
                //al_draw_rectangle(352, 370, 445, 402, al_map_rgb(0, 0, 255), 3);
                al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W / 2, 370, ALLEGRO_ALIGN_CENTER, "EXIT");
            }


        }

        // [HACKATHON 2-9]
        // TODO: Draw all bullets in your bullet array.
        // Uncomment and fill in the code below.
        for (i = 0; i < MAX_ENEMY; i++) {
            draw_movable_object(enemies[i]);
            for (j = 0; j < MAX_BULLET; j++) {
                draw_movable_object(enemies_bullets[i][j]);
            }
        }
    }
    // [HACKATHON 3-9]
    // TODO: If active_scene is SCENE_SETTINGS.
    // Draw anything you want, or simply clear the display.
    else if (active_scene == SCENE_SETTINGS) {
        al_draw_bitmap(img_set_background, 0, 0, 0);
        al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H - 500, ALLEGRO_ALIGN_CENTER, "SETTINGS");

        al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 150, ALLEGRO_ALIGN_RIGHT, "BGM :");
        if (bgm) {
            if (pnt_in_rect(mouse_x, mouse_y, 480, 155, 65, 25))
                al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 0), 480, 150, 0, "ON");
            else
                al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), 480, 150, 0, "ON");
        }
        else {
            if (pnt_in_rect(mouse_x, mouse_y, 480, 150, 83, 25))
                al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 0), 480, 150, 0, "OFF");
            else
                al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), 480, 150, 0, "OFF");
        }

        al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 200, ALLEGRO_ALIGN_RIGHT, "SFX :");
        if (sfx) {
            if (pnt_in_rect(mouse_x, mouse_y, 480, 200, 65, 25))
                al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 0), 480, 200, 0, "ON");
            else
                al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), 480, 200, 0, "ON");
        }
        else {
            if (pnt_in_rect(mouse_x, mouse_y, 380, 205, 83, 25))
                al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 0), 480, 200, 0, "OFF");
            else
                al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), 480, 200, 0, "OFF");
        }

        if (pnt_in_rect(mouse_x, mouse_y, 175, 255, 455, 25))
            al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 0), SCREEN_W / 2, 250, ALLEGRO_ALIGN_CENTER, "CHARACTER SELECT");
        else
            al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 250, ALLEGRO_ALIGN_CENTER, "CHARACTER SELECT");

        al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 300, ALLEGRO_ALIGN_RIGHT, "MODE :");

        if (p2) {
            if (pnt_in_rect(mouse_x, mouse_y, 480, 305, 55, 25))
                al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 0), 480, 300, 0, "2P");
            else
                al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), 480, 300, 0, "2P");
        }
        else {
            if (pnt_in_rect(mouse_x, mouse_y, 480, 305, 40, 25))
                al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 0), 480, 300, 0, "1P");
            else
                al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), 480, 300, 0, "1P");
        }

        if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 - 75, SCREEN_H - 175, 150, 50)) {
            al_draw_rectangle(SCREEN_W / 2 - 75, SCREEN_H - 175, SCREEN_W / 2 + 75, SCREEN_H - 125, al_map_rgb(255, 255, 255), 3);
            al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 0), 340, 430, 0, "BACK");
        }
        else {
            al_draw_rectangle(SCREEN_W / 2 - 75, SCREEN_H - 175, SCREEN_W / 2 + 75, SCREEN_H - 125, al_map_rgb(0, 0, 255), 3);
            al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), 340, 430, 0, "BACK");
        }
    }

    else if (active_scene == SCENE_CHAR) {
        al_draw_bitmap(img_char_background, 0, 0, 0);
        al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 - 200, ALLEGRO_ALIGN_CENTER, "CHOOSE YOUR AIRCRAFT");

        al_draw_bitmap(start_img_plane, SCREEN_W / 7, SCREEN_H / 2 - 40, 0);
        al_draw_bitmap(img_bullet, SCREEN_W / 3 - 5, SCREEN_H / 2 - 13, 0);

        al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 - 110, 0, "HP");
        al_draw_filled_rectangle(SCREEN_W / 2, SCREEN_H / 2 - 70, SCREEN_W / 2 + 350, SCREEN_H / 2 - 45, al_map_rgb(150, 150, 150));
        al_draw_filled_rectangle(SCREEN_W / 2, SCREEN_H / 2 - 70, SCREEN_W / 2 + 175, SCREEN_H / 2 - 45, al_map_rgb(211, 211, 211));
        //al_draw_rectangle(SCREEN_W / 2, SCREEN_H / 2 - 70, SCREEN_W / 2 + 350, SCREEN_H / 2 - 45, al_map_rgb(192, 192, 192), 1);

        al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 - 35, 0, "DAMAGE");
        al_draw_filled_rectangle(SCREEN_W / 2, SCREEN_H / 2 + 5, SCREEN_W / 2 + 350, SCREEN_H / 2 + 30, al_map_rgb(150, 150, 150));
        al_draw_filled_rectangle(SCREEN_W / 2, SCREEN_H / 2 + 5, SCREEN_W / 2 + 300, SCREEN_H / 2 + 30, al_map_rgb(211, 211, 211));

        al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 + 40, 0, "SPEED");
        al_draw_filled_rectangle(SCREEN_W / 2, SCREEN_H / 2 + 80, SCREEN_W / 2 + 350, SCREEN_H / 2 + 105, al_map_rgb(150, 150, 150));
        al_draw_filled_rectangle(SCREEN_W / 2, SCREEN_H / 2 + 80, SCREEN_W / 2 + 225, SCREEN_H / 2 + 105, al_map_rgb(211, 211, 211));

        if (pnt_in_rect(mouse_x, mouse_y, 167, 412, 65, 25))
            al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 0), SCREEN_W / 4, 405, ALLEGRO_ALIGN_CENTER, "OK!");
        else
            al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 4, 405, ALLEGRO_ALIGN_CENTER, "OK!");

        if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 - 75, SCREEN_H - 100, 150, 50)) {
            al_draw_rectangle(SCREEN_W / 2 - 75, SCREEN_H - 100, SCREEN_W / 2 + 75, SCREEN_H - 50, al_map_rgb(255, 255, 255), 3);
            al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 0), 340, SCREEN_H - 95, 0, "BACK");
        }
        else {
            al_draw_rectangle(SCREEN_W / 2 - 75, SCREEN_H - 100, SCREEN_W / 2 + 75, SCREEN_H - 50, al_map_rgb(0, 0, 255), 3);
            al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), 340, SCREEN_H - 95, 0, "BACK");
        }

        if (pnt_in_rect(mouse_x, mouse_y, 25, SCREEN_H / 2 - 20, 35, 40))
            al_draw_filled_triangle(60, SCREEN_H / 2 - 20, 60, SCREEN_H / 2 + 20, 25, SCREEN_H / 2, al_map_rgb(140, 140, 140));
        else
            al_draw_filled_triangle(60, SCREEN_H / 2 - 20, 60, SCREEN_H / 2 + 20, 25, SCREEN_H / 2, al_map_rgb(255, 255, 255));

        if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 - 60, SCREEN_H / 2 - 20, 35, 40))
            al_draw_filled_triangle(SCREEN_W / 2 - 60, SCREEN_H / 2 - 20, SCREEN_W / 2 - 60, SCREEN_H / 2 + 20, SCREEN_W / 2 - 25, SCREEN_H / 2, al_map_rgb(140, 140, 140));
        else
            al_draw_filled_triangle(SCREEN_W / 2 - 60, SCREEN_H / 2 - 20, SCREEN_W / 2 - 60, SCREEN_H / 2 + 20, SCREEN_W / 2 - 25, SCREEN_H / 2, al_map_rgb(255, 255, 255));
    }

    else if (active_scene == SCENE_CHAR2) {
        al_draw_bitmap(img_char_background, 0, 0, 0);
        al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 - 200, ALLEGRO_ALIGN_CENTER, "CHOOSE YOUR AIRCRAFT");

        al_draw_bitmap(start_img_plane2, SCREEN_W / 7, SCREEN_H / 2 - 20, 0);
        al_draw_bitmap(img_bullet2, SCREEN_W / 3 - 5, SCREEN_H / 2 - 13, 0);

        al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 - 110, 0, "HP");
        al_draw_filled_rectangle(SCREEN_W / 2, SCREEN_H / 2 - 70, SCREEN_W / 2 + 350, SCREEN_H / 2 - 45, al_map_rgb(150, 150, 150));
        al_draw_filled_rectangle(SCREEN_W / 2, SCREEN_H / 2 - 70, SCREEN_W / 2 + 350, SCREEN_H / 2 - 45, al_map_rgb(211, 211, 211));
        //al_draw_rectangle(SCREEN_W / 2, SCREEN_H / 2 - 70, SCREEN_W / 2 + 350, SCREEN_H / 2 - 45, al_map_rgb(192, 192, 192), 1);

        al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 - 35, 0, "DAMAGE");
        al_draw_filled_rectangle(SCREEN_W / 2, SCREEN_H / 2 + 5, SCREEN_W / 2 + 350, SCREEN_H / 2 + 30, al_map_rgb(150, 150, 150));
        al_draw_filled_rectangle(SCREEN_W / 2, SCREEN_H / 2 + 5, SCREEN_W / 2 + 200, SCREEN_H / 2 + 30, al_map_rgb(211, 211, 211));

        al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 + 40, 0, "SPEED");
        al_draw_filled_rectangle(SCREEN_W / 2, SCREEN_H / 2 + 80, SCREEN_W / 2 + 350, SCREEN_H / 2 + 105, al_map_rgb(150, 150, 150));
        al_draw_filled_rectangle(SCREEN_W / 2, SCREEN_H / 2 + 80, SCREEN_W / 2 + 175, SCREEN_H / 2 + 105, al_map_rgb(211, 211, 211));

        if (pnt_in_rect(mouse_x, mouse_y, 167, 412, 65, 25))
            al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 0), SCREEN_W / 4, 405, ALLEGRO_ALIGN_CENTER, "OK!");
        else
            al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 4, 405, ALLEGRO_ALIGN_CENTER, "OK!");

        if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 - 75, SCREEN_H - 100, 150, 50)) {
            al_draw_rectangle(SCREEN_W / 2 - 75, SCREEN_H - 100, SCREEN_W / 2 + 75, SCREEN_H - 50, al_map_rgb(255, 255, 255), 3);
            al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 0), 340, SCREEN_H - 95, 0, "BACK");
        }
        else {
            al_draw_rectangle(SCREEN_W / 2 - 75, SCREEN_H - 100, SCREEN_W / 2 + 75, SCREEN_H - 50, al_map_rgb(0, 0, 255), 3);
            al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), 340, SCREEN_H - 95, 0, "BACK");
        }

        if (pnt_in_rect(mouse_x, mouse_y, 25, SCREEN_H / 2 - 20, 35, 40))
            al_draw_filled_triangle(60, SCREEN_H / 2 - 20, 60, SCREEN_H / 2 + 20, 25, SCREEN_H / 2, al_map_rgb(140, 140, 140));
        else
            al_draw_filled_triangle(60, SCREEN_H / 2 - 20, 60, SCREEN_H / 2 + 20, 25, SCREEN_H / 2, al_map_rgb(255, 255, 255));

        if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 - 60, SCREEN_H / 2 - 20, 35, 40))
            al_draw_filled_triangle(SCREEN_W / 2 - 60, SCREEN_H / 2 - 20, SCREEN_W / 2 - 60, SCREEN_H / 2 + 20, SCREEN_W / 2 - 25, SCREEN_H / 2, al_map_rgb(140, 140, 140));
        else
            al_draw_filled_triangle(SCREEN_W / 2 - 60, SCREEN_H / 2 - 20, SCREEN_W / 2 - 60, SCREEN_H / 2 + 20, SCREEN_W / 2 - 25, SCREEN_H / 2, al_map_rgb(255, 255, 255));
    }

    //else if (active_scene == SCENE_PAUSE) {
    //    al_clear_to_color(al_map_rgb(0, 0, 0));
    //    al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H - 500, ALLEGRO_ALIGN_CENTER, "RESUME");
    //    al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H - 200, ALLEGRO_ALIGN_CENTER, "EXIT");
    //}
    else if (active_scene == SCENE_OVER) {
        al_draw_bitmap(over_img_background, 0, 0, 0);
        al_draw_textf(font_pirulen_32, al_map_rgb(255, 255, 0), SCREEN_W / 2, SCREEN_H / 2 - 150, ALLEGRO_ALIGN_CENTER, "YOUR SCORE : %05d", temp);
        al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 - 100, ALLEGRO_ALIGN_CENTER, "GAME OVER");
        al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2, ALLEGRO_ALIGN_CENTER, "RESTART?");

        if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 - 85, SCREEN_H / 2 + 107, 70, 18))
            al_draw_text(font_pirulen_24, al_map_rgb(65, 105, 225), SCREEN_W / 2 - 50, SCREEN_H / 2 + 100, ALLEGRO_ALIGN_CENTER, "YES");
        else
            al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W / 2 - 50, SCREEN_H / 2 + 100, ALLEGRO_ALIGN_CENTER, "YES");

        if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 + 25, SCREEN_H / 2 + 107, 50, 18))
            al_draw_text(font_pirulen_24, al_map_rgb(65, 105, 225), SCREEN_W / 2 + 50, SCREEN_H / 2 + 100, ALLEGRO_ALIGN_CENTER, "NO");
        else
            al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W / 2 + 50, SCREEN_H / 2 + 100, ALLEGRO_ALIGN_CENTER, "NO");

    }
    al_flip_display();
}

void game_destroy(void) {
    // Destroy everything you have created.
    // Free the memories allocated by malloc or allegro functions.
    // Destroy shared resources.
    al_destroy_font(font_pirulen_32);
    al_destroy_font(font_pirulen_24);

    /* Menu Scene resources*/
    al_destroy_bitmap(main_img_background);
    al_destroy_sample(main_bgm);
    al_destroy_sample(click);
    // [HACKATHON 3-6]
    // TODO: Destroy the 2 settings images.
    // Uncomment and fill in the code below.
    al_destroy_bitmap(img_settings);
    al_destroy_bitmap(img_settings2);

    al_destroy_bitmap(img_set_background);
    al_destroy_bitmap(img_char_background);

    /* Start Scene resources*/
    al_destroy_bitmap(start_img_background);
    al_destroy_bitmap(start_img_plane);
    al_destroy_bitmap(start_img_plane2);
    al_destroy_bitmap(start_img_enemy);
    al_destroy_sample(start_bgm);
    // [HACKATHON 2-10]
    // TODO: Destroy your bullet image.
    // Uncomment and fill in the code below.
    al_destroy_bitmap(img_bullet);
    al_destroy_bitmap(img_bullet2);
    al_destroy_bitmap(img_enemy_bullet);
    al_destroy_bitmap(img_pause);
    al_destroy_bitmap(img_pause2);
    al_destroy_sample(fire);
    al_destroy_sample(laser);
    al_destroy_sample(setting_bg);
    al_destroy_sample(char_scene);
    al_destroy_bitmap(over_img_background);
    al_destroy_sample(lose);
    al_destroy_timer(game_update_timer);
    al_destroy_event_queue(game_event_queue);
    al_destroy_display(game_display);
    free(mouse_state);
}

void game_change_scene(int next_scene) {
    game_log("Change scene from %d to %d", active_scene, next_scene);
    // TODO: Destroy resources initialized when creating scene.
    if (active_scene == SCENE_MENU) {
        al_stop_sample(&main_bgm_id);
        plane.hp = 10;
        plane2.hp = 20;
        score = 0;
        if (p2) {
            first_plane = 1;
            second_plane = 1;
        }
        game_log("stop audio (bgm)");
    }
    else if (active_scene == SCENE_START) {
        al_stop_sample(&start_bgm_id);
        game_log("stop audio (bgm)");
    }
    else if (active_scene == SCENE_CHAR) {
        al_stop_sample(&char_scene_id);
        game_log("stop audio (bgm)");
    }
    else if (active_scene == SCENE_CHAR2) {
        al_stop_sample(&char_scene_id);
        game_log("stop audio (bgm)");
    }
    else if (active_scene == SCENE_OVER) {
        score = 0;
    }
    else if (active_scene == SCENE_SETTINGS) {
        al_stop_sample(&setting_bg_id);
        game_log("stop audio (bgm)");
    }
    active_scene = next_scene;
    // TODO: Allocate resources before entering scene.
    if (active_scene == SCENE_MENU) {
        if (bgm) {
            if (!al_play_sample(main_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &main_bgm_id))
                game_abort("failed to play audio (bgm)");
        }
    }
    else if (active_scene == SCENE_START) {
        int i, j;
        if (first_plane) {
            plane.img = start_img_plane;
            plane.x = 300;
            plane.y = 500;
            plane.w = al_get_bitmap_width(plane.img);
            plane.h = al_get_bitmap_height(plane.img);
            plane.hp = 10;
        }
        if (second_plane) {
            plane2.img = start_img_plane2;
            plane2.x = 450;
            plane2.y = 500;
            plane2.w = al_get_bitmap_width(plane2.img);
            plane2.h = al_get_bitmap_height(plane2.img);
            plane2.hp = 20;
        }
        for (i = 0; i < MAX_ENEMY; i++) {
            enemies[i].img = start_img_enemy;
            enemies[i].w = al_get_bitmap_width(start_img_enemy);
            enemies[i].h = al_get_bitmap_height(start_img_enemy);
            enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
            enemies[i].y = 80;
            enemies[i].vx = 0.5;
            enemies[i].vy = 0.5;
            enemies[i].hp = 5;
            for (j = 0; j < MAX_BULLET; j++) {
                enemies_bullets[i][j].img = img_enemy_bullet;
                enemies_bullets[i][j].w = al_get_bitmap_width(img_enemy_bullet);
                enemies_bullets[i][j].h = al_get_bitmap_height(img_enemy_bullet);
                enemies_bullets[i][j].vx = 0;
                enemies_bullets[i][j].vy = 1.0;
                enemies_bullets[i][j].hidden = true;
            }
        }
        // [HACKATHON 2-6]
        // TODO: Initialize bullets.
        // For each bullets in array, set their w and h to the size of
        // the image, and set their img to bullet image, hidden to true,
        // (vx, vy) to (0, -3).
        // Uncomment and fill in the code below.
        for (i = 0; i < MAX_BULLET; i++) {
            bullets[i].img = img_bullet;
            bullets[i].w = al_get_bitmap_width(img_bullet);
            bullets[i].h = al_get_bitmap_height(img_bullet);
            bullets[i].vx = 0;
            bullets[i].vy = -6;
            bullets[i].hidden = true;
        }

        for (i = 0; i < MAX_BULLET; i++) {
            bullets2[i].img = img_bullet2;
            bullets2[i].w = al_get_bitmap_width(img_bullet2);
            bullets2[i].h = al_get_bitmap_height(img_bullet2);
            bullets2[i].vx = 0;
            bullets2[i].vy = -6;
            bullets2[i].hidden = true;
        }
        if (bgm) {
            if (!al_play_sample(start_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &start_bgm_id))
                game_abort("failed to play audio (bgm)");
        }
    }
    else if (active_scene == SCENE_CHAR || active_scene == SCENE_CHAR2) {
        if (bgm)
            if (!al_play_sample(char_scene, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &char_scene_id));
    }
    else if (active_scene == SCENE_SETTINGS) {
        if (bgm)
            if (!al_play_sample(setting_bg, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &setting_bg_id));
    }
}

void on_key_down(int keycode) {
    if (active_scene == SCENE_MENU) {
        if (keycode == ALLEGRO_KEY_ENTER)
            game_change_scene(SCENE_START);
    }
    // [HACKATHON 3-10]
    // TODO: If active_scene is SCENE_SETTINGS and Backspace is pressed,
    // return to SCENE_MENU.
    else if (active_scene == SCENE_SETTINGS) {
        if (keycode == ALLEGRO_KEY_BACKSPACE)
            game_change_scene(SCENE_MENU);
    }
    else if (active_scene == SCENE_START) {
        if (pause) {
            if (key_state[ALLEGRO_KEY_ESCAPE])
                pause = false;
        }
        else {
            if (key_state[ALLEGRO_KEY_ESCAPE])
                pause = true;
        }
    }
    else if (active_scene == SCENE_OVER) {
        if (key_state[ALLEGRO_KEY_ENTER] || key_state[ALLEGRO_KEY_Y])
            game_change_scene(SCENE_START);
        else if (key_state[ALLEGRO_KEY_ESCAPE] || key_state[ALLEGRO_KEY_N])
            game_change_scene(SCENE_MENU);
    }
}

void on_mouse_down(int btn, int x, int y) {
    // [HACKATHON 3-8]
    // TODO: When settings clicked, switch to settings scene.
    // Uncomment and fill in the code below.
    if (active_scene == SCENE_MENU) {
        if (btn == 1) {
            if (sfx)
                if (!al_play_sample(click, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &click_id));

            if (pnt_in_rect(x, y, SCREEN_W - 48, 10, 38, 38))
                game_change_scene(SCENE_SETTINGS);
        }
    }
    if (active_scene == SCENE_OVER) {
        if (btn == 1) {
            if (sfx)
                if (!al_play_sample(click, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &click_id));

            if (pnt_in_rect(x, y, SCREEN_W / 2 - 50, SCREEN_H / 2 + 100, 38, 38))
                game_change_scene(SCENE_START);
            else if (pnt_in_rect(x, y, SCREEN_W / 2 + 50, SCREEN_H / 2 + 100, 38, 38))
                game_change_scene(SCENE_MENU);
        }
    }
    if (active_scene == SCENE_START) {
        if (btn == 1) {
            if (sfx)
                if (!al_play_sample(click, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &click_id));

            if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W - 48, 10, 38, 38))
                pause = true;
        }
        if (pause) {
            if (btn == 1) {
                if (sfx)
                    if (!al_play_sample(click, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &click_id));

                if (pnt_in_rect(mouse_x, mouse_y, 332, 205, 136, 24))
                    pause = 0;
                else if (pnt_in_rect(mouse_x, mouse_y, 325, 260, 150, 24)) {
                    pause = 0;
                    score = 0;
                    game_change_scene(SCENE_START);
                }
                else if (pnt_in_rect(mouse_x, mouse_y, 273, 315, 255, 24)) {
                    pause = 0;
                    game_change_scene(SCENE_MENU);
                }
                else if (pnt_in_rect(mouse_x, mouse_y, 335, 375, 130, 50))
                    exit(1);
            }
        }
    }

    if (active_scene == SCENE_SETTINGS) {
        if (btn == 1) {
            if (sfx)
                if (!al_play_sample(click, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &click_id));

            if (bgm) {
                if (pnt_in_rect(mouse_x, mouse_y, 480, 155, 65, 25))
                    bgm = 0;
            }
            else {
                if (pnt_in_rect(mouse_x, mouse_y, 480, 155, 65, 25))
                    bgm = 1;
            }

            if (sfx) {
                if (pnt_in_rect(mouse_x, mouse_y, 480, 200, 65, 25))
                    sfx = 0;
            }
            else {
                if (pnt_in_rect(mouse_x, mouse_y, 480, 200, 65, 25))
                    sfx = 1;
            }

            if (p2) {
                if (pnt_in_rect(mouse_x, mouse_y, 480, 305, 83, 25))
                    p2 = 0;
            }
            else {
                if (pnt_in_rect(mouse_x, mouse_y, 480, 305, 83, 25))
                    p2 = 1;
            }

            if (pnt_in_rect(mouse_x, mouse_y, 175, 255, 455, 25))
                game_change_scene(SCENE_CHAR);
            if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 - 75, SCREEN_H - 175, 150, 50))
                game_change_scene(SCENE_MENU);
        }
    }

    if (active_scene == SCENE_CHAR) {
        if (btn == 1) {
            if (sfx)
                if (!al_play_sample(click, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &click_id));

            if (pnt_in_rect(mouse_x, mouse_y, 25, SCREEN_H / 2 - 20, 35, 40))
                game_change_scene(SCENE_CHAR2);
            else if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 - 60, SCREEN_H / 2 - 20, 35, 40))
                game_change_scene(SCENE_CHAR2);
            else if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 - 75, SCREEN_H - 100, 150, 50))
                game_change_scene(SCENE_SETTINGS);
            else if (pnt_in_rect(mouse_x, mouse_y, 167, 412, 65, 25)) {
                first_plane = 1;
                second_plane = 0;
                game_change_scene(SCENE_SETTINGS);
            }
        }
    }

    else if (active_scene == SCENE_CHAR2) {
        if (btn == 1) {
            if (sfx)
                if (!al_play_sample(click, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &click_id));

            if (pnt_in_rect(mouse_x, mouse_y, 25, SCREEN_H / 2 - 20, 35, 40))
                game_change_scene(SCENE_CHAR);
            else if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 - 75, SCREEN_H - 100, 150, 50))
                game_change_scene(SCENE_SETTINGS);
            else if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W / 2 - 60, SCREEN_H / 2 - 20, 35, 40))
                game_change_scene(SCENE_CHAR);
            else if (pnt_in_rect(mouse_x, mouse_y, 167, 412, 65, 25)) {
                first_plane = 0;
                second_plane = 1;
                game_change_scene(SCENE_SETTINGS);
            }
        }
    }
}

void draw_movable_object(MovableObject obj) {
    if (obj.hidden)
        return;
    al_draw_bitmap(obj.img, round(obj.x - obj.w / 2), round(obj.y - obj.h / 2), 0);
}

ALLEGRO_BITMAP* load_bitmap_resized(const char* filename, int w, int h) {
    ALLEGRO_BITMAP* loaded_bmp = al_load_bitmap(filename);
    if (!loaded_bmp)
        game_abort("failed to load image: %s", filename);
    ALLEGRO_BITMAP* resized_bmp = al_create_bitmap(w, h);
    ALLEGRO_BITMAP* prev_target = al_get_target_bitmap();

    if (!resized_bmp)
        game_abort("failed to create bitmap when creating resized image: %s", filename);
    al_set_target_bitmap(resized_bmp);
    al_draw_scaled_bitmap(loaded_bmp, 0, 0,
        al_get_bitmap_width(loaded_bmp),
        al_get_bitmap_height(loaded_bmp),
        0, 0, w, h, 0);
    al_set_target_bitmap(prev_target);
    al_destroy_bitmap(loaded_bmp);

    game_log("resized image: %s", filename);

    return resized_bmp;
}

// [HACKATHON 3-3]
// TODO: Define bool pnt_in_rect(int px, int py, int x, int y, int w, int h)
// Uncomment and fill in the code below.
bool pnt_in_rect(int px, int py, int x, int y, int w, int h) {
    if (px >= x && px <= (x + w) && py >= y && py <= (y + h)) return true;
    return false;
}

bool check(float x, float y, float w, float h, float a, float b, float n, float m) {
    if (x + (w / 2) >= a - (n / 2) && x - (w / 2) <= a + (n / 2) && y + (h / 2) >= b - (m / 2) && y - (h / 2) <= b + (m / 2)) return true;
    return false;
}


// +=================================================================+
// | Code below is for debugging purpose, it's fine to remove it.    |
// | Deleting the code below and removing all calls to the functions |
// | doesn't affect the game.                                        |
// +=================================================================+

void game_abort(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
    fprintf(stderr, "error occured, exiting after 2 secs");
    // Wait 2 secs before exiting.
    al_rest(2);
    // Force exit program.
    exit(1);
}

void game_log(const char* format, ...) {
#ifdef LOG_ENABLED
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
#endif
}

void game_vlog(const char* format, va_list arg) {
#ifdef LOG_ENABLED
    static bool clear_file = true;
    // Write log to file for later debugging.
    FILE* pFile = fopen("log.txt", clear_file ? "w" : "a");
    if (pFile) {
        va_list arg2;
        va_copy(arg2, arg);
        vfprintf(pFile, format, arg2);
        va_end(arg2);
        fprintf(pFile, "\n");
        fclose(pFile);
    }
    vprintf(format, arg);
    printf("\n");
    clear_file = false;
#endif
}
