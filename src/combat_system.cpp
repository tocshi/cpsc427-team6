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

void apply_status(Entity& target, StatusEffect& status) {
	if (!registry.statuses.has(target)) {
		StatusContainer& statusContainer = registry.statuses.emplace(target);
		statusContainer.statuses.push_back(status);
	}
	else {
		StatusContainer& statusContainer = registry.statuses.get(target);
		statusContainer.statuses.push_back(status);
	}
}

// call this function once at turn start (2nd param=true), and once at turn end (2nd param=false)
void handle_status_ticks(Entity& entity, bool applied_from_turn_start) {
	if (registry.statuses.has(entity)) {
		StatusContainer& statusContainer = registry.statuses.get(entity);
		for (int i = statusContainer.statuses.size() - 1; i >= 0; i--) {
			StatusEffect& status = statusContainer.statuses[i];
			// in case something was accidentally added with 0 turn duration
			if (status.turns_remaining <= 0) {
				statusContainer.statuses.erase(statusContainer.statuses.begin()+i);
				continue;
			}
			if (status.apply_at_turn_start != applied_from_turn_start) {
				continue;
			}
			switch (status.effect) {
				case (StatusType::POISON):
					if (status.percentage && registry.stats.has(entity)) {
						Stats stats = registry.stats.get(entity);
						take_damage(entity, stats.maxhp * status.value);
					}
					else {
						take_damage(entity, status.value);
						printf("took DoT of %f", status.value);
					}
					break;
				case (StatusType::ATK_BUFF):
					Stats stats = registry.stats.get(entity);
					if (status.percentage && registry.stats.has(entity)) {
						stats.atk *= 1 + status.value;
					}
					else {
						stats.atk += status.value;
					}
					break;
			}
			// properly remove statuses that have expired, except for things with >=999 turns (we treat those as infinite)
			if (status.turns_remaining >= 999) {
				status.turns_remaining--;
			}
			if (status.turns_remaining <= 0) {
				statusContainer.statuses.erase(statusContainer.statuses.begin() + i);
			}
		}
	}
}