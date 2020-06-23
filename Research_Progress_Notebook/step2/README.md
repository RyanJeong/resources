#   Step 2.<br>

##  files<br>
```Text
step2
├── block.go
├── blockchain.go
├── main.go
├── proofofwork.go
└── utils.go
```

##  codes<br>
```Go
// block.go
package main

import (
	"bytes"
/*
	"crypto/sha256"
	"strconv"
	"time"
*/
)

// Block keeps block headers
type Block struct {
	Timestamp     int64     // current timestamp (when the block is created)
	Data          []byte    // actual valuable information containing in the block
	PrevBlockHash []byte    // stores the hash of the previous block
	Hash          []byte    // hash of the block
/*
}

// SetHash calculates and sets block hash

func (b *Block) SetHash() {
	timestamp := []byte(strconv.FormatInt(b.Timestamp, 10))

    // Concatenate datas(PrevBlockHash, Data, Timestamp converted to byte[]) to make a block header
	headers := bytes.Join([][]byte{b.PrevBlockHash, b.Data, timestamp}, []byte{})

    // calculate a SHA-256 hash on the concatenated combination
    // Sum256 returns the SHA256 checksum of the data.
	hash := sha256.Sum256(headers)
	b.Hash = hash[:]
*/
	Nonce         int // need to verify a proof
}

// NewBlock creates and returns Block
func NewBlock(data string, prevBlockHash []byte) *Block {
/*
	block := &Block{time.Now().Unix(), []byte(data), prevBlockHash, []byte{}}
	block.SetHash()
*/
	block := &Block{time.Now().Unix(), []byte(data), prevBlockHash, []byte{}, 0}
	pow := NewProofOfWork(block)
	nonce, hash := pow.Run()

	block.Hash = hash[:]
	block.Nonce = nonce

	return block
}

// NewGenesisBlock creates and returns genesis Block
func NewGenesisBlock() *Block {
	return NewBlock("Genesis Block", []byte{})
}
```

```Go
// blockchain.go
package main

// Blockchain keeps a sequence of Blocks
type Blockchain struct {
	blocks []*Block
}

// AddBlock saves provided data as a block in the blockchain
func (bc *Blockchain) AddBlock(data string) {
	prevBlock := bc.blocks[len(bc.blocks)-1]
	newBlock := NewBlock(data, prevBlock.Hash)
	bc.blocks = append(bc.blocks, newBlock)

    return
}

// NewBlockchain creates a new Blockchain with genesis Block
func NewBlockchain() *Blockchain {

	return &Blockchain{[]*Block{NewGenesisBlock()}}
}
```

```Go
// main.go
package main

import (
	"fmt"
	"strconv"
)

func main() {
    // must be at least one block
	bc := NewBlockchain()
	bc.AddBlock("Send 1 BTC to Ivan")
	bc.AddBlock("Send 2 more BTC to Ivan")
	for _, block := range bc.blocks {
		fmt.Printf("Prev. hash: %x\n", block.PrevBlockHash)
		fmt.Printf("Data: %s\n", block.Data)
		fmt.Printf("Hash: %x\n", block.Hash)
		pow := NewProofOfWork(block)
		fmt.Printf("PoW: %s\n", strconv.FormatBool(pow.Validate()))
		fmt.Println()
	}
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
			pow.block.Data,
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

	fmt.Printf("Mining the block containing \"%s\"\n", pow.block.Data)

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
$ step2
Mining the block containing "Genesis Block"
000000e05bcabc4c9ec36d909949233c8d764e3950f30319aaf126a12328e1b9

Mining the block containing "Send 1 BTC to Ivan"
000000217fd5ea594f8f26fd9bc0965b2a6d664ff660dc2e35a478da0cf19a0a

Mining the block containing "Send 2 more BTC to Ivan"
000000723653c8f6335c853d72f43f19714200447163acc7b2bc28de87857e5e

Prev. hash: 
Data: Genesis Block
Hash: 000000e05bcabc4c9ec36d909949233c8d764e3950f30319aaf126a12328e1b9
PoW: true

Prev. hash: 000000e05bcabc4c9ec36d909949233c8d764e3950f30319aaf126a12328e1b9
Data: Send 1 BTC to Ivan
Hash: 000000217fd5ea594f8f26fd9bc0965b2a6d664ff660dc2e35a478da0cf19a0a
PoW: true

Prev. hash: 000000217fd5ea594f8f26fd9bc0965b2a6d664ff660dc2e35a478da0cf19a0a
Data: Send 2 more BTC to Ivan
Hash: 000000723653c8f6335c853d72f43f19714200447163acc7b2bc28de87857e5e
PoW: true
```
