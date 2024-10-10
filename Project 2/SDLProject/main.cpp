/**
* Author: Tingting Min
* Assignment: Pong Clone
* Date due: 2023-10-12, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

/**
Required:
    1. Fins two images for two players and the background ✅
    2. Set boundary for top and bottom side ✅
    3. put two objects on each side ✅
    4. Have one object switch from manual to automatic control when pressed t ✅
    5. have the star moving ✅
    6. have the star bounce off the wall and two paddles ✅
    7. when the star pass the left or right wall, the other party win ✅
Extra credits:
    1. have a theme -> Feeding Frenzy theme  ✅
    2. have 2 players score displayed on the tag ✅
    3. have 1-3 stars when user press different number ✅
    4. have the logic if the star passing the paddle, the opposite side win 1 point, max 3 points to decide who win ✅
 Questions:
 1. Should the user press 1 - 3 to start the game or there should be default start 1
 2. It says if the object pass the left or right wall then the other party win, but if I choose to do 3 points win does that gonna affect the grading?
 3. Anything I need to change before I submit?s
 */

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
    #include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <ctime>
#include "cmath"

enum AppStatus { RUNNING, TERMINATED };

constexpr int WINDOW_WIDTH  = 640 * 2,
              WINDOW_HEIGHT = 480 * 2;

constexpr float BG_RED     = 0.9765625f,
                BG_GREEN   = 0.97265625f,
                BG_BLUE    = 0.9609375f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X      = 0,
              VIEWPORT_Y      = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0f;

constexpr GLint NUMBER_OF_TEXTURES = 1,
                LEVEL_OF_DETAIL    = 0,
                TEXTURE_BORDER     = 0;

int player1 = 0;
int player2 = 0;
bool game_active = true;
std::string win_message = "";

int numOfStars;

constexpr char BACKGROUND_SPRITE_FILEPATH[] = "background.png";
constexpr char SHARK_SPRITE_FILEPATH[] = "shark.png";
constexpr char ORCA_SPRITE_FILEPATH[] = "orca.png";
constexpr char STAR_SPRITE_FILEPATH[] = "star.png";
constexpr char TAG_SPRITE_FILEPATH[] = "tag.png";
constexpr char FONTSHEET_FILEPATH[]   = "font1.png";

constexpr int FONTBANK_SIZE = 16;
void draw_text(ShaderProgram *shader_program, GLuint font_texture_id, std::string text,
              float font_size, float spacing, glm::vec3 position);

constexpr glm::vec3 SHARK_INIT_SCALE = glm::vec3(0.8f, 2.0f, 0.0f);
constexpr glm::vec3 ORCA_INIT_SCALE = glm::vec3(0.8f, 3.0f, 0.0f);
constexpr glm::vec3 STAR_INIT_SCALE = glm::vec3(0.5f, 0.5f, 0.0f);
constexpr glm::vec3 TAG1_INIT_SCALE = glm::vec3(1.5f, 1.0f, 0.0f);
constexpr glm::vec3 TAG2_INIT_SCALE = glm::vec3(1.5f, 1.0f, 0.0f);

// Constants for dimensions
constexpr float SHARK_WIDTH = 0.5f;
constexpr float SHARK_HEIGHT = 2.3985f;
constexpr float ORCA_WIDTH = 0.5f;
constexpr float ORCA_HEIGHT = 2.3985f;
constexpr float STAR_WIDTH = 0.5f;
constexpr float STAR_HEIGHT = 0.5f;

GLuint g_background_texture_id;
GLuint g_shark_texture_id;
GLuint g_orca_texture_id;
GLuint g_star1_texture_id;
GLuint g_star2_texture_id;
GLuint g_star3_texture_id;
GLuint g_tag1_texture_id;
GLuint g_tag2_texture_id;
GLuint g_font_texture_id;

SDL_Window* g_display_window = nullptr;
AppStatus g_app_status = RUNNING;
bool is_single_player = false;

ShaderProgram g_shader_program = ShaderProgram();

glm::mat4 g_view_matrix,
          g_projection_matrix,
          g_shark_matrix,
          g_orca_matrix,
          g_star1_matrix,
          g_star2_matrix,
          g_star3_matrix,
          g_tag1_matrix,
          g_tag2_matrix,
          g_background_matrix;

float g_previous_ticks = 0.0f;

glm::vec3 g_shark_position = glm::vec3(4.0f, 0.0f, 0.0f);
glm::vec3 g_orca_position = glm::vec3(-4.0f, 0.0f, 0.0f);
glm::vec3 g_tag1_position = glm::vec3(-1.0f, 3.3f, 0.0f);
glm::vec3 g_tag2_position = glm::vec3(1.0f, 3.3f, 0.0f);

// define three star positions and velocities
glm::vec3 g_star1_position = glm::vec3(0.5f, 0.5f, 0.0f);
glm::vec3 g_star2_position = glm::vec3(-0.5f, 1.2f, 0.0f);
glm::vec3 g_star3_position = glm::vec3(0.5f, -1.2f, 0.0f);

glm::vec2 g_star1_velocity = glm::vec2(1.0f, 0.5f);
glm::vec2 g_star2_velocity = glm::vec2(1.0f, -0.5f);
glm::vec2 g_star3_velocity = glm::vec2(-1.0f, 0.5f);

bool g_star1_active = false;
bool g_star2_active = false;
bool g_star3_active = false;

glm::vec3 g_shark_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_orca_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_star1_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_star2_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_star3_movement = glm::vec3(0.0f, 0.0f, 0.0f);


float g_shark_speed = 5.0f;
float g_orca_speed = 5.0f;
float g_star_speed = 2.0f;


void initialise();
void process_input();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath);
void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id);


void draw_text(ShaderProgram *shader_program, GLuint font_texture_id, std::string text,
              float font_size, float spacing, glm::vec3 position)
{
   // Scale the size of the fontbank in the UV-plane
   // We will use this for spacing and positioning
   float width = 1.0f / FONTBANK_SIZE;
   float height = 1.0f / FONTBANK_SIZE;

   // Instead of having a single pair of arrays, we'll have a series of pairs—one for
   // each character. Don't forget to include <vector>!
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


GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void update_score(std::string player_name) {
    if (player_name == "player1") {
        player1++;
    } else if (player_name == "player2") {
        player2++;
    }

    // check win condition after updating scores
    if (player1 >= 3) {
        game_active = false;
        win_message = "Left Player Win!";
    } else if (player2 >= 3) {
        game_active = false;
        win_message = "Right Player Win!";
    }
}

void reset_star_position(glm::vec3 &star_position, glm::vec2 &star_velocity) {
    // reset position to the center and randomize velocity direction
    star_position = glm::vec3(0.0f, 0.0f, 0.0f);
    star_velocity = glm::vec2((rand() % 2) * 2 - 1, (rand() % 2) * 2 - 1) * glm::vec2(1.0f, 1.0f);
}

void check_and_update_score(glm::vec3 &star_position, glm::vec2 &star_velocity) {
    if (star_position.x + STAR_WIDTH / 2 >= 5.5f) {
        update_score("player1");
        reset_star_position(star_position, star_velocity);
    } else if (star_position.x - STAR_WIDTH / 2 <= -5.5f) {
        update_score("player2");
        reset_star_position(star_position, star_velocity);
    }
}


void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Hello, Pong Clone!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr)
    {
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_shark_texture_id = load_texture(SHARK_SPRITE_FILEPATH);
    g_orca_texture_id = load_texture(ORCA_SPRITE_FILEPATH);
    g_star1_texture_id = load_texture(STAR_SPRITE_FILEPATH);
    g_star2_texture_id = load_texture(STAR_SPRITE_FILEPATH);
    g_star3_texture_id = load_texture(STAR_SPRITE_FILEPATH);
    g_tag1_texture_id = load_texture(TAG_SPRITE_FILEPATH);
    g_tag2_texture_id = load_texture(TAG_SPRITE_FILEPATH);
    g_background_texture_id = load_texture(BACKGROUND_SPRITE_FILEPATH);  // Load the background texture
    
    g_shark_matrix     = glm::mat4(1.0f);
    g_orca_matrix     = glm::mat4(1.0f);
    g_star1_matrix     = glm::mat4(1.0f);
    g_star2_matrix     = glm::mat4(1.0f);
    g_star3_matrix     = glm::mat4(1.0f);
    g_view_matrix       = glm::mat4(1.0f);
    g_tag1_matrix       = glm::mat4(1.0f);
    g_tag2_matrix       = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    g_background_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 7.5f, 1.0f));

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    g_font_texture_id = load_texture(FONTSHEET_FILEPATH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_shark_movement = glm::vec3(0.0f);
    g_orca_movement = glm::vec3(0.0f);
    g_star1_movement = glm::vec3(0.0f);
    g_star2_movement = glm::vec3(0.0f);
    g_star3_movement = glm::vec3(0.0f);
    

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_t:  // single-player mode when `t` is pressed
                        is_single_player = !is_single_player;
                        if (!is_single_player){
                            g_orca_speed = fabs(g_orca_speed);
                        }
                        break;
                        
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_app_status = TERMINATED;
                        break;
                        
                    // if number 1 - 3 pressed, generate different numbers of stars
                    case SDLK_1:
                        g_star1_active = true;
                        g_star2_active = false;
                        g_star3_active = false;
                        break;
                        
                    case SDLK_2:
                        g_star1_active = true;
                        g_star2_active = true;
                        g_star3_active = false;
                        break;
                        
                    case SDLK_3:
                        g_star1_active = true;
                        g_star2_active = true;
                        g_star3_active = true;
                        break;
                        
                    default:
                        break;
                }
            default:
                break;
        }
    }
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_UP] && (g_shark_position.y < 2.7f))
    {
        g_shark_movement.y = 1.0f;
    }
    else if (key_state[SDL_SCANCODE_DOWN] && (g_shark_position.y > -2.7f))
    {
        g_shark_movement.y = -1.0f;
    }

    // Orca movement (Player 2 or AI)
        if (!is_single_player)
        {
            // If in two-player mode, allow movement using W and S for orca
            if (key_state[SDL_SCANCODE_W] && (g_orca_position.y < 2.9f))
            {
                g_orca_movement.y = 1.0f;
            }
            else if (key_state[SDL_SCANCODE_S] && (g_orca_position.y > -2.9f))
            {
                g_orca_movement.y = -1.0f;
            }
        }

    // This makes sure that the player can't "cheat" their way into moving faster
    if (glm::length(g_shark_movement) > 1.0f)
    {
        g_shark_movement = glm::normalize(g_shark_movement);
    }
    
    if (glm::length(g_orca_movement) > 1.0f)
    {
        g_orca_movement = glm::normalize(g_orca_movement);
    }
}


void update()
{
    if (!game_active) return;
    /* DELTA TIME */
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    g_shark_position += g_shark_movement * g_shark_speed * delta_time;
    
    if (is_single_player)
        {
            // Automatic movement for orca in single-player mode
            if (g_orca_position.y >= 2.79f)
            {
                g_orca_speed = -fabs(g_orca_speed);  // Reverse to move downward
            }
            else if (g_orca_position.y <= -3.0f)
            {
                g_orca_speed = fabs(g_orca_speed);   // Reverse to move upward
            }
            g_orca_position.y += g_orca_speed * delta_time;
        }
        else
        {
            // Update player-controlled orca movement when in two-player mode
            g_orca_position += g_orca_movement * g_orca_speed * delta_time;
        }
    
    /* GAME LOGIC */
    // Update positions based on active flags
    if (g_star1_active) g_star1_position += glm::vec3(g_star1_velocity, 0.0f) * g_star_speed * delta_time;
    if (g_star2_active) g_star2_position += glm::vec3(g_star2_velocity, 0.0f) * g_star_speed * delta_time;
    if (g_star3_active) g_star3_position += glm::vec3(g_star3_velocity, 0.0f) * g_star_speed * delta_time;

    // Collisions with top and bottom walls
    if (g_star1_position.y + STAR_HEIGHT / 2 >= 3.75f || g_star1_position.y - STAR_HEIGHT / 2 <= -3.75f) {
        g_star1_velocity.y = -g_star1_velocity.y;
    }
    if (g_star2_position.y + STAR_HEIGHT / 2 >= 3.75f || g_star2_position.y - STAR_HEIGHT / 2 <= -3.75f) {
        g_star2_velocity.y = -g_star2_velocity.y;
    }
    if (g_star3_position.y + STAR_HEIGHT / 2 >= 3.75f || g_star3_position.y - STAR_HEIGHT / 2 <= -3.75f) {
        g_star3_velocity.y = -g_star3_velocity.y;
    }

    // Collision with Shark paddle
    float star1_x_distance = fabs(g_star1_position.x - g_shark_position.x) - (STAR_WIDTH + SHARK_WIDTH) / 2;
    float star1_y_distance = fabs(g_star1_position.y - g_shark_position.y) - (STAR_HEIGHT + SHARK_HEIGHT) / 2;
    float star2_x_distance = fabs(g_star2_position.x - g_shark_position.x) - (STAR_WIDTH + SHARK_WIDTH) / 2;
    float star2_y_distance = fabs(g_star2_position.y - g_shark_position.y) - (STAR_HEIGHT + SHARK_HEIGHT) / 2;
    float star3_x_distance = fabs(g_star3_position.x - g_shark_position.x) - (STAR_WIDTH + SHARK_WIDTH) / 2;
    float star3_y_distance = fabs(g_star3_position.y - g_shark_position.y) - (STAR_HEIGHT + SHARK_HEIGHT) / 2;

    if (star1_x_distance < 0 && star1_y_distance < 0) {
        g_star1_velocity.x = -g_star1_velocity.x; // Bounce back horizontally
    }
    if (star2_x_distance < 0 && star2_y_distance < 0) {
        g_star2_velocity.x = -g_star2_velocity.x; // Bounce back horizontally
    }
    if (star3_x_distance < 0 && star3_y_distance < 0) {
        g_star3_velocity.x = -g_star3_velocity.x; // Bounce back horizontally
    }

    // Collision with Orca paddle
    star1_x_distance = fabs(g_star1_position.x - g_orca_position.x) - (STAR_WIDTH + ORCA_WIDTH) / 2;
    star1_y_distance = fabs(g_star1_position.y - g_orca_position.y) - (STAR_HEIGHT + ORCA_HEIGHT)/2;
    star2_x_distance = fabs(g_star2_position.x - g_orca_position.x) - (STAR_WIDTH + ORCA_WIDTH) / 2;
    star2_y_distance = fabs(g_star2_position.y - g_orca_position.y) - (STAR_HEIGHT + ORCA_HEIGHT)/2;
    star3_x_distance = fabs(g_star3_position.x - g_orca_position.x) - (STAR_WIDTH + ORCA_WIDTH) / 2;
    star3_y_distance = fabs(g_star3_position.y - g_orca_position.y) - (STAR_HEIGHT + ORCA_HEIGHT)/2;

    if (star1_x_distance < 0 && star1_y_distance < 0) {
        g_star1_velocity.x = -g_star1_velocity.x; // Bounce back horizontally
    }
    if (star2_x_distance < 0 && star2_y_distance < 0) {
        g_star2_velocity.x = -g_star2_velocity.x; // Bounce back horizontally
    }
    if (star3_x_distance < 0 && star3_y_distance < 0) {
        g_star3_velocity.x = -g_star3_velocity.x; // Bounce back horizontally
    }
    
    /* TRANSFORMATIONS */
    g_shark_matrix = glm::mat4(1.0f);
    g_orca_matrix = glm::mat4(1.0f);
    g_star1_matrix = glm::mat4(1.0f);
    g_star2_matrix = glm::mat4(1.0f);
    g_star3_matrix = glm::mat4(1.0f);
    g_tag1_matrix = glm::mat4(1.0f);
    g_tag2_matrix = glm::mat4(1.0f);
    
    g_shark_matrix = glm::translate(g_shark_matrix, g_shark_position);
    g_shark_matrix = glm::scale(g_shark_matrix, SHARK_INIT_SCALE);
    
    g_orca_matrix = glm::translate(g_orca_matrix, g_orca_position);
    g_orca_matrix = glm::scale(g_orca_matrix, ORCA_INIT_SCALE);
    
    // Update transformations
    g_star1_matrix = glm::mat4(1.0f);
    g_star1_matrix = glm::translate(g_star1_matrix, g_star1_position);
    g_star1_matrix = glm::scale(g_star1_matrix, STAR_INIT_SCALE);
    
    g_star2_matrix = glm::mat4(1.0f);
    g_star2_matrix = glm::translate(g_star2_matrix, g_star2_position);
    g_star2_matrix = glm::scale(g_star2_matrix, STAR_INIT_SCALE);
    
    g_star3_matrix = glm::mat4(1.0f);
    g_star3_matrix = glm::translate(g_star3_matrix, g_star3_position);
    g_star3_matrix = glm::scale(g_star3_matrix, STAR_INIT_SCALE);
    
    g_tag1_matrix = glm::translate(g_tag1_matrix, g_tag1_position);
    g_tag1_matrix = glm::scale(g_tag1_matrix, TAG1_INIT_SCALE);
    
    g_tag2_matrix = glm::translate(g_tag2_matrix, g_tag2_position);
    g_tag2_matrix = glm::scale(g_tag2_matrix, TAG2_INIT_SCALE);
    
    // Handle boundary passing without stopping the game
    check_and_update_score(g_star1_position, g_star1_velocity);
    check_and_update_score(g_star2_position, g_star2_velocity);
    check_and_update_score(g_star3_position, g_star3_velocity);

}


void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };
    
    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false,
                          0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT,
                          false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    // Draw the background first
    draw_object(g_background_matrix, g_background_texture_id);
    draw_object(g_tag1_matrix, g_tag1_texture_id);
    draw_object(g_tag2_matrix, g_tag2_texture_id);

    // Then draw the shield or other objects
    draw_object(g_shark_matrix, g_shark_texture_id);
    draw_object(g_orca_matrix, g_orca_texture_id);
    
    if (g_star1_active) draw_object(g_star1_matrix, g_star1_texture_id);
    if (g_star2_active) draw_object(g_star2_matrix, g_star2_texture_id);
    if (g_star3_active) draw_object(g_star3_matrix, g_star3_texture_id);

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    // Convert scores to strings and draw them
    std::string player1_score = std::to_string(player1);
    std::string player2_score = std::to_string(player2);
    
    // Display score for two players
    draw_text(&g_shader_program, g_font_texture_id, player1_score, 1.0f, 0.05f, glm::vec3(-1.0f, 3.4f, 0.0f));
    draw_text(&g_shader_program, g_font_texture_id, player2_score, 1.0f, 0.05f, glm::vec3(1.0f, 3.4f, 0.0f));
    // When the player 2 wins
//    draw_text(&g_shader_program, g_font_texture_id, "Player 2 Win!", 0.6f, 0.05f, glm::vec3(-3.5f, 0.5f, 0.0f));
    
    // Display the winning message
    if (game_active == false && win_message != ""){
        draw_text(&g_shader_program, g_font_texture_id, win_message, 0.5f, 0.05f, glm::vec3(-4.0f, 2.0f, 0.0f));
    }

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown() { SDL_Quit(); }


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
