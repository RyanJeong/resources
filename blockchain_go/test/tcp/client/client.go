package main

import (
	"bytes"
	"encoding/binary"
	"encoding/gob"
	"log"
	"net"
	"sync"
	"time"
)

const port = ":7654"
const ip = "192.168.0.26"

type Data struct {
	Header HeaderMsg
	Body   interface{}
}
type HeaderMsg struct {
	Type string
	Date string
}
type BodyMsg struct {
	Content string
}

type ConnectionInfo struct {
	cSend  chan Data
	conn   net.Conn
	enc    *gob.Encoder
	bufEnc bytes.Buffer
	mutex  sync.Mutex
}

func init() {
	// To enable the structure to be gob encoded
	// Using gob you can encode and decode structure
	gob.Register(BodyMsg{})

	return
}

func main() {
	conn, err := net.Dial("tcp", ip+port)
	if nil != err {
		log.Fatalf("failed to connect to server")
	}

	ci := ConnectionInfo{
		cSend: make(chan Data),
		conn:  conn,
	}
	ci.enc = gob.NewEncoder(&ci.bufEnc)

	go ci.DataSender()

	for {
		ci.cSend <- Data{
			Header: HeaderMsg{
				Type: "ping",
				Date: time.Now().UTC().Format(time.RFC3339),
			},
			Body: BodyMsg{
				Content: "Hello world!",
			},
		}
		time.Sleep(time.Duration(5) * time.Second)
	}

	return
}

func (ci *ConnectionInfo) SendMessage(msg Data) error {
	ci.mutex.Lock()
	defer ci.mutex.Unlock()

	ci.bufEnc.Reset()

	ci.enc.Encode(msg)

	bufLen := make([]byte, 4)
	binary.LittleEndian.PutUint32(bufLen, uint32(ci.bufEnc.Len()))

	if _, err := ci.conn.Write(bufLen); nil != err {
		log.Printf("failed to cSend msg length; err: %v", err)
		return err
	}

	if _, err := ci.conn.Write(ci.bufEnc.Bytes()); nil != err {
		log.Printf("failed to cSend msg; err: %v", err)
		return err
	}

	return nil
}

func (ci *ConnectionInfo) DataSender() {
	for {
		select {
		case msg := <-ci.cSend:
			if err := ci.SendMessage(msg); nil != err {
				log.Printf("failed to cSend message; err: %v", err)
			}
		}
	}

	return
}
