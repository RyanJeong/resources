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
var nodes, urls         []string
var nodesToUrls         map[string]string

//  Args[1] : node 수
//  Args[2] : byzantine node 수
//  Args[3] : localhost offset 지정
func Init() {
    var offset int

    nodesToUrls = make(map[string]string)
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
        nodesToUrls[nodes[i]] = urls[i]
    }
}

func Start() {
    //TODO  : Need to be implemented
}

func PrintArgs() {
    for i := 0; i < nNodes; i++ {
        fmt.Printf("[%d] Node name: %s, URL: %s, nodesToUrls: %s\n", i, nodes[i], urls[i], nodesToUrls[nodes[i]])
    }
    fmt.Printf("Number of byzantines: %d\n", nByzantines)
}
