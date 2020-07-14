package main

import (
	"io"
	"log"
	"net"
)

const port = ":7654"

func main() {
	l, err := net.Listen("tcp", port)
	if nil != err {
		log.Fatalf("fail to bind address to %s; err: %v", port[1:], err)
	}
	defer l.Close()

	for {
		conn, err := l.Accept()
		if nil != err {
			log.Printf("fail to accept; err: %v", err)
			continue
		}
		go ConnHandler(conn)
	}
}

func ConnHandler(conn net.Conn) {
	recv := make([]byte, 4096) // receive buffer: 4kB
	for {
		n, err := conn.Read(recv)
		if nil != err {
			if io.EOF == err {
				log.Printf("connection is closed from client; %v", conn.RemoteAddr().String())

				return
			}
			log.Printf("fail to receive data; err: %v", err)

			return
		}
		if 0 < n {
			data := recv[:n]
			log.Println(string(data))
		}
	}
}
