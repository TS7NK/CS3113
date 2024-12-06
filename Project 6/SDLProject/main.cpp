/**
* Author: Tingting Min
* Assignment: Miner's Maze: Diamond Quest
* Date due: 12/6/24, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

/**
 Requirement 1: Menu Screen (10%)  ✅
 Show the name of your game and the text "Press enter to start" (or any variation thereof). The keycode for enter is: SDLK_RETURN
 This can be a solid color background with text on it.
 The menu must be a different Scene object. Do not just show/hide text.
 
 Requirement 2: 2 Minutes of Gameplay (40%) ✅
 Must have 2 minutes worth of significant gameplay. This does not mean exactly 2 minutes, but rather that the game can go on for 2+ minutes.
 
 Must have 3 levels (Scene).  ✅
 If your idea doesn't revolve around multi-level-based design, whatever scene structure that you implement must contain around the same quantity of content as a 3-level game.
 1. Gate of the mine
 2. 1st floor of mine
 3. 2nd floor of mine
 3. 3rd floor of mine

 Requirement 3: Moving AI (20%)  ✅
 There should be at least three distinct types of these.
 They must transform in some way.
 These don't necessarily need to be enemies! Homing projectiles, moving platforms/obstacles also count as long as they are distinct from each other in behaviour.
 1. Walker -> flies
 2. Guard  -> Skulls
 3. Trap -> when user hit a rock but the rock is a bomb so user will lose a life

 Requirement 4: Players Can Win and Lose (10%)  ✅
 Some way for the player to lose (touching an AI, falling off a ledge, running out of time, etc.)
 Some way for the player to win (defeating all AI, collecting all the coins, etc.)
 Note that past students have done games where there are no win/lose conditions (e.g. a farming sim, etc.). This is also fine, but you must let us know in advance.
 -> Check user's lives == 0  => You lose!
 -> Check user's diamond collection == 3  && user's lives > 0  => You win!

 Requirement 5: Shader Logic (10%) ✅
 Use at least one logic-based (i.e. if-statements) shader effect.
 It can be any of the ones we talked about in class (or of your own design), but it must contribute the overall theme and experience of your game (i.e. don't just make the screen red for the sake of it).
 -> Use the shader for only the character lights up for level 2
 
 Extra Credit  ✅
 Apply at least one special effect. This can be one of the ones we wrote in class or a new one of your making.
 
 Tasks list:
 - Change the character so that it can have animation ✅
 - Put rocks and diamond in 3 levels ✅
 - Adjust the display for lives and diamonds ✅
 - Made 3 types of enemy ✅
 - Add 3 types of enemies  into 1st floor ✅
 - Add 3 types of enemies  into 2nd floor ✅
 - Add 3 types of enemies  into 3rd floor ✅
 - Hide the ladder make player interact with rock to show the " ✅
 - Add shader effect to slim level -> only user lights up ✅
 - Make a info page about the game -> use stardew valley note, use the tone like stardew  ✅
 - Adjust the special effect when changing levels ✅
 */

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 31
#define LEVEL1_HEIGHT 27
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
#include "Level D.h"
#include "Menu.h"
#include "Effects.h"

// ––––– CONSTANTS ––––– //

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
LevelD *g_levelD;

Effects *g_effects;
Scene   *g_levels[5];

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
int onlyLevel_D = 0;

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
    
    // Reset view matrix for each scene
    g_view_matrix = glm::mat4(1.0f);
    
    if (g_current_scene == g_levelA) {
        g_projection_matrix = glm::ortho(-10.0f, 10.0f, -1.75f, 9.75f, -1.0f, 1.0f);
    }
    else if (g_current_scene == g_levelB) {
        g_projection_matrix = glm::ortho(-10.0f, 10.0f, -6.0f, 6.0f, -1.0f, 1.0f);
    } else if (g_current_scene == g_levelC) {
        g_projection_matrix = glm::ortho(-10.0f, 10.0f, -6.75f, 7.75f, -1.0f, 1.0f);
    } else if (g_current_scene == g_levelD) {
        g_projection_matrix = glm::ortho(-10.0f, 10.0f, -4.75f, 9.00f, -1.0f, 1.0f);
    }
    else if (g_current_scene == g_menu) {
        g_projection_matrix = glm::ortho(-10.0f, 10.0f, -5.75f, 5.75f, -1.0f, 1.0f);
    }

    // Update shader with new view matrix
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Skull Cavern: Diamond Quest",
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
        
    // Set default projection matrix (e.g., for the menu)
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
    g_levelD = new LevelD();
    
    g_levels[0] = g_menu;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
    g_levels[3] = g_levelC;
    g_levels[4] = g_levelD;
    
    // Start at level X
    switch_to_scene(g_levels[0]);
    
    g_effects = new Effects(g_projection_matrix, g_view_matrix);
    g_effects->start(FADEIN, 1.0f);
    
    // Load the texture during initialization
    pseudoStartScreenTexture = Utility::load_texture("grandpa note.png");
    
    main_font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);
    
    if (g_current_scene == g_levelD){
        std::cout << "Attempting to set onlyLevel_D = 1" << std::endl;
        onlyLevel_D = 1;
    } else{
        std::cout << "Attempting to set onlyLevel_D = 0" << std::endl;
        onlyLevel_D = 0;
    }
    g_shader_program.set_light_position_matrix(g_current_scene->get_state().player->get_position());
    // g_shader_program.set_light_position_matrix(onlyLevel_D);
    
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
                        if (pseudoStartScreenActive /*|| (g_current_scene == g_levelA && g_current_scene->get_state().player->get_position().x >= 15.0f && g_current_scene->get_state().player->get_position().y == -7.0f)*/){
                            Utility::toggle_gamePause(true);
                        }// Freeze game updates
                        else Utility::toggle_gamePause(false);
                        break;
                        
                    case SDLK_j:
                        // Attack
                        g_current_scene->get_state().player->player_attack();
                        break;
                        
                    
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_A] and !(g_current_scene == g_menu))       g_current_scene->get_state().player->move_left();
    else if (key_state[SDL_SCANCODE_D] and !(g_current_scene == g_menu))  g_current_scene->get_state().player->move_right();
    else if (key_state[SDL_SCANCODE_W] and !(g_current_scene == g_menu))  g_current_scene->get_state().player->move_up();
    else if (key_state[SDL_SCANCODE_S] and !(g_current_scene == g_menu))  g_current_scene->get_state().player->move_down();
    //Added for player attack
    else if (key_state[SDL_SCANCODE_J] and !(g_current_scene == g_menu))  g_current_scene->get_state().player->player_attack();
         
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
    
    if (!(g_current_scene == g_menu)) {
        // Calculate the translation needed to follow the player
        float player_x = g_current_scene->get_state().player->get_position().x;
        float player_y = g_current_scene->get_state().player->get_position().y;

        // Set the camera's horizontal position based on player's position with some limits
        float camera_x = -player_x;

        // Define limits for camera movement
        float max_camera_x = g_current_scene->get_state().map->get_width() / 2.0f - 10.0f;
        float min_camera_x = -g_current_scene->get_state().map->get_width() / 2.0f + 10.0f;

        // Clamp the camera's x position to stay within defined bounds
        camera_x = glm::clamp(camera_x, min_camera_x, max_camera_x);

        // Set the camera's vertical position to follow the player, with limits to avoid showing outside of the level
        float camera_y = -player_y;

        // Update vertical limits to allow the camera to go lower if needed
        //float max_camera_y = g_current_scene->get_state().map->get_height() / 2.0f - 10.0f;
        float max_camera_y = 15.0f;
        // float min_camera_y = -g_current_scene->get_state().map->get_height() / 2.0f + 10.0f;
        float min_camera_y = -25.0f;

        camera_y = glm::clamp(camera_y, min_camera_y, max_camera_y);

        // Translate the view matrix based on player's current position
        g_view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(camera_x, camera_y, 0.0f));
    }
    
    if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, 3.75, 0));
    } else {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
    }
    
    
    if (g_current_scene == g_levelA && g_current_scene->get_state().player->get_position().x >= 3.85f && g_current_scene->get_state().player->get_position().y == -5.0f){
//        std::cout << "Attempting to switch from Level A to Level B." << std::endl;
        switch_to_scene(g_levelB);
        g_effects->start(FADEIN, 1.0f);
    }
    
    if (g_current_scene == g_levelB && g_current_scene->get_state().player->get_position().x > 6.0f && g_current_scene->get_state().player->get_position().y == -21.0f) {
//        std::cout << "Attempting to switch from Level B to Level C." << std::endl;
        switch_to_scene(g_levelC);
        g_effects->start(FADEIN, 1.0f);
    }
    
    if (g_current_scene == g_levelC && g_current_scene->get_state().player->get_position().x > 7.9f && g_current_scene->get_state().player->get_position().y == -14.0f) {
//        std::cout << "Attempting to switch from Level B to Level C." << std::endl;
        switch_to_scene(g_levelD);
        g_effects->start(FADEIN, 1.0f);
    }
    
    if (g_current_scene == g_levelA && g_current_scene->get_state().player->get_position().x >= 14.5f && g_current_scene->get_state().player->get_position().y == -7.0f){
        pseudoStartScreenActive = true;
    }
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    

    if (g_current_scene == g_levelD){
        g_shader_program.set_is_level_d(true);
        // g_shader_program.set_light_position_matrix(g_current_scene->get_state().player->get_position());
    }else{
        g_shader_program.set_is_level_d(false);
        // g_shader_program.set_light_position_matrix(g_current_scene->get_state().player->get_position());
    }
    // std::cout << "only_LevelD = " << onlyLevel_D << std::endl;
    g_shader_program.set_light_position_matrix(g_current_scene->get_state().player->get_position());
    
    if (pseudoStartScreenActive && g_current_scene == g_levelA) {
        // Set up the shader for full-screen texture rendering
        g_shader_program.set_view_matrix(glm::mat4(1.0f));
        g_shader_program.set_model_matrix(glm::mat4(1.0f));

        glBindTexture(GL_TEXTURE_2D, pseudoStartScreenTexture);
        Utility::draw_texture(&g_shader_program, pseudoStartScreenTexture, -7.5f,0.0f, 15.0f, 8.0f);
    }
    else if (pseudoStartScreenActive) {
        // Set up the shader for full-screen texture rendering
        g_shader_program.set_view_matrix(glm::mat4(1.0f));
        g_shader_program.set_model_matrix(glm::mat4(1.0f));

        glBindTexture(GL_TEXTURE_2D, pseudoStartScreenTexture);
        Utility::draw_texture(&g_shader_program, pseudoStartScreenTexture, -7.5f,-4.0f, 15.0f, 8.0f);
    }
    else {
        // Set the view matrix for all levels
        g_shader_program.set_view_matrix(g_view_matrix);
        // Render the current scene
        g_current_scene->render(&g_shader_program);
    }
    // Render any effects after the main scene
    g_effects->render();
    // Swap the buffers to display the scene
    SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
    SDL_Quit();
    
    delete g_menu;
    delete g_levelA;
    delete g_levelB;
    delete g_levelC;
    delete g_levelD;
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
