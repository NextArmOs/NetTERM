package main

import (
	"fmt"
	"net"
	"os"
	"strconv"
	"time"
)

func main() {
	fmt.Println("=========================================")
	fmt.Println("       Go Network Port Scanner 🌐        ")
	fmt.Println("=========================================")

	host := "127.0.0.1"
	ports := []int{22, 80, 443, 8080, 3000}

	fmt.Printf("Scanning target host: %s\n\n", host)

	for _, port := range ports {
		address := host + ":" + strconv.Itoa(port)
		conn, err := net.DialTimeout("tcp", address, 500*time.Millisecond)

		if err != nil {
			fmt.Printf("❌ Port %d : CLOSED\n", port)
			continue
		}
		conn.Close()
		fmt.Printf("🔥 Port %d : OPEN! (Service detected)\n", port)
	}
	fmt.Println("=========================================")
	os.Exit(0)
}
