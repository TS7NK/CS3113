#include "Level A.h"
#include "Utility.h"

#define LEVEL_WIDTH 23
#define LEVEL_HEIGHT 12

constexpr char SPRITESHEET_FILEPATH[] = "Soldier.png",
l1_FONTSHEET_FILEPATH[] = "font1.png";

GLuint l1_font_texture_id;
std::string l1_instruction = "A(<-) D(->) W(Up) S(Down) J(Attack) I(Info)";


unsigned int LEVELA_DATA[] =
{
    0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,0,68,74,73,76,75,93,0,0, 0, 0,0, 70,73,74,75,111,0, 0, 0, 0, 0,
    0,0,84,89,254,65,66,109,110,110,110,110,110,299,300,301,302,298,0, 0, 0, 0, 0,
    0,0,100,105,270,81,82,106,126,126,126,126,126,315,316,317,318,314,0, 0, 0, 0, 0,
    0,0,116,105,286,106,107,108,107,108,107,108,107,331,332,333,334,330,0, 0, 0, 0, 0,
    0,0,116,155,140,167,168,169,170,167,172,168,169,347,348,349,350,346,0, 0, 0, 0, 0,
    0,0,116,149,150,167,198,169,170,172,155,171,172,169,17,18,19,330, 0, 0, 0, 0,
    0,0,0,100,168,169,172,171,167,138,139,140,153,154,155,33,34,35,330, 0, 0, 0, 0, 0, 0, 0,
    0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
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
    
    GLuint map_texture_id = Utility::load_texture("Skull Cavern Opening.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 16, 22, 1);
    
    l1_font_texture_id = Utility::load_texture(l1_FONTSHEET_FILEPATH);
    
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
    
    /** Enemies' stuff  */
    m_game_state.enemies = new Entity[ENEMY_COUNT];
    
}

void LevelA::update(float delta_time)
{
    if(Utility::get_gamePaused()) return;
    
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    
    if (m_game_state.player->get_position().x >= 3.85f && m_game_state.player->get_position().y == -5.0f) m_game_state.next_scene_id = 1;
    
}

void LevelA::render(ShaderProgram *program)
{
//    std::cout << "x position = " << m_game_state.player->get_position().x << std::endl;
//    std::cout << "y position = " << m_game_state.player->get_position().y << std::endl;
    
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    
    // Update text position relative to player
    std::string l1_lives_text = "Lives: " + std::to_string(Utility::get_lives());
    std::string l1_diamonds_text = "Diamonds: " + std::to_string(Utility::get_diamonds()) + "/3";
    
    float player_x = m_game_state.player->get_position().x;
    float player_y = m_game_state.player->get_position().y + 5.0f; // Always 1.8f units above the player
    
    Utility::draw_text(program, l1_font_texture_id, l1_instruction, 0.9f, -0.5f, glm::vec3(player_x -6.5f, player_y, 0.0f));
}
