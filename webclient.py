#!/usr/bin/python3

# Utility to connect a Spaghetti player that implements that 2022 Caia protocol
# to my game framework, which uses HTTP POST request and HTTP even streams.
#
# Warning: the code here is pretty ugly!
#
# Example use:
#
# read -r session && ./webclient.py --session "$session" ~/caia/spaghetti/bin/player1
# http://localhost:8080/session.html#gameId=spaghetti&gameName=Spaghetti&sessionId=XXXXXX&playerId=1&playerKey=XXXXXX

import argparse
import json
import re
import socket
import subprocess
import sys
from urllib.parse import urlparse, urljoin, quote, unquote
from urllib.request import urlopen, Request
from urllib.error import HTTPError

argument_parser = argparse.ArgumentParser(description='Connects a Spaghetti player using the CodeCup 2021 protocol to the GameFrame server')
argument_parser.add_argument('--session', type=str, nargs=1, help='Session URL', required=True)
argument_parser.add_argument('command', type=str, nargs=1, help='Path to player executable')
argument_parser.add_argument('arg', type=str, nargs='*', help='Player arguments')

GAME_ID = 'spaghetti'
SESSION_URL_TEMPLATE = 'api/sessions/{sessionId}?playerKeys={playerKey}'
EVENT_STREAM_SUFFIX = '&format=event-stream'

MOVE_PATTERN = re.compile("^[a-i][a-g][lsr]$")

def ReadEventStream(f):
    data = b''
    while True:
        line = f.readline()
        if not line:
            break
        if line.startswith(b'data:'):
            if len(line) > 5 and line[5] == b' ':
                data += line[6:]
            else:
                data += line[5:]
        elif not line.strip():
            if data:
                yield data.decode('utf-8')
                data = b''

def PlayGame(command_args, session_url, event_stream_url, player_id, player_key):
    popen = subprocess.Popen(args=command_args, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    moves = []
    for update in ReadEventStream(urlopen(event_stream_url)):
        update = json.loads(update)
        if update['gameId'] != GAME_ID:
            print('Update has incorrect gameId!')
            sys.exit(1)
        new_moves = update['state'].split(',')
        assert len(new_moves) >= len(moves)
        assert new_moves[:len(moves)] == moves
        while len(moves) < len(new_moves):
            move = new_moves[len(moves)]
            print('Sent', move)
            assert MOVE_PATTERN.match(move)
            popen.stdin.write((move + '\n').encode('utf-8'))
            popen.stdin.flush()
            moves.append(move)
        if update['activePlayers'] == []:
            try:
              popen.stdin.write(b'Quit\n')
              popen.stdin.close()
            except:
              # Ignore errors here. It's possible the player already quit.
              pass
            popen.wait()
            return moves
        if update['activePlayers'] == [player_id]:
            assert len(moves) >= 2
            if len(moves) == 2:
                popen.stdin.write(b'Start\n')
                popen.stdin.flush()
            move = popen.stdout.readline().decode('utf-8').strip()
            print('Received', move)
            assert MOVE_PATTERN.match(move)
            update = {
                'moveCount': len(moves),
                'playerKey': player_key,
                'player': player_id,
                'move': move
            }
            request = Request(session_url)
            request.add_header('Content-Type', 'application/json; charset=utf-8')
            try:
                urlopen(request, json.dumps(update).encode('utf-8'))
            except HTTPError as e:
                print(e)
                print(e.read().decode('utf-8'))
                sys.exit(1)
            moves.append(move)

def Main():
    args = argument_parser.parse_args()
    fragment = urlparse(args.session[0]).fragment
    if not fragment:
        print('Session URL is missing a URL fragment!')
        sys.exit(1)
    params = {}
    for part in fragment.split('&'):
        if '=' in part:
            key, value = map(unquote, part.split('=', 1))
            params[key] = value
    for key in ('gameId', 'sessionId', 'playerId', 'playerKey'):
        if key not in params:
            print('Session URL is missing required parameter [{}]'.format(key))
            sys.exit(1)
    if params['gameId'] != GAME_ID:
        print('gameId must be [{}]'.format(GAME_ID))
        sys.exit(1)
    if params['playerId'] not in ('blue', 'red'):
        print('playerId must be [blue] or [red]')
        sys.exit(1)

    session_url = urljoin(
        args.session[0],
        SESSION_URL_TEMPLATE.format(
            playerKey=quote(params['playerKey']),
            sessionId=quote(params['sessionId'])))
    event_stream_url = session_url + EVENT_STREAM_SUFFIX

    moves = PlayGame(args.command + args.arg, session_url, event_stream_url, params['playerId'], params['playerKey'])
    print('Transcript:', ','.join(moves))

if __name__ == '__main__':
    Main()
