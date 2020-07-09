package main

import (
	"crypto/sha256"
	"fmt"
	"log"
	"net"
	"os"

	"golang.org/x/crypto/ripemd160"
)

func main() {
	ip, mac := getIp(), getMacAddr()
	fmt.Println(ip, mac)
	hash := hashMacAddr(mac)
	fmt.Println(mac, hash)
}

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

func hashMacAddr(mac string) string {
	temp := sha256.Sum256([]byte(mac))
	ripemd160 := ripemd160.New()
	_, err := ripemd160.Write(temp[:])
	if err != nil {
		log.Panic(err)
	}
	macAddrHash := ripemd160.Sum(nil)

	return macAddrHash
}
