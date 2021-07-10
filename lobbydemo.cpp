#include "lobbyreader.h"

#include <cstdio>

int main() {
	struct LobbyResponse L;
	if(GetLobby(&L)) {
		FreeLobbyResponse(&L);
	}
	printf("Games in lobby: %d\n", L.roomsLen);
	for(int i=0; i<L.roomsLen; i++) {
		struct LobbyGame g = L.rooms[i];
		printf("Game %d\nName [%s]\nMap [%s]\nPlayers %d/%d\n",
			g.gameId, g.name, g.mapname, g.dwCurrentPlayers, g.dwMaxPlayers);
	}
	printf("MOTD [%s]\n", L.motd);
	FreeLobbyResponse(&L);
}