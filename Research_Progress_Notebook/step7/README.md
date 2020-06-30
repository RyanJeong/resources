##  files<br>
```Text
step6
├── base58.go
├── block.go
├── blockchain.go
├── blockchain_iterator.go
├── cli.go
├── cli_createblockchain.go
├── cli_createwallet.go
├── cli_getbalance.go
├── cli_listaddress.go
├── cli_printchain.go
├── cli_reindexutxo.go
├── cli_send.go
├── cli_startnode.go
├── main.go
├── merkle_tree.go
├── merkle_tree_test.go
├── proofofwork.go
├── server.go
├── transaction.go
├── transaction_input.go
├── transaction_output.go
├── utils.go
├── utxo_set.go
├── wallet.go
└── wallets.go
```

##  codes<br>
```Go
// base58.go
package main

import (
	"bytes"
	"math/big"
)

var b58Alphabet = []byte("123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz")

// Base58Encode encodes a byte array to Base58
func Base58Encode(input []byte) []byte {
	var result []byte

	x := big.NewInt(0).SetBytes(input)

	base := big.NewInt(int64(len(b58Alphabet)))
	zero := big.NewInt(0)
	mod := &big.Int{}

	for x.Cmp(zero) != 0 {
		x.DivMod(x, base, mod)
		result = append(result, b58Alphabet[mod.Int64()])
	}

	ReverseBytes(result)
	for b := range input {
		if b == 0x00 {
			result = append([]byte{b58Alphabet[0]}, result...)
		} else {
			break
		}
	}

	return result
}

// Base58Decode decodes Base58-encoded data
func Base58Decode(input []byte) []byte {
	result := big.NewInt(0)
	zeroBytes := 0

	for b := range input {
		if b == 0x00 {
			zeroBytes++
		}
	}

	payload := input[zeroBytes:]
	for _, b := range payload {
		charIndex := bytes.IndexByte(b58Alphabet, b)
		result.Mul(result, big.NewInt(58))
		result.Add(result, big.NewInt(int64(charIndex)))
	}

	decoded := result.Bytes()
	decoded = append(bytes.Repeat([]byte{byte(0x00)}, zeroBytes), decoded...)

	return decoded
}
```

```Go
// block.go
package main

import (
    "bytes"
    "crypto/sha256"
    "encoding/gob"
    "log"
    "time"
)

// Block keeps block headers
type Block struct {
    Timestamp     int64          // current timestamp (when the block is created)
    Transactions  []*Transaction // every block must store at least one transaction
    PrevBlockHash []byte         // stores the hash of the previous block
    Hash          []byte         // hash of the block
    Nonce         int            // need to verify a proof
}

// NewBlock creates and returns Block
func NewBlock(transactions []*Transaction, prevBlockHash []byte) *Block {
    block := &Block{time.Now().Unix(), transactions, prevBlockHash, []byte{}, 0}
    pow := NewProofOfWork(block)
    nonce, hash := pow.Run()

    block.Hash = hash[:]
    block.Nonce = nonce

    return block
}

// NewGenesisBlock creates and returns genesis Block
func NewGenesisBlock(coinbase *Transaction) *Block {
    return NewBlock([]*Transaction{coinbase}, []byte{})
}

// HashTransactions returns a hash of the transactions in the block
func (b *Block) HashTransactions() []byte {
    var txHashes [][]byte
    var txHash [32]byte

    for _, tx := range b.Transactions {
		transactions = append(transactions, tx.Serialize())
    }
	mTree := NewMerkleTree(transactions)

    return mTree.RootNode.Data
}

// Serialize serializes the block
func (b *Block) Serialize() []byte {
    var result bytes.Buffer
    encoder := gob.NewEncoder(&result)

    err := encoder.Encode(b)
    if err != nil {
        log.Panic(err)
    }

    return result.Bytes()
}

// DeserializeBlock deserializes a block
func DeserializeBlock(d []byte) *Block {
    var block Block

    decoder := gob.NewDecoder(bytes.NewReader(d))
    err := decoder.Decode(&block)
    if err != nil {
        log.Panic(err)
    }

    return &block
}
```

```Go
// blockchain.go
package main

import (
	"bytes"
	"crypto/ecdsa"
	"encoding/hex"
	"errors"
	"fmt"
	"log"
	"os"

    "github.com/boltdb/bolt"
)

/*
const dbFile = "blockchain.db"
*/
// New added
const dbFile = "blockchain_%s.db"
//
const blocksBucket = "blocks"
const genesisCoinbaseData = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks"

// Blockchain keeps a sequence of Blocks
type Blockchain struct {
    tip []byte
    db  *bolt.DB
}

// CreateBlockchain creates a new blockchain DB
func CreateBlockchain(address string) *Blockchain {
    if dbExists() {
        fmt.Println("Blockchain already exists.")
        os.Exit(1)
    }

    var tip []byte

	cbtx := NewCoinbaseTX(address, genesisCoinbaseData)
	genesis := NewGenesisBlock(cbtx)

    db, err := bolt.Open(dbFile, 0600, nil)
    if err != nil {
        log.Panic(err)
    }

    err = db.Update(func(tx *bolt.Tx) error {
        b, err := tx.CreateBucket([]byte(blocksBucket))
        if err != nil {
            log.Panic(err)
        }

        err = b.Put(genesis.Hash, genesis.Serialize())
        if err != nil {
            log.Panic(err)
        }

        err = b.Put([]byte("l"), genesis.Hash)
        if err != nil {
            log.Panic(err)
        }
        tip = genesis.Hash

        return nil
    })

    if err != nil {
        log.Panic(err)
    }

    bc := Blockchain{tip, db}

    return &bc
}

// NewBlockchain creates a new Blockchain with genesis Block
/*
func NewBlockchain() *Blockchain {
*/
// New added
func NewBlockchain(nodeID string) *Blockchain {
//
    /*
	if dbExists() == false {
		fmt.Println("No existing blockchain found. Create one first.")
		os.Exit(1)
	}
    */
    // New added
	dbFile := fmt.Sprintf(dbFile, nodeID)
	if dbExists(dbFile) == false {
		fmt.Println("No existing blockchain found. Create one first.")
		os.Exit(1)
	}
    //

	var tip []byte
	db, err := bolt.Open(dbFile, 0600, nil)
	if err != nil {
		log.Panic(err)
	}

	err = db.Update(func(tx *bolt.Tx) error {
		b := tx.Bucket([]byte(blocksBucket))
		tip = b.Get([]byte("l"))

		return nil
	})
	if err != nil {
		log.Panic(err)
	}

	bc := Blockchain{tip, db}

	return &bc
}

// New added
// AddBlock saves the block into the blockchain
func (bc *Blockchain) AddBlock(block *Block) {
	err := bc.db.Update(func(tx *bolt.Tx) error {
		b := tx.Bucket([]byte(blocksBucket))
		blockInDb := b.Get(block.Hash)

		if blockInDb != nil {
			return nil
		}

		blockData := block.Serialize()
		err := b.Put(block.Hash, blockData)
		if err != nil {
			log.Panic(err)
		}

		lastHash := b.Get([]byte("l"))
		lastBlockData := b.Get(lastHash)
		lastBlock := DeserializeBlock(lastBlockData)

		if block.Height > lastBlock.Height {
			err = b.Put([]byte("l"), block.Hash)
			if err != nil {
				log.Panic(err)
			}
			bc.tip = block.Hash
		}

		return nil
	})
	if err != nil {
		log.Panic(err)
	}
}
//

// FindTransaction finds a transaction by its ID
func (bc *Blockchain) FindTransaction(ID []byte) (Transaction, error) {
	bci := bc.Iterator()

	for {
		block := bci.Next()

		for _, tx := range block.Transactions {
			if bytes.Compare(tx.ID, ID) == 0 {
				return *tx, nil
			}
		}

		if len(block.PrevBlockHash) == 0 {
			break
		}
	}

	return Transaction{}, errors.New("Transaction is not found")
}

// FindUTXO finds all unspent transaction outputs and returns transactions with spent outputs removed
func (bc *Blockchain) FindUTXO() map[string]TXOutputs {
	UTXO := make(map[string]TXOutputs)
	spentTXOs := make(map[string][]int)
	bci := bc.Iterator()

	for {
		block := bci.Next()

		for _, tx := range block.Transactions {
			txID := hex.EncodeToString(tx.ID)

		Outputs:
			for outIdx, out := range tx.Vout {
				// Was the output spent?
				if spentTXOs[txID] != nil {
					for _, spentOutIdx := range spentTXOs[txID] {
						if spentOutIdx == outIdx {
							continue Outputs
						}
					}
				}

				outs := UTXO[txID]
				outs.Outputs = append(outs.Outputs, out)
				UTXO[txID] = outs
			}

			if tx.IsCoinbase() == false {
				for _, in := range tx.Vin {
					inTxID := hex.EncodeToString(in.Txid)
					spentTXOs[inTxID] = append(spentTXOs[inTxID], in.Vout)
				}
			}
		}

		if len(block.PrevBlockHash) == 0 {
			break
		}
	}
    //

    return UTXOs
}

// Iterator returns a BlockchainIteratator
func (bc *Blockchain) Iterator() *BlockchainIterator {
    bci := &BlockchainIterator{bc.tip, bc.db}

    return bci
}

// New added
// GetBestHeight returns the height of the latest block
func (bc *Blockchain) GetBestHeight() int {
	var lastBlock Block

	err := bc.db.View(func(tx *bolt.Tx) error {
		b := tx.Bucket([]byte(blocksBucket))
		lastHash := b.Get([]byte("l"))
		blockData := b.Get(lastHash)
		lastBlock = *DeserializeBlock(blockData)

		return nil
	})
	if err != nil {
		log.Panic(err)
	}

	return lastBlock.Height
}

// GetBlock finds a block by its hash and returns it
func (bc *Blockchain) GetBlock(blockHash []byte) (Block, error) {
	var block Block

	err := bc.db.View(func(tx *bolt.Tx) error {
		b := tx.Bucket([]byte(blocksBucket))

		blockData := b.Get(blockHash)

		if blockData == nil {
			return errors.New("Block is not found.")
		}

		block = *DeserializeBlock(blockData)

		return nil
	})
	if err != nil {
		return block, err
	}

	return block, nil
}

// GetBlockHashes returns a list of hashes of all the blocks in the chain
func (bc *Blockchain) GetBlockHashes() [][]byte {
	var blocks [][]byte
	bci := bc.Iterator()

	for {
		block := bci.Next()

		blocks = append(blocks, block.Hash)

		if len(block.PrevBlockHash) == 0 {
			break
		}
	}

	return blocks
}
//

// MineBlock mines a new block with the provided transactions
func (bc *Blockchain) MineBlock(transactions []*Transaction) *Block {
    var lastHash []byte
    // New added
	var lastHeight int
    //

	for _, tx := range transactions {
		// TODO: ignore transaction if it's not valid
		if bc.VerifyTransaction(tx) != true {
			log.Panic("ERROR: Invalid transaction")
		}
	}

    // read-only
    err := bc.db.View(func(tx *bolt.Tx) error {
        b := tx.Bucket([]byte(blocksBucket))
        lastHash = b.Get([]byte("l"))

        // New added
		blockData := b.Get(lastHash)
		block := DeserializeBlock(blockData)

		lastHeight = block.Height
        //

        return nil
    })

    if err != nil {
        log.Panic(err)
    }

    /*
    newBlock := NewBlock(transactions, lastHash)
    */
    // New added
	newBlock := NewBlock(transactions, lastHash, lastHeight+1)
    //

    // read, write
    err = bc.db.Update(func(tx *bolt.Tx) error {
        b := tx.Bucket([]byte(blocksBucket))
        err := b.Put(newBlock.Hash, newBlock.Serialize())
        if err != nil {
            log.Panic(err)
        }

        err = b.Put([]byte("l"), newBlock.Hash)
        if err != nil {
            log.Panic(err)
        }

        bc.tip = newBlock.Hash

        return nil
    })
	if err != nil {
		log.Panic(err)
	}

    return newBlock
}

// SignTransaction signs inputs of a Transaction
func (bc *Blockchain) SignTransaction(tx *Transaction, privKey ecdsa.PrivateKey) {
	prevTXs := make(map[string]Transaction)

	for _, vin := range tx.Vin {
		prevTX, err := bc.FindTransaction(vin.Txid)
		if err != nil {
			log.Panic(err)
		}
		prevTXs[hex.EncodeToString(prevTX.ID)] = prevTX
	}

	tx.Sign(privKey, prevTXs)
}

// VerifyTransaction verifies transaction input signatures
func (bc *Blockchain) VerifyTransaction(tx *Transaction) bool {
	if tx.IsCoinbase() {
		return true
	}

	prevTXs := make(map[string]Transaction)

	for _, vin := range tx.Vin {
		prevTX, err := bc.FindTransaction(vin.Txid)
		if err != nil {
			log.Panic(err)
		}
		prevTXs[hex.EncodeToString(prevTX.ID)] = prevTX
	}

	return tx.Verify(prevTXs)
}

/*
func dbExists() bool {
*/
// New added
func dbExists(dbFile string) bool {
//
    if _, err := os.Stat(dbFile); os.IsNotExist(err) {
        return false
    }

    return true
}
```

```Go
// blockchain_iterator.go
package main

import (
	"log"

	"github.com/boltdb/bolt"
)

// BlockchainIterator is used to iterate over blockchain blocks
type BlockchainIterator struct {
	currentHash []byte
	db          *bolt.DB
}

// Next returns next block starting from the tip
func (i *BlockchainIterator) Next() *Block {
	var block *Block

	err := i.db.View(func(tx *bolt.Tx) error {
		b := tx.Bucket([]byte(blocksBucket))
		encodedBlock := b.Get(i.currentHash)
		block = DeserializeBlock(encodedBlock)

		return nil
	})

	if err != nil {
		log.Panic(err)
	}

	i.currentHash = block.PrevBlockHash

	return block
}
```

```Go
// cli.go
package main

import (
    "flag"
    "fmt"
    "log"
    "os"
)

// CLI responsible for processing command line arguments
type CLI struct {}

func (cli *CLI) printUsage() {
    fmt.Println("Usage:")

	fmt.Println("  createblockchain -address ADDRESS - Create a blockchain and send genesis block reward to ADDRESS")
	fmt.Println("  createwallet - Generates a new key-pair and saves it into the wallet file")
	fmt.Println("  getbalance -address ADDRESS - Get balance of ADDRESS")
	fmt.Println("  listaddresses - Lists all addresses from the wallet file")
	fmt.Println("  printchain - Print all the blocks of the blockchain")
	fmt.Println("  reindexutxo - Rebuilds the UTXO set")
	fmt.Println("  send -from FROM -to TO -amount AMOUNT - Send AMOUNT of coins from FROM address to TO")
    // New added
	fmt.Println("  startnode -miner ADDRESS - Start a node with ID specified in NODE_ID env. var. -miner enables mining")
    //
}

func (cli *CLI) validateArgs() {
    if len(os.Args) < 2 {
        cli.printUsage()
        os.Exit(1)
    }
}

// Run parses command line arguments and processes commands
func (cli *CLI) Run() {
	cli.validateArgs()

	getBalanceCmd := flag.NewFlagSet("getbalance", flag.ExitOnError)
	createBlockchainCmd := flag.NewFlagSet("createblockchain", flag.ExitOnError)
	createWalletCmd := flag.NewFlagSet("createwallet", flag.ExitOnError)
	listAddressesCmd := flag.NewFlagSet("listaddresses", flag.ExitOnError)
	printChainCmd := flag.NewFlagSet("printchain", flag.ExitOnError)
	reindexUTXOCmd := flag.NewFlagSet("reindexutxo", flag.ExitOnError)
	sendCmd := flag.NewFlagSet("send", flag.ExitOnError)
    // New added
	startNodeCmd := flag.NewFlagSet("startnode", flag.ExitOnError)
    //

	getBalanceAddress := getBalanceCmd.String("address", "", "The address to get balance for")
	createBlockchainAddress := createBlockchainCmd.String("address", "", "The address to send genesis block reward to")
	sendFrom := sendCmd.String("from", "", "Source wallet address")
	sendTo := sendCmd.String("to", "", "Destination wallet address")
	sendAmount := sendCmd.Int("amount", 0, "Amount to send")
    // New added
	sendMine := sendCmd.Bool("mine", false, "Mine immediately on the same node")
	startNodeMiner := startNodeCmd.String("miner", "", "Enable mining mode and send reward to ADDRESS")
    //

	switch os.Args[1] {
	case "getbalance":
		err := getBalanceCmd.Parse(os.Args[2:])
		if err != nil {
			log.Panic(err)
		}
	case "createblockchain":
		err := createBlockchainCmd.Parse(os.Args[2:])
		if err != nil {
			log.Panic(err)
		}
	case "createwallet":
		err := createWalletCmd.Parse(os.Args[2:])
		if err != nil {
			log.Panic(err)
		}
	case "listaddresses":
		err := listAddressesCmd.Parse(os.Args[2:])
		if err != nil {
			log.Panic(err)
		}
	case "printchain":
		err := printChainCmd.Parse(os.Args[2:])
		if err != nil {
			log.Panic(err)
		}
	case "reindexutxo":
		err := reindexUTXOCmd.Parse(os.Args[2:])
		if err != nil {
			log.Panic(err)
		}
	case "send":
		err := sendCmd.Parse(os.Args[2:])
		if err != nil {
			log.Panic(err)
		}
    // New added
	case "startnode":
		err := startNodeCmd.Parse(os.Args[2:])
		if err != nil {
			log.Panic(err)
		}
    //
	default:
		cli.printUsage()
		os.Exit(1)
	}

	if getBalanceCmd.Parsed() {
		if *getBalanceAddress == "" {
			getBalanceCmd.Usage()
			os.Exit(1)
		}
        /*
		cli.getBalance(*getBalanceAddress)
        */
        // New added
		cli.getBalance(*getBalanceAddress, nodeID)
        //
	}

	if createBlockchainCmd.Parsed() {
		if *createBlockchainAddress == "" {
			createBlockchainCmd.Usage()
			os.Exit(1)
		}
        /*
		cli.createBlockchain(*createBlockchainAddress)
        */
        // New added
		cli.createBlockchain(*createBlockchainAddress, nodeID)
        //
	}

	if createWalletCmd.Parsed() {
        /*
		cli.createWallet()
        */
        // New added
		cli.createWallet(nodeID)
        //
	}

	if listAddressesCmd.Parsed() {
        /*
		cli.listAddresses()
        */
        // New added
		cli.listAddresses(nodeID)
        //
	}

	if printChainCmd.Parsed() {
        /*
		cli.printChain()
        */
        // New added
		cli.printChain(nodeID)
        //
	}

	if reindexUTXOCmd.Parsed() {
        /*
		cli.reindexUTXO()
        */
        // New added
		cli.reindexUTXO(nodeID)
        //
	}

	if sendCmd.Parsed() {
		if *sendFrom == "" || *sendTo == "" || *sendAmount <= 0 {
			sendCmd.Usage()
			os.Exit(1)
		}

        /*
		cli.send(*sendFrom, *sendTo, *sendAmount)
        */
        // New added
		cli.send(*sendFrom, *sendTo, *sendAmount, nodeID, *sendMine)
        //
	}
    
    // New added
	if startNodeCmd.Parsed() {
		nodeID := os.Getenv("NODE_ID")
		if nodeID == "" {
			startNodeCmd.Usage()
			os.Exit(1)
		}
		cli.startNode(nodeID, *startNodeMiner)
	}
    //
}
```

```Go
// cli_createblockchain.go
package main

import (
	"fmt"
	"log"
)

/*
func (cli *CLI) createBlockchain(address string) {
*/
// New added
func (cli *CLI) createBlockchain(address, nodeID string) {
//
	if !ValidateAddress(address) {
		log.Panic("ERROR: Address is not valid")
	}
    /*
	bc := CreateBlockchain(address)
    */
    // New added
	bc := CreateBlockchain(address, nodeID)
    //
	defer bc.db.Close()
    
	UTXOSet := UTXOSet{bc}
	UTXOSet.Reindex()

	fmt.Println("Done!")
}
```

```Go
// cli_createwallet.go
package main

import "fmt"

/*
func (cli *CLI) createWallet() {
	wallets, _ := NewWallets()
	address := wallets.CreateWallet()
	wallets.SaveToFile()
*/
// New added
func (cli *CLI) createWallet(nodeID string) {
	wallets, _ := NewWallets(nodeID)
	address := wallets.CreateWallet()
	wallets.SaveToFile(nodeID)
//

	fmt.Printf("Your new address: %s\n", address)
}
```

```Go
// cli_getbalance.go
package main

import (
	"fmt"
	"log"
)

/*
func (cli *CLI) getBalance(address string) {
*/
// New added
func (cli *CLI) getBalance(address, nodeID string) {
//
	if !ValidateAddress(address) {
		log.Panic("ERROR: Address is not valid")
	}
    /*
	bc := NewBlockchain(address)
    */
    // New added
	bc := NewBlockchain(nodeID)
    //
	UTXOSet := UTXOSet{bc}
	defer bc.db.Close()

	balance := 0
	pubKeyHash := Base58Decode([]byte(address))
	pubKeyHash = pubKeyHash[1 : len(pubKeyHash)-4]
	UTXOs := UTXOSet.FindUTXO(pubKeyHash)

	for _, out := range UTXOs {
		balance += out.Value
	}

	fmt.Printf("Balance of '%s': %d\n", address, balance)
}
//
```

```Go
// cli_listaddress.go
package main

import (
	"fmt"
	"log"
)

/*
func (cli *CLI) listAddresses() {
	wallets, err := NewWallets()
*/
// New added
func (cli *CLI) listAddresses(nodeID string) {
	wallets, err := NewWallets(nodeID)
//
	if err != nil {
		log.Panic(err)
	}
	addresses := wallets.GetAddresses()

	for _, address := range addresses {
		fmt.Println(address)
	}
}
```

```Go
// cli_printchain.go
package main

import (
	"fmt"
	"strconv"
)

/*
func (cli *CLI) printChain() {
	bc := NewBlockchain("")
*/
// New added
func (cli *CLI) printChain(nodeID string) {
	bc := NewBlockchain(nodeID)
//
	defer bc.db.Close()

	bci := bc.Iterator()

	for {
		block := bci.Next()

		fmt.Printf("============ Block %x ============\n", block.Hash)
        // New added
		fmt.Printf("Height: %d\n", block.Height)
        //
		fmt.Printf("Prev. block: %x\n", block.PrevBlockHash)
		pow := NewProofOfWork(block)
		fmt.Printf("PoW: %s\n\n", strconv.FormatBool(pow.Validate()))
		for _, tx := range block.Transactions {
			fmt.Println(tx)
		}
		fmt.Printf("\n\n")

		if len(block.PrevBlockHash) == 0 {
			break
		}
	}
}
```

```Go
// cli_reindexutxo.go
// New added
package main

import "fmt"

/*
func (cli *CLI) reindexUTXO() {
	bc := NewBlockchain()
*/
// New added
func (cli *CLI) reindexUTXO(nodeID string) {
	bc := NewBlockchain(nodeID)
//
	UTXOSet := UTXOSet{bc}
	UTXOSet.Reindex()

	count := UTXOSet.CountTransactions()
	fmt.Printf("Done! There are %d transactions in the UTXO set.\n", count)
}
//
```

```Go
// cli_send.go
package main

import (
	"fmt"
	"log"
)

/*
func (cli *CLI) send(from, to string, amount int) {
*/
// New added
func (cli *CLI) send(from, to string, amount int, nodeID string, mineNow bool) {
//
	if !ValidateAddress(from) {
		log.Panic("ERROR: Sender address is not valid")
	}
	if !ValidateAddress(to) {
		log.Panic("ERROR: Recipient address is not valid")
	}

	bc := NewBlockchain(from)
	UTXOSet := UTXOSet{bc}
	defer bc.db.Close()

    /*
	tx := NewUTXOTransaction(from, to, amount, bc)
	cbTx := NewCoinbaseTX(from, "")
	txs := []*Transaction{cbTx, tx}

	newBlock := bc.MineBlock(txs)
	UTXOSet.Update(newBlock)
	fmt.Println("Success!")
    */
    // New added
	wallets, err := NewWallets(nodeID)
	if err != nil {
		log.Panic(err)
	}
	wallet := wallets.GetWallet(from)

	tx := NewUTXOTransaction(&wallet, to, amount, &UTXOSet)

	if mineNow {
		cbTx := NewCoinbaseTX(from, "")
		txs := []*Transaction{cbTx, tx}

		newBlock := bc.MineBlock(txs)
		UTXOSet.Update(newBlock)
	} else {
		sendTx(knownNodes[0], tx)
	}

	fmt.Println("Success!")
    //
}
```

```Go
// cli_startnode.go
// New added
package main

import (
	"fmt"
	"log"
)

func (cli *CLI) startNode(nodeID, minerAddress string) {
	fmt.Printf("Starting node %s\n", nodeID)
	if len(minerAddress) > 0 {
		if ValidateAddress(minerAddress) {
			fmt.Println("Mining is on. Address to receive rewards: ", minerAddress)
		} else {
			log.Panic("Wrong miner address!")
		}
	}
	StartServer(nodeID, minerAddress)
}
//
```

```Go
// main.go
package main

func main() {
    cli := CLI{bc}
    cli.Run()
}
```

```Go
// merkle_tree.go
package main

import (
	"crypto/sha256"
)

// MerkleTree represent a Merkle tree
type MerkleTree struct {
	RootNode *MerkleNode
}

// MerkleNode represent a Merkle tree node
type MerkleNode struct {
	Left  *MerkleNode
	Right *MerkleNode
	Data  []byte
}

// NewMerkleTree creates a new Merkle tree from a sequence of data
func NewMerkleTree(data [][]byte) *MerkleTree {
	var nodes []MerkleNode

	if len(data)%2 != 0 {
		data = append(data, data[len(data)-1])
	}

	for _, datum := range data {
		node := NewMerkleNode(nil, nil, datum)
		nodes = append(nodes, *node)
	}

	for i := 0; i < len(data)/2; i++ {
		var newLevel []MerkleNode

		for j := 0; j < len(nodes); j += 2 {
			node := NewMerkleNode(&nodes[j], &nodes[j+1], nil)
			newLevel = append(newLevel, *node)
		}

		nodes = newLevel
	}

	mTree := MerkleTree{&nodes[0]}

	return &mTree
}

// NewMerkleNode creates a new Merkle tree node
func NewMerkleNode(left, right *MerkleNode, data []byte) *MerkleNode {
	mNode := MerkleNode{}

	if left == nil && right == nil {
		hash := sha256.Sum256(data)
		mNode.Data = hash[:]
	} else {
		prevHashes := append(left.Data, right.Data...)
		hash := sha256.Sum256(prevHashes)
		mNode.Data = hash[:]
	}

	mNode.Left = left
	mNode.Right = right

	return &mNode
}
```

```Go
// merkle_tree_test.go
package main

import (
	"encoding/hex"
	"fmt"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestNewMerkleNode(t *testing.T) {
	data := [][]byte{
		[]byte("node1"),
		[]byte("node2"),
		[]byte("node3"),
	}

	// Level 1

	n1 := NewMerkleNode(nil, nil, data[0])
	n2 := NewMerkleNode(nil, nil, data[1])
	n3 := NewMerkleNode(nil, nil, data[2])
	n4 := NewMerkleNode(nil, nil, data[2])

	// Level 2
	n5 := NewMerkleNode(n1, n2, nil)
	n6 := NewMerkleNode(n3, n4, nil)

	// Level 3
	n7 := NewMerkleNode(n5, n6, nil)

	assert.Equal(
		t,
		"64b04b718d8b7c5b6fd17f7ec221945c034cfce3be4118da33244966150c4bd4",
		hex.EncodeToString(n5.Data),
		"Level 1 hash 1 is correct",
	)
	assert.Equal(
		t,
		"08bd0d1426f87a78bfc2f0b13eccdf6f5b58dac6b37a7b9441c1a2fab415d76c",
		hex.EncodeToString(n6.Data),
		"Level 1 hash 2 is correct",
	)
	assert.Equal(
		t,
		"4e3e44e55926330ab6c31892f980f8bfd1a6e910ff1ebc3f778211377f35227e",
		hex.EncodeToString(n7.Data),
		"Root hash is correct",
	)
}

func TestNewMerkleTree(t *testing.T) {
	data := [][]byte{
		[]byte("node1"),
		[]byte("node2"),
		[]byte("node3"),
	}
	// Level 1
	n1 := NewMerkleNode(nil, nil, data[0])
	n2 := NewMerkleNode(nil, nil, data[1])
	n3 := NewMerkleNode(nil, nil, data[2])
	n4 := NewMerkleNode(nil, nil, data[2])

	// Level 2
	n5 := NewMerkleNode(n1, n2, nil)
	n6 := NewMerkleNode(n3, n4, nil)

	// Level 3
	n7 := NewMerkleNode(n5, n6, nil)

	rootHash := fmt.Sprintf("%x", n7.Data)
	mTree := NewMerkleTree(data)

	assert.Equal(t, rootHash, fmt.Sprintf("%x", mTree.RootNode.Data), "Merkle tree root hash is correct")
}
```

```Go
// proofofwork.go
package main

import (
    "bytes"
    "crypto/sha256"
    "fmt"
    "math"
    "math/big"
)

var (
    // to avoid a possible overflow of nonce
    maxNonce = math.MaxInt64 
)

// Define the difficulty of mining
const targetBits = 16

// ProofOfWork represents a proof-of-work
type ProofOfWork struct {
    block  *Block
    target *big.Int
}

// NewProofOfWork builds and returns a ProofOfWork
func NewProofOfWork(b *Block) *ProofOfWork {
    // 1. set value of 1
    target := big.NewInt(1) 

    // 2. shift it left by (256 - targetBits)
    target.Lsh(target, uint(256-targetBits)) 

    pow := &ProofOfWork{b, target}

    return pow
}

func (pow *ProofOfWork) prepareData(nonce int) []byte {
    data := bytes.Join(
        [][]byte{
            pow.block.PrevBlockHash,
            pow.block.HashTransactions(),
            IntToHex(pow.block.Timestamp),
            IntToHex(int64(targetBits)),
            IntToHex(int64(nonce)), // the counter
        },
        []byte{},
    )

    return data
}

// Run performs a proof-of-work
func (pow *ProofOfWork) Run() (int, []byte) {
    var hashInt big.Int // integer representation of hash
    var hash [32]byte   // the hash 
    nonce := 0          // the counter

    fmt.Printf("Mining a new block")

    // although the difficulty of this PoW implementation is TOO low for the counter..
    // -> Need to improve this! 
    //
    // process:
    //   1. Prepare a data.
    //   2. Hash it with SHA-256
    //   3. Convert the hash to a big integer.
    //   4. Compare the integer with the target.
    for nonce < maxNonce {  
        data := pow.prepareData(nonce)
        hash = sha256.Sum256(data)
        fmt.Printf("\r%x", hash)
        hashInt.SetBytes(hash[:])
        if hashInt.Cmp(pow.target) == -1 {
            break
        } else {
            nonce++
        }
    }
    fmt.Print("\n\n")

    return nonce, hash[:]
}

// Validate validates block's PoW
func (pow *ProofOfWork) Validate() bool {
    var hashInt big.Int

    data := pow.prepareData(pow.block.Nonce)
    hash := sha256.Sum256(data)
    hashInt.SetBytes(hash[:])
    isValid := hashInt.Cmp(pow.target) == -1

    return isValid
}
```

```Go
// server.go
// New added
package main

import (
	"bytes"
	"encoding/gob"
	"encoding/hex"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net"
)

const protocol = "tcp"
const nodeVersion = 1
const commandLength = 12

var nodeAddress string
var miningAddress string
var knownNodes = []string{"localhost:3000"}
var blocksInTransit = [][]byte{}
var mempool = make(map[string]Transaction)

type addr struct {
	AddrList []string
}

type block struct {
	AddrFrom string
	Block    []byte
}

type getblocks struct {
	AddrFrom string
}

type getdata struct {
	AddrFrom string
	Type     string
	ID       []byte
}

type inv struct {
	AddrFrom string
	Type     string
	Items    [][]byte
}

type tx struct {
	AddFrom     string
	Transaction []byte
}

type verzion struct {
	Version    int
	BestHeight int
	AddrFrom   string
}

func commandToBytes(command string) []byte {
	var bytes [commandLength]byte

	for i, c := range command {
		bytes[i] = byte(c)
	}

	return bytes[:]
}

func bytesToCommand(bytes []byte) string {
	var command []byte

	for _, b := range bytes {
		if b != 0x0 {
			command = append(command, b)
		}
	}

	return fmt.Sprintf("%s", command)
}

func extractCommand(request []byte) []byte {
	return request[:commandLength]
}

func requestBlocks() {
	for _, node := range knownNodes {
		sendGetBlocks(node)
	}
}

func sendAddr(address string) {
	nodes := addr{knownNodes}
	nodes.AddrList = append(nodes.AddrList, nodeAddress)
	payload := gobEncode(nodes)
	request := append(commandToBytes("addr"), payload...)

	sendData(address, request)
}

func sendBlock(addr string, b *Block) {
	data := block{nodeAddress, b.Serialize()}
	payload := gobEncode(data)
	request := append(commandToBytes("block"), payload...)

	sendData(addr, request)
}

func sendData(addr string, data []byte) {
	conn, err := net.Dial(protocol, addr)
	if err != nil {
		fmt.Printf("%s is not available\n", addr)
		var updatedNodes []string

		for _, node := range knownNodes {
			if node != addr {
				updatedNodes = append(updatedNodes, node)
			}
		}

		knownNodes = updatedNodes

		return
	}
	defer conn.Close()

	_, err = io.Copy(conn, bytes.NewReader(data))
	if err != nil {
		log.Panic(err)
	}
}

func sendInv(address, kind string, items [][]byte) {
	inventory := inv{nodeAddress, kind, items}
	payload := gobEncode(inventory)
	request := append(commandToBytes("inv"), payload...)

	sendData(address, request)
}

func sendGetBlocks(address string) {
	payload := gobEncode(getblocks{nodeAddress})
	request := append(commandToBytes("getblocks"), payload...)

	sendData(address, request)
}

func sendGetData(address, kind string, id []byte) {
	payload := gobEncode(getdata{nodeAddress, kind, id})
	request := append(commandToBytes("getdata"), payload...)

	sendData(address, request)
}

func sendTx(addr string, tnx *Transaction) {
	data := tx{nodeAddress, tnx.Serialize()}
	payload := gobEncode(data)
	request := append(commandToBytes("tx"), payload...)

	sendData(addr, request)
}

func sendVersion(addr string, bc *Blockchain) {
	bestHeight := bc.GetBestHeight()
	payload := gobEncode(verzion{nodeVersion, bestHeight, nodeAddress})

	request := append(commandToBytes("version"), payload...)

	sendData(addr, request)
}

func handleAddr(request []byte) {
	var buff bytes.Buffer
	var payload addr

	buff.Write(request[commandLength:])
	dec := gob.NewDecoder(&buff)
	err := dec.Decode(&payload)
	if err != nil {
		log.Panic(err)
	}

	knownNodes = append(knownNodes, payload.AddrList...)
	fmt.Printf("There are %d known nodes now!\n", len(knownNodes))
	requestBlocks()
}

func handleBlock(request []byte, bc *Blockchain) {
	var buff bytes.Buffer
	var payload block

	buff.Write(request[commandLength:])
	dec := gob.NewDecoder(&buff)
	err := dec.Decode(&payload)
	if err != nil {
		log.Panic(err)
	}

	blockData := payload.Block
	block := DeserializeBlock(blockData)

	fmt.Println("Recevied a new block!")
	bc.AddBlock(block)

	fmt.Printf("Added block %x\n", block.Hash)

	if len(blocksInTransit) > 0 {
		blockHash := blocksInTransit[0]
		sendGetData(payload.AddrFrom, "block", blockHash)

		blocksInTransit = blocksInTransit[1:]
	} else {
		UTXOSet := UTXOSet{bc}
		UTXOSet.Reindex()
	}
}

func handleInv(request []byte, bc *Blockchain) {
	var buff bytes.Buffer
	var payload inv

	buff.Write(request[commandLength:])
	dec := gob.NewDecoder(&buff)
	err := dec.Decode(&payload)
	if err != nil {
		log.Panic(err)
	}

	fmt.Printf("Recevied inventory with %d %s\n", len(payload.Items), payload.Type)

	if payload.Type == "block" {
		blocksInTransit = payload.Items

		blockHash := payload.Items[0]
		sendGetData(payload.AddrFrom, "block", blockHash)

		newInTransit := [][]byte{}
		for _, b := range blocksInTransit {
			if bytes.Compare(b, blockHash) != 0 {
				newInTransit = append(newInTransit, b)
			}
		}
		blocksInTransit = newInTransit
	}

	if payload.Type == "tx" {
		txID := payload.Items[0]

		if mempool[hex.EncodeToString(txID)].ID == nil {
			sendGetData(payload.AddrFrom, "tx", txID)
		}
	}
}

func handleGetBlocks(request []byte, bc *Blockchain) {
	var buff bytes.Buffer
	var payload getblocks

	buff.Write(request[commandLength:])
	dec := gob.NewDecoder(&buff)
	err := dec.Decode(&payload)
	if err != nil {
		log.Panic(err)
	}

	blocks := bc.GetBlockHashes()
	sendInv(payload.AddrFrom, "block", blocks)
}

func handleGetData(request []byte, bc *Blockchain) {
	var buff bytes.Buffer
	var payload getdata

	buff.Write(request[commandLength:])
	dec := gob.NewDecoder(&buff)
	err := dec.Decode(&payload)
	if err != nil {
		log.Panic(err)
	}

	if payload.Type == "block" {
		block, err := bc.GetBlock([]byte(payload.ID))
		if err != nil {
			return
		}

		sendBlock(payload.AddrFrom, &block)
	}

	if payload.Type == "tx" {
		txID := hex.EncodeToString(payload.ID)
		tx := mempool[txID]

		sendTx(payload.AddrFrom, &tx)
		// delete(mempool, txID)
	}
}

func handleTx(request []byte, bc *Blockchain) {
	var buff bytes.Buffer
	var payload tx

	buff.Write(request[commandLength:])
	dec := gob.NewDecoder(&buff)
	err := dec.Decode(&payload)
	if err != nil {
		log.Panic(err)
	}

	txData := payload.Transaction
	tx := DeserializeTransaction(txData)
	mempool[hex.EncodeToString(tx.ID)] = tx

	if nodeAddress == knownNodes[0] {
		for _, node := range knownNodes {
			if node != nodeAddress && node != payload.AddFrom {
				sendInv(node, "tx", [][]byte{tx.ID})
			}
		}
	} else {
		if len(mempool) >= 2 && len(miningAddress) > 0 {
		MineTransactions:
			var txs []*Transaction

			for id := range mempool {
				tx := mempool[id]
				if bc.VerifyTransaction(&tx) {
					txs = append(txs, &tx)
				}
			}

			if len(txs) == 0 {
				fmt.Println("All transactions are invalid! Waiting for new ones...")
				return
			}

			cbTx := NewCoinbaseTX(miningAddress, "")
			txs = append(txs, cbTx)

			newBlock := bc.MineBlock(txs)
			UTXOSet := UTXOSet{bc}
			UTXOSet.Reindex()

			fmt.Println("New block is mined!")

			for _, tx := range txs {
				txID := hex.EncodeToString(tx.ID)
				delete(mempool, txID)
			}

			for _, node := range knownNodes {
				if node != nodeAddress {
					sendInv(node, "block", [][]byte{newBlock.Hash})
				}
			}

			if len(mempool) > 0 {
				goto MineTransactions
			}
		}
	}
}

func handleVersion(request []byte, bc *Blockchain) {
	var buff bytes.Buffer
	var payload verzion

	buff.Write(request[commandLength:])
	dec := gob.NewDecoder(&buff)
	err := dec.Decode(&payload)
	if err != nil {
		log.Panic(err)
	}

	myBestHeight := bc.GetBestHeight()
	foreignerBestHeight := payload.BestHeight

	if myBestHeight < foreignerBestHeight {
		sendGetBlocks(payload.AddrFrom)
	} else if myBestHeight > foreignerBestHeight {
		sendVersion(payload.AddrFrom, bc)
	}

	// sendAddr(payload.AddrFrom)
	if !nodeIsKnown(payload.AddrFrom) {
		knownNodes = append(knownNodes, payload.AddrFrom)
	}
}

func handleConnection(conn net.Conn, bc *Blockchain) {
	request, err := ioutil.ReadAll(conn)
	if err != nil {
		log.Panic(err)
	}
	command := bytesToCommand(request[:commandLength])
	fmt.Printf("Received %s command\n", command)

	switch command {
	case "addr":
		handleAddr(request)
	case "block":
		handleBlock(request, bc)
	case "inv":
		handleInv(request, bc)
	case "getblocks":
		handleGetBlocks(request, bc)
	case "getdata":
		handleGetData(request, bc)
	case "tx":
		handleTx(request, bc)
	case "version":
		handleVersion(request, bc)
	default:
		fmt.Println("Unknown command!")
	}

	conn.Close()
}

// StartServer starts a node
func StartServer(nodeID, minerAddress string) {
	nodeAddress = fmt.Sprintf("localhost:%s", nodeID)
	miningAddress = minerAddress
	ln, err := net.Listen(protocol, nodeAddress)
	if err != nil {
		log.Panic(err)
	}
	defer ln.Close()

	bc := NewBlockchain(nodeID)

	if nodeAddress != knownNodes[0] {
		sendVersion(knownNodes[0], bc)
	}

	for {
		conn, err := ln.Accept()
		if err != nil {
			log.Panic(err)
		}
		go handleConnection(conn, bc)
	}
}

func gobEncode(data interface{}) []byte {
	var buff bytes.Buffer

	enc := gob.NewEncoder(&buff)
	err := enc.Encode(data)
	if err != nil {
		log.Panic(err)
	}

	return buff.Bytes()
}

func nodeIsKnown(addr string) bool {
	for _, node := range knownNodes {
		if node == addr {
			return true
		}
	}

	return false
}
//
```

```Go
// transaction.go
package main

import (
	"bytes"
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/sha256"
	"math/big"

	"encoding/gob"
	"encoding/hex"
	"fmt"
	"log"
	"strings"
)

const subsidy = 10

// Transaction represents a Bitcoin transaction
type Transaction struct {
	ID   []byte
	Vin  []TXInput
	Vout []TXOutput
}

// IsCoinbase checks whether the transaction is coinbase
func (tx Transaction) IsCoinbase() bool {
	return len(tx.Vin) == 1 && len(tx.Vin[0].Txid) == 0 && tx.Vin[0].Vout == -1
}

// Serialize returns a serialized Transaction
func (tx Transaction) Serialize() []byte {
	var encoded bytes.Buffer

	enc := gob.NewEncoder(&encoded)
	err := enc.Encode(tx)
	if err != nil {
		log.Panic(err)
	}

	return encoded.Bytes()
}

// Hash returns the hash of the Transaction
func (tx *Transaction) Hash() []byte {
	var hash [32]byte

	txCopy := *tx
	txCopy.ID = []byte{}

	hash = sha256.Sum256(txCopy.Serialize())

	return hash[:]
}

// Sign signs each input of a Transaction
func (tx *Transaction) Sign(privKey ecdsa.PrivateKey, prevTXs map[string]Transaction) {
	if tx.IsCoinbase() {
		return
	}

	for _, vin := range tx.Vin {
		if prevTXs[hex.EncodeToString(vin.Txid)].ID == nil {
			log.Panic("ERROR: Previous transaction is not correct")
		}
	}

	txCopy := tx.TrimmedCopy()

	for inID, vin := range txCopy.Vin {
		prevTx := prevTXs[hex.EncodeToString(vin.Txid)]
		txCopy.Vin[inID].Signature = nil
		txCopy.Vin[inID].PubKey = prevTx.Vout[vin.Vout].PubKeyHash
		txCopy.ID = txCopy.Hash()
		txCopy.Vin[inID].PubKey = nil

		r, s, err := ecdsa.Sign(rand.Reader, &privKey, txCopy.ID)
		if err != nil {
			log.Panic(err)
		}
		signature := append(r.Bytes(), s.Bytes()...)

		tx.Vin[inID].Signature = signature
	}
}

// String returns a human-readable representation of a transaction
func (tx Transaction) String() string {
	var lines []string

	lines = append(lines, fmt.Sprintf("--- Transaction %x:", tx.ID))

	for i, input := range tx.Vin {

		lines = append(lines, fmt.Sprintf("     Input %d:", i))
		lines = append(lines, fmt.Sprintf("       TXID:      %x", input.Txid))
		lines = append(lines, fmt.Sprintf("       Out:       %d", input.Vout))
		lines = append(lines, fmt.Sprintf("       Signature: %x", input.Signature))
		lines = append(lines, fmt.Sprintf("       PubKey:    %x", input.PubKey))
	}

	for i, output := range tx.Vout {
		lines = append(lines, fmt.Sprintf("     Output %d:", i))
		lines = append(lines, fmt.Sprintf("       Value:  %d", output.Value))
		lines = append(lines, fmt.Sprintf("       Script: %x", output.PubKeyHash))
	}

	return strings.Join(lines, "\n")
}

// TrimmedCopy creates a trimmed copy of Transaction to be used in signing
func (tx *Transaction) TrimmedCopy() Transaction {
	var inputs []TXInput
	var outputs []TXOutput

	for _, vin := range tx.Vin {
		inputs = append(inputs, TXInput{vin.Txid, vin.Vout, nil, nil})
	}

	for _, vout := range tx.Vout {
		outputs = append(outputs, TXOutput{vout.Value, vout.PubKeyHash})
	}

	txCopy := Transaction{tx.ID, inputs, outputs}

	return txCopy
}

// Verify verifies signatures of Transaction inputs
func (tx *Transaction) Verify(prevTXs map[string]Transaction) bool {
	if tx.IsCoinbase() {
		return true
	}

	for _, vin := range tx.Vin {
		if prevTXs[hex.EncodeToString(vin.Txid)].ID == nil {
			log.Panic("ERROR: Previous transaction is not correct")
		}
	}

	txCopy := tx.TrimmedCopy()
	curve := elliptic.P256()

	for inID, vin := range tx.Vin {
		prevTx := prevTXs[hex.EncodeToString(vin.Txid)]
		txCopy.Vin[inID].Signature = nil
		txCopy.Vin[inID].PubKey = prevTx.Vout[vin.Vout].PubKeyHash
		txCopy.ID = txCopy.Hash()
		txCopy.Vin[inID].PubKey = nil

		r := big.Int{}
		s := big.Int{}
		sigLen := len(vin.Signature)
		r.SetBytes(vin.Signature[:(sigLen / 2)])
		s.SetBytes(vin.Signature[(sigLen / 2):])

		x := big.Int{}
		y := big.Int{}
		keyLen := len(vin.PubKey)
		x.SetBytes(vin.PubKey[:(keyLen / 2)])
		y.SetBytes(vin.PubKey[(keyLen / 2):])

		rawPubKey := ecdsa.PublicKey{curve, &x, &y}
		if ecdsa.Verify(&rawPubKey, txCopy.ID, &r, &s) == false {
			return false
		}
	}

	return true
}

// NewCoinbaseTX creates a new coinbase transaction
func NewCoinbaseTX(to, data string) *Transaction {
	if data == "" {
		randData := make([]byte, 20)
		_, err := rand.Read(randData)
		if err != nil {
			log.Panic(err)
		}

		data = fmt.Sprintf("%x", randData)
	}

	txin := TXInput{[]byte{}, -1, nil, []byte(data)}
	txout := TXOutput{subsidy, to}
	tx := Transaction{nil, []TXInput{txin}, []TXOutput{txout}}
    tx.ID = tx.Hash()

	return &tx
}

// NewUTXOTransaction creates a new transaction
func NewUTXOTransaction(from, to string, amount int, bc *Blockchain) *Transaction {
	var inputs []TXInput
	var outputs []TXOutput

    /*
	wallets, err := NewWallets()
	if err != nil {
		log.Panic(err)
	}
	wallet := wallets.GetWallet(from)
    */
	pubKeyHash := HashPubKey(wallet.PublicKey)
	acc, validOutputs := bc.FindSpendableOutputs(pubKeyHash, amount)

	if acc < amount {
		log.Panic("ERROR: Not enough funds")
	}

	// Build a list of inputs
	for txid, outs := range validOutputs {
		txID, err := hex.DecodeString(txid)
		if err != nil {
			log.Panic(err)
		}

		for _, out := range outs {
			input := TXInput{txID, out, nil, wallet.PublicKey}
			inputs = append(inputs, input)
		}
	}

	// Build a list of outputs
    // New added
	from := fmt.Sprintf("%s", wallet.GetAddress())
    //
	outputs = append(outputs, *NewTXOutput(amount, to))
	if acc > amount {
		outputs = append(outputs, *NewTXOutput(acc-amount, from)) // a change
	}

	tx := Transaction{nil, inputs, outputs}
	tx.ID = tx.Hash()
	bc.SignTransaction(&tx, wallet.PrivateKey)

	return &tx
}

// New added
// DeserializeTransaction deserializes a transaction
func DeserializeTransaction(data []byte) Transaction {
	var transaction Transaction

	decoder := gob.NewDecoder(bytes.NewReader(data))
	err := decoder.Decode(&transaction)
	if err != nil {
		log.Panic(err)
	}

	return transaction
}
//
```

```Go
// transaction_input.go
package main

import "bytes"

// TXInput represents a transaction input
type TXInput struct {
	Txid      []byte
	Vout      int
	Signature []byte
	PubKey    []byte
}

// UsesKey checks whether the address initiated the transaction
func (in *TXInput) UsesKey(pubKeyHash []byte) bool {
	lockingHash := HashPubKey(in.PubKey)

	return bytes.Compare(lockingHash, pubKeyHash) == 0
}
```

```Go
// transaction_input.go
package main

import (
	"bytes"
	"encoding/gob"
	"log"
)

// TXOutput represents a transaction output
type TXOutput struct {
	Value      int
	PubKeyHash []byte
}

// Lock signs the output
func (out *TXOutput) Lock(address []byte) {
	pubKeyHash := Base58Decode(address)
	pubKeyHash = pubKeyHash[1 : len(pubKeyHash)-4]
	out.PubKeyHash = pubKeyHash
}

// IsLockedWithKey checks if the output can be used by the owner of the pubkey
func (out *TXOutput) IsLockedWithKey(pubKeyHash []byte) bool {
	return bytes.Compare(out.PubKeyHash, pubKeyHash) == 0
}

// NewTXOutput create a new TXOutput
func NewTXOutput(value int, address string) *TXOutput {
	txo := &TXOutput{value, nil}
	txo.Lock([]byte(address))

	return txo
}

// TXOutputs collects TXOutput
type TXOutputs struct {
	Outputs []TXOutput
}

// Serialize serializes TXOutputs
func (outs TXOutputs) Serialize() []byte {
	var buff bytes.Buffer

	enc := gob.NewEncoder(&buff)
	err := enc.Encode(outs)
	if err != nil {
		log.Panic(err)
	}

	return buff.Bytes()
}

// DeserializeOutputs deserializes TXOutputs
func DeserializeOutputs(data []byte) TXOutputs {
	var outputs TXOutputs

	dec := gob.NewDecoder(bytes.NewReader(data))
	err := dec.Decode(&outputs)
	if err != nil {
		log.Panic(err)
	}

	return outputs
}
```

```Go
// utils.go
package main

import (
    "bytes"
    "encoding/binary"
    "log"
)

// IntToHex converts an int64 to a byte array
func IntToHex(num int64) []byte {
    buff := new(bytes.Buffer)
    err := binary.Write(buff, binary.BigEndian, num)
    if err != nil {
        log.Panic(err)
    }

    return buff.Bytes()
}

// ReverseBytes reverses a byte array
func ReverseBytes(data []byte) {
	for i, j := 0, len(data)-1; i < j; i, j = i+1, j-1 {
		data[i], data[j] = data[j], data[i]
	}
}
```

```Go
// utxo_set.go
package main

import (
	"encoding/hex"
	"log"

	"github.com/boltdb/bolt"
)

const utxoBucket = "chainstate"

// UTXOSet represents UTXO set
type UTXOSet struct {
	Blockchain *Blockchain
}

// FindSpendableOutputs finds and returns unspent outputs to reference in inputs
func (u UTXOSet) FindSpendableOutputs(pubkeyHash []byte, amount int) (int, map[string][]int) {
	unspentOutputs := make(map[string][]int)
	accumulated := 0
	db := u.Blockchain.db

	err := db.View(func(tx *bolt.Tx) error {
		b := tx.Bucket([]byte(utxoBucket))
		c := b.Cursor()

		for k, v := c.First(); k != nil; k, v = c.Next() {
			txID := hex.EncodeToString(k)
			outs := DeserializeOutputs(v)

			for outIdx, out := range outs.Outputs {
				if out.IsLockedWithKey(pubkeyHash) && accumulated < amount {
					accumulated += out.Value
					unspentOutputs[txID] = append(unspentOutputs[txID], outIdx)
				}
			}
		}

		return nil
	})
	if err != nil {
		log.Panic(err)
	}

	return accumulated, unspentOutputs
}

// FindUTXO finds UTXO for a public key hash
func (u UTXOSet) FindUTXO(pubKeyHash []byte) []TXOutput {
	var UTXOs []TXOutput
	db := u.Blockchain.db

	err := db.View(func(tx *bolt.Tx) error {
		b := tx.Bucket([]byte(utxoBucket))
		c := b.Cursor()

		for k, v := c.First(); k != nil; k, v = c.Next() {
			outs := DeserializeOutputs(v)

			for _, out := range outs.Outputs {
				if out.IsLockedWithKey(pubKeyHash) {
					UTXOs = append(UTXOs, out)
				}
			}
		}

		return nil
	})
	if err != nil {
		log.Panic(err)
	}

	return UTXOs
}

// CountTransactions returns the number of transactions in the UTXO set
func (u UTXOSet) CountTransactions() int {
	db := u.Blockchain.db
	counter := 0

	err := db.View(func(tx *bolt.Tx) error {
		b := tx.Bucket([]byte(utxoBucket))
		c := b.Cursor()

		for k, _ := c.First(); k != nil; k, _ = c.Next() {
			counter++
		}

		return nil
	})
	if err != nil {
		log.Panic(err)
	}

	return counter
}

// Reindex rebuilds the UTXO set
func (u UTXOSet) Reindex() {
	db := u.Blockchain.db
	bucketName := []byte(utxoBucket)

	err := db.Update(func(tx *bolt.Tx) error {
		err := tx.DeleteBucket(bucketName)
		if err != nil && err != bolt.ErrBucketNotFound {
			log.Panic(err)
		}

		_, err = tx.CreateBucket(bucketName)
		if err != nil {
			log.Panic(err)
		}

		return nil
	})
	if err != nil {
		log.Panic(err)
	}

	UTXO := u.Blockchain.FindUTXO()

	err = db.Update(func(tx *bolt.Tx) error {
		b := tx.Bucket(bucketName)

		for txID, outs := range UTXO {
			key, err := hex.DecodeString(txID)
			if err != nil {
				log.Panic(err)
			}

			err = b.Put(key, outs.Serialize())
			if err != nil {
				log.Panic(err)
			}
		}

		return nil
	})
}

// Update updates the UTXO set with transactions from the Block
// The Block is considered to be the tip of a blockchain
func (u UTXOSet) Update(block *Block) {
	db := u.Blockchain.db

	err := db.Update(func(tx *bolt.Tx) error {
		b := tx.Bucket([]byte(utxoBucket))

		for _, tx := range block.Transactions {
			if tx.IsCoinbase() == false {
				for _, vin := range tx.Vin {
					updatedOuts := TXOutputs{}
					outsBytes := b.Get(vin.Txid)
					outs := DeserializeOutputs(outsBytes)

					for outIdx, out := range outs.Outputs {
						if outIdx != vin.Vout {
							updatedOuts.Outputs = append(updatedOuts.Outputs, out)
						}
					}

					if len(updatedOuts.Outputs) == 0 {
						err := b.Delete(vin.Txid)
						if err != nil {
							log.Panic(err)
						}
					} else {
						err := b.Put(vin.Txid, updatedOuts.Serialize())
						if err != nil {
							log.Panic(err)
						}
					}

				}
			}

			newOutputs := TXOutputs{}
			for _, out := range tx.Vout {
				newOutputs.Outputs = append(newOutputs.Outputs, out)
			}

			err := b.Put(tx.ID, newOutputs.Serialize())
			if err != nil {
				log.Panic(err)
			}
		}

		return nil
	})
	if err != nil {
		log.Panic(err)
	}
}
//
```
```Go
// wallet.go
package main

import (
	"bytes"
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/sha256"
	"log"

	"golang.org/x/crypto/ripemd160"
)

const version = byte(0x00)
/*
const walletFile = "wallet.dat"
*/
const addressChecksumLen = 4

// Wallet stores private and public keys
type Wallet struct {
	PrivateKey ecdsa.PrivateKey
	PublicKey  []byte
}

// NewWallet creates and returns a Wallet
func NewWallet() *Wallet {
	private, public := newKeyPair()
	wallet := Wallet{private, public}

	return &wallet
}

// GetAddress returns wallet address
func (w Wallet) GetAddress() []byte {
	pubKeyHash := HashPubKey(w.PublicKey)

	versionedPayload := append([]byte{version}, pubKeyHash...)
	checksum := checksum(versionedPayload)

	fullPayload := append(versionedPayload, checksum...)
	address := Base58Encode(fullPayload)

	return address
}

// HashPubKey hashes public key
func HashPubKey(pubKey []byte) []byte {
	publicSHA256 := sha256.Sum256(pubKey)

	RIPEMD160Hasher := ripemd160.New()
	_, err := RIPEMD160Hasher.Write(publicSHA256[:])
	if err != nil {
		log.Panic(err)
	}
	publicRIPEMD160 := RIPEMD160Hasher.Sum(nil)

	return publicRIPEMD160
}

// ValidateAddress check if address if valid
func ValidateAddress(address string) bool {
	pubKeyHash := Base58Decode([]byte(address))
	actualChecksum := pubKeyHash[len(pubKeyHash)-addressChecksumLen:]
	version := pubKeyHash[0]
	pubKeyHash = pubKeyHash[1 : len(pubKeyHash)-addressChecksumLen]
	targetChecksum := checksum(append([]byte{version}, pubKeyHash...))

	return bytes.Compare(actualChecksum, targetChecksum) == 0
}

// Checksum generates a checksum for a public key
func checksum(payload []byte) []byte {
	firstSHA := sha256.Sum256(payload)
	secondSHA := sha256.Sum256(firstSHA[:])

	return secondSHA[:addressChecksumLen]
}

func newKeyPair() (ecdsa.PrivateKey, []byte) {
	curve := elliptic.P256()
	private, err := ecdsa.GenerateKey(curve, rand.Reader)
	if err != nil {
		log.Panic(err)
	}
	pubKey := append(private.PublicKey.X.Bytes(), private.PublicKey.Y.Bytes()...)

	return *private, pubKey
}
```

```Go
// wallets.go
package main

import (
	"bytes"
	"crypto/elliptic"
	"encoding/gob"
	"fmt"
	"io/ioutil"
	"log"
	"os"
)

// New added
const walletFile = "wallet_%s.dat"
//

// Wallets stores a collection of wallets
type Wallets struct {
	Wallets map[string]*Wallet
}

// NewWallets creates Wallets and fills it from a file if it exists
func NewWallets() (*Wallets, error) {
	wallets := Wallets{}
	wallets.Wallets = make(map[string]*Wallet)

	err := wallets.LoadFromFile()

	return &wallets, err
}

// CreateWallet adds a Wallet to Wallets
func (ws *Wallets) CreateWallet() string {
	wallet := NewWallet()
	address := fmt.Sprintf("%s", wallet.GetAddress())

	ws.Wallets[address] = wallet

	return address
}

// GetAddresses returns an array of addresses stored in the wallet file
func (ws *Wallets) GetAddresses() []string {
	var addresses []string

	for address := range ws.Wallets {
		addresses = append(addresses, address)
	}

	return addresses
}

// GetWallet returns a Wallet by its address
func (ws Wallets) GetWallet(address string) Wallet {
	return *ws.Wallets[address]
}

// LoadFromFile loads wallets from the file
/*
func (ws *Wallets) LoadFromFile() error {
*/
// New added
func (ws *Wallets) LoadFromFile(nodeID string) error {
	walletFile := fmt.Sprintf(walletFile, nodeID)
//
	if _, err := os.Stat(walletFile); os.IsNotExist(err) {
		return err
	}

	fileContent, err := ioutil.ReadFile(walletFile)
	if err != nil {
		log.Panic(err)
	}

	var wallets Wallets
	gob.Register(elliptic.P256())
	decoder := gob.NewDecoder(bytes.NewReader(fileContent))
	err = decoder.Decode(&wallets)
	if err != nil {
		log.Panic(err)
	}

	ws.Wallets = wallets.Wallets

	return nil
}

// SaveToFile saves wallets to a file
/*
func (ws Wallets) SaveToFile() {
*/
// New added
func (ws Wallets) SaveToFile(nodeID string) {
//
	var content bytes.Buffer
    // New added
	walletFile := fmt.Sprintf(walletFile, nodeID)
    //

	gob.Register(elliptic.P256())

	encoder := gob.NewEncoder(&content)
	err := encoder.Encode(ws)
	if err != nil {
		log.Panic(err)
	}

	err = ioutil.WriteFile(walletFile, content.Bytes(), 0644)
	if err != nil {
		log.Panic(err)
	}
}
//
```

##  output<br>
```Text
# NODE_3000
$ export NODE_ID=3000
$ step7 createwallet
Your new address: 15pQEfVhasDM51oea2C9meMZH7GmWqqduY

$ step7 createblockchain -address 15pQEfVhasDM51oea2C9meMZH7GmWqqduY
0000fbb29dedb57fe08a11c086016d99950df15808bd689eaabcb7bd00c8ddcd

Done!
$ cp blockchain_3000.db blockchain_genesis.db

# open a new terminal, NODE_3001
$ export NODE_ID=3001
$ step7 createwallet
Your new address: 1MV5eqajrvpTWf4hjVaJZTq5e4vQipbviS
$ step7 createwallet
Your new address: 1PciGsVPT94ghSQX77co8aNyMbXSePpfJg
$ step7 createwallet
Your new address: 12Zr7aJpnzGzTJDu3ezh9s6gEgAK7HASAZ
$ step7 createwallet
Your new address: 13SJsfKCbRmpEdudG2JXekzHCrhrut5UAs

# NODE_3000
$ step7 send -from 15pQEfVhasDM51oea2C9meMZH7GmWqqduY -to 1MV5eqajrvpTWf4hjVaJZTq5e4vQipbviS -amount 10 -mine
00007b71d1a6f27097946bdfffcd25476f75238325a5f218a97678325d2251be

Success!
$ step7 send -from 15pQEfVhasDM51oea2C9meMZH7GmWqqduY -to 1PciGsVPT94ghSQX77co8aNyMbXSePpfJg -amount 10 -mine
00002ab83bbffc1d5a167338037bd56d8325016f6d0a90ecb60adef8b1189d93

Success!
# The node must be running until the end of the scenario.
$ step7 startnode

# NODE_3001
$ blockchain_genesis.db blockchain_3001.db
$ step7 startnode
Starting node 3001
Received version command
Received inv command
Recevied inventory with 3 block
Received block command
Recevied a new block!
Added block 00002ab83bbffc1d5a167338037bd56d8325016f6d0a90ecb60adef8b1189d93
Received block command
Recevied a new block!
Added block 00007b71d1a6f27097946bdfffcd25476f75238325a5f218a97678325d2251be
Received block command
Recevied a new block!
Added block 0000fbb29dedb57fe08a11c086016d99950df15808bd689eaabcb7bd00c8ddcd

# stop the node and check the balances;
^C
$ step7 getbalance -address 1MV5eqajrvpTWf4hjVaJZTq5e4vQipbviS
Balance of '1MV5eqajrvpTWf4hjVaJZTq5e4vQipbviS': 10
$ step7 getbalance -address 1PciGsVPT94ghSQX77co8aNyMbXSePpfJg
Balance of '1PciGsVPT94ghSQX77co8aNyMbXSePpfJg': 10
$ step7 getbalance -address 15pQEfVhasDM51oea2C9meMZH7GmWqqduY

# open a new terminal, NODE_3002
$ export NODE_ID=3002
$ cp blockchain_genesis.db blockchain_3002.db
$ step7 createwallet
Your new address: 16xrAGwHxgTKVrYSQHD811YcNQJqo5ESmk
$ step7 startnode -miner 16xrAGwHxgTKVrYSQHD811YcNQJqo5ESmk
Starting node 3002
Mining is on. Address to receive rewards:  16xrAGwHxgTKVrYSQHD811YcNQJqo5ESmk
Received version command
Received inv command
Recevied inventory with 3 block
Received block command
Recevied a new block!
Added block 00002ab83bbffc1d5a167338037bd56d8325016f6d0a90ecb60adef8b1189d93
Received block command
Recevied a new block!
Added block 00007b71d1a6f27097946bdfffcd25476f75238325a5f218a97678325d2251be
Received block command
Recevied a new block!
Added block 0000fbb29dedb57fe08a11c086016d99950df15808bd689eaabcb7bd00c8ddcd

# NODE_3001
$ step7 send -from 1MV5eqajrvpTWf4hjVaJZTq5e4vQipbviS -to 12Zr7aJpnzGzTJDu3ezh9s6gEgAK7HASAZ -amount 1
Success!

# NODE_3002
Received inv command
Recevied inventory with 1 tx
Received tx command

# NODE_3001
$ step7 send -from 1PciGsVPT94ghSQX77co8aNyMbXSePpfJg -to 13SJsfKCbRmpEdudG2JXekzHCrhrut5UAs -amount 2
Success!

# NODE_3002
Received inv command
Recevied inventory with 1 tx
Received tx command
000058f4241df4230edae1cae5c8d54b334aac3f69af52092ba20165d17ee016

New block is mined!

# NODE_3001
$ step7 startnode
Starting node 3001
Received version command
Received inv command
Recevied inventory with 4 block
Received block command
Recevied a new block!
Added block 000058f4241df4230edae1cae5c8d54b334aac3f69af52092ba20165d17ee016
Received block command
Recevied a new block!
Added block 00002ab83bbffc1d5a167338037bd56d8325016f6d0a90ecb60adef8b1189d93
Received block command
Recevied a new block!
Added block 00007b71d1a6f27097946bdfffcd25476f75238325a5f218a97678325d2251be
Received block command
Recevied a new block!
Added block 0000fbb29dedb57fe08a11c086016d99950df15808bd689eaabcb7bd00c8ddcd
# stop the node and check the balances;
^C
$ step7 getbalance -address 1MV5eqajrvpTWf4hjVaJZTq5e4vQipbviS
Balance of '1MV5eqajrvpTWf4hjVaJZTq5e4vQipbviS': 9
$ step7 getbalance -address 1PciGsVPT94ghSQX77co8aNyMbXSePpfJg
Balance of '1PciGsVPT94ghSQX77co8aNyMbXSePpfJg': 8
$ step7 getbalance -address 12Zr7aJpnzGzTJDu3ezh9s6gEgAK7HASAZ
Balance of '12Zr7aJpnzGzTJDu3ezh9s6gEgAK7HASAZ': 1
$ step7 getbalance -address 13SJsfKCbRmpEdudG2JXekzHCrhrut5UAs
Balance of '13SJsfKCbRmpEdudG2JXekzHCrhrut5UAs': 2
$ step7 getbalance -address 16xrAGwHxgTKVrYSQHD811YcNQJqo5ESmk
Balance of '16xrAGwHxgTKVrYSQHD811YcNQJqo5ESmk': 10
```
