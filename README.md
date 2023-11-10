# TCP server in pure c
- A lightweight TCP server in pure c using the POSIX socket api.

# Features
- The server return the current time stamp to clients connected on port 8080.


# Usage
- Run the server

```console
$ git clone https://github.com/HoussemBousmaha/time_server.git
$ cd time_server
$ clang -o main main.c
$ ./main
```

- Send a request
```console
$ curl http://127.0.0.1:8080
```
