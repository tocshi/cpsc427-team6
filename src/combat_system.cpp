// internal
#include "combat_system.hpp"
#include "ai_system.hpp"
#include "world_system.hpp"
#include "physics_system.hpp"

// Attacker deals damage to Defender based on a "multiplier" of the attacker's ATK stat
// Parameters subject to change
std::string deal_damage(Entity& attacker, Entity& defender, float multiplier)
{
	// Damage Calculation
	Stats& attacker_stats = registry.stats.get(attacker);
	Stats& defender_stats = registry.stats.get(defender);
	float final_damage = calc_damage(attacker, defender, multiplier);

	// Post-calculation effects
	final_damage = handle_postcalc_effects(attacker, defender, final_damage);

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

	// Defender take damage
	take_damage(defender, final_damage);

	// Set hit_by_player status
	if (registry.players.has(attacker) && registry.enemies.has(defender)) {
		auto& enemy_struct = registry.enemies.get(defender);
		enemy_struct.hit_by_player = true;
	}

	return log;
}

// Entity directly takes damage
void take_damage(Entity& entity, float damage)
{
	Stats& stats = registry.stats.get(entity);
	Inventory& inv = registry.inventories.get(entity);

	// Status Effects:
	if (registry.statuses.has(entity)) {
		StatusContainer& statuses = registry.statuses.get(entity);

		// Invincibility check
		if (has_status(entity, StatusType::INVINCIBLE)) {
			damage = 0.f;
		}
	}

	stats.hp -= damage;

	// Effects:
	// Unnecessarily Thick Tome
	if (inv.artifact[(int)ARTIFACT::THICK_TOME] > 0) {
		if (stats.hp <= 0) {
			stats.hp = 1;
			StatusEffect invincible = StatusEffect(0, 3, StatusType::INVINCIBLE, false, false);
			apply_status(entity, invincible);

			inv.artifact[(int)ARTIFACT::THICK_TOME]--;
			world.logText("The Unnecessarily Thick Tome miraculously blocks the fatal blow and disintegrates!");
		}
	}
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
float calc_damage(Entity& attacker, Entity& defender, float multiplier)
{
	Stats& attacker_stats = registry.stats.get(attacker);
	Stats& defender_stats = registry.stats.get(defender);
	Motion& attacker_motion = registry.motions.get(attacker);
	Motion& defender_motion = registry.motions.get(defender);
	Inventory& attacker_inv = registry.inventories.get(attacker);
	Inventory& defender_inv = registry.inventories.get(defender);

	float attacker_atk = attacker_stats.atk;
	float defender_def = defender_stats.def;

	// Pre-calculation effects
	// Goliath's Belt
	if (attacker_inv.artifact[(int)ARTIFACT::GOLIATH_BELT] > 0 && attacker_stats.hp >= attacker_stats.maxhp * 0.8) {
		attacker_atk *= 1 + (0.2 * attacker_inv.artifact[(int)ARTIFACT::GOLIATH_BELT]);
	}

	// Blood Ruby
	if (attacker_inv.artifact[(int)ARTIFACT::GOLIATH_BELT] > 0 && attacker_stats.hp <= attacker_stats.maxhp * 0.4) {
		attacker_atk *= 1 + (0.2 * attacker_inv.artifact[(int)ARTIFACT::GOLIATH_BELT]);
	}

	float final_damage = attacker_atk * multiplier / 100;
	final_damage -= defender_def;

	if (defender_stats.guard) {
		final_damage /= 2.f;
	}

	// Minimum damage is 1
	final_damage = max(1.f, final_damage);
	
	return final_damage;
}

float handle_postcalc_effects(Entity& attacker, Entity& defender, float damage) {
	float final_damage = damage;
	Stats& attacker_stats = registry.stats.get(attacker);
	Stats& defender_stats = registry.stats.get(defender);
	Motion& attacker_motion = registry.motions.get(attacker);
	Motion& defender_motion = registry.motions.get(defender);
	Inventory& attacker_inv = registry.inventories.get(attacker);
	Inventory& defender_inv = registry.inventories.get(defender);

	// Go through every effect
	// Trust me, I am not YandereDev

	// Effects that trigger only when an enemy is the attacker
	if (registry.enemies.has(attacker)) {
		Enemy enemy = registry.enemies.get(attacker);
		// Gladiator's Hoplon
		if (defender_inv.artifact[(int)ARTIFACT::GLAD_HOPLON] > 0 && enemy.hit_by_player) {
			int stack = defender_inv.artifact[(int)ARTIFACT::GLAD_HOPLON];
			final_damage *= 0.85f;
			while (stack > 1) {
				final_damage *= 0.9f;
				stack--;
			}
		}
	}

	// Discarded Fang
	if (attacker_inv.artifact[(int)ARTIFACT::POISON_FANG] > 0) {
		int roll = irand(100);
		if (roll < 30) {
			world.logText("DEBUG: Discarded Fang Procced!");
			float damage = (attacker_stats.atk * 0.15) + (attacker_stats.atk * 0.1 * attacker_inv.artifact[(int)ARTIFACT::POISON_FANG] - 1);
			std::cout << damage << std::endl;

			StatusEffect poison = StatusEffect(damage, 5, StatusType::FANG_POISON, false, false);
			if (has_status(defender, StatusType::FANG_POISON)) { remove_status(defender, StatusType::FANG_POISON); }
			apply_status(defender, poison);
		}
	}

	// Thunderstruck Twig
	if (attacker_inv.artifact[(int)ARTIFACT::THUNDER_TWIG] > 0) {
		int roll = irand(100);
		int chance = 15 * attacker_inv.artifact[(int)ARTIFACT::THUNDER_TWIG];
		if (roll < chance) {
			world.logText("DEBUG: Thunderstruck Twig Procced!");
			float damage = attacker_stats.atk * 0.60;

			// TODO: do thunder twig effect here
		}
	}

	// Lucky Chip
	if (attacker_inv.artifact[(int)ARTIFACT::LUCKY_CHIP] > 0) {
		int roll = irand(100);
		if (roll < 7 * attacker_inv.artifact[(int)ARTIFACT::LUCKY_CHIP]) {
			world.logText("You feel extremely lucky!");
			final_damage *= 7.77f;
		}
	}
	if (defender_inv.artifact[(int)ARTIFACT::LUCKY_CHIP] > 0) {
		int roll = irand(100);
		if (roll < 7 * defender_inv.artifact[(int)ARTIFACT::LUCKY_CHIP]) {
			world.logText("You feel extremely lucky!");
			final_damage = max(1.f,final_damage - 777.f);
		}
	}

	// Pious Prayer
	if (defender_inv.artifact[(int)ARTIFACT::PIOUS_PRAYER] > 0) {
		final_damage = max(1.f, final_damage - 3.f * defender_inv.artifact[(int)ARTIFACT::PIOUS_PRAYER]);
	}

	// Blade Polish Kit
	if (attacker_inv.artifact[(int)ARTIFACT::BLADE_POLISH] > 0 && dist_to(attacker_motion.position, defender_motion.position) <= 100) {
		final_damage *= 1.2 * attacker_inv.artifact[(int)ARTIFACT::BLADE_POLISH];
	}

	// High-Quality Fletching
	if (attacker_inv.artifact[(int)ARTIFACT::HQ_FLETCHING] > 0 && dist_to(attacker_motion.position, defender_motion.position) > 100) {
		final_damage *= 1.2 * attacker_inv.artifact[(int)ARTIFACT::HQ_FLETCHING];
	}

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
		// sort the statuses to ensure that percentage buffs get applied before flat buffs
		statusContainer.sort_statuses_reverse();
		// we iterate backwards so that removing elements will not mess up the rest of the loop
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
				case (StatusType::FANG_POISON):
					if (status.percentage && registry.stats.has(entity)) {
						Stats stats = registry.stats.get(entity);
						take_damage(entity, stats.maxhp * status.value);
					}
					else {
						take_damage(entity, status.value);
						printf("took DoT of %f\n", status.value);
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
			if (status.turns_remaining <= 999) {
				status.turns_remaining--;
			}
			if (status.turns_remaining <= 0) {
				statusContainer.statuses.erase(statusContainer.statuses.begin() + i);
			}
		}
	}
}