//
// Created by fran on 24/10/24.
//

#ifndef TALLER_TP_BROADCAST_H
#define TALLER_TP_BROADCAST_H

#include "common/events/base/event.h"
#include "common/events/items/weapon_dto.h"
#include "items/bullet_dto.h"
#include "items/crate.h"
#include "items/explosion.h"
#include "player.h"
#include <list>
#include <string>

class Broadcast : public Event {
private:
  std::list<PlayerDTO> players;

  std::list<BulletDTO>
      bullets; // Incluye balas, bananas, granadas y explosiones

  std::list<CrateDTO> crates;

  std::list<WeaponDTO> weapons; // Estos weapons son los que spawnean en el mapa

  std::list<Explosion> explosions;

public:
  Broadcast(std::list<PlayerDTO> &&players, std::list<BulletDTO> &&bullets,
            std::list<CrateDTO> &&crates, std::list<WeaponDTO> &&weapons,
            std::list<Explosion> &&explosions);

  Broadcast(std::list<PlayerDTO> &&players,  std::list<BulletDTO> &&bullets);

  std::list<PlayerDTO> get_players() const override;

  std::list<BulletDTO> get_bullets() const override;

  std::list<CrateDTO> get_crates() const override;

  std::list<WeaponDTO> get_weapons() const override;

  std::list<Explosion> get_explosions() const override;
};

#endif // TALLER_TP_BROADCAST_H