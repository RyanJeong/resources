package server

import (
	"bytes"
	"encoding/gob"
	"encoding/hex"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net"
	"os"

	"github.com/ryanjeong/go_blockchain/block"
)

const protocol = "tcp"
const nodeVersion = 1
const commandLength = 12

const ip = "192.168.0.26"
const port = ":7654"

var nodeAddress string
var miningAddress string

//var KnownNodes = []string{"localhost:3000"}
var KnownNodes = []string{ip + port}
var blocksInTransit = [][]byte{}
var mempool = make(map[string]block.Transaction)

type addrMsg struct {
	AddrList []string
}

type blockMsg struct {
	AddrFrom string
	Blk      []byte
}

type getBlocksMsg struct {
	AddrFrom string
}

type getDataMsg struct {
	AddrFrom string
	Type     string
	ID       []byte
}

type invMsg struct {
	AddrFrom string
	Type     string
	Items    [][]byte
}

type txMsg struct {
	AddFrom string
	Tx      []byte
}

type versionMsg struct {
	Version    int
	BestHeight int
	AddrFrom   string
}

// start a node
func StartServer(nodeID, minerAddress string) {
	nodeAddress = getIp() + port
	//nodeAddress = fmt.Sprintf("localhost:%s", nodeID)

	miningAddress = minerAddress
	ln, err := net.Listen(protocol, nodeAddress)
	//ln, err := net.Listen(protocol, port)
	if err != nil {
		log.Panic(err)
	}
	defer ln.Close()

	//bc := block.NewBlockchain(nodeID)
	bc := block.NewBlockchain()

	if nodeAddress != KnownNodes[0] {
		sendVersion(KnownNodes[0], bc)
	}

	for {
		conn, err := ln.Accept()
		if err != nil {
			log.Panic(err)
		}
		go handleConnection(conn, bc)
	}
}

func handleConnection(conn net.Conn, bc *block.Blockchain) {
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

func handleAddr(request []byte) {
	var buf bytes.Buffer
	var payload addrMsg

	buf.Write(request[commandLength:])
	dec := gob.NewDecoder(&buf)
	err := dec.Decode(&payload)
	if err != nil {
		log.Panic(err)
	}

	KnownNodes = append(KnownNodes, payload.AddrList...)
	fmt.Printf("There are %d known nodes now!\n", len(KnownNodes))
	requestBlocks()
}

func handleBlock(request []byte, bc *block.Blockchain) {
	var buf bytes.Buffer
	var payload blockMsg

	buf.Write(request[commandLength:])
	dec := gob.NewDecoder(&buf)
	err := dec.Decode(&payload)
	if err != nil {
		log.Panic(err)
	}

	blockData := payload.Blk
	blk := block.DeserializeBlock(blockData)

	fmt.Println("Recevied a new block!")
	bc.AddBlock(blk)

	fmt.Printf("Added block %x\n", blk.Hash)

	if len(blocksInTransit) > 0 {
		blockHash := blocksInTransit[0]
		sendGetData(payload.AddrFrom, "block", blockHash)

		blocksInTransit = blocksInTransit[1:]
	} else {
		UTXOSet := block.UTXOSet{bc}
		UTXOSet.Reindex()
	}
}

func handleInv(request []byte, bc *block.Blockchain) {
	var buf bytes.Buffer
	var payload invMsg

	buf.Write(request[commandLength:])
	dec := gob.NewDecoder(&buf)
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

func handleGetBlocks(request []byte, bc *block.Blockchain) {
	var buf bytes.Buffer
	var payload getBlocksMsg

	buf.Write(request[commandLength:])
	dec := gob.NewDecoder(&buf)
	err := dec.Decode(&payload)
	if err != nil {
		log.Panic(err)
	}

	blocks := bc.GetBlockHashes()
	sendInv(payload.AddrFrom, "block", blocks)
}

func handleGetData(request []byte, bc *block.Blockchain) {
	var buf bytes.Buffer
	var payload getDataMsg

	buf.Write(request[commandLength:])
	dec := gob.NewDecoder(&buf)
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

		SendTx(payload.AddrFrom, &tx)
		// delete(mempool, txID)
	}
}

func handleTx(request []byte, bc *block.Blockchain) {
	var buf bytes.Buffer
	var payload txMsg

	buf.Write(request[commandLength:])
	dec := gob.NewDecoder(&buf)
	err := dec.Decode(&payload)
	if err != nil {
		log.Panic(err)
	}

	txData := payload.Tx
	tx := block.DeserializeTransaction(txData)
	mempool[hex.EncodeToString(tx.ID)] = tx

	if nodeAddress == KnownNodes[0] {
		for _, node := range KnownNodes {
			if node != nodeAddress && node != payload.AddFrom {
				sendInv(node, "tx", [][]byte{tx.ID})
			}
		}
	} else {
		if len(mempool) >= 2 && len(miningAddress) > 0 {
		MineTransactions:
			var txs []*block.Transaction

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

			cbTx := block.NewCoinbaseTX(miningAddress, "")
			txs = append(txs, cbTx)

			newBlock := bc.MineBlock(txs)
			UTXOSet := block.UTXOSet{bc}
			UTXOSet.Reindex()

			fmt.Println("New block is mined!")

			for _, tx := range txs {
				txID := hex.EncodeToString(tx.ID)
				delete(mempool, txID)
			}

			for _, node := range KnownNodes {
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

func handleVersion(request []byte, bc *block.Blockchain) {
	var buf bytes.Buffer
	var payload versionMsg

	buf.Write(request[commandLength:])
	dec := gob.NewDecoder(&buf)
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
		KnownNodes = append(KnownNodes, payload.AddrFrom)
	}
}

func sendAddr(address string) {
	nodes := addrMsg{KnownNodes}
	nodes.AddrList = append(nodes.AddrList, nodeAddress)
	payload := gobEncode(nodes)
	request := append(commandToBytes("addr"), payload...)

	sendData(address, request)
}

func sendBlock(addr string, b *block.Block) {
	data := blockMsg{nodeAddress, b.Serialize()}
	payload := gobEncode(data)
	request := append(commandToBytes("block"), payload...)

	sendData(addr, request)
}

func sendData(addr string, data []byte) {
	conn, err := net.Dial(protocol, addr)
	if err != nil {
		fmt.Printf("%s is not available\n", addr)
		var updatedNodes []string

		for _, node := range KnownNodes {
			if node != addr {
				updatedNodes = append(updatedNodes, node)
			}
		}

		KnownNodes = updatedNodes

		return
	}
	defer conn.Close()

	_, err = io.Copy(conn, bytes.NewReader(data))
	if err != nil {
		log.Panic(err)
	}
}

func sendInv(address, kind string, items [][]byte) {
	inventory := invMsg{nodeAddress, kind, items}
	payload := gobEncode(inventory)
	request := append(commandToBytes("inv"), payload...)

	sendData(address, request)
}

func sendGetBlocks(address string) {
	payload := gobEncode(getBlocksMsg{nodeAddress})
	request := append(commandToBytes("getblocks"), payload...)

	sendData(address, request)
}

func sendGetData(address, kind string, id []byte) {
	payload := gobEncode(getDataMsg{nodeAddress, kind, id})
	request := append(commandToBytes("getdata"), payload...)

	sendData(address, request)
}

func SendTx(addr string, tnx *block.Transaction) {
	data := txMsg{nodeAddress, tnx.Serialize()}
	payload := gobEncode(data)
	request := append(commandToBytes("tx"), payload...)

	sendData(addr, request)
}

func sendVersion(addr string, bc *block.Blockchain) {
	bestHeight := bc.GetBestHeight()
	payload := gobEncode(versionMsg{nodeVersion, bestHeight, nodeAddress})

	request := append(commandToBytes("version"), payload...)

	sendData(addr, request)
}

func gobEncode(data interface{}) []byte {
	var buf bytes.Buffer

	enc := gob.NewEncoder(&buf)
	err := enc.Encode(data)
	if err != nil {
		log.Panic(err)
	}

	return buf.Bytes()
}

// check a node is known
func nodeIsKnown(addr string) bool {
	for _, node := range KnownNodes {
		if node == addr {
			return true
		}
	}

	return false
}

// convert command consisting of a string to bytes
func commandToBytes(command string) []byte {
	var b [commandLength]byte

	for i, c := range command {
		b[i] = byte(c)
	}

	return b[:]
}

// convert command consisting of bytes to a string
func bytesToCommand(bytes []byte) string {
	var str []byte

	for _, b := range bytes {
		if b != 0x0 {
			str = append(str, b)
		}
	}

	return fmt.Sprintf("%s", str)
}

// extract request from a command
func extractCommand(request []byte) []byte {

	return request[:commandLength]
}

// request to get blocks on the node(KnownNodes)
func requestBlocks() {
	for _, node := range KnownNodes {
		sendGetBlocks(node)
	}
}

// get IP address on this PC
func getIp() string {
	var ip string

	addrs, err := net.InterfaceAddrs()
	if err != nil {
		os.Stderr.WriteString("Oops: " + err.Error() + "\n")
		os.Exit(1)
	}
	for _, a := range addrs {
		// escape loopback;
		if ipnet, ok := a.(*net.IPNet); ok && !ipnet.IP.IsLoopback() {
			if ipnet.IP.To4() != nil {
				// fmt.Println(ipnet.IP.String())
				ip = ipnet.IP.String()
				break
			}
		}
	}

	return ip
}
