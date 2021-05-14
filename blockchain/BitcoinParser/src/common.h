#ifndef __COMMON_H__
#define __COMMON_H__

#define MSG_SUCCESS         "Success."
#define MSG_FAILURE         "Failure."
#define MSG_COINBASE        "[COINBASE]"
#define MSG_LEN_IS_NEGATIVE "The length of the string is negative."
#define MSG_LEN_IS_ODD      "The length of the string si odd."
#define MSG_VARINT_PREFIX   "Data integrity is broken. Must be one of 'fd', 'fe', or 'ff'"
#define MSG_NOT_HEX         "Data integrity is broken. The data is not a hex data."
#define MSG_NOT_BIN         "Data integrity is broken. The data is not a bin data."
#define MSG_ENDIAN_ERROR    "Occurred a error while convert the endianness."
#define MSG_ALLOC_ERROR     "Cannot allocated memory."
#define MSG_CURL_INIT_ERROR "Failed to initialization for cURL"
#define MSG_VARINT_ZERO     "The value returned by varInt() is zero"

#define SIZE_DEBUG_LABEL    50
#define SIZE                65536
#define SIZE_2BYTES         2
#define SIZE_4BYTES         4
#define SIZE_6BYTES         6
#define SIZE_8BYTES         8
#define SIZE_10BYTES        10
#define SIZE_16BYTES        16
#define SIZE_18BYTES        18
#define SIZE_32BYTES        32
#define SIZE_64BYTES        64
#define SIZE_HEADER         160
#define SIZE_HEADER_HEX     80
#define SIZE_VERSION        8
#define SIZE_PREBLOCKHASH   64
#define SIZE_MERKLEROOT     64
#define SIZE_TIME           8
#define SIZE_BITS           8
#define SIZE_NONCE          8
#define SIZE_BLOCKHASH      64
#define SIZE_HASH           64
#define SIZE_HASH_HEX       32
#define SIZE_TXID           64
#define SIZE_TXID_HEX       32
#define SIZE_VOUT           8
#define SIZE_VOUT_HEX       4
#define SIZE_SEQUENCE       8
#define SIZE_SEQUENCE_HEX   4
#define SIZE_VALUE          16
#define SIZE_VALUE_HEX      8
#define SIZE_LOCKTIME       8
#define SIZE_LOCKTIME_HEX   4
#define SIZE_SEGWIT_FLAG    4
#endif
