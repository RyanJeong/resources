package cli

import (
	"crypto/sha256"
	"fmt"
	"log"
	"net"
	"os"

	"github.com/ryanjeong/go_blockchain/base58"
	"golang.org/x/crypto/ripemd160"
)

// Get the MAC address on the PC
func getMacAddr() string {
	var mac string

	in, err := net.Interfaces()
	if err != nil {
		os.Stderr.WriteString("Oops: " + err.Error() + "\n")
		os.Exit(1)
	}
	for _, i := range in {
		m := i.HardwareAddr.String()
		if m != "" {
			mac = m
			break
		}
	}

	return mac
}

// Generate the unique string using the MAC address
// TODO:Improve code to use more complex methods
func genId(mac string) string {
	b := []byte(mac)
	temp := sha256.Sum256(b)
	ripemd160 := ripemd160.New()
	_, err := ripemd160.Write(temp[:])
	if err != nil {
		log.Panic(err)
	}
	macAddrHash := ripemd160.Sum(nil)
	enc := base58.Base58Encode(macAddrHash)
	str := fmt.Sprintf("%s", enc)

	return str
}
