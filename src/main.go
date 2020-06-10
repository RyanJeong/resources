package main

import (
	"github.com/ryanjeong/go_blockchain/src/block"
)

func main() {
	bc := block.NewBlockchain()

	bc.AddBlock("Send 1 BTC to Ivan")
	bc.AddBlock("Send 2 more BTC to Ivan")

	return
}
