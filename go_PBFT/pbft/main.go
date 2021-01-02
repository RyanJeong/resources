package main

import (
/*
    TODO
	"github.com/RyanJeong/Go_PBFT/pbft/simulation"
*/
    "./simulation"
)

func main() {
    simulation.Init()
    simulation.PrintArgs()
    simulation.Start()
}
