package cli

import (
	"fmt"

	"github.com/ryanjeong/go_blockchain/wallet"
)

func (cli *CLI) createWallet(nodeID string) {
	wallets, _ := wallet.NewWallets(nodeID)
	address := wallets.CreateWallet()
	wallets.SaveWallet(nodeID)

	fmt.Printf("Your new address: %s\n", address)
}
