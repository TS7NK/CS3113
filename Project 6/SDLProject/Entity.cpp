#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

void Entity::ai_activate(Entity *player)
{
    switch (m_ai_type)
    {
        case WALKER:
            ai_walk();
            break;
            
        case GUARD:
            ai_guard(player);
            break;
            
        default:
            break;
    }
}

void Entity::ai_walk()
{
    m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
}

void Entity::ai_guard(Entity *player)
{
    switch (m_ai_state) {
        case IDLE:
            std::cout << "Enemy in IDLE state" << std::endl;
            // Transition to walking state if player is close enough
            if (glm::distance(m_position, player->get_position()) < 5.0f){
                std::cout << "Enemy transit from IDLE to Walking" << std::endl;
                m_ai_state = WALKING;
            }
            break;
            
        case WALKING: {
            std::cout << "Enemy in walking state" << std::endl;
            // Calculate direction towards the player
            glm::vec3 direction = player->get_position() - m_position;

            if (glm::length(direction) > 0.01f) {  // Prevent division by zero
                direction = glm::normalize(direction);
            }
            std::cout << "The X distance between enemy and player is = " << direction.x << std::endl;
            std::cout << "The Y distance between enemy and player is = " << direction.y << std::endl;

            m_movement = direction;  // Set the movement towards the player
            break;
        }
            
        case ATTACKING:
            break;
            
        default:
            break;
    }
}

void Entity::ai_vertical_walker(float upperLimit, float lowerLimit)
{
    if (m_position.y >= upperLimit)
    {
        m_movement.y = -1.0f;  // Move down
    }
    else if (m_position.y <= lowerLimit)
    {
        m_movement.y = 1.0f;   // Move up
    }
}

void Entity::ai_horizontally_walker(float leftLimit, float rightLimit)
{
    if (m_position.x >= rightLimit)
    {
        m_movement.x = -1.0f;  // Move left
    }
    else if (m_position.x <= leftLimit)
    {
        m_movement.x = 1.0f;   // Move right
    }
}

void Entity::ai_follow_player(Entity *player) {
    // Calculate the distance between the enemy and the player
    float distance = glm::length(player->get_position() - this->get_position());

    // Check if the player is within 5.0 units
    if (distance < 5.0f) {
        // Get the direction vector from enemy to player
        glm::vec3 direction = player->get_position() - this->get_position();

        // Normalize the direction to get only direction without magnitude
        glm::vec3 movement = glm::normalize(direction);

        // Set the enemy movement
        set_movement(movement * m_speed);
    }
}

void Entity::ai_fly_across_screen()
{
    // Set movement to a fixed diagonal direction (both x and y movement components are non-zero)
    // This makes the enemy move in a diagonal direction continuously.
    if (get_movement() == glm::vec3(0.0f,0.0f,0.0f)) {
        this->set_movement(glm::vec3(4.0f,4.0f, 0.0f)); // Adjust these values to control the speed and direction
    }
}



// Default constructor
Entity::Entity()
: m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}

Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, int walking[5][6], float animation_time,
               int animation_frames, int animation_index, int animation_cols,
               int animation_rows, float width, float height, EntityType EntityType)
: m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
m_speed(speed), m_acceleration(acceleration), m_jumping_power(jump_power), m_animation_cols(animation_cols),
m_animation_frames(animation_frames), m_animation_index(animation_index),
m_animation_rows(animation_rows), m_animation_indices(nullptr),
m_animation_time(animation_time), m_texture_id(texture_id), m_velocity(0.0f),
m_width(width), m_height(height), m_entity_type(EntityType)
{
    set_walking(walking);
}

// Simpler constructor for partial initialization
Entity::Entity(GLuint texture_id, float speed,  float width, float height, EntityType EntityType)
: m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height),m_entity_type(EntityType)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}

Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState): m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height),m_entity_type(EntityType), m_ai_type(AIType), m_ai_state(AIState)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}

// Constructor specifically for rocks (or other static objects)
Entity::Entity(GLuint texture_id, int tile_index, float width, float height, EntityType entity_type)
:m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
m_speed(0.0f), m_animation_cols(23), m_animation_rows(6), m_width(width), m_height(height),
m_entity_type(entity_type), m_texture_id(texture_id), m_tile_index(tile_index)
{
    // rock_tilesheet has 6 rows and 23 columns
    m_animation_cols = 23; // Number of columns in the tilesheet
    m_animation_rows = 6; // Number of rows in the tilesheet
}


Entity::~Entity() { }

float Entity::get_uv_offset(int tile_index) {
    // Example offsets for specific tiles
    switch (tile_index) {
        case 121: // Diamond
            return -0.010f;
        case 37:  // Normal rock
            return -0.024f;
        case 35:  // lighter rock
            return -0.024f;
        case 15:  // 放射性物质
            return -0.024f;
        case 22:  // 普通石头
            return -0.007f;
        case 84:  // 粉色矿物
            return -0.024f;
        case 6:  // 紫水晶
            return -0.010f;
        case 45: //神秘石头
            return -0.04f;
        case 50: // bue rock big
            return -0.010f;
        case 12: // bue rock big
            return -0.024f;
        default:  // No offset for most tiles
            return 0.0f;
    }
}

int Entity::get_tile_index() const {
    return m_tile_index;
}

// Added for rock and other static tilesheets
void Entity::draw_static_sprite(ShaderProgram* program, GLuint texture_id, int tile_index) {
    // Validate tile index
    if (tile_index < 0 || tile_index >= (m_animation_cols * m_animation_rows)) {
        std::cerr << "Invalid tile index: " << tile_index << std::endl;
        return;
    }
    
    // Calculate UV coordinates for the static tile
    // Retrieve the UV offset for the given tile_index
    float uv_offset = get_uv_offset(tile_index);
    //    float uv_offset = -0.017f; // Fine-tuning offset
    //    float u_coord = (float)(tile_index % m_animation_cols) / (float)m_animation_cols;
    float u_coord = ((tile_index % m_animation_cols) / (float)m_animation_cols) + uv_offset;
    float v_coord = ((tile_index / m_animation_cols) / (float)m_animation_rows);
    
    float tile_width = (1.0f / m_animation_cols);
    float tile_height = (1.0f / m_animation_rows);
    
    // Define texture coordinates
    float tex_coords[] = {
        u_coord, v_coord + tile_height,   // Bottom-left
        u_coord + tile_width, v_coord + tile_height, // Bottom-right
        u_coord + tile_width, v_coord,   // Top-right
        u_coord, v_coord                 // Top-left
    };
    
    // Define vertices for a 1x1 quad
    float vertices[] = {
        -0.5f, -0.5f,   // Bottom-left
        0.5f, -0.5f,   // Bottom-right
        0.5f,  0.5f,   // Top-right
        -0.5f,  0.5f    // Top-left
    };
    
    // Bind the texture and set up vertex attributes
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    // Draw the quad
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    // Disable vertex attributes after drawing
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}


void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;
    
    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;
    
    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };
    
    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };
    
    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool const Entity::check_collision(Entity* other) const
{
    if(!other->m_is_active) return false;
    if (other == this) return false;
    
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);
    
    bool is_colliding = x_distance <= 0.0f && y_distance <= 0.0f;
    // std::cout << "Collision result: " << (is_colliding ? "true" : "false") << std::endl;
    return is_colliding;
    
    // return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_y(Entity *collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));
            if (m_velocity.y > 0)
            {
                m_position.y   -= y_overlap;
                m_velocity.y    = 0;
                
                // Collision!
                m_collided_top  = true;
            } else if (m_velocity.y < 0)
            {
                m_position.y      += y_overlap;
                m_velocity.y       = 0;
                
                // Collision!
                m_collided_bottom  = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity *collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            if (m_velocity.x > 0)
            {
                m_position.x     -= x_overlap;
                m_velocity.x      = 0;
                
                // Collision!
                m_collided_right  = true;
                
            } else if (m_velocity.x < 0)
            {
                m_position.x    += x_overlap;
                m_velocity.x     = 0;
                
                // Collision!
                m_collided_left  = true;
            }
        }
    }
}

void const Entity::check_collision_y(Map *map)
{
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    
    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y, 0) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y, 0) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y, 0) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    
    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y, 0) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y, 0) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y, 0) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
}

void const Entity::check_collision_x(Map *map)
{
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left  = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->is_solid(left, &penetration_x, &penetration_y, 0) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y, 0) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}
void Entity::update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map)
{
    //    if(gamePaused) return;
    
    if (!m_is_active) return;
    
    m_collided_top    = false;
    m_collided_bottom = false;
    m_collided_left   = false;
    m_collided_right  = false;
    
    if (m_entity_type == ENEMY) ai_activate(player);
    
    // Update animation if there's movement or attack
    if (m_animation_indices != NULL)
    {
        if (glm::length(m_movement) != 0)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float) 1 / SECONDS_PER_FRAME;
            
            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }
    
    m_velocity.x = m_movement.x * m_speed;
    m_velocity += m_acceleration * delta_time;
    
    m_velocity.y = m_movement.y * m_speed;
    m_velocity += m_acceleration * delta_time;
    
    if (m_is_jumping)
    {
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
    }
    
    m_position.y += m_velocity.y * delta_time;
    
    check_collision_y(collidable_entities, collidable_entity_count);
    check_collision_y(map);
    
    m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count);
    check_collision_x(map);
    
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
}


void Entity::render(ShaderProgram* program)
{
    if(!m_is_active) return;
    
    // Adjust the player's scale to make them larger
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    
    // Only scale the player entity, not rocks or other entities
    if (m_entity_type == PLAYER) {
        m_model_matrix = glm::scale(m_model_matrix, glm::vec3(4.0f, 4.0f, 1.0f));  // Scaling by 2x
    }
    if (m_entity_type == ROCK) {
        m_model_matrix = glm::scale(m_model_matrix, glm::vec3(0.9f, 0.9f, 1.0f));  // Scaling by 2x
    }
    
    program->set_model_matrix(m_model_matrix);
    
    // Check if this is a static entity (like a rock)
    if (m_entity_type == ROCK) {
        draw_static_sprite(program, m_texture_id, m_tile_index);
        return;
    }
    
    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }
    
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
