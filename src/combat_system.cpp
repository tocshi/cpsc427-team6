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

	// Defender take damage, unless parried
	if (has_status(defender, StatusType::PARRYING_STANCE) && !has_status(attacker, StatusType::PARRYING_STANCE) && final_damage < defender_stats.maxhp * 0.3) {
		deal_damage(defender, attacker, multiplier);
		// show attack animation
		Motion& attacker_motion = registry.motions.get(attacker);
		createAttackAnimation(world.renderer, { attacker_motion.position.x, attacker_motion.position.y }, ATTACK::NONE);
		Mix_PlayChannel(-1, world.sword_parry, 0);
		return attacker_name.append("'s attack was parried!");
	}
	else {
		take_damage(defender, final_damage);
	}

	// create logged message
	std::string log = attacker_name.append(" attacked ").append(defender_name).append(" for ").append(std::to_string(logged_damage)).append(" damage!");

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
	
	Motion motion = registry.motions.get(entity);
	int rounded_damage = round(damage);
	createDamageText(world.renderer, motion.position + vec2(0, -32), std::to_string(rounded_damage), false);

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

void heal(Entity& entity, float amount) {
	Stats& stats = registry.stats.get(entity);

	stats.hp = min(stats.maxhp, stats.hp + amount);

	Motion motion = registry.motions.get(entity);
	int rounded_heal = round(amount);
	createDamageText(world.renderer, motion.position + vec2(0, -32), std::to_string(rounded_heal), true);
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
	if (attacker_inv.artifact[(int)ARTIFACT::BLOOD_RUBY] > 0 && attacker_stats.hp <= attacker_stats.maxhp * 0.4) {
		attacker_atk *= 1 + (0.2 * attacker_inv.artifact[(int)ARTIFACT::BLOOD_RUBY]);
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

		// Bag of Wind
		if (defender_inv.artifact[(int)ARTIFACT::WINDBAG] > 0 && !has_status(defender, StatusType::WINDBAG_CD)) {
			float hp_threshold = 0.2 + 0.05 * defender_inv.artifact[(int)ARTIFACT::WINDBAG];
			int stun_duration = 2 + defender_inv.artifact[(int)ARTIFACT::WINDBAG];
			if (defender_stats.hp - final_damage <= defender_stats.maxhp * hp_threshold) {
				for (Entity e : registry.enemies.entities) {
					Motion enemy_motion = registry.motions.get(e);
					if (dist_to(defender_motion.position, enemy_motion.position) <= 200) {
						if (!registry.knockbacks.has(e)) {
							KnockBack& knockback = registry.knockbacks.emplace(e);
							knockback.remaining_distance = 300;
							knockback.angle = atan2(enemy_motion.position.y - defender_motion.position.y, enemy_motion.position.x - defender_motion.position.x);
						}
						StatusEffect stun = StatusEffect(0, stun_duration, StatusType::STUN, false, true);
						apply_status(e, stun);
						StatusEffect cd = StatusEffect(0, 15, StatusType::WINDBAG_CD, false, false);
						apply_status(defender, cd);
					}
				}
				world.logText("You are surrounded by a raging gust!");
			}
		}
	}

	// Discarded Fang
	if (attacker_inv.artifact[(int)ARTIFACT::POISON_FANG] > 0) {
		int roll = irand(100);
		if (roll < 30) {
			world.logText("The Discarded Fang unleashes its poison!");
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
			world.logText("The Thundering Twig summons a bolt of lightning!");
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

	//// Pious Prayer
	//if (defender_inv.artifact[(int)ARTIFACT::PIOUS_PRAYER] > 0 && dist_to(attacker_motion.position, defender_motion.position) > 100) {
	//	final_damage = max(1.f, final_damage - 3.f * defender_inv.artifact[(int)ARTIFACT::PIOUS_PRAYER]);
	//}

	// Blade Polish Kit
	if (attacker_inv.artifact[(int)ARTIFACT::BLADE_POLISH] > 0 && dist_to(attacker_motion.position, defender_motion.position) <= 100) {
		final_damage *= 1 + 0.2 * attacker_inv.artifact[(int)ARTIFACT::BLADE_POLISH];
	}

	// High-Quality Fletching
	if (attacker_inv.artifact[(int)ARTIFACT::HQ_FLETCHING] > 0 && dist_to(attacker_motion.position, defender_motion.position) > 100) {
		final_damage *= 1 + 0.2 * attacker_inv.artifact[(int)ARTIFACT::HQ_FLETCHING];
	}

	// Rubber Mallet
	if (attacker_inv.artifact[(int)ARTIFACT::KB_MALLET] > 0 && dist_to(attacker_motion.position, defender_motion.position) <= 100) {
		float kb_dist = 50 + 50 * attacker_inv.artifact[(int)ARTIFACT::KB_MALLET];

		if (!registry.knockbacks.has(defender)) {
			KnockBack& knockback = registry.knockbacks.emplace(defender);
			knockback.remaining_distance = kb_dist;
			knockback.angle = atan2(defender_motion.position.y - attacker_motion.position.y, defender_motion.position.x - attacker_motion.position.x);
		}
	}

	return final_damage;
}

void apply_status(Entity& target, StatusEffect& status) {
	StatusContainer& statusContainer = registry.statuses.get(target);
	statusContainer.statuses.push_back(status);

	// recalculate stats for entity
	reset_stats(target);
	calc_stats(target);
}

// call this function once at turn start (2nd param=true), and once at turn end (2nd param=false)
// set stats_only to true if you're using this for mid-turn stat recalculation
void handle_status_ticks(Entity& entity, bool applied_from_turn_start, bool stats_only) {
	if (registry.statuses.has(entity)) {
		StatusContainer& statusContainer = registry.statuses.get(entity);
		Stats& stats = registry.stats.get(entity);
		Stats basestats = registry.basestats.get(entity);
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
					}
					break;
				case (StatusType::ATK_BUFF):
					if (status.percentage && registry.stats.has(entity)) {
						stats.atk += basestats.atk * status.value;
					}
					else {
						stats.atk += status.value;
					}
					break;
				case (StatusType::STUN):
					registry.queueables.get(entity).doing_turn = false;
					break;
				case (StatusType::EP_REGEN):
					if (!stats_only) {
						if (status.percentage && registry.stats.has(entity)) {
							stats.ep += basestats.maxep * status.value;
						}
						else {
							stats.ep += status.value;
						}
					}
					break;
				default:
					break;
			}

			// properly remove statuses that have expired, except for things with >=999 turns (we treat those as infinite)
			if (!stats_only) {
				if (status.turns_remaining <= 999) {
					status.turns_remaining--;
				}
				if (status.turns_remaining <= 0) {
					statusContainer.statuses.erase(statusContainer.statuses.begin() + i);
					reset_stats(entity);
					calc_stats(entity);
				}
			}
		}
	}
}

// Reset entity stats to base stats
void reset_stats(Entity& entity) {
	Stats& stats = registry.stats.get(entity);
	Stats basestats = registry.basestats.get(entity);

	float current_hp = stats.hp;
	float current_mp = stats.mp;
	float current_ep = stats.ep;
	bool current_guard = stats.guard;
	stats = basestats;
	stats.hp = current_hp;
	stats.mp = current_mp;
	stats.ep = current_ep;
	stats.guard = current_guard;
}

// Calculate entity stats based on inv + effects
void calc_stats(Entity& entity) {
	Stats& stats = registry.stats.get(entity);
	Stats basestats = registry.basestats.get(entity);
	Inventory& inv = registry.inventories.get(entity);

	handle_status_ticks(entity, true, true);

	// Artifact Effects
	// Arcane Spectcles
	if (inv.artifact[(int)ARTIFACT::ARCANE_SPECS] > 0) {
		stats.range += 50.f * inv.artifact[(int)ARTIFACT::ARCANE_SPECS];
	}

	// Scouting Striders
	if (inv.artifact[(int)ARTIFACT::SCOUT_STRIDE] > 0) {
		int stack = inv.artifact[(int)ARTIFACT::SCOUT_STRIDE];
		while (stack > 0) {
			stats.epratemove *= 0.88f;
			stack--;
		}
	}

	// The Art of Conservation
	if (inv.artifact[(int)ARTIFACT::ART_CONSERVE] > 0) {
		int stack = inv.artifact[(int)ARTIFACT::ART_CONSERVE];
		while (stack > 0) {
			stats.eprateatk *= 0.93f;
			stack--;
		}
	}
}

// Equip an item (returns unequipped item)
Equipment equip_item(Entity& entity, Equipment& equipment) {
	Stats& basestats = registry.basestats.get(entity);
	Inventory& inv = registry.inventories.get(entity);
	int slot = 0;

	switch (equipment.type) {
	case EQUIPMENT::SHARP:
	case EQUIPMENT::BLUNT: 
	case EQUIPMENT::RANGED:
		slot = 0;
		break;
	case EQUIPMENT::ARMOUR:
		slot = 1;
		break;
	default:
		break;
	}

	Equipment prev = {};
	// weird nullguard, source: trust me bro
	if (inv.equipped[slot].type != EQUIPMENT::EQUIPMENT_COUNT) {
		prev = unequip_item(entity, slot);
	}
	inv.equipped[slot] = equipment;

	// set base stats
	basestats.atk += inv.equipped[slot].atk;
	basestats.def += inv.equipped[slot].def;
	basestats.speed += inv.equipped[slot].speed;
	basestats.maxhp += inv.equipped[slot].hp;
	basestats.maxmp += inv.equipped[slot].mp;
	basestats.maxep += inv.equipped[slot].ep;
	basestats.range += inv.equipped[slot].range;

	// recalculate stats based on new equipment basestats
	reset_stats(entity);
	calc_stats(entity);

	return prev;
}

// Unequip an item (does not remove from inventory)
Equipment unequip_item(Entity& entity, int slot) {
	Stats& basestats = registry.basestats.get(entity);
	Inventory& inv = registry.inventories.get(entity);

	Equipment equip = inv.equipped[slot];

	// set base stats
	basestats.atk -= inv.equipped[slot].atk;
	basestats.def -= inv.equipped[slot].def;
	basestats.speed -= inv.equipped[slot].speed;
	basestats.maxhp -= inv.equipped[slot].hp;
	basestats.maxmp -= inv.equipped[slot].mp;
	basestats.maxep -= inv.equipped[slot].ep;
	basestats.range -= inv.equipped[slot].range;

	// recalculate stats based on new equipment basestats
	reset_stats(entity);
	calc_stats(entity);

	return equip;
}