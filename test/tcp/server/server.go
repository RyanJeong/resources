package main

import (
	"bytes"
	"encoding/binary"
	"encoding/gob"
	"io"
	"log"
	"net"
	"sync"
)

const port = ":7654"

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
	cRecv   chan Data
	conn    net.Conn
	recvBuf []byte
	dec     *gob.Decoder
	decBuf  bytes.Buffer
	mutex   sync.Mutex
}

func init() {
	// To enable the structure to be gob encoded
	// Using gob you can encode and decode structure
	gob.Register(BodyMsg{})
}

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

	return
}

func ConnHandler(conn net.Conn) {
	ci := ConnectionInfo{
		cRecv:   make(chan Data),
		conn:    conn,
		recvBuf: make([]byte, 4096),
	}
	ci.dec = gob.NewDecoder(&ci.decBuf)
	ci.receiver()

	return
}

func (ci *ConnectionInfo) receiver() {
	go func() {
		for {
			msg, err := ci.RecvMsg()
			if nil != err {
				if io.EOF == err {
					log.Printf("connection is closed from client; %v", ci.conn.RemoteAddr().String())

					return
				}
				log.Printf("failed to recv message! err: %v", err)
				continue
			}

			ci.cRecv <- msg
		}
	}()
	for {
		select {
		case msg := <-ci.cRecv:
			log.Println(msg)
		}
	}

	return
}

func (ci *ConnectionInfo) RecvMsg() (Data, error) {
	ci.mutex.Lock()
	defer ci.mutex.Unlock()

	bufLen := make([]byte, 4)
	_, err := ci.conn.Read(bufLen)
	if nil != err {

		return Data{}, err
	}
	msgLen := binary.LittleEndian.Uint32(bufLen)
	ci.decBuf.Reset()

	for msgLen > 0 {
		n, err := ci.conn.Read(ci.recvBuf)
		if nil != err {

			return Data{}, err
		}
		if n > 0 {
			data := ci.recvBuf[:n]
			ci.decBuf.Write(data)
			msgLen -= uint32(n)
		}
	}

	msg := Data{}
	if err = ci.dec.Decode(&msg); nil != err {
		log.Printf("failed to decode message; err: %v", err)

		return msg, err
	}

	return msg, nil
}
