package wallet

import "crypto/ecdsa"

const version = byte(0x00)
const addressChecksumLen = 4
const walletFile = "wallet_%s.dat"

// Wallet stores private and public keys
type Wallet struct {
	PrivateKey ecdsa.PrivateKey
	PublicKey  []byte
}

// Wallets stores a collection of wallets
type Wallets struct {
	Wallets map[string]*Wallet
}
