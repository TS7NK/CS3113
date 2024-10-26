#ifndef ENTITY_H
#define ENTITY_H

#include "glm/glm.hpp"
#include "ShaderProgram.h"
enum EntityType { PLATFORM, PLAYER};

class Entity
{
private:
    bool m_is_active = true;
    float acceleration_timer; // Timer to track how long acceleration should be applied

    EntityType m_entity_type;
    
    // ————— TRANSFORMATIONS ————— //
    glm::vec3 m_movement;
    glm::vec3 m_scale;
    
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    glm::mat4 m_model_matrix;

    int m_width  = 1,
        m_height = 1;
    
    float m_speed;

    // ————— TEXTURES ————— //
    GLuint    m_texture_id;

    // ————— COLLISIONS ————— //
    bool m_collided_top    = false;
    bool m_collided_bottom = false;
    bool m_collided_left   = false;
    bool m_collided_right  = false;
    
public:
    bool isWinningTile;
    
    void set_acceleration_timer(float time); // Set the timer for acceleration duration
    void update_acceleration_timer(float delta_time); // Update the timer during each update cycle
    void add_acceleration(const glm::vec3& acceleration); 
    
    // ————— STATIC VARIABLES ————— //
    static constexpr int SECONDS_PER_FRAME = 4;
    
    // ————— METHODS ————— //
    Entity();
    Entity(GLuint textureID, float speed, glm::vec3 acceleration, float width, float height, EntityType entityType,bool isWinningTile);
    //Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float width, float height, EntityType EntityType,bool isWinningTile);
    Entity(GLuint texture_id, glm::vec3 acceleration, float speed, float width, float height, EntityType EntityType,bool isWinningTile); // Simpler constructor
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    bool const check_collision(Entity* other) const;
    
    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);
    // This is where keeping Entity objects nicely organised works nicely
    void update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count);
    void render(ShaderProgram* program);
    
    void normalise_movement() { m_movement = glm::normalize(m_movement);}
    
    void move_left() { m_movement.x = -1.0f;}
    void move_right() { m_movement.x = 1.0f;}
    void move_up() { m_movement.y = 1.0f;}
    void move_down() { m_movement.y = -1.0f;}
    
    // ————— GETTERS ————— //
    EntityType const get_entity_type()    const { return m_entity_type;   };
    
    glm::vec3 const get_position()     const { return m_position; }
    glm::vec3 const get_velocity()     const { return m_velocity; }
    glm::vec3 const get_acceleration() const { return m_acceleration; }
    glm::vec3 const get_movement()     const { return m_movement; }
    
    glm::vec3 const get_scale()        const { return m_scale; }
    GLuint    const get_texture_id()   const { return m_texture_id; }
    float     const get_speed()        const { return m_speed; }
    bool      const get_collided_top() const { return m_collided_top; }
    bool      const get_collided_bottom() const { return m_collided_bottom; }
    bool      const get_collided_right() const { return m_collided_right; }
    bool      const get_collided_left() const { return m_collided_left; }
    float get_width() const { return m_width; }
    float get_height() const { return m_height; }
    
    void activate()   { m_is_active = true;  };
    void deactivate() { m_is_active = false; };
    // ————— SETTERS ————— //
    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;};
    
    void const set_position(glm::vec3 new_position) { m_position = new_position; }
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    
    // To simulate gradual stopping
    void apply_drag(float drag_coefficient);
    
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; }
    
    void const set_scale(glm::vec3 new_scale) { m_scale = new_scale; }
    void const set_texture_id(GLuint new_texture_id) { m_texture_id = new_texture_id; }
    void const set_speed(float new_speed) { m_speed = new_speed; }
    void const set_width(float new_width) {m_width = new_width; }
    void const set_height(float new_height) {m_height = new_height; }

};
#endif // ENTITY_H
