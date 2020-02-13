package network

import (
	"bytes"
	"encoding/json"
	"fmt"
	"net/http"
	"time"

/*
    TODO
	"github.com/RyanJeong/Go_PBFT/pbft/consensus"
*/
	"../consensus"
)

//func NewServer(nodeID string, isByzantine int) *Server {
func InitServer(nodeID string, isByzantine int) *Server {
	node := NewNode(nodeID, isByzantine)
	server := &Server{node.NodeTable[nodeID], time.Now().UnixNano(), node}
	fmt.Println("time:::::: ", server.beginTime)
	server.setRoute()

	return server
}

func (server *Server) Start() {
	fmt.Printf("Server will be started at %s...\n", server.url)
	if err := http.ListenAndServe(server.url, nil); err != nil {
		fmt.Println(err)
		return
	}
}

