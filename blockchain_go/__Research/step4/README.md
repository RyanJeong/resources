#   Step 4.<br>

##  files<br>
```Text
step4
├── block.go
├── blockchain.go
├── cli.go
├── main.go
├── proofofwork.go
├── transaction.go
└── utils.go
```

##  codes<br>
```Go
// block.go
package main

import (
    "bytes"
    
    // New added
    "crypto/sha256"
    //

    "encoding/gob"
    "log"
    "time"
)

// Block keeps block headers
type Block struct {
    Timestamp     int64     // current timestamp (when the block is created)

/*
    Data          []byte    // actual valuable information containing in the block
*/

    // New added
    Transactions  []*Transaction
    //

    PrevBlockHash []byte    // stores the hash of the previous block
    Hash          []byte    // hash of the block
    Nonce         int       // need to verify a proof
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

// New added
// HashTransactions returns a hash of the transactions in the block
func (b *Block) HashTransactions() []byte {
    var txHashes [][]byte
    var txHash [32]byte

    for _, tx := range b.Transactions {
        txHashes = append(txHashes, tx.ID)
    }
    txHash = sha256.Sum256(bytes.Join(txHashes, []byte{}))

    return txHash[:]
}
//

// NewBlock creates and returns Block
/*
func NewBlock(data string, prevBlockHash []byte) *Block {
*/

// New added
func NewBlock(transactions []*Transaction, prevBlockHash []byte) *Block {
//
/*
    block := &Block{time.Now().Unix(), []byte(data), prevBlockHash, []byte{}, 0}
*/
    
    // New added
    block := &Block{time.Now().Unix(), transactions, prevBlockHash, []byte{}, 0}
    //

    pow := NewProofOfWork(block)
    nonce, hash := pow.Run()

    block.Hash = hash[:]
    block.Nonce = nonce

    return block
}

/*
func NewGenesisBlock() *Block {
    return NewBlock("Genesis Block", []byte{})
}
*/

// New added
// NewGenesisBlock creates and returns genesis Block
func NewGenesisBlock(coinbase *Transaction) *Block {
    return NewBlock([]*Transaction{coinbase}, []byte{})
}
//

// New added
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
//
```

```Go
// blockchain.go
package main

import (
    "encoding/hex"  // new added
    "fmt"
    "log"
    "os"            // new added

    "github.com/boltdb/bolt"
)

const dbFile = "blockchain.db"
const blocksBucket = "blocks"

// New added
const genesisCoinbaseData = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks"
//

// Blockchain keeps a sequence of Blocks
type Blockchain struct {
    tip []byte
    db  *bolt.DB
}

// BlockchainIterator is used to iterate over blockchain blocks
type BlockchainIterator struct {
    currentHash []byte
    db          *bolt.DB
}

// New added
// MineBlock mines a new block with the provided transactions
func (bc *Blockchain) MineBlock(transactions []*Transaction) {
    var lastHash []byte

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
}
//

// New added
// FindUnspentTransactions returns a list of transactions containing unspent outputs
func (bc *Blockchain) FindUnspentTransactions(address string) []Transaction {
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

                if out.CanBeUnlockedWith(address) {
                    unspentTXs = append(unspentTXs, *tx)
                }
            }

            if tx.IsCoinbase() == false {
                for _, in := range tx.Vin {
                    if in.CanUnlockOutputWith(address) {
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
//

// New added
// FindUTXO finds and returns all unspent transaction outputs
func (bc *Blockchain) FindUTXO(address string) []TXOutput {
    var UTXOs []TXOutput
    unspentTransactions := bc.FindUnspentTransactions(address)

    for _, tx := range unspentTransactions {
        for _, out := range tx.Vout {
            if out.CanBeUnlockedWith(address) {
                UTXOs = append(UTXOs, out)
            }
        }
    }

    return UTXOs
}
//

// New added
// FindSpendableOutputs finds and returns unspent outputs to reference in inputs
func (bc *Blockchain) FindSpendableOutputs(address string, amount int) (int, map[string][]int) {
    unspentOutputs := make(map[string][]int)
    unspentTXs := bc.FindUnspentTransactions(address)
    accumulated := 0

Work:
    for _, tx := range unspentTXs {
        txID := hex.EncodeToString(tx.ID)

        for outIdx, out := range tx.Vout {
            if out.CanBeUnlockedWith(address) && accumulated < amount {
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
//

/*
// AddBlock saves provided data as a block in the blockchain
func (bc *Blockchain) AddBlock(data string) {
    var lastHash []byte

    err := bc.db.View(func(tx *bolt.Tx) error {
        b := tx.Bucket([]byte(blocksBucket))
        lastHash = b.Get([]byte("l"))

        return nil
    })

    if err != nil {
        log.Panic(err)
    }

    newBlock := NewBlock(data, lastHash)

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
}
*/

// Iterator ...
func (bc *Blockchain) Iterator() *BlockchainIterator {
    bci := &BlockchainIterator{bc.tip, bc.db}

    return bci
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

// New added
func dbExists() bool {
    if _, err := os.Stat(dbFile); os.IsNotExist(err) {
        return false
    }

    return true
}
//


/*
// NewBlockchain creates a new Blockchain with genesis Block
func NewBlockchain() *Blockchain {
    var tip []byte

    // Open the my.db data file in your current directory.
    // It will be created if it doesn't exist.
    db, err := bolt.Open(dbFile, 0600, nil)
    if err != nil {
        log.Panic(err)
    }

    err = db.Update(func(tx *bolt.Tx) error {
        b := tx.Bucket([]byte(blocksBucket))

        if b == nil {
            fmt.Println("No existing blockchain found. Creating a new one...")
            genesis := NewGenesisBlock()

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
        } else {
            tip = b.Get([]byte("l"))
        }

        return nil
    })

    if err != nil {
        log.Panic(err)
    }

    bc := Blockchain{tip, db}

    return &bc
}
*/

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

// CreateBlockchain creates a new blockchain DB
func CreateBlockchain(address string) *Blockchain {
    if dbExists() {
        fmt.Println("Blockchain already exists.")
        os.Exit(1)
    }

    var tip []byte
    db, err := bolt.Open(dbFile, 0600, nil)
    if err != nil {
        log.Panic(err)
    }

    err = db.Update(func(tx *bolt.Tx) error {
        cbtx := NewCoinbaseTX(address, genesisCoinbaseData)
        genesis := NewGenesisBlock(cbtx)

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
    "strconv"
)

// CLI responsible for processing command line arguments
type CLI struct {
    bc *Blockchain
}

// New added
func (cli *CLI) createBlockchain(address string) {
    bc := CreateBlockchain(address)
    bc.db.Close()
    fmt.Println("Done!")
}

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
//

func (cli *CLI) printUsage() {
    fmt.Println("Usage:")
/*
    fmt.Println("  addblock -data BLOCK_DATA - add a block to the blockchain")
    fmt.Println("  printchain - print all the blocks of the blockchain")
*/

    // New added
    fmt.Println("  getbalance -address ADDRESS - Get balance of ADDRESS")
    fmt.Println("  createblockchain -address ADDRESS - Create a blockchain and send genesis block reward to ADDRESS")
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

/*
func (cli *CLI) addBlock(data string) {
    cli.bc.AddBlock(data)
    fmt.Println("Success!")
}
*/

func (cli *CLI) printChain() {
/*
    bci := cli.bc.Iterator()
*/

    // New added
    // TODO: Fix this
    bc := NewBlockchain("")
    defer bc.db.Close()

    bci := bc.Iterator()
    //
    for {
        block := bci.Next()

        fmt.Printf("Prev. hash: %x\n", block.PrevBlockHash)
/*
        fmt.Printf("Data: %s\n", block.Data)
*/
        fmt.Printf("Hash: %x\n", block.Hash)
        pow := NewProofOfWork(block)
        fmt.Printf("PoW: %s\n", strconv.FormatBool(pow.Validate()))
        fmt.Println()

        if len(block.PrevBlockHash) == 0 {
            break
        }
    }
}

// New added
func (cli *CLI) send(from, to string, amount int) {
    bc := NewBlockchain(from)
    defer bc.db.Close()

    tx := NewUTXOTransaction(from, to, amount, bc)
    bc.MineBlock([]*Transaction{tx})
    fmt.Println("Success!")
}
//

/*
// Run parses command line arguments and processes commands
func (cli *CLI) Run() {
    cli.validateArgs()

    addBlockCmd := flag.NewFlagSet("addblock", flag.ExitOnError)
    printChainCmd := flag.NewFlagSet("printchain", flag.ExitOnError)

    addBlockData := addBlockCmd.String("data", "", "Block data")

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
    case "addblock":
        err := addBlockCmd.Parse(os.Args[2:])
        if err != nil {
            log.Panic(err)
        }
    case "printchain":
        err := printChainCmd.Parse(os.Args[2:])
        if err != nil {
            log.Panic(err)
        }
    default:
        cli.printUsage()
        os.Exit(1)
    }

    if addBlockCmd.Parsed() {
        if *addBlockData == "" {
            addBlockCmd.Usage()
            os.Exit(1)
        }
        cli.addBlock(*addBlockData)
    }

    if printChainCmd.Parsed() {
        cli.printChain()
    }
}
*/

// New added
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
//
```

```Go
// main.go
package main

func main() {
/*
    // must be at least one block
    bc := NewBlockchain()


    // New added
    defer bc.db.Close()
*/

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
    maxNonce = math.MaxInt64 // to avoid a possible overflow of nonce
)

const targetBits = 24 // Define the difficulty of mining

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
/*
            pow.block.Data,
*/

            // New added
            pow.block.HashTransactions(),
            //
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
//New added
package main

import (
	"bytes"
	"crypto/sha256"
	"encoding/gob"
	"encoding/hex"
	"fmt"
	"log"
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

// NewCoinbaseTX creates a new coinbase transaction
func NewCoinbaseTX(to, data string) *Transaction {
	if data == "" {
		data = fmt.Sprintf("Reward to '%s'", to)
	}

	txin := TXInput{[]byte{}, -1, data}
	txout := TXOutput{subsidy, to}
	tx := Transaction{nil, []TXInput{txin}, []TXOutput{txout}}
	tx.SetID()

	return &tx
}

// NewUTXOTransaction creates a new transaction
func NewUTXOTransaction(from, to string, amount int, bc *Blockchain) *Transaction {
	var inputs []TXInput
	var outputs []TXOutput

	acc, validOutputs := bc.FindSpendableOutputs(from, amount)

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
			input := TXInput{txID, out, from}
			inputs = append(inputs, input)
		}
	}

	// Build a list of outputs
	outputs = append(outputs, TXOutput{amount, to})
	if acc > amount {
		outputs = append(outputs, TXOutput{acc - amount, from}) // a change
	}

	tx := Transaction{nil, inputs, outputs}
	tx.SetID()

	return &tx
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
```

##  output<br>
```Text
$ step4 createblockchain -address Ivan
0000000501641432da1e4c94dab840fc37e9fb3f065b76828216be8f71269f96

Done!
$step4 send -from Ivan -to Pedro -amount 6
000000752723bbb675f706732c7f5438d36966ed25bcdb7ea729821116a2b115

Success!
$ step4 getbalance -address Ivan
Balance of 'Ivan': 4
$ step4 getbalance -address Pedro
Balance of 'Pedro': 6

$ step4 send -from Pedro -to Helen -amount 2
00000044581abcb4217e4230b27a1a658a55c406eebe8914440f9fa73fd00e0b

Success!
$ step4 send -from Ivan -to Helen -amount 2
000000c865206f3c7b2179cba4bdb9fc02092bf356aa0bd17920132971a3ae28

Success!
$ step4 send -from Helen -to Rachel -amount 3
0000009ed40e898faaf9c2205366c833ef74f6fda6d200c29849912522353272

Success!
$ step4 getbalance -address Ivan
Balance of 'Ivan': 2
$ step4 getbalance -address Pedro
Balance of 'Pedro': 4
$ step4 getbalance -address Helen
Balance of 'Helen': 1
$ step4 getbalance -address Rachel
Balance of 'Rachel': 3

$ step4 send -from Pedro -to Ivan -amount 5
2020/06/24 22:05:10 ERROR: Not enough funds
panic: ERROR: Not enough funds

$ step4 getbalance -address Pedro
Balance of 'Pedro': 4
$ step4 getbalance -address Ivan
Balance of 'Ivan': 2
```
