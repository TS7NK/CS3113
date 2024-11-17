#include "Level A.h"
#include "Utility.h"

#define LEVEL_WIDTH 23
#define LEVEL_HEIGHT 11

constexpr char SPRITESHEET_FILEPATH[] = "haley.png",
l1_FONTSHEET_FILEPATH[] = "font1.png",
             ENEMY_FILEPATH[]       = "skeleton.png";

GLuint l1_font_texture_id;
std::string l1_losing_text = "You lose!";
std::string l1_instruction = "A(<-) D(->) Space(Jump) I(Info)";


unsigned int LEVELA_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 35,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    41,42,43,44,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    50,51,52,53,0, 0, 0, 0, 0, 0, 0, 0, 41,42,43,44,0, 0, 0, 0, 0, 0, 0,
    59,60,61,62,0, 0, 0, 0, 0, 0, 0, 0, 50,51,52,53,0, 0, 0, 0, 0, 0, 0,
    0, 0,70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59,60,61,62,0, 0, 0, 0, 0, 0, 0,
    67,78,79,0, 0,19, 0, 0, 3, 0, 0, 0, 0, 0, 70,0, 0, 0, 0, 0, 0, 0, 0,
    27,27,27,27,28,27,27,27,28,19,20,0, 68,78,79,0, 3, 0, 0, 0, 0,0, 34,
    30, 1,30,30,30,30,30,30, 1,32,28,28,27,28,28,28,28,27,28,28,28,28,12,
    30, 32,30,30,30,1,30,30,32,32,30,30,30,30, 1,30,30,30,30,30,30,30,30
};

LevelA::~LevelA()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
}

void LevelA::initialise()
{
    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("outline_tiles.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 9, 9);
    
    l1_font_texture_id = Utility::load_texture(l1_FONTSHEET_FILEPATH);
    
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
        1.0f,                      // height
        PLAYER
    );
    
    m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(4.0f);
    
    /**
    Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    m_game_state.enemies[0] =  Entity(enemy_texture_id, 0.8f, 0.7f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[0].set_position(glm::vec3(11.0f, -4.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(4.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    
    m_game_state.enemies[1] =  Entity(enemy_texture_id, 0.8f, 0.7f, 1.0f, ENEMY, WALKER, WALKING);
    m_game_state.enemies[1].set_position(glm::vec3(8.0f, -3.0f, 0.0f));
    m_game_state.enemies[1].set_movement(glm::vec3(4.0f));
    m_game_state.enemies[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
}

void LevelA::update(float delta_time)
{
    if(Utility::get_gamePaused()) return;
        
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    // m_game_state.player->check_collision_x(m_game_state.enemies, ENEMY_COUNT);
    // m_game_state.player->check_collision_y(m_game_state.enemies, ENEMY_COUNT);
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

    if (m_game_state.player->get_position().x > 21.0f) m_game_state.next_scene_id = 1;
    
}

void LevelA::render(ShaderProgram *program)
{
    std::string l1_lives_text = "Lives: " + std::to_string(Utility::get_lives());
    float player_x = m_game_state.player->get_position().x;
    const float minX = 0.0f;
    float text_x = std::max(player_x - 5.0f, minX);
    Utility::draw_text(program, l1_font_texture_id, l1_lives_text, 1.0f, -0.5f, glm::vec3(text_x, 0.5f, 0.0f));
    
    Utility::draw_text(program, l1_font_texture_id, l1_instruction, 0.9f, -0.5f, glm::vec3(text_x+ 6.5f, 0.5f, 0.0f));
    
    if(Utility::get_lives() <= 0){
        Utility::draw_text(program, l1_font_texture_id, l1_losing_text, 1.0f, 0.05f, glm::vec3(m_game_state.player->get_position().x, -3.4f, 0.0f));
        return;
    }
    
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    
    // Render all active enemies, including those with left or right collisions
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (m_game_state.enemies[i].is_active()) {
            m_game_state.enemies[i].render(program);
        }
    }
}
