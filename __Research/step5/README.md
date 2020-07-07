##  files<br>
```Text
step5
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
├── cli_send.go
├── main.go
├── proofofwork.go
├── transaction.go
├── transaction_input.go
├── transaction_output.go
├── utils.go
├── wallet.go
└── wallets.go
```

##  codes<br>
```Go
// New added
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
//
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
        /*
        txHashes = append(txHashes, tx.ID)
        */
        // New added
		txHashes = append(txHashes, tx.Hash())
        //
    }
    txHash = sha256.Sum256(bytes.Join(txHashes, []byte{}))

    return txHash[:]
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
    // New added
	"bytes"
	"crypto/ecdsa"
	"errors"
    //

    "encoding/hex"
    "fmt"
    "log"
    "os"

    "github.com/boltdb/bolt"
)

const dbFile = "blockchain.db"
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

    // New added
	cbtx := NewCoinbaseTX(address, genesisCoinbaseData)
	genesis := NewGenesisBlock(cbtx)
    //

    db, err := bolt.Open(dbFile, 0600, nil)
    if err != nil {
        log.Panic(err)
    }

    err = db.Update(func(tx *bolt.Tx) error {
        /*
        cbtx := NewCoinbaseTX(address, genesisCoinbaseData)
        genesis := NewGenesisBlock(cbtx)
        */

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

// New added
// NewBlockchain creates a new Blockchain with genesis Block
func NewBlockchain(address string) *Blockchain {
	if dbExists() == false {
		fmt.Println("No existing blockchain found. Create one first.")
		os.Exit(1)
	}

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
//

// FindSpendableOutputs finds and returns unspent outputs to reference in inputs
/*
func (bc *Blockchain) FindSpendableOutputs(address string, amount int) (int, map[string][]int) {
*/
// New added
func (bc *Blockchain) FindSpendableOutputs(pubKeyHash []byte, amount int) (int, map[string][]int) {
//
    unspentOutputs := make(map[string][]int)
    /*
    unspentTXs := bc.FindUnspentTransactions(address)
    */
    // New added
	unspentTXs := bc.FindUnspentTransactions(pubKeyHash)
    //
    accumulated := 0

Work:
    for _, tx := range unspentTXs {
        txID := hex.EncodeToString(tx.ID)

        for outIdx, out := range tx.Vout {
            /*
            if out.CanBeUnlockedWith(address) && accumulated < amount {
            */
            // New added
			if out.IsLockedWithKey(pubKeyHash) && accumulated < amount {
            //
                accumulated += out.Value
                unspentOutputs[txID] = append(unspentOutputs[txID], outIdx)

                if accumulated >= amount {
                    break Work
                }
            }
        }
    }

    return accumulated, unspentOutputs
}

// New added
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
//

// FindUnspentTransactions returns a list of transactions containing unspent outputs
/*
func (bc *Blockchain) FindUnspentTransactions(address string) []Transaction {
*/
// New added
func (bc *Blockchain) FindUnspentTransactions(pubKeyHash []byte) []Transaction {
//
    var unspentTXs []Transaction
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
                    for _, spentOut := range spentTXOs[txID] {
                        if spentOut == outIdx {
                            continue Outputs
                        }
                    }
                }

                /*
                if out.CanBeUnlockedWith(address) {
                */
                // New added
				if out.IsLockedWithKey(pubKeyHash) {
                //
                    unspentTXs = append(unspentTXs, *tx)
                }
            }

            if tx.IsCoinbase() == false {
                for _, in := range tx.Vin {
                    /*
                    if in.CanUnlockOutputWith(address) {
                    */
                    // New added
					if in.UsesKey(pubKeyHash) {
                    //
                        inTxID := hex.EncodeToString(in.Txid)
                        spentTXOs[inTxID] = append(spentTXOs[inTxID], in.Vout)
                    }
                }
            }
        }

        if len(block.PrevBlockHash) == 0 {
            break
        }
    }

    return unspentTXs
}

// FindUTXO finds and returns all unspent transaction outputs
/*
func (bc *Blockchain) FindUTXO(address string) []TXOutput {
*/
// New added
func (bc *Blockchain) FindUTXO(pubKeyHash []byte) []TXOutput {
//
    var UTXOs []TXOutput
    /*
    unspentTransactions := bc.FindUnspentTransactions(address)
    */
    // New added
	unspentTransactions := bc.FindUnspentTransactions(pubKeyHash)
    //

    for _, tx := range unspentTransactions {
        for _, out := range tx.Vout {
            /*
            if out.CanBeUnlockedWith(address) {
            */
            // New added
			if out.IsLockedWithKey(pubKeyHash) {
            //
                UTXOs = append(UTXOs, out)
            }
        }
    }

    return UTXOs
}

// Iterator returns a BlockchainIteratator
func (bc *Blockchain) Iterator() *BlockchainIterator {
    bci := &BlockchainIterator{bc.tip, bc.db}

    return bci
}

// MineBlock mines a new block with the provided transactions
func (bc *Blockchain) MineBlock(transactions []*Transaction) {
    var lastHash []byte

    // New added
	for _, tx := range transactions {
		if bc.VerifyTransaction(tx) != true {
			log.Panic("ERROR: Invalid transaction")
		}
	}
    //

    // read-only
    err := bc.db.View(func(tx *bolt.Tx) error {
        b := tx.Bucket([]byte(blocksBucket))
        lastHash = b.Get([]byte("l"))

        return nil
    })

    if err != nil {
        log.Panic(err)
    }

    newBlock := NewBlock(transactions, lastHash)

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
    // New added
	if err != nil {
		log.Panic(err)
	}
    //
}

// New added
// VerifyTransaction verifies transaction input signatures
func (bc *Blockchain) VerifyTransaction(tx *Transaction) bool {
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
//

func dbExists() bool {
    if _, err := os.Stat(dbFile); os.IsNotExist(err) {
        return false
    }

    return true
}

/*
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


// NewBlockchain creates a new Blockchain with genesis Block
func NewBlockchain(address string) *Blockchain {
    if dbExists() == false {
        fmt.Println("No existing blockchain found. Create one first.")
        os.Exit(1)
    }

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
*/
```

```Go
// blockchain_iterator.go
// New added
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
//
```

```Go
// cli.go
package main

import (
    "flag"
    "fmt"
    "log"
    "os"
    /*
    "strconv"
    */
)

// CLI responsible for processing command line arguments
type CLI struct {
    /*
    bc *Blockchain
    */
}

/*
func (cli *CLI) createBlockchain(address string) {
    bc := CreateBlockchain(address)
    bc.db.Close()
    fmt.Println("Done!")
}
*/

/*
func (cli *CLI) getBalance(address string) {
    bc := NewBlockchain(address)
    defer bc.db.Close()

    balance := 0
    UTXOs := bc.FindUTXO(address)

    for _, out := range UTXOs {
        balance += out.Value
    }

    fmt.Printf("Balance of '%s': %d\n", address, balance)
}
*/

func (cli *CLI) printUsage() {
    fmt.Println("Usage:")

    /*
    fmt.Println("  getbalance -address ADDRESS - Get balance of ADDRESS")
    fmt.Println("  createblockchain -address ADDRESS - Create a blockchain and send genesis block reward to ADDRESS")
    fmt.Println("  printchain - Print all the blocks of the blockchain")
    fmt.Println("  send -from FROM -to TO -amount AMOUNT - Send AMOUNT of coins from FROM address to TO")
    */
    // New added
	fmt.Println("  createblockchain -address ADDRESS - Create a blockchain and send genesis block reward to ADDRESS")
	fmt.Println("  createwallet - Generates a new key-pair and saves it into the wallet file")
	fmt.Println("  getbalance -address ADDRESS - Get balance of ADDRESS")
	fmt.Println("  listaddresses - Lists all addresses from the wallet file")
	fmt.Println("  printchain - Print all the blocks of the blockchain")
	fmt.Println("  send -from FROM -to TO -amount AMOUNT - Send AMOUNT of coins from FROM address to TO")
    //
}

func (cli *CLI) validateArgs() {
    if len(os.Args) < 2 {
        cli.printUsage()
        os.Exit(1)
    }
}

// New added
// Run parses command line arguments and processes commands
func (cli *CLI) Run() {
	cli.validateArgs()

	getBalanceCmd := flag.NewFlagSet("getbalance", flag.ExitOnError)
	createBlockchainCmd := flag.NewFlagSet("createblockchain", flag.ExitOnError)
	createWalletCmd := flag.NewFlagSet("createwallet", flag.ExitOnError)
	listAddressesCmd := flag.NewFlagSet("listaddresses", flag.ExitOnError)
	sendCmd := flag.NewFlagSet("send", flag.ExitOnError)
	printChainCmd := flag.NewFlagSet("printchain", flag.ExitOnError)

	getBalanceAddress := getBalanceCmd.String("address", "", "The address to get balance for")
	createBlockchainAddress := createBlockchainCmd.String("address", "", "The address to send genesis block reward to")
	sendFrom := sendCmd.String("from", "", "Source wallet address")
	sendTo := sendCmd.String("to", "", "Destination wallet address")
	sendAmount := sendCmd.Int("amount", 0, "Amount to send")

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
	case "send":
		err := sendCmd.Parse(os.Args[2:])
		if err != nil {
			log.Panic(err)
		}
	default:
		cli.printUsage()
		os.Exit(1)
	}

	if getBalanceCmd.Parsed() {
		if *getBalanceAddress == "" {
			getBalanceCmd.Usage()
			os.Exit(1)
		}
		cli.getBalance(*getBalanceAddress)
	}

	if createBlockchainCmd.Parsed() {
		if *createBlockchainAddress == "" {
			createBlockchainCmd.Usage()
			os.Exit(1)
		}
		cli.createBlockchain(*createBlockchainAddress)
	}

	if createWalletCmd.Parsed() {
		cli.createWallet()
	}

	if listAddressesCmd.Parsed() {
		cli.listAddresses()
	}

	if printChainCmd.Parsed() {
		cli.printChain()
	}

	if sendCmd.Parsed() {
		if *sendFrom == "" || *sendTo == "" || *sendAmount <= 0 {
			sendCmd.Usage()
			os.Exit(1)
		}

		cli.send(*sendFrom, *sendTo, *sendAmount)
	}
}
//

/*
func (cli *CLI) printChain() {

    bc := NewBlockchain("")
    defer bc.db.Close()

    bci := bc.Iterator()
    for {
        block := bci.Next()

        fmt.Printf("Prev. hash: %x\n", block.PrevBlockHash)
        fmt.Printf("Hash: %x\n", block.Hash)
        pow := NewProofOfWork(block)
        fmt.Printf("PoW: %s\n", strconv.FormatBool(pow.Validate()))
        fmt.Println()

        if len(block.PrevBlockHash) == 0 {
            break
        }
    }
}

func (cli *CLI) send(from, to string, amount int) {
    bc := NewBlockchain(from)
    defer bc.db.Close()

    tx := NewUTXOTransaction(from, to, amount, bc)
    bc.MineBlock([]*Transaction{tx})
    fmt.Println("Success!")
}

// Run parses command line arguments and processes commands
func (cli *CLI) Run() {
    cli.validateArgs()

    getBalanceCmd := flag.NewFlagSet("getbalance", flag.ExitOnError)
    createBlockchainCmd := flag.NewFlagSet("createblockchain", flag.ExitOnError)
    sendCmd := flag.NewFlagSet("send", flag.ExitOnError)
    printChainCmd := flag.NewFlagSet("printchain", flag.ExitOnError)

    getBalanceAddress := getBalanceCmd.String("address", "", "The address to get balance for")
    createBlockchainAddress := createBlockchainCmd.String("address", "", "The address to send genesis block reward to")
    sendFrom := sendCmd.String("from", "", "Source wallet address")
    sendTo := sendCmd.String("to", "", "Destination wallet address")
    sendAmount := sendCmd.Int("amount", 0, "Amount to send")

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
    case "printchain":
        err := printChainCmd.Parse(os.Args[2:])
        if err != nil {
            log.Panic(err)
        }
    case "send":
        err := sendCmd.Parse(os.Args[2:])
        if err != nil {
            log.Panic(err)
        }
    default:
        cli.printUsage()
        os.Exit(1)
    }

    if getBalanceCmd.Parsed() {
        if *getBalanceAddress == "" {
            getBalanceCmd.Usage()
            os.Exit(1)
        }
        cli.getBalance(*getBalanceAddress)
    }

    if createBlockchainCmd.Parsed() {
        if *createBlockchainAddress == "" {
            createBlockchainCmd.Usage()
            os.Exit(1)
        }
        cli.createBlockchain(*createBlockchainAddress)
    }

    if printChainCmd.Parsed() {
        cli.printChain()
    }

    if sendCmd.Parsed() {
        if *sendFrom == "" || *sendTo == "" || *sendAmount <= 0 {
            sendCmd.Usage()
            os.Exit(1)
        }

        cli.send(*sendFrom, *sendTo, *sendAmount)
    }
}
*/
```
```Go
// cli_createblockchain.go
// New added
package main

import (
	"fmt"
	"log"
)

func (cli *CLI) createBlockchain(address string) {
	if !ValidateAddress(address) {
		log.Panic("ERROR: Address is not valid")
	}
	bc := CreateBlockchain(address)
	bc.db.Close()
	fmt.Println("Done!")
}
//
```
```Go
// cli_createwallet.go
// New added
package main

import "fmt"

func (cli *CLI) createWallet() {
	wallets, _ := NewWallets()
	address := wallets.CreateWallet()
	wallets.SaveToFile()

	fmt.Printf("Your new address: %s\n", address)
}
//
```
```Go
// cli_getbalance.go
// New added
package main

import (
	"fmt"
	"log"
)

func (cli *CLI) getBalance(address string) {
	if !ValidateAddress(address) {
		log.Panic("ERROR: Address is not valid")
	}
	bc := NewBlockchain(address)
	defer bc.db.Close()

	balance := 0
	pubKeyHash := Base58Decode([]byte(address))
	pubKeyHash = pubKeyHash[1 : len(pubKeyHash)-4]
	UTXOs := bc.FindUTXO(pubKeyHash)

	for _, out := range UTXOs {
		balance += out.Value
	}

	fmt.Printf("Balance of '%s': %d\n", address, balance)
}
//
```
```Go
// cli_listaddress.go
// New added
package main

import (
	"fmt"
	"log"
)

func (cli *CLI) listAddresses() {
	wallets, err := NewWallets()
	if err != nil {
		log.Panic(err)
	}
	addresses := wallets.GetAddresses()

	for _, address := range addresses {
		fmt.Println(address)
	}
}
//
```
```Go
// cli_printchain.go
// New added
package main

import (
	"fmt"
	"strconv"
)

func (cli *CLI) printChain() {
	bc := NewBlockchain("")
	defer bc.db.Close()

	bci := bc.Iterator()

	for {
		block := bci.Next()

		fmt.Printf("============ Block %x ============\n", block.Hash)
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
//
```
```Go
// cli_send.go
// New added
package main

import (
	"fmt"
	"log"
)

func (cli *CLI) send(from, to string, amount int) {
	if !ValidateAddress(from) {
		log.Panic("ERROR: Sender address is not valid")
	}
	if !ValidateAddress(to) {
		log.Panic("ERROR: Recipient address is not valid")
	}

	bc := NewBlockchain(from)
	defer bc.db.Close()

	tx := NewUTXOTransaction(from, to, amount, bc)
	bc.MineBlock([]*Transaction{tx})
	fmt.Println("Success!")
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
const targetBits = 24 

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
// transaction.go
package main

import (
/*
	"bytes"
	"crypto/sha256"
	"encoding/gob"
	"encoding/hex"
	"fmt"
	"log"
*/

    // New added
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
    //
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

// New added
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
//

/*
// SetID sets ID of a transaction
func (tx *Transaction) SetID() {
	var encoded bytes.Buffer
	var hash [32]byte

	enc := gob.NewEncoder(&encoded)
	err := enc.Encode(tx)
	if err != nil {
		log.Panic(err)
	}
	hash = sha256.Sum256(encoded.Bytes())
	tx.ID = hash[:]
}

// TXInput represents a transaction input
type TXInput struct {
	Txid      []byte
	Vout      int
	ScriptSig string
}

// TXOutput represents a transaction output
type TXOutput struct {
	Value        int
	ScriptPubKey string
}

// CanUnlockOutputWith checks whether the address initiated the transaction
func (in *TXInput) CanUnlockOutputWith(unlockingData string) bool {
	return in.ScriptSig == unlockingData
}

// CanBeUnlockedWith checks if the output can be unlocked with the provided data
func (out *TXOutput) CanBeUnlockedWith(unlockingData string) bool {
	return out.ScriptPubKey == unlockingData
}
*/

// NewCoinbaseTX creates a new coinbase transaction
func NewCoinbaseTX(to, data string) *Transaction {
	if data == "" {
		data = fmt.Sprintf("Reward to '%s'", to)
	}

    /*
	txin := TXInput{[]byte{}, -1, data}
    */
    // New added
	txin := TXInput{[]byte{}, -1, nil, []byte(data)}
    //
	txout := TXOutput{subsidy, to}
	tx := Transaction{nil, []TXInput{txin}, []TXOutput{txout}}
    /*
	tx.SetID()
    */
    // New added
    tx.ID = tx.Hash()
    //

	return &tx
}

// NewUTXOTransaction creates a new transaction
func NewUTXOTransaction(from, to string, amount int, bc *Blockchain) *Transaction {
	var inputs []TXInput
	var outputs []TXOutput

/*
	acc, validOutputs := bc.FindSpendableOutputs(from, amount)
*/
    // New added
	wallets, err := NewWallets()
	if err != nil {
		log.Panic(err)
	}
	wallet := wallets.GetWallet(from)
	pubKeyHash := HashPubKey(wallet.PublicKey)
	acc, validOutputs := bc.FindSpendableOutputs(pubKeyHash, amount)
    //

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
        /*
			input := TXInput{txID, out, from}
        */
            // New added
			input := TXInput{txID, out, nil, wallet.PublicKey}
            //
			inputs = append(inputs, input)
		}
	}

	// Build a list of outputs
    /*
	outputs = append(outputs, TXOutput{amount, to})
	if acc > amount {
		outputs = append(outputs, TXOutput{acc - amount, from}) // a change
	}

	tx := Transaction{nil, inputs, outputs}
	tx.SetID()
    */
    // New added
	outputs = append(outputs, *NewTXOutput(amount, to))
	if acc > amount {
		outputs = append(outputs, *NewTXOutput(acc-amount, from)) // a change
	}

	tx := Transaction{nil, inputs, outputs}
	tx.ID = tx.Hash()
	bc.SignTransaction(&tx, wallet.PrivateKey)
    //

	return &tx
}
```

```Go
// transaction_input.go
// New added
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
//
```

```Go
// transaction_input.go
// New added
package main

import "bytes"

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
//
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

// New added
// ReverseBytes reverses a byte array
func ReverseBytes(data []byte) {
	for i, j := 0, len(data)-1; i < j; i, j = i+1, j-1 {
		data[i], data[j] = data[j], data[i]
	}
}
//
```

```Go
// wallet.go
// New added
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
const walletFile = "wallet.dat"
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
//
```

```Go
// wallets.go
// New added
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
func (ws *Wallets) LoadFromFile() error {
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
func (ws Wallets) SaveToFile() {
	var content bytes.Buffer

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
$ step5# ./step5 createwallet
Your new address: 13CvU4QSuKFdRKs1MkugyVo3T6Tudk19SA
$ step5 createwallet
Your new address: 1FHjBKvinq7x4PMnHF4oyp5mSLULzXQGnz
$ step5 createwallet
Your new address: 1Eq9fZbskQ5WsYWicMwQNn5v9zoKsHBrbW

$ step5 createblockchain -address 13CvU4QSuKFdRKs1MkugyVo3T6Tudk19SA
00000061fd7d2b03505eba0f37df3ff8e667d1c4a0d731108fe8bd7d9223b8a5

Done!

$ step5 getbalance -address 13CvU4QSuKFdRKs1MkugyVo3T6Tudk19SA
Balance of '13CvU4QSuKFdRKs1MkugyVo3T6Tudk19SA': 10
$ step5 send -from 1FHjBKvinq7x4PMnHF4oyp5mSLULzXQGnz -to 13CvU4QSuKFdRKs1MkugyVo3T6Tudk19SA -amount 5
2020/06/30 15:23:17 ERROR: Not enough funds
panic: ERROR: Not enough funds
$ step5 send -from 13CvU4QSuKFdRKs1MkugyVo3T6Tudk19SA -to 1FHjBKvinq7x4PMnHF4oyp5mSLULzXQGnz -amount 6
0000009be927ae1de0a58b2ac5de8586c4a27edacb9bfd4065fde4dc1bf9938a

Success!

$ step5 getbalance -address 13CvU4QSuKFdRKs1MkugyVo3T6Tudk19SA
Balance of '13CvU4QSuKFdRKs1MkugyVo3T6Tudk19SA': 4
$ step5 getbalance -address 1FHjBKvinq7x4PMnHF4oyp5mSLULzXQGnz
Balance of '1FHjBKvinq7x4PMnHF4oyp5mSLULzXQGnz': 6
$ step5 getbalance -address 1Eq9fZbskQ5WsYWicMwQNn5v9zoKsHBrbW
Balance of '1Eq9fZbskQ5WsYWicMwQNn5v9zoKsHBrbW': 0
```
