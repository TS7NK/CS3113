#include "Level C.h"
#include "Utility.h"

#define LEVEL_WIDTH  20
#define LEVEL_HEIGHT 11

constexpr char SPRITESHEET_FILEPATH[] = "haley.png",
                l3_FONTSHEET_FILEPATH[] = "font1.png",
                ENEMY_FILEPATH[]       = "skeleton.png";

GLuint l3_font_texture_id;
std::string l3_losing_text = "You Lose!";
std::string l3_wining_text = "You Win!";
std::string l3_instruction = "A(<-) D(->) Space(Jump) I(Info)";
bool l3_game_over = false;
bool l3_player_win = false;

unsigned int LEVELC_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,47, 0,16, 0, 0,73, 0, 0, 0, 0, 0, 0, 0, 0, 0,76,21,13, 13,
    0,56, 0,25, 0, 0, 0, 0, 0, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 13,
    13,12,12,10,12,12,39,39,40, 8, 0, 0, 0, 0,40,40,39,39,39,13,
    13,22,0, 0, 0, 0, 0, 0, 48, 0, 8, 0, 0, 0,17, 48, 0, 0, 0, 11,
    11,48, 0,54,55, 0, 48, 0, 0, 0, 0, 8, 0, 0,26, 0, 0, 57, 0,13,
    13, 0, 0,63,64,0, 0, 0, 48, 0, 0, 0, 36, 36,36,37,36,0, 7,13,
    13,34, 0, 0, 0, 0, 48, 0, 0, 0, 0, 65,66, 0, 0, 0, 0, 7, 0,11,
    11,10,12,24,0, 0, 0, 0, 0, 0, 0, 74,75, 0, 0, 0, 7, 0, 0, 13,
     0, 0, 4,13,13,13, 3, 3,12,12,10,12,12,12,10,12,12,10,12,13,
     0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 4,22
};

LevelC::~LevelC()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
}

void LevelC::initialise()
{
    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("outline_tiles.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 9, 9);
    
    l3_font_texture_id = Utility::load_texture(l3_FONTSHEET_FILEPATH);
    
    int player_walking_animation[4][4] =
    {
        { 12, 13, 14, 15 },  // move to the left,
        { 4,  5,  6,  7 },   // move to the right,
        { 8,  9,  10, 11},   // move upwards,
        { 0,  1,  2,  3 }    // move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    
    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        0.7f,                      // width
        1.0f,                       // height
        PLAYER
    );
        
    m_game_state.player->set_position(glm::vec3(-0.2f, 0.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(4.0f);
    
    /**
    Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    m_game_state.enemies[0] =  Entity(enemy_texture_id, 0.8f, 0.7f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[0].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    
    m_game_state.enemies[1] =  Entity(enemy_texture_id, 0.8f, 0.7f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[1].set_position(glm::vec3(6.0f, -5.0f, 0.0f));
    m_game_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
}

void LevelC::update(float delta_time)
{    
    if(Utility::get_gamePaused() || l3_player_win || l3_game_over) return;
    
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
            if (m_game_state.enemies[i].is_active()) {
                m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
                bool isColliding = m_game_state.player->check_collision(&m_game_state.enemies[i]);

                if (isColliding && !m_game_state.enemies[i].recentlyCollided) {
                    m_game_state.enemies[i].recentlyCollided = true; // Set the flag when first colliding
                    if (m_game_state.player->get_collided_right() || m_game_state.player->get_collided_left()) {
                        Utility::update_lives();
                        std::cout << "Player hit by enemy from side; current lives = " << Utility::get_lives() << std::endl;
                    } else if (m_game_state.player->get_collided_bottom()) {
                        m_game_state.enemies[i].deactivate();
                        std::cout << "Enemy defeated from above; enemy " << i << " deactivated." << std::endl;
                    }
                } else if (!isColliding) {
                    m_game_state.enemies[i].recentlyCollided = false; // Reset the flag when not colliding
                }
            }
        }

    if (Utility::get_lives() <= 0){
        l3_game_over = true;
    }
    
    if ((m_game_state.player->get_position().x == 1.85f && m_game_state.player->get_position().y == -7.0f) and Utility::get_lives()!= 0){
        l3_player_win = true;
    }
}

void LevelC::render(ShaderProgram *program)
{
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    // Render all active enemies, including those with left or right collisions
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (m_game_state.enemies[i].is_active()) {
            m_game_state.enemies[i].render(program);
        }
    }
    
    std::string l3_lives_text = "Lives: " + std::to_string(Utility::get_lives());
    float player_x = m_game_state.player->get_position().x;
    const float minX = 0.0f;
    float text_x = std::max(player_x - 5.0f, minX);
    
    Utility::draw_text(program, l3_font_texture_id, l3_lives_text, 1.0f, -0.5f, glm::vec3(text_x, 0.5f, 0.0f));
    
    Utility::draw_text(program, l3_font_texture_id, l3_instruction, 0.9f, -0.5f, glm::vec3(text_x+ 6.5f, 0.5f, 0.0f));
    
    if (l3_game_over) {
        Utility::draw_text(program, l3_font_texture_id, l3_losing_text, 2.0f, 0.05f, glm::vec3(m_game_state.player->get_position().x -3.0f, -4.4f, 0.0f));
    } else if (l3_player_win) {
        Utility::draw_text(program, l3_font_texture_id, l3_wining_text, 2.0f, 0.05f, glm::vec3(m_game_state.player->get_position().x + 1.0f, -4.4f, 0.0f));
    }
}
