#ifndef TALLER_TP_EVENTS_PROTOCOL_H
#define TALLER_TP_EVENTS_PROTOCOL_H

#include "common/events/base/event.h"
#include "common/protocol/common/protocol.h"


#define EVENT_TYPE_SIZE sizeof(int8_t)
#define PLAYERS_ID_SIZE sizeof(uint16_t)
#define GAME_CODE_SIZE sizeof(uint32_t)
#define CONNECTED_SIZE sizeof(uint8_t)
#define MAX_PLAYER_SIZE sizeof(uint8_t)
#define ACTUAL_PLAYER_SIZE sizeof(uint8_t)


#define COORDINATE_SIZE sizeof(int16_t)
#define LEN_SIZE sizeof(uint8_t)
#define PLAYER_COORDINATE sizeof(int16_t)
#define PLAYER_IS_RIGHT_SIZE sizeof(uint8_t)
#define PLAYER_STATE_SIZE sizeof(uint8_t)


#define WEAPONS_SIZE (sizeof(uint8_t) + COORDINATE_SIZE * 2 + sizeof(uint8_t))
#define HEALTH_SIZE sizeof(uint8_t)
#define CHESTPLATE_SIZE sizeof(uint8_t)

#define GAME_ROOM_SIZE (MAX_PLAYER_SIZE + ACTUAL_PLAYER_SIZE + GAME_CODE_SIZE)

#define READ_NEW_PLAYER_SIZE (ACTUAL_PLAYER_SIZE + MAX_PLAYER_SIZE)

#define READ_CREATE_GAME_SIZE                                                  \
  (GAME_CODE_SIZE + 2 * PLAYERS_ID_SIZE + ACTUAL_PLAYER_SIZE + MAX_PLAYER_SIZE)

#define READ_JOIN_GAME_SIZE (2 * PLAYERS_ID_SIZE + CONNECTED_SIZE + ACTUAL_PLAYER_SIZE + MAX_PLAYER_SIZE)

#define ANGLE_SIZE sizeof(uint16_t)
#define BULLET_ID_SIZE sizeof(uint8_t)
#define READ_BULLET_SIZE (COORDINATE_SIZE * 2 + BULLET_ID_SIZE + ANGLE_SIZE)
#define SEND_BULLET_SIZE READ_BULLET_SIZE

#define READ_CRATE_SIZE (COORDINATE_SIZE * 2 + sizeof(uint8_t) * 2)
#define SEND_CRATE_SIZE READ_CRATE_SIZE

#define READ_PLAYER_SIZE ( \
    2 * PLAYER_COORDINATE + PLAYERS_ID_SIZE + \
    PLAYER_STATE_SIZE + PLAYER_IS_RIGHT_SIZE +\
    WEAPONS_SIZE + HEALTH_SIZE + CHESTPLATE_SIZE)

#define TILE_ID_SIZE sizeof(uint8_t)
#define READ_TILE_SIZE (COORDINATE_SIZE * 3 + TILE_ID_SIZE)

#define BACKGROUND_ID_SIZE sizeof(uint8_t)
#define READ_BACKGROUND_SIZE (COORDINATE_SIZE * 2 + BACKGROUND_ID_SIZE)
#define SEND_BACKGROUND_SIZE READ_BACKGROUND_SIZE

#define SEND_CREATE_GAME_SIZE                                                  \
  (READ_CREATE_GAME_SIZE + EVENT_TYPE_SIZE)

#define SEND_JOIN_GAME_SIZE                                                    \
  (READ_JOIN_GAME_SIZE + EVENT_TYPE_SIZE)

#define SEND_PLAYER_SIZE READ_PLAYER_SIZE

#define SEND_TILE_SIZE READ_TILE_SIZE

#define SEND_NEW_PLAYER_SIZE (READ_NEW_PLAYER_SIZE + EVENT_TYPE_SIZE)


class EventsProtocol : public Protocol {
private:
  Encoder encoder;

  void send_create(const std::shared_ptr<Event> &event);

  void send_join(const std::shared_ptr<Event> &event);

  std::shared_ptr<Event> read_create();

  std::shared_ptr<Event> read_join();

public:
  EventsProtocol(Socket *socket, Encoder encoder);

  void send_element(std::shared_ptr<Event> &event);

  std::shared_ptr<Event> read_element();

  EventType read_event_type();

  std::shared_ptr<Event> read_broadcast();

  void send_broadcast(const std::shared_ptr<Event> &event);

    void send_refresh(const std::shared_ptr<Event> &event);

    std::shared_ptr<Event> read_refresh_game();

    std::shared_ptr<Event> read_new_player();

    void send_new_player(const std::shared_ptr<Event> &event);

    void send_start_game(const std::shared_ptr<Event> &event);

    WeaponDTO read_weapon(std::vector<int8_t> &data);

    HelmetDTO read_helmet(std::vector<int8_t> &data);

    Chestplate read_chestplate(std::vector<int8_t> &data);

    void add_chestplate(std::vector<int8_t> &data, Chestplate chestplate, size_t& offset);

    void add_health(std::vector<int8_t> &data, HelmetDTO helmet, size_t& offset);

    void add_weapon(std::vector<int8_t> &data, WeaponDTO weapon, size_t& offset);

    void add_players(const std::shared_ptr<Event> &event, std::vector<int8_t> &data, size_t& offset);
  
  void send_map(const std::shared_ptr<Event> &event);

  std::shared_ptr<Event> read_map();

  void send_tile(const std::shared_ptr<Event> &event);

  std::shared_ptr<Event> read_tile();

  void add_platforms(const std::shared_ptr<Event> &event, std::vector<int8_t> &data, size_t& offset);

    void add_bullets(const std::shared_ptr<Event> &event, std::vector<int8_t> &data, size_t &offset);
  
  void add_crates(const std::shared_ptr<Event> &event, std::vector<int8_t> &data, size_t &offset);

};

#endif // TALLER_TP_EVENTS_PROTOCOL_H