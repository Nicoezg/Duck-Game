#include "server/lobby.h"
#include "common/events/connection/game_creation.h"
#include "common/events/connection/game_join.h"
#include "common/protocol/common/protocol_error.h"
#include "monitor_games.h"
#include "server/monitor_lobby.h"
#include "common/events/connection/refresh_games.h"
#include <sys/socket.h>

Lobby::Lobby(Socket &&client, MonitorGames *games)
        : client(std::move(client)), protocol(&this->client, Encoder()),
          games(games), is_running(true), is_connected(false) {}

void Lobby::run() {
    try {
        int game_code = SIN_CODIGO;
        while (!is_connected) {
            std::shared_ptr<Action> action = protocol.read_element();
            std::shared_ptr<Event> response = process_action(action, game_code);
            protocol.send_element(response);
            is_connected = response->is_connected();
        }
        games->add_to_game(game_code, std::move(client));

    } catch (const ProtocolError &e) {
        is_running = false;
        is_connected = false;
        return;
    }
    is_running = false;
}

std::shared_ptr<Event>
Lobby::process_action(const std::shared_ptr<Action> &action, int &game_code) {
    std::shared_ptr<Event> response;
    switch (action->get_type()) {
        case CREATE_REQUEST:
            response = create_game(action->get_game_mode(), action->get_max_players());
            game_code = response->get_game_code();
            break;
        case JOIN_REQUEST:
            response = join_game(action->get_game_code(), action->get_game_mode());
            game_code = action->get_game_code();
            break;
        case REFRESH_REQUEST:
            response = std::make_shared<Refresh>(games->get_not_active_games());
            break;
        default:
            response = not_connected_to_game();
            break;
    }

    return response;
}

std::shared_ptr<Event> Lobby::not_connected_to_game() {
    return std::make_shared<GameJoin>(SIN_CODIGO, SIN_CODIGO, false, 0, 0);
}

std::shared_ptr<Event> Lobby::join_game(int game_code, GameMode mode) {
    if (!games->game_exists(game_code)) {
        return std::make_shared<GameJoin>(SIN_CODIGO, SIN_CODIGO, false, 0, 0);
    }

    int player_id_1 = games->get_player_id(game_code, mode);
    std::cout << "Game: " << game_code << " PlayerDTO id: " << player_id_1
              << " Mode: " << mode << std::endl;

    int player_id_2 = SIN_ASIGNAR;
    if (mode == DOS_JUGADORES) {
        player_id_2 = games->get_player_id(game_code, mode - 1);
        std::cout << "Game: " << game_code << " PlayerDTO id: " << player_id_2
                  << " Mode: " << mode << std::endl;
    }

    int max, actual;
    games->get_max_and_actual_players(game_code, actual, max);
    return std::make_shared<GameJoin>(player_id_1, player_id_2, true, actual, max);
}

std::shared_ptr<Event> Lobby::create_game(GameMode mode, int max_players) {
    int game_code = games->create_game(max_players);

    int player_id_1 = games->get_player_id_admin(game_code, mode);
    std::cout << "Game: " << game_code << " PlayerDTO id: " << player_id_1
              << " Mode: " << mode << std::endl;

    int player_id_2 = SIN_ASIGNAR;
    if (mode == DOS_JUGADORES) {
        player_id_2 = games->get_player_id_admin(game_code, mode - 1);
        std::cout << "Game: " << game_code << " PlayerDTO id: " << player_id_2
                  << " Mode: " << mode << std::endl;
    }

    int actual, max;
    games->get_max_and_actual_players(game_code, actual, max);
    return std::make_shared<GameCreation>(game_code, player_id_1, player_id_2, actual, max);
}

bool Lobby::is_closed() const { return !is_running; }

void Lobby::close() {
    if (!is_running || is_connected)
        return;
    is_running = false;
    client.shutdown_and_close(SHUT_RDWR);
}