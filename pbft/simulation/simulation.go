package simulation

import (
    "strconv"
	"os"
    "fmt"

/*
    TODO
	"github.com/RyanJeong/Go_PBFT/pbft/network"
    "../network"
*/
)

var nNodes, nByzantines int
var nodes, urls []string

func Init() {
    var offset int

    if len(os.Args) > 1 {
        nNodes, _ = strconv.Atoi(os.Args[1])
    } else {
        nNodes = 100
    }
    if len(os.Args) > 2 {
        nByzantines, _ = strconv.Atoi(os.Args[2])
    } else {
        nByzantines = 0
    }
    if len(os.Args) > 3 {
        offset, _ = strconv.Atoi(os.Args[3])
    } else {
        offset = 4000
    }
    for i := 0; i < nNodes; i++ {
        nodes   = append(nodes, "Node-" + strconv.Itoa(i))
        urls    = append(urls, "localhost:" + strconv.Itoa(i + offset))
    }
}

/*
	nodeID := os.Args[1]
	server := network.NewServer(nodeID, 0)

	server.Start()
*/
func Start() {
    /*
    network.Init(nodes, hosts, nByzantines)
    network.Start()
    */
}

func PrintArgs() {
    for i := 0; i < nNodes; i++ {
        fmt.Printf("[%d] Node name: %s, URL: %s\n", i, nodes[i], urls[i])
    }
    fmt.Printf("Number of byzantines: %d\n", nByzantines)
}
