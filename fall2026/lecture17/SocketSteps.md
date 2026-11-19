# Socket servers, one step at a time

These six programs build up a TCP server in small steps. After them come
four short side trips into [UDP and Unix-domain sockets](#other-kinds-of-sockets-udp-and-unix). Each one changes as
little as possible from the one before, so you can `diff` neighbouring steps
to see exactly what's new:

```bash
diff step2HelloLoop.c step3EchoLoop.c
```

Error checking is deliberately minimal so the socket calls stand out. A real
server would check every return value and loop on short sends.

| Step | File | What's new |
|---|---|---|
| 1 | `step1HelloOnce.c` | `socket`, `bind`, `listen`, one `accept`, send a fixed message, exit |
| 2 | `step2HelloLoop.c` | `accept` inside a loop, so many clients can connect one after another |
| 3 | `step3EchoLoop.c` | `recv`/`send` loop: echoes text until the client hangs up (still one client at a time) |
| 4 | `step4EchoPoll.c` | `poll()` watches the listener and every client, so many clients can talk at once |
| 5 | `step5EchoSelect.c` | Same as step 4, written with `select()` |
| 6 | `step6Client.c` | Our own client: works with all five servers, so we don't need `nc` |

Build everything with `make`. Run each server with a port number, for
example `./step1HelloOnce 5000`, then connect from a **second terminal**.
If you see `bind: Address already in use`, someone else (maybe you, in
another terminal) is using that port. Pick a different number.

## Testing with `nc` (netcat)

`nc HOST PORT` connects to a server. Whatever you type goes to the server,
and whatever the server sends is printed. On our Linux machines `nc` is the
OpenBSD version, and two flags matter here:

- `-d` means don't read the keyboard. `nc` then quits as soon as the server
  hangs up. Use it for steps 1 and 2, where the server talks and you only
  listen.
- `-N` means that when you press Ctrl-D, `nc` tells the server you're done.
  The server's `recv()` then returns 0. Use it for steps 3–5. Without it,
  Ctrl-D does nothing visible and `nc` just sits there.

Ctrl-C always works: it kills `nc`, and the kernel closes its socket.

### Step 1: one client, then exit

```bash
./step1HelloOnce 5000
```

In another terminal:

```bash
nc -d 127.0.0.1 5000
```

You see `Hello from the step 1 server! Goodbye.` and the server exits.
Run the same `nc` again and you get `Connection refused`. Nobody's listening
any more.

Without `-d`, plain `nc 127.0.0.1 5000` prints the message and then waits
for you. Press Ctrl-D to quit.

### Step 2: many clients, one after another

```bash
./step2HelloLoop 5000
```

```bash
nc -d 127.0.0.1 5000
```

Each connection gets `Hello, you are client #N!` with N counting up, and the
server keeps running. Connect five times in a row:

```bash
for i in 1 2 3 4 5; do nc -d 127.0.0.1 5000; done
```

### Step 3: echo, but only one client at a time

```bash
./step3EchoLoop 5000
```

```bash
nc -N 127.0.0.1 5000
```

Type lines and they come back. Ctrl-D hangs up. To send text without typing:

```bash
printf 'one\ntwo\n' | nc -N 127.0.0.1 5000
```

**See the limitation.** Open two client terminals and connect both. Text from
the first client echoes. Text from the second gets no reply until the first
one hangs up. Then it all arrives at once. The server is stuck in the inner
`recv` loop with the first client and hasn't called `accept` for the second.
The kernel already finished the second connection, which is what the
`listen` backlog is for.

### Steps 4 and 5: many clients at once

```bash
./step4EchoPoll 5000
```

or

```bash
./step5EchoSelect 5000
```

Connect two or three clients with `nc -N 127.0.0.1 5000` and type in any of
them, in any order. Every client gets its echo right away. The server prints
which file descriptor each client got and when each one hangs up.

### Step 6: our own client

```bash
./step6Client 127.0.0.1 5000
```

This works against any of the five servers:

- **Steps 1 and 2.** It prints the message and exits by itself when the
  server hangs up, with no `-d` needed.
- **Steps 3–5.** Type lines and they echo back. Ctrl-D hangs up cleanly, with
  no `-N` needed.

Why does a client need `poll()`? It waits on two things, the keyboard and
the socket, and it can't know which will have something first. If it blocked
in `fgets` it would miss the step 1 server's message. If it blocked in `recv`
you couldn't type. `nc` uses the same trick.

## Other kinds of sockets: UDP and Unix

Steps 1–6 are all TCP: `AF_INET` plus `SOCK_STREAM`. Change either argument to
`socket()` and you get a different kind of socket, but the program still has
the same shape. Each server below is step 3 with as little changed as possible.

| File | What's different from step 3 |
|---|---|
| `udpEcho.c` | `SOCK_DGRAM`: no `listen`/`accept`, one socket for everyone, `recvfrom`/`sendto` |
| `udpSend.c` | A UDP client that sends each argument as its own datagram |
| `unixEcho.c` | `AF_UNIX`: the address is a file name instead of an IP and port |
| `socketPair.c` | `socketpair()`: two connected Unix sockets, no name at all, shared with `fork` |

The comments at the top of each file are different, so a plain `diff` is
noisy. To compare just the code:

```bash
diff <(sed -n '/^#include/,$p' step3EchoLoop.c) <(sed -n '/^#include/,$p' unixEcho.c)
```

### UDP: datagrams, not a stream

```bash
./udpEcho 5000
```

```bash
nc -u 127.0.0.1 5000
```

Type lines and they come back. There's no Ctrl-D hang-up, because there was
never a connection, so use Ctrl-C. Connect two or three `nc -u` clients: they
all work at once with no `poll()`. The server never sits in a loop with one
client the way step 3 does, because every datagram arrives on the same socket
with its sender's address attached.

Now use our own client:

```bash
./udpSend 127.0.0.1 5000 one two three
```

The server prints three lines: 3, 3, and 5 bytes. **UDP keeps message
boundaries.** One `sendto` is one `recvfrom`, unlike TCP, where those three
sends could arrive as one `recv` of `onetwothree`.

Stop the server and run `udpSend` again. Every `sendto` still reports
success. TCP said "Connection refused"; UDP drops the datagram and nobody
tells the sender. The client only finds out because it stops waiting after
one second (`SO_RCVTIMEO`). Retrying, ordering, and noticing loss are the
application's job now, which is why DNS, games, and video calls use UDP and
the web (and your Assignment 4 server) don't.

### Unix-domain sockets: IPC on one machine

```bash
./unixEcho echo.sock
```

```bash
nc -U -N echo.sock
```

This behaves exactly like step 3, from the same directory. The difference
is where the bytes go: never onto the network, just from one process to
another through the kernel. Things to look at:

- `ls -l echo.sock` starts with `s`. The socket is a file, and its
  permissions decide who's allowed to connect.
- The server prints the client's **pid and uid**. The kernel knows who's on
  the other end and tells us (`SO_PEERCRED`). A TCP server only gets an
  address.
- Ctrl-C the server and `echo.sock` is still there. `bind` fails with
  "Address already in use" if the file exists, which is why the server
  `unlink`s it first.
- `ss -lx` lists the Unix sockets that are listening on your machine right
  now. Many system services (the display server, systemd, Docker) talk this way.

### `socketpair`: a two-way pipe

```bash
./socketPair
```

The parent sends lines and the child sends them back in upper case. Compare
with `fdPipe.c` from lecture 15. A pipe only goes one way, so a conversation
needs two pipes. Each end of a socket pair can send *and* receive, so one
pair is enough. There's no file name, `bind`, or `listen`, because the two
ends are connected from the start and get shared through `fork`.

## Things to try

- Start step 3 and step 4 side by side on different ports. Connect two
  clients to each and compare.
- In step 4, set `MAX_FDS` to 3 and connect four clients. What happens to the
  fourth?
- Step 4 marks an empty slot with `fd = -1`, and `poll` skips negative
  descriptors. Step 5 has to rebuild its `fd_set` on every pass. Why can't it
  reuse the one from last time?
- Kill a client with Ctrl-C while it's connected to step 4. Which branch of
  the server handles it?
- Run `udpEcho` and `step3EchoLoop` on the *same* port number at the same
  time. Why doesn't the second `bind` fail?
- Use `chmod 600 echo.sock` and then try connecting as another user (or
  `sudo -u nobody nc -U echo.sock`). What error do you get?
- Can `udpSend` reach `step3EchoLoop`? Can `nc -N 127.0.0.1 5000` reach
  `udpEcho`? What does each side see?
