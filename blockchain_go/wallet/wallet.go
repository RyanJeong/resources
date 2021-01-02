package wallet

import (
	"bytes"
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/sha256"
	"encoding/gob"
	"fmt"
	"io/ioutil"
	"log"
	"os"

	"github.com/ryanjeong/go_blockchain/base58"
	"golang.org/x/crypto/ripemd160"
)

const version = byte(0x00)
const checksumLen = 4
const walletFile = "WLT/wallet_%s.dat"

// store private and public keys
type Wallet struct {
	PrivateKey ecdsa.PrivateKey
	PublicKey  []byte
}

// store a collection of wallets
type Wallets struct {
	Wallets map[string]*Wallet
}

// create a private key and generate a public key by the private key
// A private key is simply a number, picked at random.
// The public key is calculated from the private key using elliptic curve multiplication,
// which is irreversible: K = k * G
// where k is the private key, G is a constant point called the generator point and K is the resulting public key.
// K = 1E99423A4ED27608A15A2616A2B0E9E52CED330AC530EDCC32C8FFC6A526AEDD * G
//
// Public Key K is defined as a point K = (x,y)
// K = (x, y)
//
// where,
//
// x = F028892BAD7ED57D2FB57BF33081D5CFCF6F9ED3D3D7F159C2E2FFF579DC341A
// y = 07CF33DA18BD734C600B96A72BBC4749D5141C90EC8AC328AE52DDFE2E505BDB
func newKeyPair() (ecdsa.PrivateKey, []byte) {
	curve := elliptic.P256()
	private, err := ecdsa.GenerateKey(curve, rand.Reader)
	if err != nil {
		log.Panic(err)
	}
	pubKey := append(private.PublicKey.X.Bytes(), private.PublicKey.Y.Bytes()...)

	return *private, pubKey
}

// create and return a wallet
func NewWallet() *Wallet {
	privKey, pubKey := newKeyPair()
	w := Wallet{privKey, pubKey}

	return &w
}

// add a wallet to wallet collection
func (ws *Wallets) CreateWallet() string {
	w := NewWallet()
	address := fmt.Sprintf("%s", w.GetAddress())
	ws.Wallets[address] = w

	return address
}

// create wallets and fill it from a file if it exists
func NewWallets(nodeID string) (*Wallets, error) {
	ws := Wallets{}
	ws.Wallets = make(map[string]*Wallet)
	err := ws.LoadWallet(nodeID)

	return &ws, err
}

// return wallet address
// version(1 byte) + payload(20 bytes) + checksum(4 bytes)
func (w Wallet) GetAddress() []byte {
	payload := HashPubKey(w.PublicKey)
	vPayload := append([]byte{version}, payload...)
	checksum := checksum(vPayload)
	vPayloadC := append(vPayload, checksum...)

	// Most Bitcoin addresses are 34 characters.
	// Some Bitcoin addresses can be shorter than 34 characters (as few as 26) and still be valid. A significant percentage of Bitcoin addresses are only 33 characters, and some addresses may be even shorter.
	address := base58.Base58Encode(vPayloadC)

	return address
}

// return an array of addresses stored in the wallet file
func (ws *Wallets) GetAddresses() []string {
	var addresses []string

	for address := range ws.Wallets {
		addresses = append(addresses, address)
	}

	return addresses
}

// hash public key
// SHA256->RIPEMD-160
// return 20 bytes(160 bits)
func HashPubKey(pubKey []byte) []byte {
	temp := sha256.Sum256(pubKey)
	ripemd160 := ripemd160.New()
	_, err := ripemd160.Write(temp[:])
	if err != nil {
		log.Panic(err)
	}
	pubKeyHash := ripemd160.Sum(nil)

	return pubKeyHash
}

// check if address is valid
// Version(1 byte) + Payload(20 bytes) + Checksum(4 bytes)
func ValidateAddress(address string) bool {
	pubKeyHash := base58.Base58Decode([]byte(address))
	actualChecksum := pubKeyHash[len(pubKeyHash)-checksumLen:]

	version := pubKeyHash[0]
	payload := pubKeyHash[1 : len(pubKeyHash)-checksumLen]
	targetChecksum := checksum(append([]byte{version}, payload...))

	return bytes.Compare(actualChecksum, targetChecksum) == 0
}

// generate a checksum
// SHA256 -> SHA256 -> get first 4 bytes
func checksum(payload []byte) []byte {
	first := sha256.Sum256(payload)
	second := sha256.Sum256(first[:])

	return second[:checksumLen]
}

// return a wallet by its address
func (ws Wallets) GetWallet(address string) Wallet {

	return *ws.Wallets[address]
}

// load wallets from the file
func (ws *Wallets) LoadWallet(nodeID string) error {
	var wallets Wallets

	file := fmt.Sprintf(walletFile, nodeID)
	if _, err := os.Stat(file); os.IsNotExist(err) {
		return err
	}
	content, err := ioutil.ReadFile(file)
	if err != nil {
		log.Panic(err)
	}
	gob.Register(elliptic.P256())
	decoder := gob.NewDecoder(bytes.NewReader(content))
	err = decoder.Decode(&wallets)
	if err != nil {
		log.Panic(err)
	}
	ws.Wallets = wallets.Wallets

	return nil
}

// save wallets to a file
func (ws Wallets) SaveWallet(nodeID string) {
	var content bytes.Buffer

	file := fmt.Sprintf(walletFile, nodeID)
	gob.Register(elliptic.P256())
	encoder := gob.NewEncoder(&content)
	err := encoder.Encode(ws)
	if err != nil {
		log.Panic(err)
	}
	err = ioutil.WriteFile(file, content.Bytes(), 0644)
	if err != nil {
		log.Panic(err)
	}

	return
}
