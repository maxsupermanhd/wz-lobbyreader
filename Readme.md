# wz-lobbyreader
Library for dialing and reading lobby server response of Warzone 2100 RTS.

## Usage
`lobbyreader.h` has 2 structures, `LobbyGame` describes lobby room (was copypasted from game sources) and `LobbyResponse` stores std::vector of rooms with motd and lobby code.
Structures do not require any additional allocation or free because std::string and std::vector used everywhere possible.
