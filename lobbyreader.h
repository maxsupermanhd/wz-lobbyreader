#ifndef LOBBYREADER_H_INCLUDED
#define LOBBYREADER_H_INCLUDED

#include <vector>
#include <cstdint>

#define	LobbyStringSize 64
#define Lobbyextra_string_size 157
#define Lobbymap_string_size 40
#define	Lobbyhostname_string_size 40
#define Lobbymodlist_string_size 255

struct LobbyGame {
	uint32_t	GAMESTRUCT_VERSION;

	char		name[LobbyStringSize];
	
	int32_t dwSize;
	int32_t dwFlags;
	char host[40];	// host's ip address (can fit a full IPv4 and IPv6 address + terminating NUL)
	int32_t dwMaxPlayers;
	int32_t dwCurrentPlayers;
	int32_t dwUserFlags[4];
	
	char		secondaryHosts[2][40];
	char		extra[Lobbyextra_string_size];		// extra string (future use)
	uint16_t	hostPort;						// server port
	char		mapname[Lobbymap_string_size];		// map server is hosting
	char		hostname[Lobbyhostname_string_size];	// ...
	char		versionstring[LobbyStringSize];		//
	char		modlist[Lobbymodlist_string_size];	// ???
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
	uint32_t roomsLen = 0;
	struct LobbyGame* rooms = NULL;
	uint32_t motdLen = 0;
	char* motd = NULL;
};

int GetLobby(struct LobbyResponse* r, int t = 3);
void FreeLobbyResponse(LobbyResponse* g);

#endif