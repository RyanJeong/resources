package cli

import (
	"fmt"
	"log"

	"github.com/ryanjeong/go_blockchain/block"
	"github.com/ryanjeong/go_blockchain/server"
	"github.com/ryanjeong/go_blockchain/wallet"
)

func (cli *CLI) send(from, to string, amount int, nodeID string, mineNow bool) {
	if !wallet.ValidateAddress(from) {
		log.Panic("ERROR: Sender address is not valid")
	}
	if !wallet.ValidateAddress(to) {
		log.Panic("ERROR: Recipient address is not valid")
	}

	bc := block.NewBlockchain(nodeID)
	UTXOSet := block.UTXOSet{bc}
	defer bc.Db.Close()

	wallets, err := wallet.NewWallets(nodeID)
	if err != nil {
		log.Panic(err)
	}
	wallet := wallets.GetWallet(from)

	tx := block.NewUTXOTransaction(&wallet, to, amount, &UTXOSet)

	if mineNow {
		cbTx := block.NewCoinbaseTX(from, "")
		txs := []*block.Transaction{cbTx, tx}

		newBlock := bc.MineBlock(txs)
		UTXOSet.Update(newBlock)
	} else {
		server.SendTx(server.KnownNodes[0], tx)
	}

	fmt.Println("Success!")
}
