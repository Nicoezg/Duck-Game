
#include "game_map.h"
#include "common/events/player.h"
#include "duck.h"
#include <iostream>
#include <stdexcept>
#include <yaml-cpp/yaml.h>

#define HEIGHT 500
#define WIDTH 500

GameMap::GameMap() { map = mapLoader.getNextMap(); }

void GameMap::addPlayer(int player_id) {

  Duck *duck = new Duck(player_id, 10 * player_id, (map.getGroundLevel() * 16) + 16, *this);
  players.push_back(duck);
}

void GameMap::addBullet(std::unique_ptr<Bullet> bullet) {
  bullets.push_back(std::move(bullet));
}

void GameMap::addThrowable(std::unique_ptr<Throwable> throwable) {
  throwables.push_back(std::move(throwable));
}

Duck *GameMap::findPlayer(int playerId) {
  for (auto player : players) {
    if (player->getId() == playerId) {
      return player;
    }
  }
  return nullptr;
}

void GameMap::update() {
  //reapDead();
  for (auto player : players) {
    player->update();
  }

   bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(), [](const std::shared_ptr<Bullet>& bullet) {
            return bullet->outOfRange();
        }),
        bullets.end()
    );

    for (const auto& bullet : bullets) {
    bullet->update();
  }

  for (const auto &throwable : throwables) {
    throwable->update();
  }
}


bool GameMap::checkCollisionsWithBorders(int playerId) {
  Duck *player = findPlayer(playerId);
  if (!player)
    return true;

  if (player->getPositionX() >= WIDTH || player->getPositionX() <= 0) {
    return true;
  }
  if (player->getPositionY() >= HEIGHT || player->getPositionY() <= 0) {
    return true;
  }
  return false;
}

void GameMap::process_action(std::shared_ptr<Action> &action) {
  int player_id = action->get_player_id();
  Duck *duck = findPlayer(player_id);
  if (!duck)
    return;

  switch (action->get_type()) {
  case MOVE:
    duck->move(action->is_right());
    break;
  case JUMP_FLAP:
    if (!duck->isJumping()) {
      duck->jump();
    } else {
      duck->flap();
    }
    break;
  case STILL:
    duck->stopMoving();
    break;
  case SHOOT:
    duck->shoot();
    break;
  case PLAY_DEAD:
    duck->playDead();
    break;
  case AIM_UPWARDS:
    duck->aimUpwards();
    break;
  default:
    std::cout << "Acción inválida" << std::endl;
    break;
  }
}

std::list<BulletDTO> GameMap::getBulletsState() {
  std::list<BulletDTO> bulletsList;

  for (const auto &bullet : bullets) {
    int posX = bullet->getPosX();
    int posY = bullet->getPosY();
    float angle = bullet->getAngle();
    enum BulletId id = bullet->getId();

    bulletsList.emplace_back(posX, posY, id, angle);
  }

  for (const auto &throwable : throwables) {
    int posX = throwable->getPosX();
    int posY = throwable->getPosY();
    float angle = 0;
    enum BulletId id = BulletId::THROWN_GRENADE;

    bulletsList.emplace_back(posX, posY, id, angle);
  }

  return bulletsList;
}

std::list<PlayerDTO> GameMap::getState() {
  std::list<PlayerDTO> playersList;
  for (auto player : players) {
    int id = player->getId();
    int posX = player->getPositionX();
    int posY = player->getPositionY();
    bool dir = player->getDirection();
    State state = player->getState();
    HelmetDTO helmet;
    WeaponDTO weapon = WeaponDTO(player->getWeapon()->getWeaponId(), posX, posY, player->isShooting());
    if (!player->isWearingHelmet()) {
      helmet = HelmetDTO(NO_HELMET);
    } else {
      helmet = HelmetDTO(KNIGHT);
    }

    playersList.emplace_back(id, posX, posY, dir, state, weapon,
                             helmet, Chestplate(player->isWearingArmour()));
  }
  return playersList;
}

void GameMap::reapDead() {
  auto it = std::remove_if(players.begin(), players.end(), [](Duck *player) {
    if (player->getState() == State::DEAD) {
      delete player;
      return true;
    }
    return false;
  });

  players.erase(it, players.end());
}

GameMap::~GameMap() {
  for (auto player : players) {
    delete player;
  }

  for (auto &bullet : bullets) {
    bullet.reset();
  }
}

MapDTO GameMap::getMapDTO(){
  return mapLoader.getNextMapDTO();
}

Map GameMap::getMap() {
  return map;
}

