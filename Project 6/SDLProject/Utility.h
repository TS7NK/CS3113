#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

class Utility {
public:
    // ————— METHODS ————— //
    static GLuint load_texture(const char* filepath);
    static void draw_text(ShaderProgram *program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position);
    static void draw_texture(ShaderProgram *program, GLuint textureID, float x, float y, float width, float height);
    
    static int get_lives();         // method to get player's lives
    static void update_lives();     // method to update player's lives
    
    static int get_diamonds();       // method to get diamonds
    static void update_diamonds();   // method to update diamonds
    
    static bool get_gamePaused();
    static void toggle_gamePause(bool state);
    
private:
    static int lives;
    static int diamonds;             // New variable to track diamonds
    
};
