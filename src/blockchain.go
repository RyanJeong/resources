package main

type Blockchain struct {
	tip []byte
	db  *bolt.DB
}

func NewBlockchain() *Blockchain {
	var tip []byte
	db, err := bolt.Open(dbFile, 0600, nil)

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
			b, err := tx.CreateBucket([]byte(blocksBucket))
			err = b.Put(genesis.Hash, genesis.Serialize())
			err = b.Put([]byte("l"), genesis.Hash)
			tip = genesis.Hash
		} else {
			tip = b.Get([]byte("l"))
		}

		return nil
	})

	bc := Blockchain{tip, db}

	return &bc
}
