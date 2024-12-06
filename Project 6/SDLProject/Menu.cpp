#include "Menu.h"
#include "Utility.h"

#define LEVEL_WIDTH 17
#define LEVEL_HEIGHT 8

constexpr char SPRITESHEET_FILEPATH[] = "Soldier.png",
               FONTSHEET_FILEPATH[] = "font1.png",
               ENEMY_FILEPATH[]       = "desert zombie.png",
               BACKGROUND_FILEPATH[]  = "background.png";

GLuint g_background_texture_id;
GLuint font_texture_id;
glm::mat4 g_background_matrix;

std::string gameName_text = "Skull Cavern: Diamond Quest";
std::string enter_text = "Press enter to start";

unsigned int MENU_DATA[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

Menu::~Menu()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
}

void Menu::initialise()
{
    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("level1_tileset.png");
    
    g_background_texture_id = Utility::load_texture(BACKGROUND_FILEPATH);  // Load the background texture
    g_background_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
    
    font_texture_id = Utility::load_texture(FONTSHEET_FILEPATH);

    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, MENU_DATA, map_texture_id, 1.0f, 4, 1, 0);
    
    int player_walking_animation[5][6] =
    {
        {0,1,2,3,4,5}, // move left
        {0,1,2,3,4,5}, // move right
        {0,1,2,3,4,5}, // move up
        {0,1,2,3,4,5}, // move down
        {16,17,16,17,16,17}   // attack
    };

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    
    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        6,                         // animation frame amount
        0,                         // current animation index
        6,                         // animation column amount
        4,                         // animation row amount
        0.7f,                      // width
        1.0f,                      // height
        PLAYER
    );
    
    m_game_state.player->set_position(glm::vec3(9.0f, -6.0f, 0.0f));
        
    m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(3.0f);
    
    /**
    Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
    m_game_state.enemies[i] =  Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
    }

    m_game_state.enemies[0].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
}

void Menu::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    
    if (m_game_state.player->get_position().y < -10.0f) m_game_state.next_scene_id = 1;
}

void Menu::render(ShaderProgram *program)
{
    Utility::draw_text(program, font_texture_id, gameName_text, 1.3f, -0.7f, glm::vec3(-2.5f, -1.5f, 0.0f));
    Utility::draw_text(program, font_texture_id, enter_text, 0.7f, -0.15f, glm::vec3(-0.3f, -7.0f, 0.0f));
}
