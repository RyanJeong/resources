package block

import (
	"bytes"
	"crypto/sha256"
	"fmt"
	"math"
	"math/big"
	"strconv"
	"time"

	"github.com/ryanjeong/go_blockchain/src/util"
)

const targetBits = 24

type Block struct {
	Timestamp     int64
	Data          []byte
	PrevBlockHash []byte
	Hash          []byte
	Nonce         int
}

type Blockchain struct {
	Blocks []*Block
}

type ProofOfWork struct {
	block  *Block
	target *big.Int
}

func (bc *Blockchain) AddBlock(data string) {
	prevBlock := bc.Blocks[len(bc.Blocks)-1]
	newBlock := NewBlock(data, prevBlock.Hash)
	bc.Blocks = append(bc.Blocks, newBlock)

	/* Display the new block's infomation */
	fmt.Printf("Timestamp: %d\n", newBlock.Timestamp)
	fmt.Printf("Data: %s\n", newBlock.Data)
	fmt.Printf("Prev. hash: %x\n", newBlock.PrevBlockHash)
	fmt.Printf("Hash: %x\n", newBlock.Hash)
	fmt.Println()
	pow := NewProofOfWork(newBlock)
	fmt.Printf("PoW: %s\n", strconv.FormatBool(pow.Validate()))
	fmt.Println()

	return
}

func (b *Block) SetHash() {
	timestamp := []byte(strconv.FormatInt(b.Timestamp, 10))
	headers := bytes.Join([][]byte{b.PrevBlockHash, b.Data, timestamp}, []byte{})
	hash := sha256.Sum256(headers)

	b.Hash = hash[:]

	return
}

func NewBlock(data string, prevBlockHash []byte) *Block {
	block := &Block{time.Now().Unix(), []byte(data), prevBlockHash, []byte{}, 0}
	pow := NewProofOfWork(block)
	nonce, hash := pow.Run()

	block.Hash = hash[:]
	block.Nonce = nonce

	return block
}

func NewGenesisBlock() *Block {

	return NewBlock("Genesis Block", []byte{})
}

func NewBlockchain() *Blockchain {

	return &Blockchain{[]*Block{NewGenesisBlock()}}
}

func NewProofOfWork(b *Block) *ProofOfWork {
	target := big.NewInt(1)
	target.Lsh(target, uint(256-targetBits))

	pow := &ProofOfWork{b, target}

	return pow
}

func (pow *ProofOfWork) prepareData(nonce int) []byte {
	data := bytes.Join(
		[][]byte{
			pow.block.PrevBlockHash,
			pow.block.Data,
			util.IntToHex(pow.block.Timestamp),
			util.IntToHex(int64(targetBits)),
			util.IntToHex(int64(nonce)),
		},
		[]byte{},
	)

	return data
}

func (pow *ProofOfWork) Validate() bool {
	var hashInt big.Int

	data := pow.prepareData(pow.block.Nonce)
	hash := sha256.Sum256(data)
	hashInt.SetBytes(hash[:])

	isValid := hashInt.Cmp(pow.target) == -1

	return isValid
}

func (pow *ProofOfWork) Run() (int, []byte) {
	var hashInt big.Int
	var hash [32]byte
	nonce := 0
	maxNonce := math.MaxInt64

	fmt.Printf("Mining the block containing \"%s\"\n", pow.block.Data)
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
