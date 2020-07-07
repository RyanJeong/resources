package cli

import (
	"fmt"
	"log"

	"github.com/ryanjeong/go_blockchain/block"
	"github.com/ryanjeong/go_blockchain/wallet"
)

func (cli *CLI) createBlockchain(address, nodeID string) {
	if !wallet.ValidateAddress(address) {
		log.Panic("ERROR: Address is not valid")
	}
	bc := block.CreateBlockchain(address, nodeID)
	defer bc.Db.Close()

	UTXOSet := block.UTXOSet{bc}
	UTXOSet.Reindex()

	fmt.Println("Done!")
}
