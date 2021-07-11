// wz-lobbyreader
// Copyright (C) 2021 Zhuchkov Maxim
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "lobbyreader.h"

#include <cstdio>
#include <errno.h>
#include <cstring>

int main() {
	struct LobbyResponse L;
	if(GetLobby(&L) != LOBBYREADER_SUCCESS) {
		printf("Error! Code %d: [%s]\n", errno, strerror(errno));
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