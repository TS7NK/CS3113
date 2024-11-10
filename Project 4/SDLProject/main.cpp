/**
* Author: Tingting Min
* Assignment: Rise of the AI
* Date due: 2024-11-9, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

/**
 Requirement 1: 3 AI With Different Behaviors (60%)
 There needs to be 3 or more enemies with basic AI. (20% each)
 Each AI should have a different behavior. For instance, one could me walking, another patrolling, another jumping or shooting. All AI must be doing something other than standing around.
 It is OK to use the same image/textures for all 3 of your AI.
 Note on the level's layout:
 Your AI must not all be on one (flat) floor. You will lose points if all 3 of your AI are on the same floor.
 At least 1 of your AI must be on a platform.
 
 Requirement 2: Defeat Enemies (10%)
 The player must be able to get rid of each individual enemy one at a time (i.e. jumping).
 Enemies that are gone should not be seen anymore or able to be "collidable".
 
 Requirement 3: 'You Lose' Message (10%)
 If the player touches an enemy, gets shot, etc., show text the text “You Lose”. Each letter must be rendered independently using the text function we wrote in class.
 
 Requirement 4: 'You Win' Message (10%)
 If the player gets rid of all enemies, show text the text “You Win”. Each letter must be rendered independently using the text function we wrote in class.
  
 Extra credit:
 Implement another way for an enemy to kill the player, or for the player to kill an enemy. This does not necessarily have to be shooting–you can definitely get as creative as you like.
 */

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define ENEMY_COUNT 3
#define LEVEL1_WIDTH 17
#define LEVEL1_HEIGHT 7

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

// #include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"

// ————— GAME STATE ————— //
struct GameState
{
    Entity *player;
    Entity *enemies;
    
    Map *map;
};

enum AppStatus { RUNNING, TERMINATED };

// ————— CONSTANTS ————— //
//constexpr int WINDOW_WIDTH  = 640 * 2,
//          WINDOW_HEIGHT = 400 * 2;

constexpr int WINDOW_WIDTH  = 600 * 2,
          WINDOW_HEIGHT = 350 * 2;

constexpr float BG_RED     = 0.9765625f,
                BG_GREEN   = 0.97265625f,
                BG_BLUE    = 0.9609375f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char GAME_WINDOW_NAME[] = "Hello, Maps!";

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr char SPRITESHEET_FILEPATH[] = "character.png",
               MAP_TILESET_FILEPATH[] = "TX Tileset Ground.png",
               BKG_FILEPATH[] = "background.png",
               ENEMY_FILEPATH[] = "skeleton.png",
               FONT_FILEPATH[] = "font1.png";

constexpr int FONTBANK_SIZE = 16;
std::string winning_message = "You win!";
std::string losing_message = "You lose!";
bool is_game_over = false;

constexpr int NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL  = 0;
constexpr GLint TEXTURE_BORDER   = 0;

int remaining_enemies = ENEMY_COUNT;

unsigned int LEVEL_1_DATA[] =
{
    0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3,
    0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7,
    0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// ————— VARIABLES ————— //
GameState g_game_state;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix, g_background_matrix;

GLuint g_background_texture_id;
GLuint g_font_texture_id;

float g_previous_ticks = 0.0f,
      g_accumulator    = 0.0f;


void initialise();
void process_input();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath);
void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id);

// ————— GENERAL FUNCTIONS ————— //
GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    
    g_background_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 7.5f, 1.0f));
    
    GLuint texture_id;
    glGenTextures(NUMBER_OF_TEXTURES, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    stbi_image_free(image);
    
    return texture_id;
}

void draw_text(ShaderProgram *shader_program, GLuint font_texture_id, std::string text,
              float font_size, float spacing, glm::vec3 position)
{
   float width = 1.0f / FONTBANK_SIZE;
   float height = 1.0f / FONTBANK_SIZE;

   std::vector<float> vertices;
   std::vector<float> texture_coordinates;

   // For every character...
   for (int i = 0; i < text.size(); i++) {
       // 1. Get their index in the spritesheet, as well as their offset (i.e. their
       //    position relative to the whole sentence)
       int spritesheet_index = (int) text[i];  // ascii value of character
       float offset = (font_size + spacing) * i;

       // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
       float u_coordinate = (float) (spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
       float v_coordinate = (float) (spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

       // 3. Inset the current pair in both vectors
       vertices.insert(vertices.end(), {
           offset + (-0.5f * font_size), 0.5f * font_size,
           offset + (-0.5f * font_size), -0.5f * font_size,
           offset + (0.5f * font_size), 0.5f * font_size,
           offset + (0.5f * font_size), -0.5f * font_size,
           offset + (0.5f * font_size), 0.5f * font_size,
           offset + (-0.5f * font_size), -0.5f * font_size,
       });

       texture_coordinates.insert(texture_coordinates.end(), {
           u_coordinate, v_coordinate,
           u_coordinate, v_coordinate + height,
           u_coordinate + width, v_coordinate,
           u_coordinate + width, v_coordinate + height,
           u_coordinate + width, v_coordinate,
           u_coordinate, v_coordinate + height,
       });
   }

   // 4. And render all of them using the pairs
   glm::mat4 model_matrix = glm::mat4(1.0f);
   model_matrix = glm::translate(model_matrix, position);

   shader_program->set_model_matrix(model_matrix);
   glUseProgram(shader_program->get_program_id());

   glVertexAttribPointer(shader_program->get_position_attribute(), 2, GL_FLOAT, false, 0,
                         vertices.data());
   glEnableVertexAttribArray(shader_program->get_position_attribute());

   glVertexAttribPointer(shader_program->get_tex_coordinate_attribute(), 2, GL_FLOAT,
                         false, 0, texture_coordinates.data());
   glEnableVertexAttribArray(shader_program->get_tex_coordinate_attribute());

   glBindTexture(GL_TEXTURE_2D, font_texture_id);
   glDrawArrays(GL_TRIANGLES, 0, (int) (text.size() * 6));

   glDisableVertexAttribArray(shader_program->get_position_attribute());
   glDisableVertexAttribArray(shader_program->get_tex_coordinate_attribute());
}

void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void initialise()
{
    // ————— GENERAL ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow(GAME_WINDOW_NAME,
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        LOG("ERROR: Could not create OpenGL context.\n");
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— VIDEO SETUP ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-7.0f, 7.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    // ————— MAP SET-UP ————— //
    GLuint map_texture_id = load_texture(MAP_TILESET_FILEPATH);
    g_game_state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, LEVEL_1_DATA, map_texture_id, 1.0f, 4, 2);
    
    // ————— ENEMY SET-UP ————— //
    GLuint enemy_texture_id = load_texture(ENEMY_FILEPATH);
    g_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++){
        g_game_state.enemies[i] =  Entity(enemy_texture_id, 0.8f, 0.5f, 1.0f, ENEMY);
        if (i == 0){
            // Enemy[0] is guarding
            g_game_state.enemies[0].set_position(glm::vec3(5.8f, 3.0f, 0.0f));
            g_game_state.enemies[0].set_movement(glm::vec3(0.0f));
            g_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
            g_game_state.enemies[0].set_entity_type(ENEMY);
            g_game_state.enemies[0].set_ai_type(WALKER);
            g_game_state.enemies[0].set_ai_state(WALKING);
        }
        if (i == 1){
            // Enemy[1] is following
            g_game_state.enemies[1].set_position(glm::vec3(12.0f, 3.5f, 0.0f));
            g_game_state.enemies[1].set_movement(glm::vec3(0.0f));
            g_game_state.enemies[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
            g_game_state.enemies[1].set_entity_type(ENEMY);
            g_game_state.enemies[1].set_ai_type(GUARD);
            g_game_state.enemies[1].set_ai_state(IDLE);
        }
        if (i == 2){
            // Enemy[2] is attacking
            g_game_state.enemies[2].set_position(glm::vec3(6.3f, -1.0f, 0.0f));
            g_game_state.enemies[2].set_movement(glm::vec3(0.0f));
            g_game_state.enemies[2].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
            g_game_state.enemies[2].set_entity_type(ENEMY);
            g_game_state.enemies[2].set_ai_type(JUMPER);
            g_game_state.enemies[2].set_ai_state(JUMPING);
        }
    }
    
    // ————— GEORGE SET-UP ————— //
    GLuint player_texture_id = load_texture(SPRITESHEET_FILEPATH);

    int player_walking_animation[4][4] =
    {
        { 8, 9, 10, 11 },   // for George to move to the left,
        { 12, 13, 14, 15 }, // for George to move to the right,
        { 4, 5, 6, 7 },     // for George to move upwards,
        { 0, 1, 2, 3}       // for George to move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f,-4.905f, 0.0f);
    g_font_texture_id = load_texture(FONT_FILEPATH);

    g_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        3.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        0.5f,                      // width
        1.0f,                      // height
        PLAYER
    );
    
    // ————— BACKGROUND ————— //
    g_background_texture_id = load_texture(BKG_FILEPATH);

    // Jumping
    g_game_state.player->set_jumping_power(4.0f);
    
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_game_state.player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
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
                        
                    case SDLK_SPACE:
                        // Jump
                        if (g_game_state.player->get_collided_bottom())
                        {
                            g_game_state.player->jump();
                        }
                        break;
                    
                    default:
                        break;
                }
                
            default:
                break;
        }
        
        if(is_game_over){
            g_app_status = TERMINATED;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_A])       g_game_state.player->move_left();
    else if (key_state[SDL_SCANCODE_D]) g_game_state.player->move_right();
         
    if (glm::length(g_game_state.player->get_movement()) > 1.0f){
        g_game_state.player->normalise_movement();
    }
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP)
    {
        g_game_state.player->update(FIXED_TIMESTEP, g_game_state.player, NULL, 0, g_game_state.map);
        // Perform horizontal and vertical collision checks separately
        g_game_state.player->check_collision_x(g_game_state.enemies, ENEMY_COUNT);
        g_game_state.player->check_collision_y(g_game_state.enemies, ENEMY_COUNT);
        for (int i = 0; i < ENEMY_COUNT; i++) {
            if (g_game_state.enemies[i].is_active()) {
                    g_game_state.enemies[i].update(FIXED_TIMESTEP, g_game_state.player, g_game_state.enemies, ENEMY_COUNT, g_game_state.map);
            }
        }
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    g_view_matrix = glm::mat4(1.0f);
    
    // Camera Follows the player
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_game_state.player->get_position().x - 5.5f, 1.0f, 0.0f));
    
    if (g_game_state.player->get_position().y < -5.0f){
        is_game_over = true;
    }
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        // if (!g_game_state.enemies[i].is_active()) continue;
        std::cout << "Enemy " << i << " is active = " <<g_game_state.enemies[i].is_active()<<std::endl;
        std::cout << "Player is collide with enemy = " <<g_game_state.player->check_collision(&g_game_state.enemies[i])<<std::endl;
        std::cout << "Player is collide on the right = " <<g_game_state.player->get_collided_right() <<std::endl;
        std::cout << "Player is collide on the left = " <<g_game_state.player->get_collided_left() <<std::endl;
        
        if (g_game_state.player->check_collision(&g_game_state.enemies[i])) {
            std::cout << "Collision detected with enemy " << i << std::endl;
            if(g_game_state.player->get_collided_right()){
                is_game_over = true;
                std::cout << "Player loses; remaining enemies = " << remaining_enemies << std::endl;
                break;
            }
            
            if (g_game_state.player->get_collided_bottom()) {
                g_game_state.enemies[i].deactivate();
                remaining_enemies -= 1;
                std::cout << "Enemy defeated; remaining enemies = " << remaining_enemies << std::endl;
                }
            }
        }
    
    int active_enemy_count = 0;
        for (int i = 0; i < ENEMY_COUNT; i++) {
            if (g_game_state.enemies[i].is_active()) {
                active_enemy_count++;
            }
        }
    remaining_enemies = active_enemy_count;
    
    // Check game over conditions
    if (remaining_enemies == 0) {
        is_game_over = true;
    }
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    // draw_object(g_background_matrix, g_background_texture_id);
    
    g_shader_program.set_view_matrix(g_view_matrix);
    g_game_state.player->render(&g_shader_program);
    g_game_state.map->render(&g_shader_program);
    
    // Render all active enemies, including those with left or right collisions
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (g_game_state.enemies[i].is_active()) {
            g_game_state.enemies[i].render(&g_shader_program);
        }
    }
        
    // Check game-over or win message conditions here if needed
    if (is_game_over) {
        glm::vec3 text_position = glm::vec3(g_game_state.player->get_position().x + 1.0f, 1.0f, 0.0f);
        if (remaining_enemies == 0) {
            draw_text(&g_shader_program, g_font_texture_id, winning_message, 0.9f, 0.1f, text_position);
        } else {
            draw_text(&g_shader_program, g_font_texture_id, losing_message, 0.9f, 0.1f, text_position);
        }
    }
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    delete [] g_game_state.enemies;
    delete    g_game_state.player;
    delete    g_game_state.map;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
