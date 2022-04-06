#pragma once

#include <vector>
#include <algorithm>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"


std::string deal_damage(Entity& attacker, Entity& defender, float multiplier, bool doProcs = true);
void take_damage(Entity& entity, float damage);
void take_damage_mp(Entity& entity, float damage);
void take_damage_ep(Entity& entity, float damage);
float calc_damage(Entity& attacker, Entity& defender, float multiplier);
float handle_postcalc_effects(Entity& attacker, Entity& defender, float damage);
void apply_status(Entity& target, StatusEffect& status);
void handle_status_ticks(Entity& entity, bool applied_from_turn_start, bool stats_only);
void reset_stats(Entity& entity);
void calc_stats(Entity& entity);
void heal(Entity& entity, float amount);
Equipment equip_item(Entity& entity, Equipment& equipment);
Equipment unequip_item(Entity& entity, int slot);
