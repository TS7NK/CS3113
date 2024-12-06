#include "Level B.h"
#include "Utility.h"

#define LEVEL_WIDTH  22
#define LEVEL_HEIGHT 27

constexpr char SPRITESHEET_FILEPATH[] = "Soldier.png",
l2_FONTSHEET_FILEPATH[] = "font1.png",
ROCK_FILEPATH[] = "rock_tilesheet.png",
ENEMY_FLY_FILEPATH[]       = "fly fish.png",
ENEMY_FILEPATH[]       = "desert zombie.png";

GLuint l2_font_texture_id;
std::string l2_losing_text = "You lose!";
std::string l2_instruction = "A(<-) D(->) J(Attack) I(Info)";

// replace 173 with 11
unsigned int LEVELB_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,148,234,235,168,233,234,235,233,233,92,99,92,125,92,65,66,92,92,102,92,159,
    0,148,234,235,168,233,234,235,233,233,92,99,92,125,92,81,82,92,92,102,92,159,
    0,164,167,168,168,155,170,155,170,233,234,235,233,125,92,92,92,92,92,102,168,159,
    0,180,167,168,170,170,171,154,137,168,168,170,168,168,168,168,168,168,168,168,168,159,
    0,196,197,154,167,168,154,171,137,154,155,170,154,155,137,137,137,155,137,154,137,159,
    0,0,213,214,215,216,199,170,171,170,154,137,137,138,170,154,137,137,154,137,205,0,
    0, 0, 0, 0, 0,213,214,215,216,198,199,137,137,137,155,154,155,154,236,214, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,213,214,215,216,199,137,137,137,155,154,159, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,213,216,137,154,137,138,138,159, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,232,170,138,138,155,138,6,93, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,180,138,155,137,138,137,22,109,76,159,
    0, 0, 0, 0, 0, 0, 0, 0, 0,70,144,73,145,137,155,137,154,138,38,125,92,159,
    0, 0, 0, 0, 0, 0, 0,70,85,86,130,131,161,138,138,138,138,138,54,125,108,159,
    0, 0, 0, 0, 0,70,85,86,101,102,146,147,177,155,138,137,138,137,234,125,124,159,
    0, 0, 0,68,85,86,101,97,117,118,123,121,193,155,138,138,137,138,234,234,234,159,
    0, 0, 0,84,101,102,117,118,133,138,138,137,138,137,138,155,137,205,214,215,215, 0,
    0, 0, 0,100,117,118,133,140,149,138,138,137,154,138,137,138, 205, 0, 0, 0, 0, 0,
    0, 0, 0,116,133,137,138,137,138,137,138,154,138,205,214,215, 0, 0, 0, 0, 0, 0,
    0, 0, 0,132,137,138,137,155,154,154,137,138,220, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0,148,137,138,137,170,137,137,137,138,236, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0,164,138,137,137,138,138,170,138,137,143, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0,180,138,137,138,138,138,170,138,137,143, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0,196,197,138,137,138,155,137,137,138,143, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0,213,214,197,137,138,154,205,215, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,213,214,215,215, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

LevelB::~LevelB()
{
    delete [] m_game_state.enemies;
    delete [] m_game_state.rocks;
    delete    m_game_state.player;
    delete    m_game_state.map;
}

void LevelB::initialise()
{
    m_game_state.next_scene_id = -1;
    
    GLuint l2_map_texture_id = Utility::load_texture("level1_tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, l2_map_texture_id, 1.0f, 16, 18, 2);
    
    l2_font_texture_id = Utility::load_texture(l2_FONTSHEET_FILEPATH);
    
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
    
    m_game_state.player->set_position(glm::vec3(11.0f, -3.0f, 0.0f));
    
    /** Enemies' stuf f */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    GLuint enemy_fly_texture_id = Utility::load_texture(ENEMY_FLY_FILEPATH);
    
    m_game_state.enemies = new Entity[ENEMY_COUNT];
    
    // 2 follow/ flying enemies
    m_game_state.enemies[0] = Entity(enemy_texture_id, 0.8f, 0.7f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[0].set_position(glm::vec3(11.0f, -8.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    m_game_state.enemies[3] =  Entity(enemy_fly_texture_id, 1.8f, 0.7f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[3].set_position(glm::vec3(7.0f, -19.0f, 0.0f));
    m_game_state.enemies[3].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[3].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // 1 moving vertically enemies
    m_game_state.enemies[1] =  Entity(enemy_texture_id, 0.8f, 0.7f, 1.0f, ENEMY, WALKER, WALKING);
    m_game_state.enemies[1].set_position(glm::vec3(16.0f, -12.0f, 0.0f));
    m_game_state.enemies[1].set_movement(glm::vec3(0.0f, 1.0f, 0.0f)); // Start moving up
    m_game_state.enemies[1].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // 1 moving horizontally enemy
    m_game_state.enemies[2] =  Entity(enemy_texture_id, 1.0f, 0.7f, 1.0f, ENEMY, WALKER, WALKING);
    m_game_state.enemies[2].set_position(glm::vec3(5.0f, -19.0f, 0.0f));
    m_game_state.enemies[2].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[2].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    /** Rock' stuff */
    GLuint rock_texture_id = Utility::load_texture(ROCK_FILEPATH);
    m_game_state.rocks = new Entity[ROCK_COUNT];
    
    // Add rocks with different tile indices
    m_game_state.rocks[0] = Entity(rock_texture_id,37, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[0].set_position(glm::vec3(7.0f, -3.0f, 0.0f));
    m_game_state.rocks[1] = Entity(rock_texture_id,121, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[1].set_position(glm::vec3(20.0f, -16.2f, 0.0f));
    m_game_state.rocks[2] = Entity(rock_texture_id,35, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[2].set_position(glm::vec3(12.0f, -4.0f, 0.0f));
    m_game_state.rocks[3] = Entity(rock_texture_id,15, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[3].set_position(glm::vec3(17.0f, -6.0f, 0.0f));
    m_game_state.rocks[4] = Entity(rock_texture_id,22, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[4].set_position(glm::vec3(13.0f, -10.0f, 0.0f));
    m_game_state.rocks[5] = Entity(rock_texture_id,37, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[5].set_position(glm::vec3(4.0f, -4.0f, 0.0f));
    m_game_state.rocks[6] = Entity(rock_texture_id,35, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[6].set_position(glm::vec3(8.0f, -6.0f, 0.0f));
    m_game_state.rocks[7] = Entity(rock_texture_id,22, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[7].set_position(glm::vec3(12.0f, -7.0f, 0.0f));
    m_game_state.rocks[8] = Entity(rock_texture_id,37, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[8].set_position(glm::vec3(17.0f, -12.0f, 0.0f));
    m_game_state.rocks[9] = Entity(rock_texture_id,35, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[9].set_position(glm::vec3(13.0f, -16.0f, 0.0f));
    m_game_state.rocks[10] = Entity(rock_texture_id,35, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[10].set_position(glm::vec3(9.0f, -17.0f, 0.0f));
    m_game_state.rocks[11] = Entity(rock_texture_id,37, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[11].set_position(glm::vec3(10.0f, -17.0f, 0.0f));
    m_game_state.rocks[12] = Entity(rock_texture_id,35, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[12].set_position(glm::vec3(9.0f, -23.0f, 0.0f));
    m_game_state.rocks[13] = Entity(rock_texture_id,22, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[13].set_position(glm::vec3(9.0f, -23.0f, 0.0f));
    m_game_state.rocks[14] = Entity(rock_texture_id,22, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[14].set_position(glm::vec3(7.0f, -20.0f, 0.0f));
    m_game_state.rocks[15] = Entity(rock_texture_id,22, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[15].set_position(glm::vec3(5.0f, -22.0f, 0.0f));
    m_game_state.rocks[16] = Entity(rock_texture_id,22, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[16].set_position(glm::vec3(14.0f, -13.0f, 0.0f));
    m_game_state.rocks[17] = Entity(rock_texture_id,35, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[17].set_position(glm::vec3(15.0f, -13.0f, 0.0f));
    m_game_state.rocks[18] = Entity(rock_texture_id,37, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[18].set_position(glm::vec3(10.0f, -20.0f, 0.0f));
    m_game_state.rocks[19] = Entity(rock_texture_id,22, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[19].set_position(glm::vec3(16.0f, -5.0f, 0.0f));
    m_game_state.rocks[20] = Entity(rock_texture_id,35, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[20].set_position(glm::vec3(7.0f, -24.0f, 0.0f));
    m_game_state.rocks[21] = Entity(rock_texture_id,6, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[21].set_position(glm::vec3(13.0f, -12.0f, 0.0f));
    m_game_state.rocks[22] = Entity(rock_texture_id,84, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[22].set_position(glm::vec3(16.0f, -17.0f, 0.0f));
    m_game_state.rocks[23] = Entity(rock_texture_id,35, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[23].set_position(glm::vec3(20.0f, -5.0f, 0.0f));
    m_game_state.rocks[24] = Entity(rock_texture_id,37, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[24].set_position(glm::vec3(10.0f, -21.0f, 0.0f));
    m_game_state.rocks[25] = Entity(rock_texture_id,37, 0.8f, 0.8f, ROCK);
    m_game_state.rocks[25].set_position(glm::vec3(6.2f, -22.0f, 0.0f));
    
}

void LevelB::update(float delta_time)
{
    if (Utility::get_gamePaused() || Utility::get_lives() <= 0) return;
    
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    m_game_state.rocks->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    
    // Check collison with rocks
    m_game_state.player->check_collision_x(m_game_state.rocks, ROCK_COUNT);
    m_game_state.player->check_collision_y(m_game_state.rocks, ROCK_COUNT);
    
    // Check collision with diamond rocks and ladder rock
    for (int i = 0; i < ROCK_COUNT; i++) {
        if (m_game_state.rocks[i].get_tile_index() == 121) {  // 121 is the diamond rock
            bool isColliding = m_game_state.player->check_collision(&m_game_state.rocks[i]);
            if (isColliding) {
                Utility::update_diamonds();
                m_game_state.rocks[i].deactivate(); // Deactivate the diamond rock after collection
                std::cout << "Diamond collected! Current diamonds = " << Utility::get_diamonds() << "/3" << std::endl;
            }
        }
        if (i == 25) {  // Assuming rock index 25 is the ladder-triggering rock
            bool isColliding = m_game_state.player->check_collision(&m_game_state.rocks[i]);
            if (isColliding && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J]) {
                m_game_state.rocks[i].deactivate();  // Deactivate rock after interaction
                
                // Update the LEVELB_DATA to include the ladder (173)
                int ladder_x = 6;  // Update this to the actual position you want
                int ladder_y = 22; // Update this to the actual position you want
                LEVELB_DATA[ladder_y * LEVEL_WIDTH + ladder_x] = 173;  // Update the level data
                
                // Debugging output
                std::cout << "Ladder revealed at position (" << ladder_x << ", " << ladder_y << ") with tile value: " << LEVELB_DATA[ladder_y * LEVEL_WIDTH + ladder_x] << std::endl;
                
                // Rebuild the map
                m_game_state.map->update_level_data(LEVELB_DATA); // Make sure the map uses the updated level data
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
            
            // Update enemy AI behavior
            if (i == 0 || i == 3) {
                m_game_state.enemies[i].ai_follow_player(m_game_state.player);
            }
            else if (i == 2){
                m_game_state.enemies[i].ai_horizontally_walker(5.0f, 12.0f);
            }
            else if (i == 1) {
                m_game_state.enemies[i].ai_vertical_walker(-6.0f, -12.0f);
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
    
    if (m_game_state.player->get_position().x > 6.0f && m_game_state.player->get_position().y == -21.0f) m_game_state.next_scene_id = 1;
    
}

void LevelB::render(ShaderProgram *program)
{
    if (Utility::get_gamePaused()) return;
    
//    std::cout << "X position = " << m_game_state.player->get_position().x << std::endl;
//    std::cout << "Y position = " << m_game_state.player->get_position().y << std::endl;
    
    // Render the map, player, rocks, and active enemies first
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    
    for (int i = 0; i < ROCK_COUNT; i++) {
        m_game_state.rocks[i].render(program);
    }
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (m_game_state.enemies[i].is_active()) {
            m_game_state.enemies[i].render(program);
        }
    }
    
    // Update text position relative to player
    std::string l2_lives_text = "Lives: " + std::to_string(Utility::get_lives());
    std::string l2_diamonds_text = "Diamonds: " + std::to_string(Utility::get_diamonds()) + "/3";
    
    float player_x = m_game_state.player->get_position().x;
    float player_y = m_game_state.player->get_position().y + 1.8f; // Always 1.8f units above the player
    
    // If user go below the y -> fix the lives display
    if(m_game_state.player->get_position().y <= -14.0f){
        Utility::draw_text(program, l2_font_texture_id, l2_lives_text, 1.0f, -0.5f, glm::vec3(player_x - 8.5f, -13.2f, 0.0f));
        Utility::draw_text(program, l2_font_texture_id, l2_diamonds_text, 1.0f, -0.5f, glm::vec3(player_x - 3.0f, -13.2f, 0.0f));
    } else{
        Utility::draw_text(program, l2_font_texture_id, l2_lives_text, 1.0f, -0.6f, glm::vec3(player_x - 8.5f, player_y, 0.0f));
        Utility::draw_text(program, l2_font_texture_id, l2_diamonds_text, 1.0f, -0.5f, glm::vec3(player_x - 4.0f, player_y, 0.0f));
    }
    
    // Check if the player has run out of lives
    if (Utility::get_lives() <= 0) {
        // This is when the camera is fixed for the bottom part of the map
        if(m_game_state.player->get_position().y <= -16.0f){
            Utility::draw_text(program, l2_font_texture_id, l2_lives_text, 1.0f, -0.5f, glm::vec3(player_x - 8.5f, -13.2f, 0.0f));
            Utility::draw_text(program, l2_font_texture_id, l2_losing_text, 1.5f, 0.05f,
                               glm::vec3(m_game_state.player->get_position().x - 4.5f, m_game_state.player->get_position().y + 1.8f, 0.0f));
        }else{
            Utility::draw_text(program, l2_font_texture_id, l2_lives_text, 1.0f, -0.6f, glm::vec3(player_x - 8.5f, player_y, 0.0f));
            Utility::draw_text(program, l2_font_texture_id, l2_losing_text, 1.5f, 0.05f,
                               glm::vec3(m_game_state.player->get_position().x - 4.0f, m_game_state.player->get_position().y - 4.0f, 0.0f));
        }
        return;
    }
}
