#include "Level D.h"
#include "Utility.h"

#define LEVEL_WIDTH 31
#define LEVEL_HEIGHT 15

constexpr char SPRITESHEET_FILEPATH[] = "Soldier.png",
l4_FONTSHEET_FILEPATH[] = "font1.png",
ROCK_FILEPATH[] = "rock_tilesheet.png",
ENEMY_GREEN_FILEPATH[]       = "green ghost.png",
ENEMY_SPIDER_FILEPATH[] = "spider.png",
ENEMY_FILEPATH[]       = "purple man.png";

GLuint l4_font_texture_id;
std::string l4_losing_text = "You Lose!";
std::string l4_losing_text_v2 = "You didn't collect all diamonds!";
std::string l4_winning_text = "You Win!";
bool l4_game_over = false;
bool l4_player_win = false;
bool l4_show_losing_message = false;
bool l4_show_winning_message = false;

unsigned int LEVELD_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,72,73,74,67,76,93, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0,129,4, 5, 6, 7, 8,93, 0, 0, 0, 0, 0, 0, 0, 0,88,89,90,83,92,109,110,111, 0, 0, 0, 0, 0,
    0, 70,144,145,89,65,66,20,21,109,110,111, 0, 0, 0, 0, 0, 0,88,105,106,99,108,125,126,109,5,6,7,71, 0,
    72,86,131,97,105,81,82,36,37,125,126,98, 0, 0, 0, 0, 0, 0,104,122,122,115,124,141,142,125,65,66,22,87, 0,
    88,102,147,160,121,122,123,52,53,141,176,98, 0, 0, 0, 0, 0,70,71,185,186,204,187,188,158,141,81,82,38,103, 0,
    104,118,163,192,185,186,187,188,185,172,158,98, 0, 0, 0,70,85,86,87,151,152,212,212,140,185,158,122,123,123,87, 0,
    104,124,226,166,167,168,169,170,171,172,172,72,73,7,8,86,101,102,103,212,212,211,212,212,212,186,187,188,185,103, 0,
    104,165,154,155,153,149,150,151,152,153,154,88,89,23,24,97,117,118,119,199,200,166,183,154,212,212,212,172,139,87, 0,
    104,155,209,210,211,212,225,227,228,246,155,104,105,39,40,161,192,192,188,209,212,212,166,199,200,169,170,171,140,103, 0,
    104,171,201,202,203,169,170,171,172,137,138,120,121,55,56,192,186,187,150,212,140,154,138,137,212,212,153,209,210,119, 0,
    104,171,217,218,219,139,140,170,170,212,166,185,186,187,188,185,171,200,171,171,212,212,212,171,139,171,209,212,137,87, 0,
    104,171,233,234,235,171,199,200,201,209,210,211,226,212,212,212,172,205,215,213,171,137,212,212,212,212,212,210,210,103, 0,
    120,213,214,215,213,214,215,216,212,212,172,212,169,212,169,154,236,0, 0, 0, 0,232,214,215,171,171,149,150,137,119, 0,
    0, 0, 0, 0, 0, 0, 0,232,212,154,212,169,155,153,212,170,221, 0, 0, 0, 0, 0, 0, 0,214,215,213, 0, 0,0, 0,
    0, 0, 0, 0, 0, 0, 0,  0,213,214,215,213,214,215,213,214,213, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0
};

LevelD::~LevelD()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
}

void LevelD::initialise()
{
    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("level3_tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELD_DATA, map_texture_id, 1.0f,16,18, 4);
    
    l4_font_texture_id = Utility::load_texture(l4_FONTSHEET_FILEPATH);
    
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
    
    m_game_state.player->set_position(glm::vec3(21.0f, -4.0f, 0.0f));
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    GLuint enemy_green_texture_id = Utility::load_texture(ENEMY_GREEN_FILEPATH);
    GLuint enemy_spider_texture_id = Utility::load_texture(ENEMY_SPIDER_FILEPATH);
    
    m_game_state.enemies = new Entity[ENEMY_COUNT];
    
    // 2 follower -> purple man
    m_game_state.enemies[0] =  Entity(enemy_texture_id, 1.1f, 0.7f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[0].set_position(glm::vec3(7.0f, -7.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(1.0f,0.0f, 0.0f));
    // green ghost
    m_game_state.enemies[1] =  Entity(enemy_green_texture_id, 1.1f, 0.7f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[1].set_position(glm::vec3(24.0f, -9.0f, 0.0f));
    m_game_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[1].set_acceleration(glm::vec3(1.0f, 0.0f, 0.0f));
    // 1 walking
    m_game_state.enemies[2] =  Entity(enemy_spider_texture_id, 1.0f, 0.7f, 1.0f, ENEMY, WALKER, WALKING);
    m_game_state.enemies[2].set_position(glm::vec3(18.0f, -8.0f, 0.0f));
    m_game_state.enemies[2].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[2].set_acceleration(glm::vec3(1.0f, 0.0f, 0.0f));
    
    /* Rock' stuff */
    GLuint rock_texture_id = Utility::load_texture(ROCK_FILEPATH);
    m_game_state.rocks = new Entity[ROCK_COUNT];
    
    // Add rocks with different tile indices
    // Diamond
    m_game_state.rocks[0] = Entity(rock_texture_id,121, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[0].set_position(glm::vec3(0.9f, -7.2f, 0.0f));
    m_game_state.rocks[17] = Entity(rock_texture_id,121, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[17].set_position(glm::vec3(11.0f, -13.0f, 0.0f));
    // green jade
    m_game_state.rocks[1] = Entity(rock_texture_id,4, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[1].set_position(glm::vec3(19.0f, -5.0f, 0.0f));
    m_game_state.rocks[2] = Entity(rock_texture_id,4, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[2].set_position(glm::vec3(23.0f, -7.0f, 0.0f));
    m_game_state.rocks[3] = Entity(rock_texture_id,4, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[3].set_position(glm::vec3(18.0f, -10.0f, 0.0f));
    m_game_state.rocks[4] = Entity(rock_texture_id,4, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[4].set_position(glm::vec3(19.0f, -9.0f, 0.0f));
    m_game_state.rocks[5] = Entity(rock_texture_id,4, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[5].set_position(glm::vec3(16.0f, -9.0f, 0.0f));
    m_game_state.rocks[6] = Entity(rock_texture_id,4, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[6].set_position(glm::vec3(11.0f, -10.0f, 0.0f));
    // radio reactive rock
    m_game_state.rocks[7] = Entity(rock_texture_id,15, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[7].set_position(glm::vec3(23.0f, -4.0f, 0.0f));
    m_game_state.rocks[8] = Entity(rock_texture_id,15, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[8].set_position(glm::vec3(25.0f, -9.0f, 0.0f));
    m_game_state.rocks[9] = Entity(rock_texture_id,15, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[9].set_position(glm::vec3(10.0f, -6.0f, 0.0f));
    m_game_state.rocks[10] = Entity(rock_texture_id,15, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[10].set_position(glm::vec3(13.0f, -11.0f, 0.0f));
    m_game_state.rocks[11] = Entity(rock_texture_id,15, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[11].set_position(glm::vec3(8.0f, -13.0f, 0.0f));
    m_game_state.rocks[12] = Entity(rock_texture_id,15, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[12].set_position(glm::vec3(3.0f, -9.0f, 0.0f));
    m_game_state.rocks[13] = Entity(rock_texture_id,15, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[13].set_position(glm::vec3(6.0f, -7.0f, 0.0f));
    m_game_state.rocks[14] = Entity(rock_texture_id,15, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[14].set_position(glm::vec3(8.0f, -10.0f, 0.0f));
    m_game_state.rocks[15] = Entity(rock_texture_id,15, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[15].set_position(glm::vec3(8.0f, -10.0f, 0.0f));
    m_game_state.rocks[16] = Entity(rock_texture_id,4, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[16].set_position(glm::vec3(8.9f, -5.0f, 0.0f));
    
}

void LevelD::update(float delta_time)
{
    if(Utility::get_gamePaused() || l4_player_win || l4_game_over) return;
    
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    m_game_state.rocks->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    
    // Check collison with rocks
    m_game_state.player->check_collision_x(m_game_state.rocks, ROCK_COUNT);
    m_game_state.player->check_collision_y(m_game_state.rocks, ROCK_COUNT);
    
    // Check collision with diamond rocks
    for (int i = 0; i < ROCK_COUNT; i++) {
        if (m_game_state.rocks[i].get_tile_index() == 121) { // Assuming 121 indicates diamond rocks
            bool isColliding = m_game_state.player->check_collision(&m_game_state.rocks[i]);
            if (isColliding) {
                Utility::update_diamonds();
                m_game_state.rocks[i].deactivate(); // Deactivate the diamond rock after collection
                std::cout << "Diamond collected! Current diamonds = " << Utility::get_diamonds() << "/3" << std::endl;
            }
        }
        if (i == 16) {  // Assuming rock index 25 is the ladder-triggering rock
            bool isColliding = m_game_state.player->check_collision(&m_game_state.rocks[i]);
            if (isColliding && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J]) {
                m_game_state.rocks[i].deactivate();  // Deactivate rock after interaction
                
                // Update the LEVELB_DATA to include the ladder (173)
                int ladder_x = 9;  // Move right by 2 tiles
                int ladder_y = -9; // Move up by 4 tiles
                LEVELD_DATA[(LEVEL_HEIGHT - 1 - (-ladder_y)) * LEVEL_WIDTH + ladder_x] = 173;
                
                // Debugging output
                std::cout << "Ladder revealed at position (" << ladder_x << ", " << ladder_y << ") with tile value: " << LEVELD_DATA[ladder_y * LEVEL_WIDTH + ladder_x] << std::endl;
                
                // Rebuild the map
                m_game_state.map->update_level_data(LEVELD_DATA); // Make sure the map uses the updated level data
                m_game_state.map->build();
            }
        }
        bool isColliding = m_game_state.player->check_collision(&m_game_state.rocks[i]);
        if (isColliding && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J]) {
            m_game_state.rocks[i].deactivate();  // Deactivate rock after interaction
        }
    }
    // Check collision with enemies
    m_game_state.player->check_collision_x(m_game_state.enemies, ENEMY_COUNT);
    m_game_state.player->check_collision_y(m_game_state.enemies, ENEMY_COUNT);
    
    // Update enemies
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (m_game_state.enemies[i].is_active()) {
            
            if (i == 0 || i == 1) {
                m_game_state.enemies[i].ai_follow_player(m_game_state.player);
            }
            else if (i == 2){
                m_game_state.enemies[i].ai_horizontally_walker(18.0f, 22.0f);
            }
            
            // Update enemy position
            m_game_state.enemies[i].update(delta_time, m_game_state.player, nullptr, 0, m_game_state.map);
            
            // Check if player is within 0.5f units and pressing 'J' key to attack
            float distance_y = glm::length(m_game_state.player->get_position().y - m_game_state.enemies[i].get_position().y);
            float distance_x = glm::length(m_game_state.player->get_position().x - m_game_state.enemies[i].get_position().x);
            //            std::cout << "Player x distance = " << distance_x << std::endl;
            //            std::cout << "Player y distance = " << distance_y << std::endl;
            const Uint8 *state = SDL_GetKeyboardState(nullptr);
            if ((distance_x <= 0.3f || distance_y <= 0.3f) && state[SDL_SCANCODE_J]) {
                // Player is attacking, deactivate the enemy
                m_game_state.enemies[i].deactivate();
                std::cout << "Player attacked and defeated enemy " << i << std::endl;
                continue; // Move to the next enemy since this one is now deactivated
            }
            
            // Check collision between player and enemy if the enemy is still active
            bool isColliding = m_game_state.player->check_collision(&m_game_state.enemies[i]);
            if (isColliding) {
                // Decrease player's life if collided
                if (!m_game_state.enemies[i].recentlyCollided) {
                    m_game_state.enemies[i].recentlyCollided = true;
                    Utility::update_lives();
                    std::cout << "Player hit by enemy; current lives = " << Utility::get_lives() << std::endl;
                }
            } else {
                // Reset collision flag if no longer colliding
                m_game_state.enemies[i].recentlyCollided = false;
            }
        }
    }
    
    if (Utility::get_lives() <= 0){
        l4_game_over = true;
    }
    
    // When the player reach the ladder of success, and she didn't collect 3/3 diamonds
    if (((m_game_state.player->get_position().y == -6.0f && m_game_state.player->get_position().x >= 8.9f) ||
         (m_game_state.player->get_position().x >= 8.1f && m_game_state.player->get_position().y == -5.0f)) &&
        (Utility::get_diamonds() != 3) && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J]) {
        l4_game_over = true;
        l4_show_losing_message = true;
    }
    
    // When the player reach the ladder of success, and she collect 3/3 diamonds
    if (((m_game_state.player->get_position().y == -6.0f && m_game_state.player->get_position().x >= 8.9f) ||
         (m_game_state.player->get_position().x >= 8.1f && m_game_state.player->get_position().y == -5.0f)) &&
        (Utility::get_diamonds() == 3) && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J]) {
        l4_game_over = true;
        l4_show_winning_message = true;
    }
}

void LevelD::render(ShaderProgram *program)
{
//    std::cout<< "X position = " << m_game_state.player->get_position().x << std::endl;
//    std::cout<< "Y position = " << m_game_state.player->get_position().y << std::endl;
    
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    
    // Render all the rocks
    for (int i = 0; i < ROCK_COUNT; i++) {
        m_game_state.rocks[i].render(program);
    }
    
    // Render all active enemies, including those with left or right collisions
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (m_game_state.enemies[i].is_active()) {
            m_game_state.enemies[i].render(program);
        }
    }
    
    // When the player reaches the ladder of success and collect 3 diamonds
    if (l4_show_winning_message) {
        Utility::draw_text(program, l4_font_texture_id, l4_winning_text, 1.5f, 0.05f,
                           glm::vec3(m_game_state.player->get_position().x, m_game_state.player->get_position().y-2.0f, 0.0f));
        return;
    }
    
    // When the player reaches the ladder of success but didn't collect enough diamonds
    if (l4_show_losing_message) {
        Utility::draw_text(program, l4_font_texture_id, l4_losing_text, 1.5f, 0.05f,
                           glm::vec3(m_game_state.player->get_position().x, m_game_state.player->get_position().y, 0.0f));
        Utility::draw_text(program, l4_font_texture_id, l4_losing_text_v2, 1.0f, -0.5f,
                           glm::vec3(m_game_state.player->get_position().x - 2.0f, m_game_state.player->get_position().y - 3.0f, 0.0f));
        return;
    }
    
    // Update text position relative to player
    std::string l4_lives_text = "Lives: " + std::to_string(Utility::get_lives());
    std::string l4_diamonds_text = "Diamonds: " + std::to_string(Utility::get_diamonds()) + "/3";
    
    float player_x = m_game_state.player->get_position().x;
    float player_y = m_game_state.player->get_position().y + 4.0f;
    // Draw the lives text and instructions 3.0 units above the player
    Utility::draw_text(program, l4_font_texture_id, l4_lives_text, 1.0f, -0.6f, glm::vec3(player_x - 3.5f, player_y, 0.0f));
    Utility::draw_text(program, l4_font_texture_id, l4_diamonds_text, 1.0f, -0.5f, glm::vec3(player_x + 1.0f, player_y, 0.0f));
    
    // Check if the player has run out of lives -> Any time
    if (Utility::get_lives() <= 0) {
        Utility::draw_text(program, l4_font_texture_id, l4_lives_text, 1.0f, -0.6f, glm::vec3(player_x - 3.5f, player_y, 0.0f));
        Utility::draw_text(program, l4_font_texture_id, l4_losing_text, 1.5f, 0.05f,
                           glm::vec3(m_game_state.player->get_position().x, m_game_state.player->get_position().y - 1.0f, 0.0f));
        return;
    }
}
