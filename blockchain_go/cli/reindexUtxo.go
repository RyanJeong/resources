package cli

import (
	"fmt"

	"github.com/ryanjeong/go_blockchain/block"
)

//func (cli *CLI) reindexUTXO(nodeID string) {
func (cli *CLI) reindexUTXO() {
	// bc := block.NewBlockchain(nodeID)
	bc := block.NewBlockchain()
	UTXOSet := block.UTXOSet{bc}
	UTXOSet.Reindex()

	count := UTXOSet.CountTransactions()
	fmt.Printf("Done! There are %d transactions in the UTXO set.\n", count)
}
