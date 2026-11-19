# Assignment 4: Build a multi-client server

For the last assignment, you're going to write a server that a bunch of people can connect to at once. It runs on your own machine, it listens on a TCP port, and it does *something* with what its clients send it. I'm not picky about what, just feel free to get creative with it!

Some ideas to get you going:

- **A chat server.** Everybody who connects picks a nickname, and whatever one person types shows up for everyone else. Add `/who` to list who's online, or `/msg name hello` for whispers.
- **A rock-paper-scissors tournament.** Players join, the server pairs them up, collects both moves, announces the winner, and keeps a scoreboard.
- **A tiny MUD.** A handful of rooms, `look` and `go north` and `say`, and you can see the other players wandering around.
- **A trivia night.** The server asks a question, the first correct answer gets the point, and `scores` shows the standings.
- **Something else entirely:** a group guess-the-number game, a collaborative story where each person adds a line, a little auction house, tic-tac-toe with matchmaking. If you're not sure your idea is big enough (or too big), ask!

## What your server needs to do

It should:

- **Handle several clients at the same time.** A second client should be able to connect and get served while the first is still connected. Nobody should have to wait in line for somebody else to hang up.
- **Do something with each client's input.** Read what clients send, act on it, and send something back to at least the client who sent it. Pick a simple text protocol (one command per line works great) and write it down in your README so the rest of us know what to type.
- **Work with `nc`.** Someone should be able to play with your server using `nc 127.0.0.1 <port>` and nothing else. You're welcome to write your own client too, but it's optional.
- **Survive clients leaving.** When a client disconnects (closes `nc`, hits Ctrl-C, whatever), your server should notice, close that client's socket, clean up whatever state it kept for them, and keep running for everyone else. A client vanishing must not crash the server!
- **Handle errors and clean up.** Check the return values of the socket calls you use, and free and close what you acquire.

You can pick how you handle multiple clients:

- **One thread with `poll()` (or `select()`).** One loop watches the listening socket and every client socket at once and only reads from the ones that are ready. Since everything happens on one thread, sharing data between clients (like a list of who's in the chat room) needs no locking.
- **One thread per client.** Each client gets a thread that reads in a loop. This makes each client's code really straightforward, but any data the threads share needs a mutex. Assignment 3 skills, activate!

Forking a process per client works for servers where clients never interact, but as soon as clients need to see each other (chat, games, the MUD), separate processes make sharing hard. Unless you have a good reason, I'd pick one of the two above.

## A few simple pointers
So while some of our examples were pretty simple, you don't really control how many bytes `recv` pulls in so you'll need to check and make sure you see a newline before you consider a command *done* and take action based on it.

Likewise, `send` can send fewer bytes than you asked for. Write a little helper that loops until everything's been sent, just like the copy loop from the file lectures.

If you send to a client that has already hung up, the kernel can kill your whole server with `SIGPIPE`. Pass `MSG_NOSIGNAL` as the flags argument to `send`, and then deal with the error it returns instead.

## Getting started

The lecture 17 examples are a good place to start. They all echo text back, so they're one step away from doing something more interesting:

- [`step4EchoPoll.c`](https://github.com/clarissalittler/cs201/blob/main/fall2026/lecture17/step4EchoPoll.c) serves many clients from one thread using `poll()`.
- [`echoServerThread.c`](https://github.com/clarissalittler/cs201/blob/main/fall2026/lecture17/echoServerThread.c) starts a thread for each client.
- [`step6Client.c`](https://github.com/clarissalittler/cs201/blob/main/fall2026/lecture17/step6Client.c) is a small client, if you want to write your own.
- [`SocketSteps.md`](https://github.com/clarissalittler/cs201/blob/main/fall2026/lecture17/SocketSteps.md) walks through the whole progression with `nc` commands to try.

Heads up: those examples skip most error checking and the short-send loop so the socket calls are easy to see, so you'll want to fix that.

A good first step is to copy one of them and change the echo into something tiny of your own, like sending the line back in uppercase. Then add per-client state (a nickname, a score), and then the part where clients affect each other.

To try it out, start your server in one terminal:

```sh
./myserver 5000
```

Then connect from two or three more terminals:

```sh
nc 127.0.0.1 5000
```

If you get "Address already in use," pick another port. If you're working over SSH, open several SSH sessions to the same machine.

## Show us what you built

Include transcripts (copy-and-paste from the terminals is fine) showing:

1. **At least two clients connected at the same time**, taking turns or talking over each other, with the server responding to both. Show the output from each client's terminal.
2. **A client leaving partway through.** Kill one `nc` while the others are connected, then show that the remaining clients still work.
3. **Two commands arriving at once.** Something like `printf 'cmd one\ncmd two\n' | nc -N 127.0.0.1 5000` (using real commands from your protocol), showing that your server handles both.

Then, in a few paragraphs or a diagram, explain:

- How your server waits on several clients at once. Walk through what happens when two clients send something at nearly the same time.
- What state you keep for each client and for the server as a whole, and where it lives.
- What happens, step by step, when a client disconnects. Who closes the socket, and who frees that client's state?
- If you used threads: what your mutex protects, and what could go wrong without it.

## Submission

Submit your C source, a Makefile or build instructions, a README describing your protocol (what clients can type and what they'll get back), your test transcripts, and your explanation. Compile with warnings turned on, something like:

```sh
cc -std=c11 -Wall -Wextra -Wpedantic -g -pthread myserver.c -o myserver
```

## If you want to go further

None of these are required, but they're fun:

- Write your own client with `poll()` on the keyboard and the socket, maybe with a nicer display.
- Kick idle clients after a timeout (`poll()` takes one).
- Save state to a file so the scoreboard or the MUD survives a restart.
- Limit how long a line can be, and decide what to do when someone sends more.
