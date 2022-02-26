// internal
#include "combat_system.hpp"
#include "ai_system.hpp"
#include "world_system.hpp"

// Attacker deals damage to Defender based on a "multiplier" of the attacker's ATK stat
// Parameters subject to change
std::string deal_damage(Entity& attacker, Entity& defender, float multiplier)
{
	// Pre-calculation effects
	// TODO: add effects

	// Damage Calculation
	Stats& attacker_stats = registry.stats.get(attacker);
	Stats& defender_stats = registry.stats.get(defender);
	float final_damage = calc_damage(attacker_stats, defender_stats, multiplier);

	// Post-calculation effects
	// TODO: add effects

	// Defender take damage
	take_damage(defender, final_damage);

	// Temp logging
	std::string attacker_name = "";
	std::string defender_name = "";
	
	attacker_name = attacker_stats.prefix + " " + attacker_stats.name;
	attacker_name[0] = toupper(attacker_name[0]);

	defender_name = defender_stats.prefix + " " + defender_stats.name;

	if (registry.players.has(attacker)) { attacker_name = "You"; }
	else if (registry.players.has(defender)) { defender_name = "you"; }

	int logged_damage = round(final_damage);

	std::string log = attacker_name.append(" attacked ").append(defender_name).append(" for ").append(std::to_string(logged_damage)).append(" damage!");
	return log;
}

// Entity directly takes damage
void take_damage(Entity& entity, float damage)
{
	Stats& stats = registry.stats.get(entity);
	stats.hp -= damage;
}

void take_damage_mp(Entity& entity, float damage)
{
	Stats& stats = registry.stats.get(entity);
	stats.mp -= damage;
}

void take_damage_ep(Entity& entity, float damage)
{
	Stats& stats = registry.stats.get(entity);
	stats.ep -= damage;
}

// Damage calculation
float calc_damage(Stats& attacker, Stats& defender, float multiplier)
{
	float final_damage = attacker.atk * multiplier / 100;
	final_damage -= defender.def;

	if (defender.guard) {
		final_damage /= 2.f;
	}

	// Minimum damage is 1
	final_damage = max(1.f, final_damage);
	
	return final_damage;
}