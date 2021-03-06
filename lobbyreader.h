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

#ifndef LOBBYREADER_H_INCLUDED
#define LOBBYREADER_H_INCLUDED

#include <vector>
#include <stdint.h>
#include <string>

#define	LobbyStringSize 64
#define Lobbyextra_string_size 157
#define Lobbymap_string_size 40
#define	Lobbyhostname_string_size 40
#define Lobbymodlist_string_size 255

#define LOBBYREADER_SUCCESS 0
#define LOBBYREADER_FAIL 1
#define LOBBYREADER_IGNORE_FIRST_BATCH 1

struct LobbyGame {
	uint32_t	GAMESTRUCT_VERSION;

	std::string	name;
	
	int32_t dwSize;
	int32_t dwFlags;
	std::string host;	// host's ip address (can fit a full IPv4 and IPv6 address + terminating NUL)
	int32_t dwMaxPlayers;
	int32_t dwCurrentPlayers;
	int32_t dwUserFlags[4];
	
	std::string	secondaryHosts[2];
	std::string	extra;		// extra string (future use)
	uint16_t	hostPort;						// server port
	std::string	mapname;		// map server is hosting
	std::string	hostname;	// ...
	std::string	versionstring;		//
	std::string	modlist;	// ???
	uint32_t	game_version_major;				//
	uint32_t	game_version_minor;				//
	uint32_t	privateGame;					// if true, it is a private game
	uint32_t	pureMap;						// If this map has mods in it.
	uint32_t	Mods;							// number of concatenated mods?
	
	// Game ID, used on the lobby server to link games with multiple address families to eachother
	uint32_t	gameId;
	uint32_t	limits;							// holds limits bitmask (NO_VTOL|NO_TANKS|NO_BORGS)
	uint32_t	future3;						// for future use
	uint32_t	future4;						// for future use
};

struct LobbyResponse {
	std::vector<struct LobbyGame> rooms;
	uint32_t code = 0;
	std::string motd = "";
};

int GetLobby(struct LobbyResponse* r, int t = 3);

#endif