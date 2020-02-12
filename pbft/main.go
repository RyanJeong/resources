package main

import (
    "strconv"
	"os"

/*
	"github.com/RyanJeong/Go_PBFT/pbft/network"
*/
)


func main() {
	
    nNodes, _       := strconv.Atoi(os.Args[1])
    nByzantine, _   := strconv.Atoi(os.Args[2])
    println(nNodes)
    println(nByzantine)
    for i := 0; i < nNodes; i++ {
        println("Node-" + strconv.Itoa(i))
        println("localhost:" + strconv.Itoa(i + 4243))
    }
/*
	nodeID := os.Args[1]
	server := network.NewServer(nodeID, 0)

	server.Start()
*/
}
