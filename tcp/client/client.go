package main

import (
	"log"
	"net"
	"time"
)

const ip = "192.168.0.26"
const port = ":7654"

func main() {
	conn, err := net.Dial("tcp", ip+port)
	if nil != err {
		log.Fatalf("failed to connect to server")
	}

	// do something ...

	for {
		// heartbeat
		conn.Write([]byte("ping"))
		time.Sleep(time.Duration(1) * time.Second)
	}
}
