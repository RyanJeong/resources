"use strict";

var search_data = [


    {
        label: "51 percent attack",
        uri: "/en/glossary/51-percent-attack",
        category: "Glossary"
    },
    {
        label: "Address",
        uri: "/en/glossary/address",
        category: "Glossary"
    },
    {
        label: "Ancestor mining",
        uri: "/en/glossary/cpfp",
        category: "Glossary"
    },
    {
        label: "Archival node",
        uri: "/en/glossary/node",
        category: "Glossary"
    },
    {
        label: "Bare multisig",
        uri: "/en/glossary/multisig",
        category: "Glossary"
    },
    {
        label: "Base58check",
        uri: "/en/glossary/base58check",
        category: "Glossary"
    },
    {
        label: "Best block chain",
        uri: "/en/glossary/block-chain",
        category: "Glossary"
    },
    {
        label: "Best header chain",
        uri: "/en/glossary/header-chain",
        category: "Glossary"
    },
    {
        label: "Bitcoins",
        uri: "/en/glossary/denominations",
        category: "Glossary"
    },
    {
        label: "Block",
        uri: "/en/glossary/block",
        category: "Glossary"
    },
    {
        label: "Block 0",
        uri: "/en/glossary/genesis-block",
        category: "Glossary"
    },
    {
        label: "Block chain",
        uri: "/en/glossary/block-chain",
        category: "Glossary"
    },
    {
        label: "Block header",
        uri: "/en/glossary/block-header",
        category: "Glossary"
    },
    {
        label: "Block height",
        uri: "/en/glossary/block-height",
        category: "Glossary"
    },
    {
        label: "Block reward",
        uri: "/en/glossary/block-reward",
        category: "Glossary"
    },
    {
        label: "Blocks-first sync",
        uri: "/en/glossary/blocks-first-sync",
        category: "Glossary"
    },
    {
        label: "Bloom filter",
        uri: "/en/glossary/bloom-filter",
        category: "Glossary"
    },
    {
        label: "Chain code",
        uri: "/en/glossary/chain-code",
        category: "Glossary"
    },
    {
        label: "Change address",
        uri: "/en/glossary/change-address",
        category: "Glossary"
    },
    {
        label: "Change output",
        uri: "/en/glossary/change-address",
        category: "Glossary"
    },
    {
        label: "Child key",
        uri: "/en/glossary/child-key",
        category: "Glossary"
    },
    {
        label: "Child pays for parent",
        uri: "/en/glossary/cpfp",
        category: "Glossary"
    },
    {
        label: "Child private key",
        uri: "/en/glossary/child-key",
        category: "Glossary"
    },
    {
        label: "Child public key",
        uri: "/en/glossary/child-key",
        category: "Glossary"
    },
    {
        label: "Coinbase",
        uri: "/en/glossary/coinbase",
        category: "Glossary"
    },
    {
        label: "Coinbase transaction",
        uri: "/en/glossary/coinbase-transaction",
        category: "Glossary"
    },
    {
        label: "CompactSize",
        uri: "/en/glossary/compactsize",
        category: "Glossary"
    },
    {
        label: "Compressed public key",
        uri: "/en/glossary/compressed-public-key",
        category: "Glossary"
    },
    {
        label: "Confirmation score",
        uri: "/en/glossary/confirmation-score",
        category: "Glossary"
    },
    {
        label: "Confirmations",
        uri: "/en/glossary/confirmation-score",
        category: "Glossary"
    },
    {
        label: "Confirmed transaction",
        uri: "/en/glossary/confirmation-score",
        category: "Glossary"
    },
    {
        label: "Consensus",
        uri: "/en/glossary/consensus",
        category: "Glossary"
    },
    {
        label: "Consensus rules",
        uri: "/en/glossary/consensus-rules",
        category: "Glossary"
    },
    {
        label: "CPFP",
        uri: "/en/glossary/cpfp",
        category: "Glossary"
    },
    {
        label: "Data carrier transaction",
        uri: "/en/glossary/null-data-transaction",
        category: "Glossary"
    },
    {
        label: "Data-pushing opcode",
        uri: "/en/glossary/op-code",
        category: "Glossary"
    },
    {
        label: "Denomination",
        uri: "/en/glossary/denominations",
        category: "Glossary"
    },
    {
        label: "Difficulty",
        uri: "/en/glossary/difficulty",
        category: "Glossary"
    },
    {
        label: "DNS seed",
        uri: "/en/glossary/dns-seed",
        category: "Glossary"
    },
    {
        label: "Double spend",
        uri: "/en/glossary/double-spend",
        category: "Glossary"
    },
    {
        label: "Escrow contract",
        uri: "/en/glossary/escrow-contract",
        category: "Glossary"
    },
    {
        label: "Extended key",
        uri: "/en/glossary/extended-key",
        category: "Glossary"
    },
    {
        label: "Fork",
        uri: "/en/glossary/fork",
        category: "Glossary"
    },
    {
        label: "Free transaction",
        uri: "/en/glossary/high-priority-transaction",
        category: "Glossary"
    },
    {
        label: "Full node",
        uri: "/en/glossary/node",
        category: "Glossary"
    },
    {
        label: "Generation transaction",
        uri: "/en/glossary/coinbase-transaction",
        category: "Glossary"
    },
    {
        label: "Genesis block",
        uri: "/en/glossary/genesis-block",
        category: "Glossary"
    },
    {
        label: "Hard fork",
        uri: "/en/glossary/hard-fork",
        category: "Glossary"
    },
    {
        label: "Hardened extended key",
        uri: "/en/glossary/hardened-extended-key",
        category: "Glossary"
    },
    {
        label: "HD protocol",
        uri: "/en/glossary/hd-protocol",
        category: "Glossary"
    },
    {
        label: "HD wallet",
        uri: "/en/glossary/hd-protocol",
        category: "Glossary"
    },
    {
        label: "HD wallet seed",
        uri: "/en/glossary/hd-wallet-seed",
        category: "Glossary"
    },
    {
        label: "Header",
        uri: "/en/glossary/block-header",
        category: "Glossary"
    },
    {
        label: "Header chain",
        uri: "/en/glossary/header-chain",
        category: "Glossary"
    },
    {
        label: "Headers-first sync",
        uri: "/en/glossary/headers-first-sync",
        category: "Glossary"
    },
    {
        label: "Height",
        uri: "/en/glossary/block-height",
        category: "Glossary"
    },
    {
        label: "High-priority transaction",
        uri: "/en/glossary/high-priority-transaction",
        category: "Glossary"
    },
    {
        label: "IBD",
        uri: "/en/glossary/initial-block-download",
        category: "Glossary"
    },
    {
        label: "Initial block download",
        uri: "/en/glossary/initial-block-download",
        category: "Glossary"
    },
    {
        label: "Input",
        uri: "/en/glossary/input",
        category: "Glossary"
    },
    {
        label: "Internal byte order",
        uri: "/en/glossary/internal-byte-order",
        category: "Glossary"
    },
    {
        label: "Inventory",
        uri: "/en/glossary/inventory",
        category: "Glossary"
    },
    {
        label: "Lightweight client",
        uri: "/en/glossary/simplified-payment-verification",
        category: "Glossary"
    },
    {
        label: "Locktime",
        uri: "/en/glossary/locktime",
        category: "Glossary"
    },
    {
        label: "Mainnet",
        uri: "/en/glossary/mainnet",
        category: "Glossary"
    },
    {
        label: "Majority attack",
        uri: "/en/glossary/51-percent-attack",
        category: "Glossary"
    },
    {
        label: "MASF",
        uri: "/en/glossary/masf",
        category: "Glossary"
    },
    {
        label: "Master chain code",
        uri: "/en/glossary/master-chain-code-and-private-key",
        category: "Glossary"
    },
    {
        label: "Master private key",
        uri: "/en/glossary/master-chain-code-and-private-key",
        category: "Glossary"
    },
    {
        label: "Maximum Block Size",
        uri: "/en/glossary/block-size-limit",
        category: "Glossary"
    },
    {
        label: "Merkle block",
        uri: "/en/glossary/merkle-block",
        category: "Glossary"
    },
    {
        label: "Merkle root",
        uri: "/en/glossary/merkle-root",
        category: "Glossary"
    },
    {
        label: "Merkle tree",
        uri: "/en/glossary/merkle-tree",
        category: "Glossary"
    },
    {
        label: "Message header",
        uri: "/en/glossary/message-header",
        category: "Glossary"
    },
    {
        label: "Miner",
        uri: "/en/glossary/mining",
        category: "Glossary"
    },
    {
        label: "Miner-activated soft fork",
        uri: "/en/glossary/masf",
        category: "Glossary"
    },
    {
        label: "Miners fee",
        uri: "/en/glossary/transaction-fee",
        category: "Glossary"
    },
    {
        label: "Minimum relay fee",
        uri: "/en/glossary/minimum-relay-fee",
        category: "Glossary"
    },
    {
        label: "Mining",
        uri: "/en/glossary/mining",
        category: "Glossary"
    },
    {
        label: "Multisig",
        uri: "/en/glossary/multisig",
        category: "Glossary"
    },
    {
        label: "nBits",
        uri: "/en/glossary/nbits",
        category: "Glossary"
    },
    {
        label: "Network difficulty",
        uri: "/en/glossary/difficulty",
        category: "Glossary"
    },
    {
        label: "Network magic",
        uri: "/en/glossary/start-string",
        category: "Glossary"
    },
    {
        label: "nLockTime",
        uri: "/en/glossary/locktime",
        category: "Glossary"
    },
    {
        label: "Node",
        uri: "/en/glossary/node",
        category: "Glossary"
    },
    {
        label: "Non-data-pushing opcode",
        uri: "/en/glossary/op-code",
        category: "Glossary"
    },
    {
        label: "Null data transaction",
        uri: "/en/glossary/null-data-transaction",
        category: "Glossary"
    },
    {
        label: "OP_RETURN transaction",
        uri: "/en/glossary/null-data-transaction",
        category: "Glossary"
    },
    {
        label: "Opcode",
        uri: "/en/glossary/op-code",
        category: "Glossary"
    },
    {
        label: "Opt-in replace by fee",
        uri: "/en/glossary/rbf",
        category: "Glossary"
    },
    {
        label: "Orphan block",
        uri: "/en/glossary/orphan-block",
        category: "Glossary"
    },
    {
        label: "Outpoint",
        uri: "/en/glossary/outpoint",
        category: "Glossary"
    },
    {
        label: "Output",
        uri: "/en/glossary/output",
        category: "Glossary"
    },
    {
        label: "P2PKH address",
        uri: "/en/glossary/p2pkh-address",
        category: "Glossary"
    },
    {
        label: "P2PKH output",
        uri: "/en/glossary/p2pkh-address",
        category: "Glossary"
    },
    {
        label: "P2SH address",
        uri: "/en/glossary/p2sh-address",
        category: "Glossary"
    },
    {
        label: "P2SH multisig",
        uri: "/en/glossary/p2sh-multisig",
        category: "Glossary"
    },
    {
        label: "P2SH output",
        uri: "/en/glossary/p2sh-address",
        category: "Glossary"
    },
    {
        label: "Parent key",
        uri: "/en/glossary/parent-key",
        category: "Glossary"
    },
    {
        label: "Parent private key",
        uri: "/en/glossary/parent-key",
        category: "Glossary"
    },
    {
        label: "Parent public key",
        uri: "/en/glossary/parent-key",
        category: "Glossary"
    },
    {
        label: "Payment protocol",
        uri: "/en/glossary/payment-protocol",
        category: "Glossary"
    },
    {
        label: "Payment request",
        uri: "/en/glossary/payment-protocol",
        category: "Glossary"
    },
    {
        label: "Peer",
        uri: "/en/glossary/node",
        category: "Glossary"
    },
    {
        label: "POW",
        uri: "/en/glossary/proof-of-work",
        category: "Glossary"
    },
    {
        label: "Private extended key",
        uri: "/en/glossary/extended-key",
        category: "Glossary"
    },
    {
        label: "Private key",
        uri: "/en/glossary/private-key",
        category: "Glossary"
    },
    {
        label: "Proof of work",
        uri: "/en/glossary/proof-of-work",
        category: "Glossary"
    },
    {
        label: "Pruned node",
        uri: "/en/glossary/node",
        category: "Glossary"
    },
    {
        label: "Pubkey script",
        uri: "/en/glossary/pubkey-script",
        category: "Glossary"
    },
    {
        label: "Public extended key",
        uri: "/en/glossary/extended-key",
        category: "Glossary"
    },
    {
        label: "Public key",
        uri: "/en/glossary/public-key",
        category: "Glossary"
    },
    {
        label: "Raw transaction",
        uri: "/en/glossary/serialized-transaction",
        category: "Glossary"
    },
    {
        label: "RBF",
        uri: "/en/glossary/rbf",
        category: "Glossary"
    },
    {
        label: "Redeem script",
        uri: "/en/glossary/redeem-script",
        category: "Glossary"
    },
    {
        label: "RedeemScript",
        uri: "/en/glossary/redeem-script",
        category: "Glossary"
    },
    {
        label: "Regression test mode",
        uri: "/en/glossary/regression-test-mode",
        category: "Glossary"
    },
    {
        label: "Regtest",
        uri: "/en/glossary/regression-test-mode",
        category: "Glossary"
    },
    {
        label: "Relay fee",
        uri: "/en/glossary/minimum-relay-fee",
        category: "Glossary"
    },
    {
        label: "Replace by fee",
        uri: "/en/glossary/rbf",
        category: "Glossary"
    },
    {
        label: "Root seed",
        uri: "/en/glossary/hd-wallet-seed",
        category: "Glossary"
    },
    {
        label: "RPC byte order",
        uri: "/en/glossary/rpc-byte-order",
        category: "Glossary"
    },
    {
        label: "Satoshis",
        uri: "/en/glossary/denominations",
        category: "Glossary"
    },
    {
        label: "ScriptPubKey",
        uri: "/en/glossary/pubkey-script",
        category: "Glossary"
    },
    {
        label: "ScriptSig",
        uri: "/en/glossary/signature-script",
        category: "Glossary"
    },
    {
        label: "Sequence number",
        uri: "/en/glossary/sequence-number",
        category: "Glossary"
    },
    {
        label: "Serialized block",
        uri: "/en/glossary/serialized-block",
        category: "Glossary"
    },
    {
        label: "Serialized transaction",
        uri: "/en/glossary/serialized-transaction",
        category: "Glossary"
    },
    {
        label: "Sighash",
        uri: "/en/glossary/signature-hash",
        category: "Glossary"
    },
    {
        label: "SIGHASH_ALL",
        uri: "/en/glossary/sighash-all",
        category: "Glossary"
    },
    {
        label: "SIGHASH_ANYONECANPAY",
        uri: "/en/glossary/sighash-anyonecanpay",
        category: "Glossary"
    },
    {
        label: "SIGHASH_NONE",
        uri: "/en/glossary/sighash-none",
        category: "Glossary"
    },
    {
        label: "SIGHASH_SINGLE",
        uri: "/en/glossary/sighash-single",
        category: "Glossary"
    },
    {
        label: "Signature",
        uri: "/en/glossary/signature",
        category: "Glossary"
    },
    {
        label: "Signature hash",
        uri: "/en/glossary/signature-hash",
        category: "Glossary"
    },
    {
        label: "Signature script",
        uri: "/en/glossary/signature-script",
        category: "Glossary"
    },
    {
        label: "Simplified Payment Verification",
        uri: "/en/glossary/simplified-payment-verification",
        category: "Glossary"
    },
    {
        label: "Soft fork",
        uri: "/en/glossary/soft-fork",
        category: "Glossary"
    },
    {
        label: "SPV",
        uri: "/en/glossary/simplified-payment-verification",
        category: "Glossary"
    },
    {
        label: "Stale block",
        uri: "/en/glossary/stale-block",
        category: "Glossary"
    },
    {
        label: "Standard Transaction",
        uri: "/en/glossary/standard-transaction",
        category: "Glossary"
    },
    {
        label: "Start string",
        uri: "/en/glossary/start-string",
        category: "Glossary"
    },
    {
        label: "Target",
        uri: "/en/glossary/nbits",
        category: "Glossary"
    },
    {
        label: "Testnet",
        uri: "/en/glossary/testnet",
        category: "Glossary"
    },
    {
        label: "Thin client",
        uri: "/en/glossary/simplified-payment-verification",
        category: "Glossary"
    },
    {
        label: "Transaction fee",
        uri: "/en/glossary/transaction-fee",
        category: "Glossary"
    },
    {
        label: "Transaction malleability",
        uri: "/en/glossary/malleability",
        category: "Glossary"
    },
    {
        label: "Transaction mutability",
        uri: "/en/glossary/malleability",
        category: "Glossary"
    },
    {
        label: "Txid",
        uri: "/en/glossary/txid",
        category: "Glossary"
    },
    {
        label: "TxIn",
        uri: "/en/glossary/input",
        category: "Glossary"
    },
    {
        label: "TxOut",
        uri: "/en/glossary/output",
        category: "Glossary"
    },
    {
        label: "UASF",
        uri: "/en/glossary/uasf",
        category: "Glossary"
    },
    {
        label: "Unconfirmed transaction",
        uri: "/en/glossary/confirmation-score",
        category: "Glossary"
    },
    {
        label: "User-activated soft fork",
        uri: "/en/glossary/uasf",
        category: "Glossary"
    },
    {
        label: "UTXO",
        uri: "/en/glossary/unspent-transaction-output",
        category: "Glossary"
    },
    {
        label: "Wallet",
        uri: "/en/glossary/wallet",
        category: "Glossary"
    },
    {
        label: "Wallet Import Format",
        uri: "/en/glossary/wallet-import-format",
        category: "Glossary"
    },
    {
        label: "Watch-only address",
        uri: "/en/glossary/watch-only-address",
        category: "Glossary"
    },
    {
        label: "WIF",
        uri: "/en/glossary/wallet-import-format",
        category: "Glossary"
    },


    {
        label: "AbandonTransaction",
        uri: "/en/developer-reference.html#abandontransaction",
        category: "RPCs"
    },
    {
        label: "AddMultiSigAddress",
        uri: "/en/developer-reference.html#addmultisigaddress",
        category: "RPCs"
    },
    {
        label: "AddNode",
        uri: "/en/developer-reference.html#addnode",
        category: "RPCs"
    },
    {
        label: "AddWitnessAddress",
        uri: "/en/developer-reference.html#addwitnessaddress",
        category: "RPCs"
    },
    {
        label: "BackupWallet",
        uri: "/en/developer-reference.html#backupwallet",
        category: "RPCs"
    },
    {
        label: "BumpFee",
        uri: "/en/developer-reference.html#bumpfee",
        category: "RPCs"
    },
    {
        label: "ClearBanned",
        uri: "/en/developer-reference.html#clearbanned",
        category: "RPCs"
    },
    {
        label: "CreateMultisig",
        uri: "/en/developer-reference.html#createmultisig",
        category: "RPCs"
    },
    {
        label: "CreateRawTransaction",
        uri: "/en/developer-reference.html#createrawtransaction",
        category: "RPCs"
    },
    {
        label: "DecodeRawTransaction",
        uri: "/en/developer-reference.html#decoderawtransaction",
        category: "RPCs"
    },
    {
        label: "DecodeScript",
        uri: "/en/developer-reference.html#decodescript",
        category: "RPCs"
    },
    {
        label: "DisconnectNode",
        uri: "/en/developer-reference.html#disconnectnode",
        category: "RPCs"
    },
    {
        label: "DumpPrivKey",
        uri: "/en/developer-reference.html#dumpprivkey",
        category: "RPCs"
    },
    {
        label: "DumpWallet",
        uri: "/en/developer-reference.html#dumpwallet",
        category: "RPCs"
    },
    {
        label: "EncryptWallet",
        uri: "/en/developer-reference.html#encryptwallet",
        category: "RPCs"
    },
    {
        label: "EstimateFee",
        uri: "/en/developer-reference.html#estimatefee",
        category: "RPCs"
    },
    {
        label: "EstimatePriority",
        uri: "/en/developer-reference.html#estimatepriority",
        category: "RPCs"
    },
    {
        label: "FundRawTransaction",
        uri: "/en/developer-reference.html#fundrawtransaction",
        category: "RPCs"
    },
    {
        label: "Generate",
        uri: "/en/developer-reference.html#generate",
        category: "RPCs"
    },
    {
        label: "GenerateToAddress",
        uri: "/en/developer-reference.html#generatetoaddress",
        category: "RPCs"
    },
    {
        label: "GetAccount",
        uri: "/en/developer-reference.html#getaccount",
        category: "RPCs"
    },
    {
        label: "GetAccountAddress",
        uri: "/en/developer-reference.html#getaccountaddress",
        category: "RPCs"
    },
    {
        label: "GetAddedNodeInfo",
        uri: "/en/developer-reference.html#getaddednodeinfo",
        category: "RPCs"
    },
    {
        label: "GetAddressesByAccount",
        uri: "/en/developer-reference.html#getaddressesbyaccount",
        category: "RPCs"
    },
    {
        label: "GetBalance",
        uri: "/en/developer-reference.html#getbalance",
        category: "RPCs"
    },
    {
        label: "GetBestBlockHash",
        uri: "/en/developer-reference.html#getbestblockhash",
        category: "RPCs"
    },
    {
        label: "GetBlock",
        uri: "/en/developer-reference.html#getblock",
        category: "RPCs"
    },
    {
        label: "GetBlockChainInfo",
        uri: "/en/developer-reference.html#getblockchaininfo",
        category: "RPCs"
    },
    {
        label: "GetBlockCount",
        uri: "/en/developer-reference.html#getblockcount",
        category: "RPCs"
    },
    {
        label: "GetBlockHash",
        uri: "/en/developer-reference.html#getblockhash",
        category: "RPCs"
    },
    {
        label: "GetBlockHeader",
        uri: "/en/developer-reference.html#getblockheader",
        category: "RPCs"
    },
    {
        label: "GetBlockTemplate",
        uri: "/en/developer-reference.html#getblocktemplate",
        category: "RPCs"
    },
    {
        label: "GetChainTips",
        uri: "/en/developer-reference.html#getchaintips",
        category: "RPCs"
    },
    {
        label: "GetConnectionCount",
        uri: "/en/developer-reference.html#getconnectioncount",
        category: "RPCs"
    },
    {
        label: "GetDifficulty",
        uri: "/en/developer-reference.html#getdifficulty",
        category: "RPCs"
    },
    {
        label: "GetGenerate",
        uri: "/en/developer-reference.html#getgenerate",
        category: "RPCs"
    },
    {
        label: "GetHashesPerSec",
        uri: "/en/developer-reference.html#gethashespersec",
        category: "RPCs"
    },
    {
        label: "GetInfo",
        uri: "/en/developer-reference.html#getinfo",
        category: "RPCs"
    },
    {
        label: "GetMemoryInfo",
        uri: "/en/developer-reference.html#getmemoryinfo",
        category: "RPCs"
    },
    {
        label: "GetMempoolInfo",
        uri: "/en/developer-reference.html#getmempoolinfo",
        category: "RPCs"
    },
    {
        label: "GetMempoolEntry",
        uri: "/en/developer-reference.html#getmempoolentry",
        category: "RPCs"
    },
    {
        label: "GetMempoolAncestors",
        uri: "/en/developer-reference.html#getmempoolancestors",
        category: "RPCs"
    },
    {
        label: "GetMempoolDescendants",
        uri: "/en/developer-reference.html#getmempooldescendants",
        category: "RPCs"
    },
    {
        label: "GetMiningInfo",
        uri: "/en/developer-reference.html#getmininginfo",
        category: "RPCs"
    },
    {
        label: "GetNetTotals",
        uri: "/en/developer-reference.html#getnettotals",
        category: "RPCs"
    },
    {
        label: "GetNetworkHashPS",
        uri: "/en/developer-reference.html#getnetworkhashps",
        category: "RPCs"
    },
    {
        label: "GetNetworkInfo",
        uri: "/en/developer-reference.html#getnetworkinfo",
        category: "RPCs"
    },
    {
        label: "GetNewAddress",
        uri: "/en/developer-reference.html#getnewaddress",
        category: "RPCs"
    },
    {
        label: "GetPeerInfo",
        uri: "/en/developer-reference.html#getpeerinfo",
        category: "RPCs"
    },
    {
        label: "GetRawChangeAddress",
        uri: "/en/developer-reference.html#getrawchangeaddress",
        category: "RPCs"
    },
    {
        label: "GetRawMempool",
        uri: "/en/developer-reference.html#getrawmempool",
        category: "RPCs"
    },
    {
        label: "GetRawTransaction",
        uri: "/en/developer-reference.html#getrawtransaction",
        category: "RPCs"
    },
    {
        label: "GetReceivedByAccount",
        uri: "/en/developer-reference.html#getreceivedbyaccount",
        category: "RPCs"
    },
    {
        label: "GetReceivedByAddress",
        uri: "/en/developer-reference.html#getreceivedbyaddress",
        category: "RPCs"
    },
    {
        label: "GetTransaction",
        uri: "/en/developer-reference.html#gettransaction",
        category: "RPCs"
    },
    {
        label: "GetTxout",
        uri: "/en/developer-reference.html#gettxout",
        category: "RPCs"
    },
    {
        label: "GetTxOutProof",
        uri: "/en/developer-reference.html#gettxoutproof",
        category: "RPCs"
    },
    {
        label: "GetTxoutSetInfo",
        uri: "/en/developer-reference.html#gettxoutsetinfo",
        category: "RPCs"
    },
    {
        label: "GetUnconfirmedBalance",
        uri: "/en/developer-reference.html#getunconfirmedbalance",
        category: "RPCs"
    },
    {
        label: "GetWalletInfo",
        uri: "/en/developer-reference.html#getwalletinfo",
        category: "RPCs"
    },
    {
        label: "GetWork",
        uri: "/en/developer-reference.html#getwork",
        category: "RPCs"
    },
    {
        label: "Help",
        uri: "/en/developer-reference.html#help",
        category: "RPCs"
    },
    {
        label: "ImportAddress",
        uri: "/en/developer-reference.html#importaddress",
        category: "RPCs"
    },
    {
        label: "ImportMulti",
        uri: "/en/developer-reference.html#importmulti",
        category: "RPCs"
    },
    {
        label: "ImportPrivkey",
        uri: "/en/developer-reference.html#importprivkey",
        category: "RPCs"
    },
    {
        label: "ImportPrunedFunds",
        uri: "/en/developer-reference.html#importprunedfunds",
        category: "RPCs"
    },
    {
        label: "ImportWallet",
        uri: "/en/developer-reference.html#importwallet",
        category: "RPCs"
    },
    {
        label: "KeypoolRefill",
        uri: "/en/developer-reference.html#keypoolrefill",
        category: "RPCs"
    },
    {
        label: "ListAccounts",
        uri: "/en/developer-reference.html#listaccounts",
        category: "RPCs"
    },
    {
        label: "ListAddressGroupings",
        uri: "/en/developer-reference.html#listaddressgroupings",
        category: "RPCs"
    },
    {
        label: "ListBanned",
        uri: "/en/developer-reference.html#listbanned",
        category: "RPCs"
    },
    {
        label: "ListLockUnspent",
        uri: "/en/developer-reference.html#listlockunspent",
        category: "RPCs"
    },
    {
        label: "ListReceivedByAccount",
        uri: "/en/developer-reference.html#listreceivedbyaccount",
        category: "RPCs"
    },
    {
        label: "ListReceivedByAddress",
        uri: "/en/developer-reference.html#listreceivedbyaddress",
        category: "RPCs"
    },
    {
        label: "ListSinceBlock",
        uri: "/en/developer-reference.html#listsinceblock",
        category: "RPCs"
    },
    {
        label: "ListTransactions",
        uri: "/en/developer-reference.html#listtransactions",
        category: "RPCs"
    },
    {
        label: "ListUnspent",
        uri: "/en/developer-reference.html#listunspent",
        category: "RPCs"
    },
    {
        label: "LockUnspent",
        uri: "/en/developer-reference.html#lockunspent",
        category: "RPCs"
    },
    {
        label: "Move",
        uri: "/en/developer-reference.html#move",
        category: "RPCs"
    },
    {
        label: "Ping",
        uri: "/en/developer-reference.html#ping-rpc",
        category: "RPCs"
    },
    {
        label: "PreciousBlock",
        uri: "/en/developer-reference.html#preciousblock",
        category: "RPCs"
    },
    {
        label: "PrioritiseTransaction",
        uri: "/en/developer-reference.html#prioritisetransaction",
        category: "RPCs"
    },
    {
        label: "PruneBlockChain",
        uri: "/en/developer-reference.html#pruneblockchain",
        category: "RPCs"
    },
    {
        label: "RemovePrunedFunds",
        uri: "/en/developer-reference.html#removeprunedfunds",
        category: "RPCs"
    },
    {
        label: "SendFrom",
        uri: "/en/developer-reference.html#sendfrom",
        category: "RPCs"
    },
    {
        label: "SendMany",
        uri: "/en/developer-reference.html#sendmany",
        category: "RPCs"
    },
    {
        label: "SendRawTransaction",
        uri: "/en/developer-reference.html#sendrawtransaction",
        category: "RPCs"
    },
    {
        label: "SendToAddress",
        uri: "/en/developer-reference.html#sendtoaddress",
        category: "RPCs"
    },
    {
        label: "SetAccount",
        uri: "/en/developer-reference.html#setaccount",
        category: "RPCs"
    },
    {
        label: "SetBan",
        uri: "/en/developer-reference.html#setban",
        category: "RPCs"
    },
    {
        label: "SetGenerate",
        uri: "/en/developer-reference.html#setgenerate",
        category: "RPCs"
    },
    {
        label: "SetNetworkActive",
        uri: "/en/developer-reference.html#setnetworkactive",
        category: "RPCs"
    },
    {
        label: "SetTxFee",
        uri: "/en/developer-reference.html#settxfee",
        category: "RPCs"
    },
    {
        label: "SignMessage",
        uri: "/en/developer-reference.html#signmessage",
        category: "RPCs"
    },
    {
        label: "SignMessageWithPrivKey",
        uri: "/en/developer-reference.html#signmessagewithprivkey",
        category: "RPCs"
    },
    {
        label: "SignRawTransaction",
        uri: "/en/developer-reference.html#signrawtransaction",
        category: "RPCs"
    },
    {
        label: "Stop",
        uri: "/en/developer-reference.html#stop",
        category: "RPCs"
    },
    {
        label: "SubmitBlock",
        uri: "/en/developer-reference.html#submitblock",
        category: "RPCs"
    },
    {
        label: "ValidateAddress",
        uri: "/en/developer-reference.html#validateaddress",
        category: "RPCs"
    },
    {
        label: "VerifyChain",
        uri: "/en/developer-reference.html#verifychain",
        category: "RPCs"
    },
    {
        label: "VerifyMessage",
        uri: "/en/developer-reference.html#verifymessage",
        category: "RPCs"
    },
    {
        label: "VerifyTxOutProof",
        uri: "/en/developer-reference.html#verifytxoutproof",
        category: "RPCs"
    },
    {
        label: "WalletLock",
        uri: "/en/developer-reference.html#walletlock",
        category: "RPCs"
    },
    {
        label: "WalletPassphrase",
        uri: "/en/developer-reference.html#walletpassphrase",
        category: "RPCs"
    },
    {
        label: "WalletPassphraseChange",
        uri: "/en/developer-reference.html#walletpassphrasechange",
        category: "RPCs"
    },


    {
        label: "GetBlock",
        uri: "/en/developer-reference.html#get-block",
        category: "REST"
    },
    {
        label: "GetBlock-noTxDetails",
        uri: "/en/developer-reference.html#get-blocknotxdetails",
        category: "REST"
    },
    {
        label: "GetChainInfo",
        uri: "/en/developer-reference.html#get-chaininfo",
        category: "REST"
    },
    {
        label: "GetUtxos",
        uri: "/en/developer-reference.html#get-getutxos",
        category: "REST"
    },
    {
        label: "GetHeaders",
        uri: "/en/developer-reference.html#get-headers",
        category: "REST"
    },
    {
        label: "GetMemPool-contents",
        uri: "/en/developer-reference.html#get-mempoolcontents",
        category: "REST"
    },
    {
        label: "GetMemPool-info",
        uri: "/en/developer-reference.html#get-mempoolinfo",
        category: "REST"
    },


    {
        label: "OP_0 (OP_FALSE)",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_PUSHDATA1",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_PUSHDATA2",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_PUSHDATA4",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_1NEGATE",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_1 (OP_TRUE)",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_2",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_3",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_4",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_5",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_6",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_7",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_8",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_9",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_10",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_11",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_12",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_13",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_14",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_15",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_16",
        uri: "https://en.bitcoin.it/wiki/Script#Constants",
        category: "Opcodes"
    },
    {
        label: "OP_NOP",
        uri: "https://en.bitcoin.it/wiki/Script#Flow_control",
        category: "Opcodes"
    },
    {
        label: "OP_IF",
        uri: "https://en.bitcoin.it/wiki/Script#Flow_control",
        category: "Opcodes"
    },
    {
        label: "OP_NOTIF",
        uri: "https://en.bitcoin.it/wiki/Script#Flow_control",
        category: "Opcodes"
    },
    {
        label: "OP_ELSE",
        uri: "https://en.bitcoin.it/wiki/Script#Flow_control",
        category: "Opcodes"
    },
    {
        label: "OP_ENDIF",
        uri: "https://en.bitcoin.it/wiki/Script#Flow_control",
        category: "Opcodes"
    },
    {
        label: "OP_VERIFY",
        uri: "https://en.bitcoin.it/wiki/Script#Flow_control",
        category: "Opcodes"
    },
    {
        label: "OP_RETURN",
        uri: "https://en.bitcoin.it/wiki/Script#Flow_control",
        category: "Opcodes"
    },
    {
        label: "OP_TOALTSTACK",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_FROMALTSTACK",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_IFDUP",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_DEPTH",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_DROP",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_DUP",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_NIP",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_OVER",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_PICK",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_ROLL",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_ROT",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_SWAP",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_TUCK",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_2DROP",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_2DUP",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_3DUP",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_2OVER",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_2ROT",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_2SWAP",
        uri: "https://en.bitcoin.it/wiki/Script#Stack",
        category: "Opcodes"
    },
    {
        label: "OP_CAT (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Splice",
        category: "Opcodes"
    },
    {
        label: "OP_SUBSTR (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Splice",
        category: "Opcodes"
    },
    {
        label: "OP_LEFT (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Splice",
        category: "Opcodes"
    },
    {
        label: "OP_RIGHT (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Splice",
        category: "Opcodes"
    },
    {
        label: "OP_SIZE",
        uri: "https://en.bitcoin.it/wiki/Script#Splice",
        category: "Opcodes"
    },
    {
        label: "OP_INVERT (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Bitwise_logic",
        category: "Opcodes"
    },
    {
        label: "OP_AND (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Bitwise_logic",
        category: "Opcodes"
    },
    {
        label: "OP_OR (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Bitwise_logic",
        category: "Opcodes"
    },
    {
        label: "OP_XOR (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Bitwise_logic",
        category: "Opcodes"
    },
    {
        label: "OP_EQUAL",
        uri: "https://en.bitcoin.it/wiki/Script#Bitwise_logic",
        category: "Opcodes"
    },
    {
        label: "OP_EQUALVERIFY",
        uri: "https://en.bitcoin.it/wiki/Script#Bitwise_logic",
        category: "Opcodes"
    },
    {
        label: "OP_1ADD",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_1SUB",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_2MUL (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_2DIV (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_NEGATE",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_ABS",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_NOT",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_0NOTEQUAL",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_ADD",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_SUB",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_MUL (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_DIV (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_MOD (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_LSHIFT (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_RSHIFT (disabled)",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_BOOLAND",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_BOOLOR",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_NUMEQUAL",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_NUMEQUALVERIFY",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_NUMNOTEQUAL",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_LESSTHAN",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_GREATERTHAN",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_LESSTHANOREQUAL",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_GREATERTHANOREQUAL",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_MIN",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_MAX",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_WITHIN",
        uri: "https://en.bitcoin.it/wiki/Script#Arithmetic",
        category: "Opcodes"
    },
    {
        label: "OP_RIPEMD160",
        uri: "https://en.bitcoin.it/wiki/Script#Crypto",
        category: "Opcodes"
    },
    {
        label: "OP_SHA1",
        uri: "https://en.bitcoin.it/wiki/Script#Crypto",
        category: "Opcodes"
    },
    {
        label: "OP_SHA256",
        uri: "https://en.bitcoin.it/wiki/Script#Crypto",
        category: "Opcodes"
    },
    {
        label: "OP_HASH160",
        uri: "https://en.bitcoin.it/wiki/Script#Crypto",
        category: "Opcodes"
    },
    {
        label: "OP_HASH256",
        uri: "https://en.bitcoin.it/wiki/Script#Crypto",
        category: "Opcodes"
    },
    {
        label: "OP_CODESEPARATOR",
        uri: "https://en.bitcoin.it/wiki/Script#Crypto",
        category: "Opcodes"
    },
    {
        label: "OP_CHECKSIG",
        uri: "https://en.bitcoin.it/wiki/Script#Crypto",
        category: "Opcodes"
    },
    {
        label: "OP_CHECKSIGVERIFY",
        uri: "https://en.bitcoin.it/wiki/Script#Crypto",
        category: "Opcodes"
    },
    {
        label: "OP_CHECKMULTISIG",
        uri: "https://en.bitcoin.it/wiki/Script#Crypto",
        category: "Opcodes"
    },
    {
        label: "OP_CHECKMULTISIGVERIFY",
        uri: "https://en.bitcoin.it/wiki/Script#Crypto",
        category: "Opcodes"
    },
    {
        label: "OP_CHECKLOCKTIMEVERIFY",
        uri: "https://en.bitcoin.it/wiki/Script#Locktime",
        category: "Opcodes"
    },
    {
        label: "OP_CHECKSEQUENCEVERIFY ",
        uri: "https://en.bitcoin.it/wiki/Script#Locktime",
        category: "Opcodes"
    },
    {
        label: "OP_PUBKEYHASH (invalid)",
        uri: "https://en.bitcoin.it/wiki/Script#Pseudo-words",
        category: "Opcodes"
    },
    {
        label: "OP_PUBKEY (invalid)",
        uri: "https://en.bitcoin.it/wiki/Script#Pseudo-words",
        category: "Opcodes"
    },
    {
        label: "OP_INVALIDOPCODE (invalid)",
        uri: "https://en.bitcoin.it/wiki/Script#Pseudo-words",
        category: "Opcodes"
    },
    {
        label: "OP_RESERVED (reserved)",
        uri: "https://en.bitcoin.it/wiki/Script#Reserved_words",
        category: "Opcodes"
    },
    {
        label: "OP_VER (reserved)",
        uri: "https://en.bitcoin.it/wiki/Script#Reserved_words",
        category: "Opcodes"
    },
    {
        label: "OP_VERIF (reserved)",
        uri: "https://en.bitcoin.it/wiki/Script#Reserved_words",
        category: "Opcodes"
    },
    {
        label: "OP_VERNOTIF (reserved)",
        uri: "https://en.bitcoin.it/wiki/Script#Reserved_words",
        category: "Opcodes"
    },
    {
        label: "OP_RESERVED1 (reserved)",
        uri: "https://en.bitcoin.it/wiki/Script#Reserved_words",
        category: "Opcodes"
    },
    {
        label: "OP_RESERVED2 (reserved)",
        uri: "https://en.bitcoin.it/wiki/Script#Reserved_words",
        category: "Opcodes"
    },
    {
        label: "OP_NOP1 (reserved)",
        uri: "https://en.bitcoin.it/wiki/Script#Reserved_words",
        category: "Opcodes"
    },
    {
        label: "OP_NOP4 (reserved)",
        uri: "https://en.bitcoin.it/wiki/Script#Reserved_words",
        category: "Opcodes"
    },
    {
        label: "OP_NOP5 (reserved)",
        uri: "https://en.bitcoin.it/wiki/Script#Reserved_words",
        category: "Opcodes"
    },
    {
        label: "OP_NOP6 (reserved)",
        uri: "https://en.bitcoin.it/wiki/Script#Reserved_words",
        category: "Opcodes"
    },
    {
        label: "OP_NOP7 (reserved)",
        uri: "https://en.bitcoin.it/wiki/Script#Reserved_words",
        category: "Opcodes"
    },
    {
        label: "OP_NOP8 (reserved)",
        uri: "https://en.bitcoin.it/wiki/Script#Reserved_words",
        category: "Opcodes"
    },
    {
        label: "OP_NOP9 (reserved)",
        uri: "https://en.bitcoin.it/wiki/Script#Reserved_words",
        category: "Opcodes"
    },
    {
        label: "OP_NOP10 (reserved)",
        uri: "https://en.bitcoin.it/wiki/Script#Reserved_words",
        category: "Opcodes"
    },


    {
        label: "BIP1: BIP purpose and guidelines",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0001.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP9: soft fork mechanism",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0009.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP11: m-of-n multisig transactions",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0011.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP13: P2SH address format",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0013.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP14: protocol version and user agent",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0014.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP16: pay to script hash (P2SH)",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0016.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP21: URI scheme",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0021.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP22: GetBlockTemplate fundamentals",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0022.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP23: GetBlockTemplate pooled mining",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0023.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP30: dealing with duplicate transactions",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0030.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP31: P2P pong message",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0031.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP32: HD wallets",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0032.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP34: block height in coinbase",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0034.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP35: P2P mempool message",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0035.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP37: bloom filtering",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0037.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP38: passphrase-protected private key",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0038.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP39: mnemonic code for HD wallets",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0039.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP42: finite monetary supply",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0042.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP44: HD wallet account hierarchy",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0044.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP50: March 2013 fork post-mortem",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0050.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP60: fixed-length version message",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0060.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP61: P2P reject message",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0061.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP62: dealing with malleability",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0062.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP64: P2P geutxos message",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0064.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP65: OP_CHECKLOCKTIMEVERIFY",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0065.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP66: strict DER signatures",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0066.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP70: payment protocol",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0070.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP71: payment protocol MIME types",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0071.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP72: payment protocol URIs",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0072.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP73: accept header with payment protocol",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0073.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP111: NODE_BLOOM service bit",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0111.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP112: OP_CHECKSEQUENCEVERIFY",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0112.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP113: median time for lock-time",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0113.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP125: replace-by-fee signaling",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0125.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP130: sendheaders message",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0130.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP133: feefilter message",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0133.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP141: segregated witness",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0141.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP151: P2P communication encryption",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0151.mediawiki",
        category: "BIPs"
    },
    {
        label: "BIP152: compact block relay",
        uri: "https://github.com/bitcoin/bips/blob/master/bip-0152.mediawiki",
        category: "BIPs"
    },


    {
        label: "addr",
        uri: "/en/developer-reference.html#addr",
        category: "P2P Messages"
    },
    {
        label: "alert",
        uri: "/en/developer-reference.html#alert",
        category: "P2P Messages"
    },
    {
        label: "block",
        uri: "/en/developer-reference.html#block",
        category: "P2P Messages"
    },
    {
        label: "feefilter",
        uri: "/en/developer-reference.html#feefilter",
        category: "P2P Messages"
    },
    {
        label: "filteradd",
        uri: "/en/developer-reference.html#filteradd",
        category: "P2P Messages"
    },
    {
        label: "filterclear",
        uri: "/en/developer-reference.html#filterclear",
        category: "P2P Messages"
    },
    {
        label: "filterload",
        uri: "/en/developer-reference.html#filterload",
        category: "P2P Messages"
    },
    {
        label: "getaddr",
        uri: "/en/developer-reference.html#getaddr",
        category: "P2P Messages"
    },
    {
        label: "getblocks",
        uri: "/en/developer-reference.html#getblocks",
        category: "P2P Messages"
    },
    {
        label: "getdata",
        uri: "/en/developer-reference.html#getdata",
        category: "P2P Messages"
    },
    {
        label: "getheaders",
        uri: "/en/developer-reference.html#getheaders",
        category: "P2P Messages"
    },
    {
        label: "headers",
        uri: "/en/developer-reference.html#headers",
        category: "P2P Messages"
    },
    {
        label: "inv",
        uri: "/en/developer-reference.html#inv",
        category: "P2P Messages"
    },
    {
        label: "mempool",
        uri: "/en/developer-reference.html#mempool",
        category: "P2P Messages"
    },
    {
        label: "merkleblock",
        uri: "/en/developer-reference.html#merkleblock",
        category: "P2P Messages"
    },
    {
        label: "notfound",
        uri: "/en/developer-reference.html#notfound",
        category: "P2P Messages"
    },
    {
        label: "ping",
        uri: "/en/developer-reference.html#ping",
        category: "P2P Messages"
    },
    {
        label: "pong",
        uri: "/en/developer-reference.html#pong",
        category: "P2P Messages"
    },
    {
        label: "reject",
        uri: "/en/developer-reference.html#reject",
        category: "P2P Messages"
    },
    {
        label: "tx",
        uri: "/en/developer-reference.html#tx",
        category: "P2P Messages"
    },
    {
        label: "verack",
        uri: "/en/developer-reference.html#verack",
        category: "P2P Messages"
    },
    {
        label: "version",
        uri: "/en/developer-reference.html#version",
        category: "P2P Messages"
    }
];


// code adapted from http://jqueryui.com/autocomplete/#categories
// MIT license: https://jquery.org/license/

$.widget("custom.catcomplete", $.ui.autocomplete, {
    _create: function () {
        this._super();
        this.widget().menu("option", "items", "> :not(.ui-autocomplete-category)");
    },
    _renderMenu: function (ul, items) {
        var that = this,
            currentCategory = "";
        $.each(items, function (index, item) {
            var li;
            if (item.category !== currentCategory) {
                ul.append("<li class='ui-autocomplete-category'>" + item.category + "</li>");
                currentCategory = item.category;
            }
            li = that._renderItemData(ul, item);
            if (item.category) {
                li.attr("aria-label", item.category + " : " + item.label);
            }
        });
    }
});
$(function () {
    $("#glossary_term").catcomplete({
        source: search_data,
        delay: 0,
        minLength: 2,
        autoFocus: true,
        select: function (event, ui) {
            if (ui.item.uri.indexOf("html") >= 0) {
                location.href = ui.item.uri;
            } else {
                location.href = ui.item.uri + '.html';
            }
        }
    });
});

