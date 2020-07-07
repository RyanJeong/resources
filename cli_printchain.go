package main

import (
	"fmt"
	"strconv"

	"github.com/ryanjeong/go_blockchain/block"
)

func (cli *CLI) printChain(nodeID string) {
	bc := block.NewBlockchain(nodeID)
	defer bc.Db.Close()

	bci := bc.Iterator()

	for {
		blk := bci.Next()

		fmt.Printf("============ Block %x ============\n", blk.Hash)
		fmt.Printf("Height: %d\n", blk.Height)
		fmt.Printf("Prev. block: %x\n", blk.PrevBlockHash)
		pow := block.NewProofOfWork(blk)
		fmt.Printf("PoW: %s\n\n", strconv.FormatBool(pow.Validate()))
		for _, tx := range blk.Transactions {
			fmt.Println(tx)
		}
		fmt.Printf("\n\n")

		if len(blk.PrevBlockHash) == 0 {
			break
		}
	}
}
