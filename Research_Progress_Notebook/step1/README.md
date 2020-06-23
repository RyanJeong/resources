#   Step 1.<br>

##  files<br>
```Text
step1
├── block.go
├── blockchain.go
└── main.go
```

##  codes<br>
```Go
// block.go
package main

import (
	"bytes"
	"crypto/sha256"
	"strconv"
	"time"
)

// Block keeps block headers
type Block struct {
	Timestamp     int64     // current timestamp (when the block is created)
	Data          []byte    // actual valuable information containing in the block
	PrevBlockHash []byte    // stores the hash of the previous block
	Hash          []byte    // hash of the block
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

    return
}

// NewBlock creates and returns Block
func NewBlock(data string, prevBlockHash []byte) *Block {
	block := &Block{time.Now().Unix(), []byte(data), prevBlockHash, []byte{}}
	block.SetHash()

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
		fmt.Println()
	}

    return
}
```

##  output<br>
```Text
$ step1
Prev. hash:
Data: Genesis Block
Hash: 7d39a1ecbfdcb78061a4fc6f3cdbdaac13c24c4914c57dec8a0ef272d7e6c608

Prev. hash: 7d39a1ecbfdcb78061a4fc6f3cdbdaac13c24c4914c57dec8a0ef272d7e6c608
Data: Send 1 BTC to Ivan
Hash: ed1996cbbe9801c6d357ea8a15c36d9b01c5af3db92485f1578fc502760840c9

Prev. hash: ed1996cbbe9801c6d357ea8a15c36d9b01c5af3db92485f1578fc502760840c9
Data: Send 2 more BTC to Ivan
Hash: dcc9e23090de3d988e077b1f5fec3af9be6fe45907aa7cba7ac37c28fa4de5f1
```
