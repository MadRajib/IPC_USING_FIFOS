## To clone
```
$ git clone https://github.com/MadRajib/IPC_USING_FIFOS.git
```
## To build
```bash
$ cd IPC_USING_FIFOS
$ make server
$ make client
```

## To run server
```bash
IPC_USING_FIFOS $ ./server 
reader
Server: read 28 bytes: "/tmp/fifo_client;index.html
"
fifo_path:/tmp/fifo_client req_url:index.html/
Response sent!
Server: read 37 bytes: "/tmp/fifo_client;index.html/madrajib
"
fifo_path:/tmp/fifo_client req_url:index.html/madrajib
Response sent!
```

## To run client
```bash
IPC_USING_FIFOS $ ./client 
index.html
/tmp/fifo_client;index.html

Server: 17 bytes: "hello anonymous!
"
index.html/madrajib
/tmp/fifo_client;index.html/madrajib

Server: 14 bytes: "hello madrajib"
```