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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

static void SetSocketTimeout(int fd, int timeout) {
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
}

static int OpenSocket(int t) {
	int sockfd = 0;
	struct sockaddr_in serv_addr;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}
	SetSocketTimeout(sockfd, t);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(9990);
	char ip[64] = {0};
	struct hostent *he;
	struct in_addr **addr_list;
	if((he = gethostbyname("lobby.wz2100.net")) == NULL) {
		close(sockfd);
		return -1;
	}
	addr_list = (struct in_addr **) he->h_addr_list;
	for(int i = 0; addr_list[i] != NULL; i++) {
		strcpy(ip, inet_ntoa(*addr_list[i]));
	}
	if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0) {
		close(sockfd);
		return -1;
	}
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		close(sockfd);
		return -1;
	}
	return sockfd;
}

static int ReadU32(int s, uint32_t* t) {
	uint32_t r;
	if(recv(s, &r, sizeof(uint32_t), MSG_WAITALL) != sizeof(uint32_t)) {
		return LOBBYREADER_FAIL;
	}
	r = ntohl(r);
	*t = r;
	return LOBBYREADER_SUCCESS;
}

static int ReadU16(int s, uint16_t* t) {
	uint16_t r;
	if(recv(s, &r, sizeof(uint16_t), MSG_WAITALL) != sizeof(uint16_t)) {
		return LOBBYREADER_FAIL;
	}
	r = ntohs(r);
	*t = r;
	return LOBBYREADER_SUCCESS;
}

static int ReadI32(int s, int32_t* t) {
	int32_t r;
	if(recv(s, &r, sizeof(int32_t), MSG_WAITALL) != sizeof(int32_t)) {
		return LOBBYREADER_FAIL;
	}
	r = ntohl(r);
	*t = r;
	return LOBBYREADER_SUCCESS;
}

static int ReadChars(int s, char* p, size_t size) {
	if(recv(s, p, size, MSG_WAITALL) != size) {
		return LOBBYREADER_FAIL;
	}
	return LOBBYREADER_SUCCESS;
}

static int ReadString(int s, std::string &p, size_t size) {
	char* m = (char*)malloc(size+1);
	if(recv(s, m, size, MSG_WAITALL) != size) {
		return LOBBYREADER_FAIL;
	}
	m[size] = '\0';
	p = std::string(m, size);
	free(m);
	return LOBBYREADER_SUCCESS;
}

static int ReadGAMESTRUCT(int s, LobbyGame* g) {
	if( ReadU32(s, &g->GAMESTRUCT_VERSION) ||
		ReadString(s, g->name, LobbyStringSize) ||
		ReadI32(s, &g->dwSize) ||
		ReadI32(s, &g->dwFlags) ||
		ReadString(s, g->host, 40) ||
		ReadI32(s, &g->dwMaxPlayers) ||
		ReadI32(s, &g->dwCurrentPlayers) ||
		ReadI32(s, &g->dwUserFlags[0]) ||
		ReadI32(s, &g->dwUserFlags[1]) ||
		ReadI32(s, &g->dwUserFlags[2]) ||
		ReadI32(s, &g->dwUserFlags[3]) ||
		ReadString(s, g->secondaryHosts[0], 40) ||
		ReadString(s, g->secondaryHosts[1], 40) ||
		ReadString(s, g->extra, Lobbyextra_string_size) ||
		ReadU16(s, &g->hostPort) ||
		ReadString(s, g->mapname, Lobbymap_string_size) ||
		ReadString(s, g->hostname, Lobbyhostname_string_size) ||
		ReadString(s, g->versionstring, LobbyStringSize) ||
		ReadString(s, g->modlist, Lobbymodlist_string_size) ||
		ReadU32(s, &g->game_version_major) ||
		ReadU32(s, &g->game_version_minor) ||
		ReadU32(s, &g->privateGame) ||
		ReadU32(s, &g->pureMap) ||
		ReadU32(s, &g->Mods) ||
		ReadU32(s, &g->gameId) ||
		ReadU32(s, &g->limits) ||
		ReadU32(s, &g->future3) ||
		ReadU32(s, &g->future4)) {
		return LOBBYREADER_FAIL;
	}
	return LOBBYREADER_SUCCESS;
}

int ReadRooms(int s, std::vector<struct LobbyGame> &v) {
	uint32_t roomsLen = -1;
	if(ReadU32(s, &roomsLen) || roomsLen < 0) {
		return LOBBYREADER_FAIL;
	}
	for(int i=0; i<roomsLen; i++) {
		struct LobbyGame g;
		if(ReadGAMESTRUCT(s, &g)) {
			return LOBBYREADER_FAIL;
		}
		v.push_back(g);
	}
	return LOBBYREADER_SUCCESS;
}

int GetLobby(struct LobbyResponse* r, int t /*= 3*/) {
	int s = OpenSocket(t);
	if(s < 0) {
		return LOBBYREADER_FAIL;
	}
	if( write(s, "list", strlen("list")) != strlen("list") ||
		ReadRooms(s, r->rooms)) {
		close(s);
		return LOBBYREADER_FAIL;
	}
	uint32_t code = -1, motdLen = 0, params = 0;
	if( ReadU32(s, &code) ||
		ReadU32(s, &motdLen) ||
		ReadString(s, r->motd, motdLen)) {
		close(s);
		return LOBBYREADER_FAIL;
	}
	if(ReadU32(s, &params)) {
		close(s);
		return LOBBYREADER_SUCCESS;
	}
	if((params & LOBBYREADER_IGNORE_FIRST_BATCH) == LOBBYREADER_IGNORE_FIRST_BATCH) {
		r->rooms.clear();
	}
	if(ReadRooms(s, r->rooms)) {
		close(s);
		return LOBBYREADER_FAIL;
	}
	close(s);
	return LOBBYREADER_SUCCESS;
}
