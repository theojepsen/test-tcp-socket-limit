# TCP Socket Test

Test how many TCP sockets can be opened simultaneously.

## Usage

1. Build:

    make

2. Test that the server can open 2,000 ports simultaneously, starting from port 10,000:

    ./server 10000 2000

3. From another shell (or host), run the client:

    ./client 127.0.0.1 10000 2000

The server will exit with an error if the file descriptor limit is reached:

  accept: Too many open files

## Increasing file descriptor limits

Show the soft/hard limit for file descriptors:

    ulimit -Sn
    ulimit -Hn

Set the file descriptor limit to the hard limit:

    ulimit -n unlimited

To increase the hard limit, edit `/etc/security/limits.conf` and add:

    *    soft    nofile 32768
    *    hard    nofile 32768
