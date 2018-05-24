# Gomoku Server

A Gomoku Server to host matches between Gomoku-playing computer programs.

## Features

- Nice and clean Web GUI with websocket communication.

## Compiling

**Dependencies:**

- Boost 1.67
- [WebSocket++ 0.7.0](https://github.com/zaphoyd/websocketpp/releases/tag/0.7.0)

```
$ ./make
```

## Usage

### Using the server

```
$ ./gomoku-server <port> <player1> <player2>
```

### Creating players compatible with the server

The program should read commands from the server from `stdin` and respond to `stdout`. All output to `stderr` and `stdlog` will be ignored.

Responses from the player will always begin with `OK`, unless there is an error, in which case the player should print `ERROR` followed by a space and the error message.

#### About

The program will print out its name.

- Input: `ABOUT`
- Output: `OK <player_name>`

Example:

```
$ ABOUT
OK KonaeAkira's Gomoku Bot
```

#### New

The program should initialize itself and start with an empty board with the given properties.

- Input: `NEW <board_size> <winning_condition> <player_side> <total_time> <time_increment>`
- Output: `OK`

*Note:* The total time and time increment are given in seconds. If the time limit of any of them is exceeded, the player will be disqualified.

Example:

```
$ NEW 15 5 BLACK
OK
```

#### Turn

Server signals that it is the player's turn to move. The player should then respond with its move.

- Input: `TURN`
- Output: `OK <move_x> <move_y>`

*Note:* If an invalid move is given, the player will be automatically disqualified.

Example:

```
$ TURN
OK 0 7
```

#### Move

Server updates the player with the opponent's move.

- Input: `MOVE <move_x> <move_y>`
- Output: `OK`

Example:

```
$ MOVE 5 4
OK
```

#### Exit

Upon receiving this command, the player should clean up all its used resources and perform a clean exit.

- Input: `EXIT`
- Output: none

*Note:* The player should immediately exit without responding to the server.

Example:

```
$ EXIT
```
