//	go get github.com/go-sql-driver/mysql
package main
 
import (
	"time"
	"database/sql" 
	_ "github.com/go-sql-driver/mysql"
	"log"
)

func main() {
	db, err := sql.Open("mysql", "root:aS123123!@tcp(2.docker.ai.ourplan.kr:3306)/kem")
	if err != nil {
		log.Fatal(err.Error())
	}
	defer db.Close()

	err = db.Ping()
	if err != nil {
		log.Fatal(err.Error())
	}

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

	thing_rows, err := db.Query("SELECT * from thing")
	if err != nil {
		log.Fatal(err.Error())
	}
	defer thing_rows.Close()
	for thing_rows.Next() {
		var serialNumber string
		var gatewayMacAddr string
		var period int
		var state int

		err := thing_rows.Scan(&serialNumber, &gatewayMacAddr, &period, &state)
		if err != nil {
			log.Fatal(err.Error())
		}
		log.Println(serialNumber, gatewayMacAddr, period, state)
	}
	
	err = thing_rows.Err()
	if err != nil {
		log.Fatal(err)
	}

	// start  transaction
	tx, err := db.Begin()
	if err != nil {
		log.Fatal(err)
	}
	defer tx.Rollback()

	datetime := time.Now()
	datetime.Format(time.RFC3339)
	_, err = tx.Exec("INSERT INTO data VALUES (?, ?, ?)", datetime, "57383032333816210B", 398)
	if err != nil {
		log.Fatal(err)
	}

	//	transaction commit
	err = tx.Commit()
    if err != nil {
		log.Fatal(err)
    }
}
