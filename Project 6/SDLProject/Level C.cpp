#include "Level C.h"
#include "Utility.h"

#define LEVEL_WIDTH 31
#define LEVEL_HEIGHT 23

constexpr char SPRITESHEET_FILEPATH[] = "Soldier.png",
l3_FONTSHEET_FILEPATH[] = "font1.png",
ROCK_FILEPATH[] = "rock_tilesheet.png",
ENEMY_GHOST_FILEPATH[]       = "ghost.png",
ENEMY_BAT_FILEPATH[]    = "bat.png",
ENEMY_FILEPATH[]       = "ice skeleton.png";

GLuint l3_font_texture_id;
std::string l3_losing_text = "You Lose!";
std::string l3_wining_text = "You Win!";
bool l3_game_over = false;
bool l3_player_win = false;

unsigned int LEVELC_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0,84,185,186,187,188,89,65,66,67,65,66,23,24,102,89,118,89,89,56,141,143, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,85,201,202,195,224,105,81,82,83,81,82,39,40,118,105,56,137,138,139,140,159, 0,
    0, 0, 0, 0, 0, 0, 0,85,86,130,217,218,219,188,121,98,98,115,98,98,98,56,134,154,155,153,154,155,153,175, 0,
    0, 0, 0, 0, 0,85,86,20,102,146,185,186,187,188,209,210,211,212,225,226,227,228,210,211,212,225,226,227,228,191, 0,
    0, 0, 0,85,86,91,102,36,118,162,1,2,3,17,18,19,33,34,35,165,181,187,188,149,150,151,152,185,186,207, 0,
    0,85,86,91,97,107,118,130,118,165,181,166,167,168,201,202,203,204,137,138,139,140,209,210,211,212,225,226,227,223, 0,
    84,160,102,107,118,123,118,162,137,138,139,140,153,154,155,169,170,171,172,137,138,139,140,153,154,155,169,170,171,223, 0,
    100,161,118,123,118,123,185,186,187,188,166,167,168,198,199,200,209,210,211,212,166,167,167,167,168,169,170,171,171,223, 0,
    116,177,118,123,137,138,139,140,153,154,155,169,170,171,172,209,210,212,182,183,183,183,184,185,186,187,187,220,214,0,0,
    132,177,137,138,139,140,153,154,155,209,210,211,212,225,226,227,228,227,182,183,183,183,184,169,170,171,171,236, 0,0,0,
    148,137,138,139,140,153,154,155,169,170,171,185,186,187,188,201,202,203,182,183,183,183,184,153,154,155,155,159, 0, 0, 0,
    0,197,137,138,139,140,153,154,155,169,170,171,172,185,186,187,188,201,202,182,183,183,183,184,234,235,234,159, 0, 0, 0,
    0,213,214,215,214,216,185,186,187,188,137,138,139,166,167,168,140,153,154,155,168,169,170,171,172,185,186,72,0, 0, 0,
    0, 0, 0, 0, 0,84,183,183,137,138,225,226,227,182,183,184,137,138,139,140,153,154,155,169,170,171,172,92,109,110, 0,
    0, 0, 0, 0, 0,85,209,210,211,212,225,226,227,198,199,200,228,209,210,149,150,151,152,33,34,35,19,108,113,126,127,
    0, 0, 0,85,86,130,1,2,211,3,17,18,19,33,34,35,198,217,218,219,201,202,203,204,1,2,3,108,113,126,159,
    0,85,86,160,102,146,137,138,139,140,151,152,153,154,155,169,137,138,139,140,153,154,155,169,170,171,172,184,141,142,175,
    100,130,22,107,118,162,201,202,203,204,1,2,3,185,186,187,188,1,2,3,17,18,19,166,167,168,151,1,2,158,191,
    116,146,38,123,118,166,167,137,138,139,140,153,154,166,167,168,33,34,35,1,2,3,17,18,19,137,138,139,140,153,207,
    132,162,123,123,183,183,137,138,139,140,1,2,3,17,18,19,185,186,187,188,1,2,3,17,18,19,137,138,139,1,223,
    148,155,169,170,171,172,202,203,204,33,34,35,150,151,209,210,211,212,1,2,3,17,18,19,137,138,139,205,214,215, 0,
    213,214,215,216,225,226,205,214,215,214,215,214,215,214,215,214,215,214,215,214,215,214,215,214,215,214,215, 0, 0, 0, 0,
    0, 0, 0, 0, 214,215, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
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
    
    GLuint map_texture_id = Utility::load_texture("level2_tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 16, 15, 3);
    
    l3_font_texture_id = Utility::load_texture(l3_FONTSHEET_FILEPATH);
    
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
    
    m_game_state.player->set_position(glm::vec3(17.0f, -3.0f, 0.0f));
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    GLuint enemy_ghost_texture_id = Utility::load_texture(ENEMY_GHOST_FILEPATH);
    GLuint enemy_bat_texture_id = Utility::load_texture(ENEMY_BAT_FILEPATH);
    
    m_game_state.enemies = new Entity[ENEMY_COUNT];
    
    // 2 flyer -> ghost
    m_game_state.enemies[0] =  Entity(enemy_ghost_texture_id, 1.2f, 0.7f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[0].set_position(glm::vec3(10.0f, -11.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    m_game_state.enemies[1] =  Entity(enemy_ghost_texture_id, 1.2f, 0.7f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[1].set_position(glm::vec3(22.0f, -14.0f, 0.0f));
    m_game_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[1].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // 1 follower -> skeleton
    m_game_state.enemies[2] =  Entity(enemy_texture_id, 1.2f, 0.7f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[2].set_position(glm::vec3(16.0f, -8.0f, 0.0f));
    m_game_state.enemies[2].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[2].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // 2 follower -> bat
    m_game_state.enemies[5] =  Entity(enemy_bat_texture_id, 1.2f, 0.7f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[5].set_position(glm::vec3(27.0f, -3.0f, 0.0f));
    m_game_state.enemies[5].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[5].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    m_game_state.enemies[6] =  Entity(enemy_bat_texture_id, 1.2f, 0.7f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[6].set_position(glm::vec3(6.0f, -7.0f, 0.0f));
    m_game_state.enemies[6].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[6].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // 1 horizontal [16,22]
    m_game_state.enemies[3] =  Entity(enemy_texture_id, 1.5f, 0.7f, 1.0f, ENEMY, WALKER, WALKING);
    m_game_state.enemies[3].set_position(glm::vec3(16.0f, -14.0f, 0.0f));
    m_game_state.enemies[3].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[3].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // 1 vertical [-12,-19]
    m_game_state.enemies[4] =  Entity(enemy_texture_id, 1.5f, 0.7f, 1.0f, ENEMY, WALKER, WALKING);
    m_game_state.enemies[4].set_position(glm::vec3(11.0f, -11.0f, 0.0f));
    m_game_state.enemies[4].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[4].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    /** Rock' stuf f */
    GLuint rock_texture_id = Utility::load_texture(ROCK_FILEPATH);
    m_game_state.rocks = new Entity[ROCK_COUNT];
    
    // Add rocks with different tile indices
    // Diamond
    m_game_state.rocks[0] = Entity(rock_texture_id,121, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[0].set_position(glm::vec3(1.0f, -20.2f, 0.0f));
    // Mysterious Rock
    m_game_state.rocks[1] = Entity(rock_texture_id,45, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[1].set_position(glm::vec3(13.0f, -3.0f, 0.0f));
    m_game_state.rocks[2] = Entity(rock_texture_id,45, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[2].set_position(glm::vec3(14.0f, -6.0, 0.0f));
    m_game_state.rocks[3] = Entity(rock_texture_id,45, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[3].set_position(glm::vec3(22.0f, -11.0, 0.0f));
    m_game_state.rocks[4] = Entity(rock_texture_id,45, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[4].set_position(glm::vec3(15.0f, -14.0, 0.0f));
    m_game_state.rocks[5] = Entity(rock_texture_id,45, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[5].set_position(glm::vec3(25.0f, -20.0, 0.0f));
    m_game_state.rocks[6] = Entity(rock_texture_id,45, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[6].set_position(glm::vec3(11.0f, -20.0, 0.0f));
    m_game_state.rocks[30] = Entity(rock_texture_id,45, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[30].set_position(glm::vec3(20.0f, -5.0, 0.0f));
    m_game_state.rocks[31] = Entity(rock_texture_id,45, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[31].set_position(glm::vec3(26.0f, -3.0, 0.0f));
    m_game_state.rocks[32] = Entity(rock_texture_id,45, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[32].set_position(glm::vec3(9.0f, -14.0, 0.0f));
    m_game_state.rocks[33] = Entity(rock_texture_id,45, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[33].set_position(glm::vec3(27.0f, -18.0, 0.0f));
    // Blue rocks small
    m_game_state.rocks[7] = Entity(rock_texture_id,46, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[7].set_position(glm::vec3(10.0f, 0.0, 0.0f));
    m_game_state.rocks[8] = Entity(rock_texture_id,46, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[8].set_position(glm::vec3(18.0f, -3.0, 0.0f));
    m_game_state.rocks[9] = Entity(rock_texture_id,46, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[9].set_position(glm::vec3(11.0f, -7.0, 0.0f));
    m_game_state.rocks[10] = Entity(rock_texture_id,46, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[10].set_position(glm::vec3(23.0f, -2.0, 0.0f));
    m_game_state.rocks[11] = Entity(rock_texture_id,46, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[11].set_position(glm::vec3(24.0f, -14.0, 0.0f));
    m_game_state.rocks[12] = Entity(rock_texture_id,46, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[12].set_position(glm::vec3(19.0f, -16.0, 0.0f));
    m_game_state.rocks[13] = Entity(rock_texture_id,46, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[13].set_position(glm::vec3(27.0f, -16.0, 0.0f));
    // Blue rock big
    m_game_state.rocks[14] = Entity(rock_texture_id,50, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[14].set_position(glm::vec3(11.0f, 0.0, 0.0f));
    m_game_state.rocks[15] = Entity(rock_texture_id,50, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[15].set_position(glm::vec3(3.0f, -9.0, 0.0f));
    m_game_state.rocks[16] = Entity(rock_texture_id,50, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[16].set_position(glm::vec3(6.0f, -12.0, 0.0f));
    m_game_state.rocks[17] = Entity(rock_texture_id,50, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[17].set_position(glm::vec3(5.0f, -20.0, 0.0f));
    m_game_state.rocks[18] = Entity(rock_texture_id,50, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[18].set_position(glm::vec3(8.0f, -16.0, 0.0f));
    m_game_state.rocks[19] = Entity(rock_texture_id,50, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[19].set_position(glm::vec3(13.0f, -17.0, 0.0f));
    m_game_state.rocks[20] = Entity(rock_texture_id,50, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[20].set_position(glm::vec3(11.0f, -11.0, 0.0f));
    m_game_state.rocks[21] = Entity(rock_texture_id,50, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[21].set_position(glm::vec3(26.0f, -7.0, 0.0f));
    // Silver rock
    m_game_state.rocks[22] = Entity(rock_texture_id,12, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[22].set_position(glm::vec3(16.0f, -17.0, 0.0f));
    m_game_state.rocks[23] = Entity(rock_texture_id,12, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[23].set_position(glm::vec3(16.0f, -20.0, 0.0f));
    m_game_state.rocks[24] = Entity(rock_texture_id,12, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[24].set_position(glm::vec3(21.0f, -18.0, 0.0f));
    m_game_state.rocks[25] = Entity(rock_texture_id,12, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[25].set_position(glm::vec3(18.0f, -8.0, 0.0f));
    m_game_state.rocks[26] = Entity(rock_texture_id,12, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[26].set_position(glm::vec3(22.0f, -11.0, 0.0f));
    m_game_state.rocks[27] = Entity(rock_texture_id,12, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[27].set_position(glm::vec3(16.0f, -9.0, 0.0f));
    m_game_state.rocks[28] = Entity(rock_texture_id,12, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[28].set_position(glm::vec3(7.0f, -18.0, 0.0f));
    m_game_state.rocks[29] = Entity(rock_texture_id,12, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[29].set_position(glm::vec3(8.0f, -9.0, 0.0f));
    m_game_state.rocks[30] = Entity(rock_texture_id,12, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[30].set_position(glm::vec3(8.1f, -15.0, 0.0f));
    
}

void LevelC::update(float delta_time)
{
    if (Utility::get_gamePaused() || Utility::get_lives() <= 0) return;
    
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
        if (i == 30) {  // Assuming rock index 30 is the ladder-triggering rock
            bool isColliding = m_game_state.player->check_collision(&m_game_state.rocks[30]);
            if (isColliding && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J]) {
                m_game_state.rocks[i].deactivate();  // Deactivate rock after interaction
                
                // Update the LEVELC_DATA to include the ladder (173)
                int ladder_x = 8;   // X coordinate of the ladder tile
                int ladder_y = -15; // Y coordinate of the ladder tile (assuming negative due to OpenGL setup)
                
                // Calculate correct array index
                int ladder_index = (LEVEL_HEIGHT - (-ladder_y) + 7) * LEVEL_WIDTH + ladder_x;
                LEVELC_DATA[ladder_index] = 173;  // Update the level data with ladder tile
                
                // Debugging output
                std::cout << "Ladder revealed at position (" << ladder_x << ", " << ladder_y << ") with tile value: " << LEVELC_DATA[ladder_index] << std::endl;
                
                // Rebuild the map
                m_game_state.map->update_level_data(LEVELC_DATA); // Make sure the map uses the updated level data
                std::cout << "Building map after adding ladder..." << std::endl;
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
            
            if (i == 0 || i == 1 || i == 2 || i == 5 || i == 6) {
                m_game_state.enemies[i].ai_follow_player(m_game_state.player);
            }
            else if (i == 3){
                m_game_state.enemies[i].ai_horizontally_walker(16.0f, 22.0f);
            }
            else if (i == 4) {
                m_game_state.enemies[i].ai_vertical_walker(-12.0f, -19.0f);
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
        l3_game_over = true;
    }
    
    if ((m_game_state.player->get_position().x >= 7.9f && m_game_state.player->get_position().y == -14.0f)){
        m_game_state.next_scene_id = 1;
    }
}

void LevelC::render(ShaderProgram *program)
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
    
    // Update text position relative to player
    std::string l3_lives_text = "Lives: " + std::to_string(Utility::get_lives());
    std::string l3_diamonds_text = "Diamonds: " + std::to_string(Utility::get_diamonds()) + "/3";
    
    float player_x = m_game_state.player->get_position().x;
    float player_y = m_game_state.player->get_position().y + 3.0f;
    
    // If user go below the y -> fix the lives display
    if(m_game_state.player->get_position().y <= -15.0f){
        Utility::draw_text(program, l3_font_texture_id, l3_lives_text, 1.0f, -0.6f, glm::vec3(player_x - 3.5f, -12.0f, 0.0f));
        Utility::draw_text(program, l3_font_texture_id, l3_diamonds_text, 1.0f, -0.5f, glm::vec3(player_x + 1.0f, -12.0f, 0.0f));
    } else{
        Utility::draw_text(program, l3_font_texture_id, l3_lives_text, 1.0f, -0.6f, glm::vec3(player_x - 3.5f, player_y, 0.0f));
        Utility::draw_text(program, l3_font_texture_id, l3_diamonds_text, 1.0f, -0.5f, glm::vec3(player_x + 1.0f, player_y, 0.0f));
    }
    
    // Check if the player has run out of lives
    if (Utility::get_lives() <= 0) {
        // This is when the camera is fixed for the bottom part of the map
        if(m_game_state.player->get_position().y <= -16.0f){
            Utility::draw_text(program, l3_font_texture_id, l3_lives_text, 1.0f, -0.6f, glm::vec3(player_x - 3.5f, -12.0f, 0.0f));
            Utility::draw_text(program, l3_font_texture_id, l3_losing_text, 1.5f, 0.05f,
                               glm::vec3(m_game_state.player->get_position().x - 3.0f, m_game_state.player->get_position().y + 1.8f, 0.0f));
        }else{
            Utility::draw_text(program, l3_font_texture_id, l3_lives_text, 1.0f, -0.6f, glm::vec3(player_x - 3.5f, player_y, 0.0f));
            Utility::draw_text(program, l3_font_texture_id, l3_losing_text, 1.5f, 0.05f,
                               glm::vec3(m_game_state.player->get_position().x, m_game_state.player->get_position().y - 4.0f, 0.0f));
        }
        return;
    }
}
