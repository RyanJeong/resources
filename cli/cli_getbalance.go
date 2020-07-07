package cli

import (
	"fmt"
	"log"

	"github.com/ryanjeong/go_blockchain/base58"
	"github.com/ryanjeong/go_blockchain/block"
	"github.com/ryanjeong/go_blockchain/wallet"
)

func (cli *CLI) getBalance(address, nodeID string) {
	if !wallet.ValidateAddress(address) {
		log.Panic("ERROR: Address is not valid")
	}
	bc := block.NewBlockchain(nodeID)
	UTXOSet := block.UTXOSet{bc}
	defer bc.Db.Close()

	balance := 0
	pubKeyHash := base58.Base58Decode([]byte(address))
	pubKeyHash = pubKeyHash[1 : len(pubKeyHash)-4]
	UTXOs := UTXOSet.FindUTXO(pubKeyHash)

	for _, out := range UTXOs {
		balance += out.Value
	}

	fmt.Printf("Balance of '%s': %d\n", address, balance)
}
