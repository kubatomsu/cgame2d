
#include "game.h"
#include "graphics.h"
#include "entity.h"
#include "input.h"
#include "engine.h"
#include "m_math.h"

unsigned int player_texture_run_horizontal;
unsigned int player_texture_run_up;
unsigned int player_texture_run_down;
unsigned int player_fist_texture;
unsigned int enemy_texture;
int player_entity_id = -1;
entity_t* player_entity;


// id = unique number for each entity.
// entity = pointer to entity that we should update
void exaple_update_function(int id, entity_t* entity) {
    
    // all entity variables can be found in entity.cpp in struct entity_t
    entity->position.x += 0.1f;
    
}

void player_bullet_update(int id, entity_t* entity) {
    entity->position += entity->velocity * delta_time;
    entity->time += delta_time;
    if(entity->time > 0.06f) entity_destroy(id);
}

m_v2 player_cursor = {0.5,0.5};
void player_update(int id, entity_t* entity) { 
    m_v2 dir = {};
    
#define  PLAYER_ON_RUN entity->texture_scale.x = 1.0f / 4.0f;
#define PLAYER_ON_STOP_RUNNING entity->texture_scale.x = 0.25f; entity->time = 0;
    
    
    
    if(input_down(GLFW_KEY_W)){
        dir.y++;
        entity->texture = player_texture_run_up;
        PLAYER_ON_RUN;
    }
    if(input_down(GLFW_KEY_S)){
        dir.y--;
        entity->texture = player_texture_run_down;
        PLAYER_ON_RUN;
    }
    if(input_down(GLFW_KEY_D)){
        dir.x++;
        entity->scale.x = 1;
        entity->texture = player_texture_run_horizontal;
        PLAYER_ON_RUN;
    }
    if(input_down(GLFW_KEY_A)){
        dir.x--;
        entity->scale.x = -1;
        entity->texture = player_texture_run_horizontal;
        PLAYER_ON_RUN;
    }
    
    const float mouse_speed = 20;
    player_cursor += input_mouse_offset* mouse_speed * m_v2{1,-1} / m_v2{(float)window_x, (float)window_y};
    m_v2 cursor_ndir = m_v2_normalize(player_cursor);
    player_cursor = m_v2_lerp(player_cursor, cursor_ndir * 5, delta_time * 40);
    cursor_pos = entity->position + player_cursor;
    
    
    
    
    entity->velocity = entity->velocity / ( 1.0f + (delta_time * 15.0f));
    entity->velocity += dir * delta_time * 350.0f;
    
    if(input_pressed(GLFW_KEY_SPACE)) entity->velocity += dir * 180.0f;
    
    entity->position += entity->velocity * delta_time;
    
    if(dir != m_v2{}) {
        if(entity->time > 1.0f){
            entity->texture_offset.x += 1.0f;
            entity->time = 0;
            entity->mode = 0;
        }
        entity->time += delta_time * 14;
    }else{
        entity->texture_offset.x = 1.0f;
        entity->time = 0;
    }
    
    
    
    if(input_pressed(GLFW_MOUSE_BUTTON_1)){
        entity_t bullet = ENTITY_DEFAULT;
        bullet.position = entity->position + cursor_ndir * 1.4f;
        bullet.texture = player_fist_texture;
        bullet.velocity = cursor_ndir * 30.0f;
        bullet.update_func = player_bullet_update;
        entity_spawn(bullet);
    }
    
    
    
    main_camera.position = m_v2_lerp(
                                     main_camera.position,
                                     entity->position + entity->velocity * 0.1f + player_cursor * 0.35f,
                                     delta_time * 8.0f
                                     );
    
}

void projectile_update(int id, entity_t* entity) {
    entity->position += entity->velocity;
    entity->color -= m_v3{0.8,1,1} * delta_time * 0.05f;
    
}

void enemy_update(int id, entity_t* entity) {
    if(entity->time > 1.0f){
        
    }
    entity->time += delta_time;
}

void game_initialize() {
    
    player_texture_run_down = texture_import("player_run_down.png", GL_NEAREST, GL_REPEAT);
    player_texture_run_up = texture_import("player_run_up.png", GL_NEAREST, GL_REPEAT);
    player_texture_run_horizontal = texture_import("player_run_horizontal.png", GL_NEAREST, GL_REPEAT);
    
    player_fist_texture = texture_import("fist.png", GL_LINEAR, GL_REPEAT);
    
    enemy_texture = texture_import("test.png", GL_LINEAR, GL_REPEAT);
    
}

void game_load_level() {
    
    
    entity_t e = ENTITY_DEFAULT;
    e.position = { 2, 4 };
    e.texture = player_texture_run_up;
    e.update_func = player_update;
    e.color = {1,1,1};
    e.texture_scale = {0.25,1};
    e.health = 4;
    player_entity_id = entity_spawn(e);
    
    
    for(int i = 0; i < 10; i++) {
        entity_t e = ENTITY_DEFAULT;
        e.position = m_randv2() * 10;
        e.update_func = enemy_update;
        e.texture = enemy_texture;
        //entity_spawn(e);
    }
    
    
    
    for(int i = 0; i < 10; i++) {
        entity_t e = ENTITY_DEFAULT;
        e.position = m_randv2() * 10;
        e.scale = {2,2};
        e.texture = 0;
        //e.flags.collision_static = 1;
        entity_spawn(e);
    }
    
};

void game_early_update() {
    
}

void game_late_update() {
    if(player_entity_id >= 0) {
        player_entity = entity_get(player_entity_id);
    }
    
    printf("player ptr = 0x%x\n",(int)player_entity);
    
}

void game_on_render_update() {
    
    for(int i = 0; i < player_entity->health; i++) {
        draw_quad(
                  camera_ndc_to_world(&main_camera, { -0.5, 0.4}),
                  {(float)main_camera.distance * 0.1f,(float)main_camera.distance * 0.1f},
                  enemy_texture,
                  {1,1},
                  {},
                  {1,0,0}
                  );
    }
}