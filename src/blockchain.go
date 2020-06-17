package main

import (
	"fmt"

	"github.com/boltdb/bolt"
)

type Blockchain struct {
	tip []byte
	db  *bolt.DB
}

func NewBlockchain() *Blockchain {
	var tip []byte

	/* This is a standard way of opening a BoltDB file.
	   Notice that it won’t return an error if there’s no such file. */
	if db, err := bolt.Open(dbFile, 0600, nil); err != nil {
		return fmt.Errorf(err)
	}

	/*
		- read-write
		err := db.Update(func(tx *bolt.Tx) error {
			...
			return nil
		})

		- read-only
		err := db.View(func(tx *bolt.Tx) error {
			...
			return nil
		})

		- Update
		err := db.Batch(func(tx *bolt.Tx) error {
			...
			return nil
		})
	*/
	err = db.Update(func(tx *bolt.Tx) error {
		b := tx.Bucket([]byte(blocksBucket))

		if b == nil {
			genesis := NewGenesisBlock()
			if b, err := tx.CreateBucket([]byte(blocksBucket)); err != nil {
				log.Fatal(err)
			}
			if err = b.Put(genesis.Hash, genesis.Serialize()); err != nil {
				log.Fatal(err)
			}
			if err = b.Put([]byte("l"), genesis.Hash); err != nil {
				log.Fatal(err)
			}
			tip = genesis.Hash
		} else {
			tip = b.Get([]byte("l"))
		}

		return nil
	})
	if err != nil {
		return fmt.Errorf(err)
	}

	bc := Blockchain{tip, db}

	return &bc
}
