/**
* Author: Tingting Min
* Assignment: Platformer
* Date due: 2023-11-23, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

/**
 Requirement 1: Menu Screen (10%) ✅
 Show the name of your game and the text "Press enter to start" (or any variation thereof). The keycode for enter is: SDLK_RETURN
 This can be a solid color background with text on it.
 The menu must be a different Scene object. Do not just show/hide text.

 Requirement 2: 3 Levels (40%) ✅
 Your game needs to have 3 levels. They do not need to be long or complicated.
 They must scroll! (no single screen games) If you do not have 3 scrolling levels, your grade for the entire project will be 0.
 This is a platformer game, it needs to have platforms.

 Requirement 3: 3 Lives (25%) ✅
 The player gets 3 lives for the entire game (not per level).
 If the player runs out of lives, show a “You Lose” text.
 If the player gets to the end of your game, show text a "You Win” text.

 Requirement 4: AI (25%) ✅
 At least 1 type of moving AI (place a couple of these AI in your game).
 If the player touches the AI, the player dies.
 Each of your levels must have at least 1 AI.

 Common Issues
 You might find it easier to work on the movement first (without any obstacles in the way). Once the movement is working, then add your environment.

 Extra Credit
 1. Have a pseudo-start screen where, if the player presses a button, the entire screen freezes. Do not make this a separate Scene object. ✅
 2. Add at least one special effect to your game. ✅
 */


#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "Level A.h"
#include "Level B.h"
#include "Level C.h"
#include "Menu.h"
#include "Effects.h"

// ––––– CONSTANTS ––––– //
//constexpr int WINDOW_WIDTH  = 640,
//          WINDOW_HEIGHT = 480;

constexpr int WINDOW_WIDTH  = 600 * 2,
          WINDOW_HEIGHT = 400 * 2;

constexpr float BG_RED     = 0.0f,
                BG_GREEN   = 0.0f,
                BG_BLUE    = 0.0f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
                F_SHADER_PATH[] = "shaders/fragment_textured.glsl",
                FONTSHEET_FILEPATH[] = "font1.png";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ––––– GLOBAL VARIABLES ––––– //
Scene  *g_current_scene;

Menu *g_menu;
LevelA *g_levelA;
LevelB *g_levelB;
LevelC *g_levelC;

Effects *g_effects;
Scene   *g_levels[4];

GLuint main_font_texture_id;

bool game_over = false;

bool pseudoStartScreenActive = false;
GLuint pseudoStartScreenTexture;

SDL_Window* g_display_window;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

bool g_is_colliding_bottom = false;

AppStatus g_app_status = RUNNING;

void swtich_to_scene(Scene *scene);
void initialise();
void process_input();
void update();
void render();
void shutdown();

// ––––– GENERAL FUNCTIONS ––––– //
void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise(); // DON'T FORGET THIS STEP!
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Spirit's Eve",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-10.0f, 10.0f, -5.75f, 5.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    g_menu   = new Menu();
    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_levelC = new LevelC();
    
    g_levels[0] = g_menu;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
    g_levels[3] = g_levelC;
    
    // Start at level A
    switch_to_scene(g_levels[0]);
    
    g_effects = new Effects(g_projection_matrix, g_view_matrix);
    g_effects->start(FADEIN, 1.0f);
    
    // Load the texture during initialization
    pseudoStartScreenTexture = Utility::load_texture("note.png");
    
    main_font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
    
    glDisable(GL_DEPTH_TEST);  // Add this to your initialization if depth testing is not required

}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_app_status = TERMINATED;
                        break;
                    
                    case SDLK_RETURN:
                        // Press enter to start
                        if (g_current_scene == g_menu){
                            switch_to_scene(g_levelA);
                        }
                        
                    case SDLK_SPACE:
                        // Jump
                        if (g_current_scene->get_state().player->get_collided_bottom())
                                                {
                                                    g_current_scene->get_state().player->jump();
                                                }
                                                 break;
                    case SDLK_i:
                        // pseudo-start screen
                        pseudoStartScreenActive = !pseudoStartScreenActive;  // Toggle pseudo screen
                        if (pseudoStartScreenActive){
                            Utility::toggle_gamePause(true);
                        }// Freeze game updates
                        else Utility::toggle_gamePause(false);
                        break;
                    
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_A] and !(g_current_scene == g_menu))           g_current_scene->get_state().player->move_left();
        else if (key_state[SDL_SCANCODE_D] and !(g_current_scene == g_menu))  g_current_scene->get_state().player->move_right();
         
    if (glm::length( g_current_scene->get_state().player->get_movement()) > 1.0f)
        g_current_scene->get_state().player->normalise_movement();
}

void update()
{
    if (Utility::get_gamePaused() || pseudoStartScreenActive) return; // Skip updating if the game is paused
    
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);
        g_effects->update(FIXED_TIMESTEP);
        
         if (g_is_colliding_bottom == false && g_current_scene->get_state().player->get_collided_bottom()) g_effects->start(NONE, 1.0f);
        
        g_is_colliding_bottom = g_current_scene->get_state().player->get_collided_bottom();
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    // Prevent the camera from showing anything outside of the "edge" of the level
    g_view_matrix = glm::mat4(0.8f);
    if(!(g_current_scene == g_menu)){
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-4.5f, 1.0f, 0.0f));
    }
    
    if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, 3.75, 0));
    } else {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
    }
    
    if (g_current_scene == g_levelA && g_current_scene->get_state().player->get_position().x > 21.0f){
//        std::cout << "Attempting to switch from Level A to Level B." << std::endl;
        switch_to_scene(g_levelB);
        g_effects->start(FADEIN, 0.5f);
    }
    
    if (g_current_scene == g_levelB && g_current_scene->get_state().player->get_position().x > 19.0f && g_current_scene->get_state().player->get_position().y == -6.0f) {
//        std::cout << "Attempting to switch from Level B to Level C." << std::endl;
        switch_to_scene(g_levelC);
        g_effects->start(FADEIN, 0.5f);
    }
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (pseudoStartScreenActive) {
        // Set up the shader for full-screen texture rendering
        g_shader_program.set_view_matrix(glm::mat4(1.0f));
        g_shader_program.set_model_matrix(glm::mat4(1.0f));

        glBindTexture(GL_TEXTURE_2D, pseudoStartScreenTexture);
        Utility::draw_texture(&g_shader_program, pseudoStartScreenTexture, -5.0f, -3.0f, 10.0f, 7.0f);
    } else {
        // Regular game rendering
        g_shader_program.set_view_matrix(g_view_matrix);
        g_current_scene->render(&g_shader_program);
    }

    g_effects->render();
    SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
    SDL_Quit();
    
    delete g_menu;
    delete g_levelA;
    delete g_levelB;
    delete g_levelC;
    delete g_effects;
}

// ––––– DRIVER GAME LOOP ––––– //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        if (g_current_scene->get_state().next_scene_id >= 0) switch_to_scene(g_levels[g_current_scene->get_state().next_scene_id]);
        render();
    }
    
    shutdown();
    return 0;
}
