** Usage:
```Text
cd $GOPATH/src/github.com/ryanjeong/go_blockchain/msol
go get ./
go install ./

export NODE_ID=3000

msol createwallet
> Your new address: CENTRAL

msol createblockchain -address CENTRAL
> 0000fa3c18f6851bb5a6ab8a5ad1f62dff5d977c25dbc9646a35d55b53573e2a

> Done!

cp blockchain_3000.db genesis.db

export NODE_ID=3001

msol createwallet
> Your new address: WALLET1
msol createwallet
> Your new address: WALLET2
msol createwallet
> Your new address: WALLET3

export NODE_ID=3000

msol send -from CENTRAL -to WALLET1 -amount 10 -mine
> 000059865917c8d4d68bf30cc96d02b7faa5af04df3ac6e206df30178486cf4b

> Success!
msol send -from CENTRAL -to WALLET1 -amount 10 -mine
> 0000acbebf56855eeb3c2622851da4750cd7729a13b6143d8760b01877d09e40

> Success!

# this process must continue to run until the end of the 'scenario'
msol startnode
> Starting node 3000

# open a new terminal
export NODE_ID=3001

cp genesis.db blockchain_3001.db

msol startnode
> Starting node 3001

# in a terminal with NODE_ID=3000;
> Received version command
> Received getblocks command
> Received getdata command
> Received getdata command
> Received getdata command

# in a terminal with NODE_ID=3001;
> Received version command
> Received inv command
> Recevied inventory with 3 block
> Received block command
> Recevied a new block!
> Added block 0000d6c5c5ff75fdea2567ed1a71b47b9ac55cc22fae1eb00039b8ad585ef115
> Received block command
> Recevied a new block!
> Added block 0000a8eb17f90029807a91eadd09dc63fad8e98f35912ddd15fa004572bb4613
> Received block command
> Recevied a new block!
> Added block 000081dd40725b2495f08893f5fa4c57354742bebbe8f014b8982b15cb9b262c

# shutdown the process with NODE_ID=3001 and;
msol getbalance -address WALLET1
> Balance of 'WALLET1': 10
msol getbalance -address WALLET2
> Balance of 'WALLET2': 10
msol getbalance -address CENTRAL
> Balance of 'CENTRAL': 10

# open a new terminal
export NODE_ID=3002

cp genesis.db blockchain_3002.db

# this process must continue to run too until the end of the 'scenario'
msol startnode -miner WALLET3
> Starting node 3002

# in a terminal with NODE_ID=3000;
> ...
> Received version command
> Received getblocks command
> Received getdata command
> Received getdata command
> Received getdata command

# in a terminal with NODE_ID=3002;
> Mining is on. Address to receive rewards:  WALLET3
> Received version command
> Received inv command
> Recevied inventory with 3 block
> Received block command
> Recevied a new block!
> Added block 0000d6c5c5ff75fdea2567ed1a71b47b9ac55cc22fae1eb00039b8ad585ef115
> Received block command
> Recevied a new block!
> Added block 0000a8eb17f90029807a91eadd09dc63fad8e98f35912ddd15fa004572bb4613
> Received block command
> Recevied a new block!
> Added block 000081dd40725b2495f08893f5fa4c57354742bebbe8f014b8982b15cb9b262c

# open the terminal with NODE_ID=3001;
msol send -from WALLET1 -to WALLET2 -amount 1
> Success!

# in a terminal with NODE_ID=3002;
> ...
> Received inv command
> Recevied inventory with 1 tx
> Received tx command

msol send -from WALLET1 -to WALLET3 -amount 1
> Success!

# in a terminal with NODE_ID=3002;
> ...
> Received inv command
> Recevied inventory with 1 tx

# for download current blocks ...
msol startnode

# shutdown the process with NODE_ID=3001 and;

# TODO:

## Miner1
1NeP3MYBaKPWgtLVFBCW77NyqiQDjBh6Lb

## Miner2
13vEZe4qepD3q6DtuSaDA5RBPQkYToZgQT

## Addresses
1C64BDr2Zxde7Wzh7piPDp4mktLLR31EUu
1NNNANQFp1c1i1ALkZ7DUEVs5K8rFyC1P1
1H9eWy46ZficUtmArPmB1dGQTTLsXN9Fp4

```


