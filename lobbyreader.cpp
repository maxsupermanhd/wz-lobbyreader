#include "lobbyreader.h"

#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

static void SetSocketTimeout(int fd, int timeout) {
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
}

static int OpenSocket() {
	int sockfd = 0;
	struct sockaddr_in serv_addr;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(9990);
	if(inet_pton(AF_INET, "88.198.45.216", &serv_addr.sin_addr)<=0) {
		return -2;
	}
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		return -3;
	}
	return sockfd;
}

static bool ReadU32(int s, uint32_t* t) {
	uint32_t r;
	if(recv(s, &r, sizeof(uint32_t), MSG_WAITALL) != sizeof(uint32_t)) {
		return false;
	}
	r = ntohl(r);
	*t = r;
	return true;
}

static bool ReadU16(int s, uint16_t* t) {
	uint16_t r;
	if(recv(s, &r, sizeof(uint16_t), MSG_WAITALL) != sizeof(uint16_t)) {
		return false;
	}
	r = ntohs(r);
	*t = r;
	return true;
}

static bool ReadI32(int s, int32_t* t) {
	int32_t r;
	if(recv(s, &r, sizeof(int32_t), MSG_WAITALL) != sizeof(int32_t)) {
		return false;
	}
	r = ntohl(r);
	*t = r;
	return true;
}

static bool ReadChars(int s, char* p, size_t size) {
	if(recv(s, p, size, MSG_WAITALL) != size) {
		return false;
	}
	return true;
}

static bool ReadString(int s, std::string &p, size_t size) {
	char* m = (char*)malloc(size+1);
	if(recv(s, m, size, MSG_WAITALL) != size) {
		return false;
	}
	m[size] = '\0';
	p = std::string(m, size);
	free(m);
	return true;
}

static bool ReadGAMESTRUCT(int s, LobbyGame* g) {
	if( !ReadU32(s, &g->GAMESTRUCT_VERSION) ||
		!ReadString(s, g->name, LobbyStringSize) ||
		!ReadI32(s, &g->dwSize) ||
		!ReadI32(s, &g->dwFlags) ||
		!ReadString(s, g->host, 40) ||
		!ReadI32(s, &g->dwMaxPlayers) ||
		!ReadI32(s, &g->dwCurrentPlayers) ||
		!ReadI32(s, &g->dwUserFlags[0]) ||
		!ReadI32(s, &g->dwUserFlags[1]) ||
		!ReadI32(s, &g->dwUserFlags[2]) ||
		!ReadI32(s, &g->dwUserFlags[3]) ||
		!ReadString(s, g->secondaryHosts[0], 40) ||
		!ReadString(s, g->secondaryHosts[1], 40) ||
		!ReadString(s, g->extra, Lobbyextra_string_size) ||
		!ReadU16(s, &g->hostPort) ||
		!ReadString(s, g->mapname, Lobbymap_string_size) ||
		!ReadString(s, g->hostname, Lobbyhostname_string_size) ||
		!ReadString(s, g->versionstring, LobbyStringSize) ||
		!ReadString(s, g->modlist, Lobbymodlist_string_size) ||
		!ReadU32(s, &g->game_version_major) ||
		!ReadU32(s, &g->game_version_minor) ||
		!ReadU32(s, &g->privateGame) ||
		!ReadU32(s, &g->pureMap) ||
		!ReadU32(s, &g->Mods) ||
		!ReadU32(s, &g->gameId) ||
		!ReadU32(s, &g->limits) ||
		!ReadU32(s, &g->future3) ||
		!ReadU32(s, &g->future4)) {
		return false;
	}
	return true;
}

int GetLobby(struct LobbyResponse* r, int t /*= 3*/) {
	int s = OpenSocket();
	if(s < 0) {
		return 1;
	}
	SetSocketTimeout(s, t);
	if(write(s, "list", strlen("list")) != strlen("list")) {
		return 1;
	}
	uint32_t roomsLen = -1;
	if(!ReadU32(s, &roomsLen) || roomsLen < 0) {
		return 1;
	}
	for(int i=0; i<roomsLen; i++) {
		struct LobbyGame g;
		if(!ReadGAMESTRUCT(s, &g)) {
			return 1;
		}
		r->rooms.push_back(g);
	}
	uint32_t code = -1, motdLen = 0, params = 0;
	if( !ReadU32(s, &code) ||
		!ReadU32(s, &motdLen) ||
		!ReadString(s, r->motd, motdLen)) {
		return 1;
	}
	if(!ReadU32(s, &params)) {
		return 0;
	}
	#define IGNORE_FIRST_BATCH 1
	if((params & IGNORE_FIRST_BATCH) == IGNORE_FIRST_BATCH) {
		r->rooms.clear();
		if(!ReadU32(s, &roomsLen) || roomsLen < 0) {
			return 1;
		}
		for(int i=0; i<roomsLen; i++) {
			struct LobbyGame g;
			if(!ReadGAMESTRUCT(s, &g)) {
				return 1;
			}
			r->rooms.push_back(g);
		}
	}
	return 0;
}
