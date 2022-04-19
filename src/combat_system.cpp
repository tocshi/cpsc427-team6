// internal
#include "combat_system.hpp"
#include "ai_system.hpp"
#include "world_system.hpp"
#include "physics_system.hpp"

// Attacker deals damage to Defender based on a "multiplier" of the attacker's ATK stat
// Parameters subject to change
std::string deal_damage(Entity& attacker, Entity& defender, float multiplier, bool doProcs)
{
	if (!registry.stats.has(attacker) || !registry.stats.has(attacker)) { return "Error getting stats!"; }
	// Damage Calculation
	Stats attacker_stats = registry.stats.get(attacker);
	Stats defender_stats = registry.stats.get(defender);
	float final_damage = calc_damage(attacker, defender, multiplier);

	// Post-calculation effects
	final_damage = handle_postcalc_effects(attacker, defender, final_damage, doProcs);

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
	if (has_status(defender, StatusType::PARRYING_STANCE) && !has_status(attacker, StatusType::PARRYING_STANCE) 
		&& final_damage < defender_stats.maxhp * 0.3
		&& defender_stats.ep >= 30 * defender_stats.eprateatk) {
		defender_stats.ep -= 30 * defender_stats.eprateatk;
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

		// apparition teleport
		if (registry.enemies.get(defender).type == ENEMY_TYPE::APPARITION && registry.stats.get(defender).hp > 0 && !registry.motions.get(defender).in_motion) {
			Mix_PlayChannel(-1, world.ghost_sound, 0);
			teleport(defender, attacker);
			registry.motions.get(defender).destination = registry.motions.get(defender).position;
			registry.motions.get(defender).in_motion = false;
		}
	}

	return log;
}

void teleport(Entity& enemy, Entity& player)
{
	Motion& player_motion = registry.motions.get(player);
	Motion& enemy_motion = registry.motions.get(enemy);
	bool valid = true;
	vec2 new_pos = enemy_motion.position;
	do {
		float angle = irandRange(-M_PI, M_PI);
		new_pos = dirdist_extrapolate(player_motion.position, angle, irandRange(600, 800));
		Motion test = {};
		test.position = new_pos;
		test.scale = { ENEMY_BB_WIDTH, ENEMY_BB_HEIGHT };
		for (Entity e : registry.solid.entities) {
			if ((registry.players.has(e) || registry.players.has(e)) &&
				collides_circle(test, registry.motions.get(e))) {
				valid = false;
				break;
			}
		}
	} while (!valid);
	enemy_motion.position = new_pos;
	world.update_bar_and_shadow();
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
	createDamageText(world.renderer, motion.position + vec2(irandRange(-24, 24), irandRange(-64, -0)), std::to_string(rounded_damage), false);

	// Effects:
	// Unnecessarily Thick Tome
	if (inv.artifact[(int)ARTIFACT::THICK_TOME] > 0) {
		if (stats.hp <= 0) {
			stats.hp = 1;
			StatusEffect invincible = StatusEffect(0, 3, StatusType::INVINCIBLE, false, false);
			apply_status(entity, invincible);

			inv.artifact[(int)ARTIFACT::THICK_TOME]--;
			world.logText("The Unnecessarily Thick Tome miraculously blocks the fatal blow and disintegrates!", {0.2, 1.0, 1.0});
		}
	}
}

void heal(Entity& entity, float amount) {
	Stats& stats = registry.stats.get(entity);

	stats.hp = min(stats.maxhp, stats.hp + amount);

	Motion motion = registry.motions.get(entity);
	int rounded_heal = round(amount);
	createDamageText(world.renderer, motion.position + vec2(irandRange(-24, 24), irandRange(-64, -0)), std::to_string(rounded_heal), true);
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
		while (final_damage > 1) {
			if (defender_stats.ep <= defender_stats.maxep * 0.5f) {
				break;
			}
			// defender loses 2 EP per % of HP damage blocked
			defender_stats.ep -= defender_stats.maxep / defender_stats.maxhp;
			final_damage -= 0.5f;
		}
	}

	// Minimum damage is 1
	final_damage = max(1.f, final_damage);
	
	return final_damage;
}

float handle_postcalc_effects(Entity& attacker, Entity& defender, float damage, bool doProcs) {
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
					if (registry.bosses.has(e)) { continue; }
					Motion enemy_motion = registry.motions.get(e);
					if (dist_to_edge(enemy_motion, defender_motion) <= 150.f) {
						if (!registry.knockbacks.has(e)) {
							KnockBack& knockback = registry.knockbacks.emplace(e);
							knockback.remaining_distance = 300;
							knockback.angle = atan2(enemy_motion.position.y - defender_motion.position.y, enemy_motion.position.x - defender_motion.position.x);
							enemy_motion.in_motion = false;
						}
						StatusEffect stun = StatusEffect(0, stun_duration, StatusType::STUN, false, true);
						apply_status(e, stun);
						StatusEffect cd = StatusEffect(0, 15, StatusType::WINDBAG_CD, false, false);
						apply_status(defender, cd);
					}
				}
				// play bag of wind sound
				world.playBagOfWindSound();

				// oh man please don't look jank
				Entity wind0 = createAttackAnimation(world.renderer, defender_motion.position, ATTACK::ROUNDSLASH);
				Entity wind1 = createAttackAnimation(world.renderer, defender_motion.position, ATTACK::ROUNDSLASH);
				registry.motions.get(wind1).angle = M_PI;
				registry.expandTimers.insert(wind0, { 500, 5000 });
				registry.expandTimers.insert(wind1, { 500, 5000 });

				world.logText("You are surrounded by a raging gust!", { 0.2, 1.0, 1.0 });
			}
		}

		// Malediction
		if (defender_inv.artifact[(int)ARTIFACT::MALEDICTION] > 0 && !has_status(defender, StatusType::MALEDICTION_CD)) {
			int cd_duration = 11 - defender_inv.artifact[(int)ARTIFACT::MALEDICTION];
			for (Entity e : registry.enemies.entities) {
				if (registry.hidden.has(e)) { continue; }
				StatusEffect debuff = StatusEffect(-0.4, 3, StatusType::ATK_BUFF, true, true);
				apply_status(e, debuff);
			}
			StatusEffect cd = StatusEffect(0, cd_duration, StatusType::MALEDICTION_CD, false, false);
			apply_status(defender, cd);
			Entity curse = createBigSlash(world.renderer, defender_motion.position, 0, defender_stats.range);
			registry.renderRequests.get(curse).used_texture = TEXTURE_ASSET_ID::CURSE;
			registry.expandTimers.get(curse).counter_ms = 1000;
			world.logText("A dreadful curse befalls your enemies!", { 0.2, 1.0, 1.0 });

			// play malediction sound
			world.playMaledictionSound();
		}
	}

	// Discarded Fang
	if (attacker_inv.artifact[(int)ARTIFACT::POISON_FANG] > 0 && doProcs) {
		bool valid = true;
		// rocks cannot be poisoned
		if (registry.enemies.has(defender)) {
			if (registry.enemies.get(defender).type == ENEMY_TYPE::LIVING_ROCK) {
				valid = false;
			}
		}
		int roll = irand(100);
		if (roll < 30 && valid) {
			world.logText("The Discarded Fang unleashes its poison!", { 0.2, 1.0, 1.0 });
			float damage = (attacker_stats.atk * 0.15) + (attacker_stats.atk * 0.15 * attacker_inv.artifact[(int)ARTIFACT::POISON_FANG] - 1);
			std::cout << damage << std::endl;

			StatusEffect poison = StatusEffect(damage, 5, StatusType::FANG_POISON, false, false);
			if (has_status(defender, StatusType::FANG_POISON)) { remove_status(defender, StatusType::FANG_POISON); }
			apply_status(defender, poison);
		}
	}

	// Thunderstruck Twig
	if (attacker_inv.artifact[(int)ARTIFACT::THUNDER_TWIG] > 0 && doProcs) {
		int roll = irand(100);
		int chance = 20 * attacker_inv.artifact[(int)ARTIFACT::THUNDER_TWIG];
		if (roll < chance) {
			// using bigslash as a template entity LOL
			Entity lightning = createBigSlash(world.renderer, { defender_motion.position.x, defender_motion.position.y - 512.f }, 0, 0);
			registry.renderRequests.get(lightning).used_texture = TEXTURE_ASSET_ID::LIGHTNING;
			registry.motions.get(lightning).scale = {1024, 1024};
			Entity explosion = createExplosion(world.renderer, defender_motion.position);
			registry.motions.get(explosion).scale *= 2.f;
			registry.colors.insert(explosion, { 2.f, 0.8f, 2.f, 1.f });
			Mix_PlayChannel(-1, world.thunder_sound, 0);

			for (Entity& e : registry.enemies.entities) {
				Motion enemy_motion = registry.motions.get(e);
				if (dist_to_edge(enemy_motion, defender_motion) <= 10.f || e == defender) {
					deal_damage(attacker, e, 70, false);
				}
			}
		}
	}

	// Lucky Chip
	if (attacker_inv.artifact[(int)ARTIFACT::LUCKY_CHIP] > 0) {
		int roll = irand(100);
		if (roll < 7 * attacker_inv.artifact[(int)ARTIFACT::LUCKY_CHIP]) {
			world.logText("You feel extremely lucky!", { 0.2, 1.0, 1.0 });
			final_damage *= 7.f;
		}
	}
	if (defender_inv.artifact[(int)ARTIFACT::LUCKY_CHIP] > 0) {
		int roll = irand(100);
		if (roll < 7 * defender_inv.artifact[(int)ARTIFACT::LUCKY_CHIP]) {
			world.logText("You feel extremely lucky!", { 0.2, 1.0, 1.0 });
			final_damage = max(1.f,final_damage - 777.f);
		}
	}

	//// Pious Prayer
	//if (defender_inv.artifact[(int)ARTIFACT::PIOUS_PRAYER] > 0 && dist_to(attacker_motion.position, defender_motion.position) > 150) {
	//	final_damage = max(1.f, final_damage - 3.f * defender_inv.artifact[(int)ARTIFACT::PIOUS_PRAYER]);
	//}

	// Blade Polish Kit
	if (attacker_inv.artifact[(int)ARTIFACT::BLADE_POLISH] > 0 && dist_to_edge(attacker_motion, defender_motion) <= 100.f) {
		final_damage *= 1 + 0.2 * attacker_inv.artifact[(int)ARTIFACT::BLADE_POLISH];
	}

	// High-Quality Fletching
	if (attacker_inv.artifact[(int)ARTIFACT::HQ_FLETCHING] > 0 && dist_to_edge(attacker_motion, defender_motion) >= 100.f) {
		final_damage *= 1 + 0.2 * attacker_inv.artifact[(int)ARTIFACT::HQ_FLETCHING];
	}

	// Rubber Mallet
	if (attacker_inv.artifact[(int)ARTIFACT::KB_MALLET] > 0 && dist_to_edge(attacker_motion, defender_motion) <= 100.f && !registry.bosses.has(defender) && doProcs) {
		float kb_dist = 50 + 50 * attacker_inv.artifact[(int)ARTIFACT::KB_MALLET];

		if (!registry.knockbacks.has(defender)) {
			KnockBack& knockback = registry.knockbacks.emplace(defender);
			knockback.remaining_distance = kb_dist;
			knockback.angle = atan2(defender_motion.position.y - attacker_motion.position.y, defender_motion.position.x - attacker_motion.position.x);
			defender_motion.in_motion = false;
		}
	}

	// Resolve Last!
	// Arcane Funnel
	if (attacker_inv.artifact[(int)ARTIFACT::ARCANE_FUNNEL] > 0 && final_damage >= defender_stats.hp) {
		StatusEffect funnel = StatusEffect(0, attacker_inv.artifact[(int)ARTIFACT::ARCANE_FUNNEL], StatusType::ARCANE_FUNNEL, false, true);
		if (has_status(attacker, StatusType::ARCANE_FUNNEL)) { remove_status(attacker, StatusType::ARCANE_FUNNEL); }
		apply_status(attacker, funnel);
	}

	// Fungifier
	if (attacker_inv.artifact[(int)ARTIFACT::FUNGIFIER] > 0 && final_damage >= defender_stats.hp) {
		float multiplier = 130 * attacker_inv.artifact[(int)ARTIFACT::FUNGIFIER];
		Entity mushroom = createTrap(world.renderer, attacker, {0, 0}, { 64, 64 }, multiplier, 2, 1, TEXTURE_ASSET_ID::MUSHROOM);
		registry.motions.get(mushroom).destination = defender_motion.position;
	}
	return final_damage;
}

void apply_status(Entity& target, StatusEffect& status) {
	StatusContainer& statusContainer = registry.statuses.get(target);
	statusContainer.statuses.push_back(status);
	statusContainer.sort_statuses_reverse();

	ParticleEmitter emitter;
	bool add_emitter = false;
	switch (status.effect) {
		case StatusType::POISON:
		case StatusType::FANG_POISON:
			emitter = setupParticleEmitter(PARTICLE_TYPE::POISON);
			add_emitter = true;
			break;
		case StatusType::ATK_BUFF:
			if (status.value < 0) {
				emitter = setupParticleEmitter(PARTICLE_TYPE::ATK_DOWN);
				add_emitter = true;
			}
			else if (status.value > 0) {
				emitter = setupParticleEmitter(PARTICLE_TYPE::ATK_UP);
				add_emitter = true;
			}
			break;
		case StatusType::RANGE_BUFF:
			if (status.value < 0) {
				emitter = setupParticleEmitter(PARTICLE_TYPE::RANGE_DOWN);
				add_emitter = true;
			}
			else if (status.value > 0) {
				emitter = setupParticleEmitter(PARTICLE_TYPE::RANGE_UP);
				add_emitter = true;
			}
			break;
		case StatusType::DEF_BUFF:
			if (status.value < 0) {
				emitter = setupParticleEmitter(PARTICLE_TYPE::DEF_DOWN);
				add_emitter = true;
			}
			else if (status.value > 0) {
				emitter = setupParticleEmitter(PARTICLE_TYPE::DEF_UP);
				add_emitter = true;
			}
			break;
		case StatusType::SLIMED:
			emitter = setupParticleEmitter(PARTICLE_TYPE::SLIMED);
			add_emitter = true;
			break;
		case StatusType::STUN:
			emitter = setupParticleEmitter(PARTICLE_TYPE::STUN);
			add_emitter = true;
			break;
		case StatusType::INVINCIBLE:
			emitter = setupParticleEmitter(PARTICLE_TYPE::INVINCIBLE);
			add_emitter = true;
			break;
		case StatusType::HP_REGEN:
			emitter = setupParticleEmitter(PARTICLE_TYPE::HP_REGEN);
			add_emitter = true;
			break;
		default:
			break;
	}
	if (add_emitter) {
		if (!registry.particleContainers.has(target)) {
			ParticleContainer& particleContainer = registry.particleContainers.emplace(target);
			particleContainer.emitters.push_back(emitter);
		}
		else {
			ParticleContainer& particleContainer = registry.particleContainers.get(target);
			particleContainer.emitters.push_back(emitter);
		}
	}

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
			//if (status.turns_remaining <= 0) {
			//	remove_status_particle(entity, status);
			//	statusContainer.statuses.erase(statusContainer.statuses.begin()+i);
			//	continue;
			//}
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
				case (StatusType::DEF_BUFF):
					if (status.percentage && registry.stats.has(entity)) {
						stats.def += basestats.def * status.value;
					}
					else {
						stats.def += status.value;
					}
					break;
				case (StatusType::RANGE_BUFF):
					if (status.percentage && registry.stats.has(entity)) {
						stats.range += basestats.range * status.value;
					}
					else {
						stats.range += status.value;
					}
					world.remove_fog_of_war();
					world.create_fog_of_war();
					break;
				case (StatusType::SLIMED):
					if (status.percentage && registry.stats.has(entity)) {
						stats.epratemove *= status.value;
					}
					else {
						stats.epratemove += status.value;
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
				case (StatusType::HP_REGEN):
					if (!stats_only) {
						if (status.percentage && registry.stats.has(entity)) {
							heal(entity, basestats.maxhp * status.value);
						}
						else {
							heal(entity, status.value);
						}
					}
					break;
				default:
					break;
			}

			// properly remove statuses that have expired, except for things with >=999 turns (we treat those as infinite)
			if (!stats_only) {
				if (status.turns_remaining <= 0) {
					remove_status_particle(entity, status);
					statusContainer.statuses.erase(statusContainer.statuses.begin() + i);
					reset_stats(entity);
					calc_stats(entity);
				}
				if (status.turns_remaining <= 999) {
					status.turns_remaining--;
				}
			}
		}
	}
}

// Call at the beginning of every player turn to manage traps
void handle_traps() {
	for (Entity& t : registry.traps.entities) {
		// this looks stupid, but trust me, it isn't
		if (!registry.traps.has(t) || !registry.renderRequests.has(t)) {
			continue;
		}

		Trap& trap = registry.traps.get(t);

		if (registry.renderRequests.get(t).used_texture == TEXTURE_ASSET_ID::MUSHROOM && registry.motions.get(t).position != registry.motions.get(t).destination) {
			registry.motions.get(t).position = registry.motions.get(t).destination;
			Mix_PlayChannel(3, world.trap_sound, 0);
		}

		if (trap.turns <= 0 || trap.triggers <= 0) {
			if (registry.renderRequests.get(t).used_texture == TEXTURE_ASSET_ID::MUSHROOM) {
				trigger_trap(t, t);
			}
			if (registry.motions.has(t)) {
				registry.remove_all_components_of(t);
			}
		}
		trap.turns--;
	}
}

// Trigger trap effects
// t is trap
// trapped is the unfortunate victim
void trigger_trap(Entity t, Entity trapped) {
	// this looks stupid, but trust me, it isn't
	if (!registry.stats.has(t) || !registry.renderRequests.has(t) || !registry.traps.has(t)) {
		return; }
	// Look, C++ has a tendency to screw around with copy-by-reference, and the trap system is ESPECIALLY
	// prone to it not copying correctly. I don't know why this even happens, but it seems that as long as
	// I replace every call to trap with registry.traps.get(t), C++ plays nice and doesn't mess with the 
	// values in the trap component. Thanks for coming to my TED Talk.
	//Trap& trap = registry.traps.get(t);
	if (registry.traps.get(t).triggers <= 0) { 
		if (registry.motions.has(t)) {
			registry.remove_all_components_of(t);
		}
		return; 
	}
	Motion& trap_motion = registry.motions.get(t);

	// do trap effect based on texture
	switch (registry.renderRequests.get(t).used_texture) {
	case TEXTURE_ASSET_ID::MUSHROOM:
		if (true) {
			Mix_PlayChannel(-1, world.fire_explosion_sound, 0);
			Entity explosion = createExplosion(world.renderer, trap_motion.position);
			registry.motions.get(explosion).scale *= 2.f;
			registry.colors.insert(explosion, { 0.8f, 2.f, 2.f, 1.f });

			for (Entity& e : registry.enemies.entities) {
				if (!e) { continue; }
				Motion enemy_motion = registry.motions.get(e);
				if (dist_to_edge(enemy_motion, registry.motions.get(t)) <= 50.f) {
					deal_damage(registry.traps.get(t).owner, e, registry.traps.get(t).multiplier, false);
				}
			}
		}
		break;
	case TEXTURE_ASSET_ID::BURRS:
		if (true) {
			StatusEffect burrs = StatusEffect(0, 0, StatusType::BURR_DEBUFF, false, true);

			// don't trigger if already triggered this turn
			if (has_status(trapped, StatusType::BURR_DEBUFF)) { return; }
			apply_status(trapped, burrs);
			deal_damage(registry.traps.get(t).owner, trapped, registry.traps.get(t).multiplier, false);
		}
		break;
	case TEXTURE_ASSET_ID::FATE:
		if (true) {
			StatusEffect boss_poison = StatusEffect(0.2 * registry.traps.get(t).multiplier, 5, StatusType::POISON, false, false);
			StatusEffect boss_atk_buff = StatusEffect(0.3, 3, StatusType::ATK_BUFF, true, true);
			StatusEffect boss_regen_buff = StatusEffect(10, 3, StatusType::HP_REGEN, false, true);
			vec4 color = vec4(1.f);
			if (registry.colors.has(t)) { color = registry.colors.get(t); }

			Mix_PlayChannel(-1, world.smokescreen_sound, 0);
			Entity smoke = createBigSlash(world.renderer, trap_motion.position, 0, 200);
			registry.renderRequests.get(smoke).used_texture = TEXTURE_ASSET_ID::SMOKE;
			registry.expandTimers.get(smoke).counter_ms = 1000;
			registry.colors.insert(smoke, color);

			// switch based on colour
			if (color == vec4(1.f, 0.f, 0.f, 0.9f)) {
				for (Entity e : registry.enemies.entities) {
					apply_status(e, boss_atk_buff);
				}
				world.logText("A magical mist empowers the enemies!");
			}
			else if (color == vec4(0.f, 1.f, 0.f, 0.9f)) {
				for (Entity e : registry.enemies.entities) {
					apply_status(e, boss_regen_buff);
				}
				world.logText("A magical mist mends the enemies' wounds!");
			}
			else if (color == vec4(0.f, 0.f, 1.f, 0.9f)) {
				world.logText("An apparition materializes from the mist!");
				Entity summon = createApparition(world.renderer, trap_motion.position);
				world.turnOrderSystem.turnQueue.addNewEntity(summon);
				ExpandTimer iframe = registry.iFrameTimers.emplace(summon);
				iframe.counter_ms = 50;
			}
			else {
				world.logText("You inhale a poisonous mist!");
				apply_status(trapped, boss_poison);
			}
		}
		break;
	default:
		break;
	}
	registry.traps.get(t).triggers--;
	if (registry.traps.get(t).triggers <= 0) {
		if (registry.motions.has(t)) {
			registry.remove_all_components_of(t);
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

	// Messenger's Cap
	if (inv.artifact[(int)ARTIFACT::MESSENGER_CAP] > 0) {
		stats.speed += (0.05 + 0.05 * inv.artifact[(int)ARTIFACT::MESSENGER_CAP]) * basestats.atk;
	}

	// Warm Cloak
	if (inv.artifact[(int)ARTIFACT::WARM_CLOAK] > 0) {
		stats.def += (0.05 + 0.05 * inv.artifact[(int)ARTIFACT::WARM_CLOAK]) * basestats.atk;
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