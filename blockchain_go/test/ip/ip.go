package main

import (
	"fmt"
	"net"
	"os"
)

func main() {
	fmt.Println(getIp())
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
