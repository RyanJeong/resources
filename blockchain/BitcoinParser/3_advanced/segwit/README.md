#   What is Segregated Witness?<br>
### Segregated Witness is a proposal to change the structure of bitcoin [transaction data](../../2_technical/4_transaction/README.md#transaction-data).<br>
The main reason for doing this is to fix the problem of transaction malleability (don’t worry, I’ll explain this in a moment).<br>
However, this change also allows for other benefits, such as increasing the number of transactions that can fit in to a block.<br>

##  What are the changes?<br>

##  Result:<br>

##  What are the benefits?<br>
1. [Fixes Transaction Malleability](#1-fixes-transaction-malleability)<br>
2. [Increases Block Capacity](#2-increases-block-capacity)<br>

### 1. Fixes Transaction Malleability<br>

### 2. Increases Block Capacity<br>

### Why is it being implemented in this way?<br>

##  When will this come in to effect?<br>
>   Segregated Witness will come in to effect when 95% of miners signal that they are ready for it.<br>

### Why are miners being given the decision on activation?<br>

### Is there a time limit on deployment?<br>

### How to parse a raw data included segwit
```Bash
bitcoin-cli getrawtransaction 80975cddebaa93aa21a6477c0d050685d6820fa1068a2731db0f39b535cbd369
```
Result in HEX: 010000000001045ecd4aef9b6e0d86310203ae0925f2562420c151bcd41b7464f05eed6c26a10862000000fc0047304402207e3e1158831eca394e472e43ec2a4c9f10d034a83f0f7142e6c38c243e6074f9022000b10a29bccf3c31f61e047a400d1a8d620cf8be7fb39ea5c51c6aeac83e7e6b0147304402205bc85c03a0f786bdf6a985911cf27d94d6f4c0f00295236a304967564cca492a022011e0d80900998f601290223240ed21d13dcce11b1d045383361978ac02e27c97014c69522102194e1b5671daff4edc82ce01589e7179a874f63d6e5157fa0def116acd2c3a522103a043861e123bc67ddcfcd887b167e7ff9d00702d1466524157cf3b28c7aca71b2102a49a62a9470a31ee51824f0ee859b0534a4f555c0e2d7a9d9915d6986bfc200453aeffffffff0b5c10d858331e2c60061b8c178c7a9cea7d668db500d16fceeed5b808854cad7d000000fc0047304402203299b925b1f2c87282d2889c2bb0e07372f916d7c4781f43f2e6d1403b2425fe0220466d075c56cdcf1d659dd40edcfc68298826f935beabe12f7404c7fd1e496c8601473044022048dfe509326808f9367c88da0f14968121d31b45461a11e6ed640e72f6a53a300220517914666f2f0f1d2c306de49599bf0a95f59cd57eaaef49e44a4e48a8d9e139014c69522103b5fd9803c0046386a9c7a1ac80c102ac4bc1e6dfaec036b0ca3adebe1ca961c92102b8b42d1c84d778c4fa78c609611a9cb847c3d7bff231e5751f380503c583d36321030d2c5aee1d650c2a3150e1c66a1f1e7236ecabdc12e55b0f545fff14667a515f53aeffffffffb229063113b096fe69c1c0bdb07c35655240ebf789bd25f202b58483796b06c8a3000000fdfd000047304402204d4da5303be178d649cfab85f4d6777c365934f015b773f2269e2cc4a819eaae02207f79285ddc34c6def51df243a5abc5f36179f407172bcae88feb04da1ab1b00001483045022100b831d970bc3ea88bc6b717bbd1ad8aca9bcc8e6545988ee9718db75891db2e1702200d6bf7c4b91abcc32a610cf52112e550ae853b2f216b88803b560f5adc0d9742014c69522102c44af6aea46b1b7a9373078437ecdf993b701efd2cc297414d8eab5063887dce2103546047f27105c7db32ebe5f3f8655856d2c27ecff80614b36da6e3cf84e88d8321022fa39834a8308abba605b1b2315b508a3268b5a43bc43d60c844f65db8fb78ad53aeffffffff1dd2d6d772ca3ea10e22347e19dd47f4019909ab471c8a8cdab29e0d2df00fd89a0000002322002044c55c1da36a576217259c3bc21b0c3943f7eb3ff4e3c381d9fd3502434b9e87ffffffff05c0d401000000000017a914a1932cfd432d928311b4ada550bbc468d1e909b787a08601000000000017a9146b0e7a66416f1d8598b5956576adb22daf79853e873a4a00000000000017a914ec4c73145428abbe0b1c40fbf58c59f0ef3c29f487382c05000000000017a914abb18a298e5b629bf5652f341d2cd8207ccc214a8780100200000000001976a91438d769cf2899983022b5611ab4d35bf7907dae2088ac000000040047304402202c3f94e5daf4057377d9f16d45b57e962de42fb42cb7e95a0382b7c66624980a02204098f6acd43b0391ea1b4a8102797e78895848fb7e883f98d207d14d45945a69014730440220448460edd5291a548c571ccf3a72caf47b02364035dc84f420d311e3a0c5494802205bb1cc89f20dc1e2c1f6eadb74898f8eecc46fbf488b676636b45fafaeb96e0f01695221021e6617e06bb90f621c3800e8c37ab081a445ae5527f6c5f68a022e7133f9b5fe2103bea1a8ce6369435bb74ff1584a136a7efeebfe4bc320b4d59113c92acd869f38210280631b27700baf7d472483fadfe1c4a7340a458f28bf6bae5d3234312d684c6553ae00000000

* 1. Version:       01000000<br>

* 2. Dummpy + Flag: 0001<br>

* 3. Input count:   04<br>
    * [1]TXID:            5ecd4aef ... 6c26a108<br>
    * [1]VOUT:            62000000<br>
    * [1]SCRIPTSIGSIZE:   fc<br>
    * [1]SCRIPTSIG:       00473044 ... 200453ae<br>
    * [1]SEQUENCE:        ffffffff<br>
    
    * [2]TXID:            0b5c10d8 ... 08854cad<br>
    * [2]VOUT:            7d000000<br>
    * [2]SCRIPTSIGSIZE:   fc<br>
    * [2]SCRIPTSIG:       00473044 ... 515f53ae<br>
    * [2]SEQUENCE:        ffffffff<br>
    
    * [3]TXID:            b2290631 ... 796b06c8<br>
    * [3]VOUT:            a3000000<br>
    * [3]SCRIPTSIGSIZE:   fdfd00<br>
    * [3]SCRIPTSIG:       00473044 ... 78ad53ae<br>
    * [3]SEQUENCE:        ffffffff<br>
    
    * [4]TXID:            1dd2d6d7 ... 2df00fd8<br>
    * [4]VOUT:            9a000000<br>
    * [4]SCRIPTSIGSIZE:   23<br>
    * [4]SCRIPTSIG:       22002044 ... 434b9e87<br>
    * [4]SEQUENCE:        ffffffff<br>

* 4. Output count:  05<br>
    * [1]VALUE:               c0d4010000000000<br>
    * [1]SCRIPTPUBKEYSIZE:    17<br>
    * [1]SCRIPTPUBKEY:        a914a1932cfd432d928311b4ada550bbc468d1e909b787<br>
    
    * [2]VALUE:               a086010000000000<br>
    * [2]SCRIPTPUBKEYSIZE:    17<br>
    * [2]SCRIPTPUBKEY:        a9146b0e7a66416f1d8598b5956576adb22daf79853e87<br>
    
    * [3]VALUE:               3a4a000000000000<br>
    * [3]SCRIPTPUBKEYSIZE:    17<br>
    * [3]SCRIPTPUBKEY:        a914ec4c73145428abbe0b1c40fbf58c59f0ef3c29f487<br>
    
    * [4]VALUE:               382c050000000000<br>
    * [4]SCRIPTPUBKEYSIZE:    17<br>
    * [4]SCRIPTPUBKEY:        a914abb18a298e5b629bf5652f341d2cd8207ccc214a87<br>
    
    * [5]VALUE:               8010020000000000<br>
    * [5]SCRIPTPUBKEYSIZE:    17<br>
    * [5]SCRIPTPUBKEY:        76a91438d769cf2899983022b5611ab4d35bf7907dae2088ac<br>

* 5. Segwit<br>
    * [TX01] Segwit: 00<br>
    
    * [TX02] Segwit: 00<br>
    
    * [TX03] Segwit: 00<br>
    
    * [TX04] Segwit: 04<br>
        * [TX04][DATA01] SIZE: 00<br>
        
        * [TX04][DATA02] SIZE: 47<br>
        * [TX04][DATA02] DATA: 30440220 ... 945a6901<br>
        
        * [TX04][DATA03] SIZE: 47<br>
        * [TX04][DATA03] DATA: 30440220 ... b96e0f01<br>
        
        * [TX04][DATA04] SIZE: 69<br>
        * [TX04][DATA04] DATA: 5221021e ... 4c6553ae<br>

* 6. Locktime: 00000000<br>

##  Resources<br>
*   [BIP 141 (Consensus Layer)](https://github.com/bitcoin/bips/blob/master/bip-0141.mediawiki)<br>
*   [BIP 144 (Peer Services)](https://github.com/bitcoin/bips/blob/master/bip-0144.mediawiki)<br>

### Further Reading<br>
*   [Bitcoin.org - Segregated Witness Benefits](https://bitcoincore.org/en/2016/01/26/segwit-benefits/)<br>
*   [Segregated Witness Explained [Video]](https://www.youtube.com/watch?v=DzBAG2Jp4bg) - A good video explanation of SegWit, with helpful visualisations of the change in transaction data structure.<br>
