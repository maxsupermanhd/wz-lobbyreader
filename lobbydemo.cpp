#include "lobbyreader.h"

#include <cstdio>

int main() {
	struct LobbyResponse L;
	if(GetLobby(&L)) {
		printf("Error!\n");
		return 1;
	}
	printf("Games in lobby: %ld\n", L.rooms.size());
	for(const auto g : L.rooms) {
		printf("Game %d\nName [%s]\nMap [%s]\nPlayers %d/%d\n",
			g.gameId, g.name.c_str(), g.mapname.c_str(), g.dwCurrentPlayers, g.dwMaxPlayers);
	}
	printf("MOTD [%s]\n", L.motd.c_str());
	return 0;
}