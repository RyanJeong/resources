//	go get github.com/go-sql-driver/mysql
package main
 
import (
	"database/sql" 
    "encoding/hex"
    "fmt"
	"time"
	_ "github.com/go-sql-driver/mysql"
	"log"
	"io"
	"net"
    "strings"
)

type status byte

const (
    PERIOD                      int = 365

    CONNECTED                   status = 0x00
    DISCONNECTED                status = 0x01
    TEST_RECEIVED               status = 0x02
    TEST_PASSED                 status = 0x03
    FIRE_DETECTED               status = 0x04

    ADD_A_GATEWAY               status = 0x00
    ADD_A_THING                 status = 0x10
    SEND_A_DATA                 status = 0x20
    /*  TODO: NEED TO ADD PACKET CMDs   */
    TEST_RESULT                 status = 0x30
    HEAT_TEST_RESULT1           status = 0x32
    HEAT_TEST_RESULT2           status = 0x33
    REQUESTED_TEST_RESULT       status = 0x40
    REQUESTED_HEAT_TEST_RESULT1 status = 0x42
    REQUESTED_HEAT_TEST_RESULT2 status = 0x43
    INIT_TEST_RESULT            status = 0x50
    INIT_HEAT_TEST_RESULT1      status = 0x52
    INIT_HEAT_TEST_RESULT2      status = 0x53
    ALERT_A_FIRE_DETECTED       status = 0x60
)

func main() {
	l, err := net.Listen("tcp", ":5000")
	if err != nil {
		log.Fatalf("Failed to bind address to 5000; err: %v", err)
	}
	defer l.Close()
	
	for {
		conn, err := l.Accept()
		if err != nil {
			log.Printf("Failed to accept; err: %v", err)
			continue
		}
		go ConnHandler(conn)
	}
}

func ConnHandler(conn net.Conn) {
    var tempTime time.Time

	recvBuf := make([]byte, 4096)
	
	db, err := sql.Open("mysql", "root:aS123123!@tcp(2.docker.ai.ourplan.kr:3306)/kem")
	if err != nil {
		log.Fatal(err.Error())
	}
	defer db.Close()

	err = db.Ping()
	if err != nil {
		log.Fatal(err.Error())
	}

	for {
		n, err := conn.Read(recvBuf)
		if err != nil {
			if io.EOF == err {
				log.Printf("Connection is closed from client; msg: %v", conn.RemoteAddr().String())

				return
			}
			log.Printf("Failed to receive data; err: %v", err)
			
			return	
		}
		if n > 0 {
			data := recvBuf[:n]
			log.Println("Received")
            var length = len(data)
            for i := 0; i < length; i++ {
                fmt.Print(data[i], " ")
            }
            fmt.Println("")

			// start  transaction
			tx, err := db.Begin()
			if err != nil {
				log.Fatal(err)
			}
			defer tx.Rollback()

			datetime := time.Now()
			datetime.Format(time.RFC3339)

            switch status(data[7]) {
            /*  Change a string to prepared statement after all tests done  */
            case ADD_A_GATEWAY:
                MAC := strings.ToUpper(hex.EncodeToString(data[1:7]))
                log.Println("ADD_A_GATEWAY")
                log.Println(MAC)
                _, err = tx.Exec("INSERT INTO gateway VALUES (?, ?) ON DUPLICATE KEY UPDATE macAddr=?, state=?", MAC, CONNECTED, MAC, CONNECTED)
                if err != nil {
                    log.Fatal(err)
                }
            case ADD_A_THING:
                MAC := strings.ToUpper(hex.EncodeToString(data[1:7]))
                ID  := strings.ToUpper(hex.EncodeToString(data[8:17]))
                log.Println("ADD_A_THING")
                log.Println(MAC)
                log.Println(ID)
                _, err = tx.Exec("INSERT INTO thing VALUES (?, ?, ?, ?) ON DUPLICATE KEY UPDATE serialNumber=?, gatewayMacAddr=?, period=?, state=?",
                                    ID, MAC, PERIOD, CONNECTED, ID, MAC, PERIOD, CONNECTED)
                /*  for write db requested test time    */
                tempTime = datetime
                if err != nil {
                    log.Fatal(err)
                }
            case SEND_A_DATA:
                ID  := strings.ToUpper(hex.EncodeToString(data[8:17]))
                VALUE   := int(data[17])
                log.Println("SEND_A_DATA")
                log.Println(ID)
                log.Println(VALUE)
                _, err = tx.Exec("INSERT INTO data VALUES (?, ?, ?)", datetime, ID, VALUE)
                if err != nil {
                    log.Fatal(err)
                }
            case TEST_RESULT:
                log.Println("TEST_RESULT")
            case HEAT_TEST_RESULT1:
                log.Println("HEAT_TEST_RESULT1")
            case HEAT_TEST_RESULT2:
                log.Println("HEAT_TEST_RESULT2")
            case REQUESTED_TEST_RESULT:
                log.Println("REQUESTED_TEST_RESULT")
            case REQUESTED_HEAT_TEST_RESULT1:
                log.Println("REQUESTED_HEAT_TEST_RESULT1")
            case REQUESTED_HEAT_TEST_RESULT2:
                log.Println("REQUESTED_HEAT_TEST_RESULT2")
            case INIT_TEST_RESULT:
                ID      := strings.ToUpper(hex.EncodeToString(data[8:17]))
                VALUE1  := int(data[17])
                VALUE2  := int(data[18])
                log.Println("INIT_TEST_RESULT")
                log.Println(ID)
                log.Println(VALUE1)
                log.Println(VALUE2)
                _, err = tx.Exec("INSERT INTO test VALUES (?, ?, ?, ?, ?, ?)", tempTime, ID, datetime, 0x00, VALUE1, VALUE2)
                if err != nil {
                    log.Fatal(err)
                }
            case INIT_HEAT_TEST_RESULT1:
                ID      := strings.ToUpper(hex.EncodeToString(data[8:17]))
                VALUE1  := int(data[17])
                VALUE2  := int(data[18])
                log.Println("INIT_HEAT_TEST_RESULT1")
                log.Println(ID)
                log.Println(VALUE1)
                log.Println(VALUE2)
                _, err = tx.Exec("INSERT INTO test VALUES (?, ?, ?, ?, ?, ?)", tempTime, ID, datetime, 0x01, VALUE1, VALUE2)
                if err != nil {
                    log.Fatal(err)
                }
            case INIT_HEAT_TEST_RESULT2:
                ID      := strings.ToUpper(hex.EncodeToString(data[8:17]))
                VALUE1  := int(data[17])
                VALUE2  := int(data[18])
                log.Println("INIT_HEAT_TEST_RESULT2")
                log.Println(ID)
                log.Println(VALUE1)
                log.Println(VALUE2)
                _, err = tx.Exec("INSERT INTO test VALUES (?, ?, ?, ?, ?, ?)", tempTime, ID, datetime, 0x02, VALUE1, VALUE2)
                if err != nil {
                    log.Fatal(err)
                }
            case ALERT_A_FIRE_DETECTED:
                log.Println("ALERT_A_FIRE_DETECTED")
            default:
                log.Println("OP error")
            }


			//	transaction commit
			err = tx.Commit()
			if err != nil {
				log.Fatal(err)
			}
		}
	}
}
/*

	gateway_rows, err := db.Query("SELECT * from gateway")
	if err != nil {
		log.Fatal(err.Error())
	}
	defer gateway_rows.Close()

	for gateway_rows.Next() {
		var macAddr string
		var nThings int

		err := gateway_rows.Scan(&macAddr, &nThings)
		if err != nil {
			log.Fatal(err.Error())
		}
		log.Println(macAddr, nThings)
	}

	err = gateway_rows.Err()
	if err != nil {
		log.Fatal(err)
	}


----

			_, err = tx.Exec("INSERT INTO data VALUES (?, ?, ?)", datetime, "57383032333816210B", 398)
			if err != nil {
				log.Fatal(err)
			}

*/
