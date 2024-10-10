/**
* Author: Tingting Min
* Assignment: Simple 2D Scene
* Date due: 2023-09-28, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

enum AppStatus { RUNNING, TERMINATED };

constexpr int WINDOW_WIDTH  = 640 * 2,
              WINDOW_HEIGHT = 420 * 2;

// Background color components -> Light yellow
constexpr float BG_RED     = 1.0f,
                BG_BLUE    = 1.0f,
                BG_GREEN   = 0.0f,
                BG_OPACITY = 1.0f;

// Our viewport—or our "camera"'s—position and dimensions
constexpr int VIEWPORT_X      = 0,
              VIEWPORT_Y      = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr GLint NUMBER_OF_TEXTURES = 1,
                LEVEL_OF_DETAIL    = 0,
                TEXTURE_BORDER     = 0;

constexpr char UMARU_SPRITE_FILEPATH[] = "umaru.png",
               COLA_SPRITE_FILEPATH[]  = "cola.png";

constexpr glm::vec3 INIT_SCALE       = glm::vec3(5.0f, 5.98f, 0.0f),
                    INIT_POS_UMARU   = glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_POS_COLA    = glm::vec3(-3.0f, 2.5f, 0.0f);

constexpr float ROT_INCREMENT = 1.0f;

// global variables for Cola's movement and scaling
float g_scale_factor_cola = 1.0f;
bool g_increasing_scale_cola = true;
constexpr float MAX_SCALE = 5.0f;
constexpr float MIN_SCALE = 3.0f;
constexpr float SCALE_STEP = 0.02f;
constexpr float COLA_RADIUS  = 3.0f;
constexpr float COLA_ORBIT_SPEED   = 1.0f;
float cola_angle = 0.0f;
float cola_x_offset = 0.0f,
      cola_y_offset = 0.0f;


// global variables for Umaru's movement and scalig
int g_frame_counter = 0;
float g_previous_ticks = 0;
float g_pulse_time = 0.0f;
constexpr float UMARU_BASE_SCALE = 4.0f,
                UMARU_MAX_AMPLITUDE = 0.1f;
constexpr float UMARU_RADIUS  = 1.0f;
constexpr float UMARU_ORBIT_SPEED   = 1.0f;
float       umaru_angle = 0.0f;
float       umaru_x_offset = 0.0f,
            umaru_y_offset = 0.0f;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();

glm::mat4 g_view_matrix,
          g_umaru_matrix,
          g_cola_matrix,
          g_projection_matrix;

glm::vec3 g_rotation_umaru = glm::vec3(0.0f, 0.0f, 0.0f),
          g_rotation_cola  = glm::vec3(0.0f, 0.0f, 0.0f);

GLuint g_umaru_texture_id,
       g_cola_texture_id;


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


void initialise()
{
    // Initialise video and joystick subsystems
    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("Himouto! Umaru-chan",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr)
    {
        std::cerr << "Error: SDL window could not be created.\n";
        SDL_Quit();
        exit(1);
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_umaru_matrix        = glm::mat4(1.0f);
    g_cola_matrix         = glm::mat4(1.0f);
    g_view_matrix         = glm::mat4(1.0f);
    g_projection_matrix   = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_umaru_texture_id   = load_texture(UMARU_SPRITE_FILEPATH);
    g_cola_texture_id = load_texture(COLA_SPRITE_FILEPATH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
        {
            g_app_status = TERMINATED;
        }
    }
}


void update()
{
    /* Delta time calculations */
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    g_frame_counter++;
    float scale_factor = UMARU_BASE_SCALE + UMARU_MAX_AMPLITUDE * glm::sin(g_pulse_time);

    /* Game logic */
    cola_angle += COLA_ORBIT_SPEED * delta_time;
    umaru_angle += UMARU_ORBIT_SPEED * delta_time;

    /* Model matrix reset */
    g_umaru_matrix = glm::mat4(1.0f);
    g_cola_matrix  = glm::mat4(1.0f);
    
    /* Transformations for Cola*/
    // Calculate new x, y using trigonometry for orbit
    cola_x_offset = COLA_RADIUS * glm::cos(cola_angle);
    cola_y_offset = COLA_RADIUS * glm::sin(cola_angle);
    g_cola_matrix = glm::translate(g_cola_matrix, glm::vec3(cola_x_offset, cola_y_offset, 0.0f));
    g_cola_matrix = glm::scale(g_cola_matrix,
                               glm::vec3(g_scale_factor_cola, g_scale_factor_cola, 1.0f));
    if (g_increasing_scale_cola) {
        g_scale_factor_cola += SCALE_STEP;
        if (g_scale_factor_cola >= MAX_SCALE) {
            g_increasing_scale_cola = false;  // Switch direction
        }
    } else {
        g_scale_factor_cola -= SCALE_STEP;
        if (g_scale_factor_cola <= MIN_SCALE) {
            g_increasing_scale_cola = true;   // Switch direction
        }
    }
    
    /* Transformations for Umaru*/
    // Calculate new x, y using trigonometry for orbit
    umaru_x_offset = UMARU_RADIUS * glm::cos(umaru_angle);
    umaru_y_offset = UMARU_RADIUS * glm::sin(umaru_angle);
    // Reset the model matrix and apply transformations
    g_umaru_matrix = glm::translate(g_umaru_matrix, glm::vec3(umaru_x_offset, umaru_y_offset, 0.0f));
    g_umaru_matrix = glm::scale(g_umaru_matrix, glm::vec3(scale_factor, scale_factor, 1.0f));
}


void draw_object(glm::mat4 &object_g_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_g_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so use 6, not 3
}


void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] =
    {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,
    };

    // Textures
    float texture_coordinates[] =
    {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false,
                          0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT,
                          false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // Bind texture
    draw_object(g_umaru_matrix, g_umaru_texture_id);
    draw_object(g_cola_matrix, g_cola_texture_id);

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
    SDL_Quit();
}


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

