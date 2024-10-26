/**
* Author: Tingting Min
* Assignment: Lunar Lander
* Date due: 2024-10-26, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
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

// Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(0.5f, 0.5f, 0.5f), m_model_matrix(1.0f),
    m_speed(0.0f),
m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f), isWinningTile(false)
{
    // Initialize m_walking with zeros or any default value
}

// Simpler constructor for partial initialization
Entity::Entity(GLuint texture_id, glm::vec3 acceleration, float speed,  float width, float height, EntityType EntityType, bool isWinningTile)
    : m_texture_id(texture_id), m_acceleration(0.0f), m_speed(speed), m_width(width), m_height(height),m_entity_type(EntityType),m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f), isWinningTile(false)
    {
    // Initialize m_walking with zeros or any default value,
    }

Entity::Entity(GLuint textureID, float speed, glm::vec3 acceleration,float width, float height, EntityType entityType, bool isWinningTile) {
    // Initialize m_walking with zeros or any default value,
}

Entity::~Entity() { }

bool const Entity::check_collision(Entity* other) const
{
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void Entity::apply_drag(float drag_coefficient) {
    m_velocity.x *= drag_coefficient;
    m_velocity.y *= drag_coefficient;
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

void Entity::set_acceleration_timer(float time) {
    acceleration_timer = time;
}

void Entity::update_acceleration_timer(float delta_time) {
    if (acceleration_timer > 0.0f) {
        acceleration_timer -= delta_time;
        if (acceleration_timer <= 0.0f) {
            // When timer runs out, stop the horizontal acceleration
            m_acceleration.x = 0.0f;
        }
    }
}

void Entity::add_acceleration(const glm::vec3& acceleration) {
    m_acceleration += acceleration;
}

void Entity::update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count)
{
    if (!m_is_active) return;
    
    // Apply gravity acceleration continuously when not colliding with the bottom
        if (!m_collided_bottom) {
            m_acceleration.y = -20.62f; // Adjust this value for faster falling
        } else {
            m_acceleration.y = 0.0f;  // Stop gravity when on a platform
        }
    
    // Update the acceleration timer
    update_acceleration_timer(delta_time);

    // Update velocity based on current acceleration
    m_velocity += m_acceleration * delta_time;

    // Apply velocity to position
    m_position += m_velocity * delta_time;

    // Apply some form of drag to reduce velocity over time for a drifting effect
    float drag = 0.7f;  // This value can be adjusted for stronger/weaker drag
    m_velocity.x *= drag;
    
    // Check for collisions
    for (int i = 0; i < collidable_entity_count; i++){
        if (check_collision(&collidable_entities[i])) return;
    }

    m_collided_top    = false;
    m_collided_bottom = false;
    m_collided_left   = false;
    m_collided_right  = false;
    
    m_velocity.y = m_movement.y * m_speed;
    
    check_collision_y(collidable_entities, collidable_entity_count);
    check_collision_x(collidable_entities, collidable_entity_count);
    
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}


void Entity::render(ShaderProgram* program)
{
    if (!m_is_active) return;
    
    program->set_model_matrix(m_model_matrix);

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
