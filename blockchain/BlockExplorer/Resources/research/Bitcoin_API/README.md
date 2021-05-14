# Bitcoin API Description(September 17 2019)<br>
* Bitcoin API의 결과 값들을 한글로 표현하기 위해 API 수행 반환 값들을 정리 및 한글로 번역한 문서입니다.<br>
비트코인 내부 구조 및 자료형을 참고하기 위한 사이트입니다.<br>
    * https://dev.visucore.com/bitcoin/doxygen/chain_8h_source.html<br>
    * https://bitcoin.org/en/developer-reference#get-block<br><br>
## 목록<br>
1. [getblockhash](#getblockhash)<br>
2. [getbestblockhash](#getbestblockhash)<br>
3. [getblock](#getblock)<br>
4. [getrawtransaction](#getrawtransaction)<br>
5. [getblockchaininfo](#getblockchaininfo)<br>
6. [getmempoolinfo](#getmempoolinfo)<br>
7. [getmininginfo](#getmininginfo)<br>
8. [getnetworkinfo](#getnetworkinfo)<br>
9. [getwalletinfo](#getwalletinfo)<br>
* 필요에 따라 더 추가될 수 있음<br><br>
## getblockhash<br>
### 블록 헤더의 해쉬 값을 반환합니다.<br>
* usage:<br>
   * getblockhash *height*<br>
* result:<br>
   * "hash": (hex, char[64]) the block hash<br>
      * 블록 헤더의 해쉬 값 입니다. <br><br>
## getbestblockhash<br>
### 가장 긴 블록체인, 즉 메인 체인의 가장 최근 블록 헤더 해쉬 값을 반환합니다. <br>
* usage:<br>
   * getbestblockhash<br>
* result:<br>
   * "hash": (hex, char[64]) the hash of the best (tip) block in the longest blockchain<br>
      * 메인 체인의 가장 최근 블록 헤더 해쉬 값 입니다. <br><br>
## getblock<br>
### 블록에 대한 정보들을 반환합니다. <br>
* usage:<br>
   * getblock *hash*<br>
* result:<br>
   * "hash": (hex, char[64]) the block hash (same as provided)<br>
      * 블록 헤더의 해쉬 값입니다. getblock 명령에 사용되는 인자와 같은 값입니다. <br>
   * "confirmations": (int) The number of confirmations, or -1 if the block is not on the main chain<br>
      * 블록이 메인넷에 연결되기 위하여 검증받은 횟수를 나타내며, 아직 블록이 메인넷에 붙지 않은 상태라면 -1을 반환합니다. <br>
   * "strippedsize": (int) The block size excluding witness data<br>
      * 블록 내에서 witness data(전자 서명)을 제외한 크기를 나타냅니다. <br>
   * "size": (int) The block size<br>
      * 블록의 총 크기를 나타냅니다. <br>
   * "weight": (int) The block weight as defined in BIP 141<br>
      * BIP 141에서 정의된 블록의 데이터 양을 나타냅니다. 세그윗(SegWit)이 적용된 이후부터 weight의 단위는 kWU를 사용합니다. 세그윗은 블록 내에 최대한 거래를 포함시키면서 동시에 거래 거래 가변성(Transaction Malleability) 문제를 해결하기 위한 기법으로, 이 기법이 적용된 이후에는 거래의 크기를 byte(b)가 아니라 weight unit(wu)로 표현합니다. <br> <br>
      서명을 제외한 부분은 1 b당 4 WU, witness 영역은 1 b당 1 WU로, 한 블록은 4,000 kWU로 제한됩니다. 그러나 오랫동안 byte 단위를 사용했기에 WU는 적응하기 어려운 부분이 있어 WU를 byte 단위로 표현한 vbyte로 표현하기도 합니다 (1 vbyte = 4 WU).<br><br>
      거래의 크기를 구하는 예를 들어보면, P2PKH 거래의 크기가 250 byte라면 이 거래의 크기는 1000 WU이고, 250 vbyte가 됩니다. P2WPKH 거래의 크기가 250 byte로 동일하지만 서명 부분이 80 byte라면, 이 거래의 크기는 760 WU이고, 190 vbyte가 됩니다.<br><br>
      > Weight(kWU)<br>
The Weight column is related to SegWit (a backwards-compatible patch that increases the Bitcoin block size). Weight units are a measurement used to compare the size of different Bitcoin transactions to each other in proportion to the consensus-enforced maximum block size limit. Weight units are also used to measure the size of other block chain data, such as block headers.<br><br>
      > https://medium.com/coinmonks/understanding-some-bitcoin-metrics-ac51f28a4145<br>
      * Weight는 비트코인의 제한된 블록 사이즈 내에 더 많은 거래를 넣기 위한 기법인 SegWit과 관련이 있습니다. Weight 단위는 두 가지 용도로 사용되는데, 블록 내에 기록되어 있는 거래량을 비교하는데 사용되거나, 블록 헤더의 데이터 양을 측정하기 위해 사용됩니다. Weight에서 사용하는 공식 단위는 KWD 입니다.<br>
   * "height": (int) The block height or index<br>
      * 메인 체인 내에서의 블록의 위치를 나타내며, 인덱스라고도 불립니다. <br>
   * "version": (int) The block version<br>
      * 블록의 버전을 나타냅니다.<br><br>
      > Version 1 was introduced in the genesis block (January 2009).<br>
Version 2 was introduced in Bitcoin Core 0.7.0 (September 2012) as a soft fork. As described in BIP34, valid version 2 blocks require a block height parameter in the coinbase. Also described in BIP34 are rules for rejecting certain blocks; based on those rules, Bitcoin Core 0.7.0 and later versions began to reject version 2 blocks without the block height in coinbase at block height 224,412 (March 2013) and began to reject new version 1 blocks three weeks later at block height 227,930.<br><br>
Version 3 blocks were introduced in Bitcoin Core 0.10.0 (February 2015) as a soft fork. When the fork reached full enforcement (July 2015), it required strict DER encoding of all ECDSA signatures in new blocks as described in BIP66. Transactions that do not use strict DER encoding had previously been non-standard since Bitcoin Core 0.8.0 (February 2012).<br><br>
Version 4 blocks specified in BIP65 and introduced in Bitcoin Core 0.11.2 (November 2015) as a soft fork became active in December 2015. These blocks now support the new OP_CHECKLOCKTIMEVERIFY opcode described in that BIP.<br><br>
The mechanism used for the version 2, 3, and 4 upgrades is commonly called IsSuperMajority() after the function added to Bitcoin Core to manage those soft forking changes. See BIP34 for a full description of this method.<br><br>
As of this writing, a newer method called version bits is being designed to manage future soft forking changes, although it’s not known whether version 4 will be the last soft fork to use the IsSuperMajority() function. Draft BIP9 describes the version bits design as of this writing, although it is still being actively edited and may substantially change while in the draft state.<br><br>
https://bitcoin.org/en/developer-reference#block-versions<br>
      * version 1은 값이 1이고, genesis block(2009년 1월)에서 도입되었습니다.<br>
      * version 2는 값이 2이며, 비트코인 코어 0.7.0(2012년 9월)에서 소프트 포크로 도입되었습니다. <br>
      * version 3은 값이 3이고, 비트코인 코어 0.10.0(2015년 2월)에서 소프트 포크로 도입되었습니다.<br>
      * version 4는 값이 4이며, 비트코인 0.11.2(2015년 11월)에 소프트 포크로 도입되었습니다. <br><br>
      > It is used for signaling to activate soft forks. The first block version is version 1. Previous soft forks used a system called IsSuperMajority (ISM). These used fixed version numbers, consuming numbers 2, 3, and 4. We now use a new soft fork mechanism called BIP 9 Versionbits. BIP 9 specifies that all version numbers must have the top 3 bits of the version number be 001, which results in a version number of 0x20000000. From there, other bits are set for various soft forks, and unset once the fork has activated. For example, segwit uses bi 1, so setting bit 1 results in a version number of 0x20000002.<br><br>
      https://bitcointalk.org/index.php?topic=2052931.0<br>
      * version 필드는 소프트 포크를 동작시키기 위한 신호로써 사용되는데, 제네시스 블록의 버전 같은 경우에는 1을 사용합니다. 이전 소프트 포크들은 ISM이라는 시스템콜을 사용하여 version 번호를 2, 3, 4 중에서 변경하여 사용하였습니다.<br><br>
      오늘날 소프트 포크에는 BIP 9 메커니즘을 사용하는데, BIP 9는 모든 version 번호를 전체 32비트 중 상위 세 비트를 001로 고정해야 합니다. 다른 비트들은 다양한 소프트 포크들의 목적에 따라 설정하면 되고, 해당 포크가 활성화되면 설정한 비트를 해제합니다. 예를 들어, 세그윗(SegWit)을 사용하기 위한 소프트 포크를 진행할 경우, 세그윗은 1번 비트를 세팅(하위 두 번째 비트)합니다. 따라서 version 번호는 0x2000 0002가 됩니다.<br><br>
      > BIP-9 표준은 Soft Fork가 진행될 때 Miner들이 해당 기능을 지원할 준비가 되었는지를 Version에 표시하도록 권고하고 있음 (강제 사항은 아님).<br><br>
      32 bit Version의 처음 3 비트는 001로 시작하고, 나머지 29개 비트는 Soft Fork가 진행될 때 해당 bit를 세트함. SegWit의 경우는 비트 1 이었으므로 version = 0x20000002 이었음. 특정 비트가 세트된 블록이 얼마나 많은지를 파악하면, 얼마나 많은 Miner들이 해당 Soft Fork를 지원하는지 파악할 수 있음.<br><br>
      동시에 29개 Soft Fork 진행 가능.<br><br>
      아래 예시에서 2016.11.1~2017.11.15 까지 0x20000002로 생성된 블록의 변화를 관찰할 수 있음. 현재는 Soft Fork가 없는 0x20000000 으로 Mining되고 있음<br><br>
      http://www.smallake.kr/wp-content/uploads/2018/08/%EB%B9%84%ED%8A%B8%EC%BD%94%EC%9D%B8V1.10.pdf<br>
      > Coinbase Tx는 ‘채굴자’가 ‘본인 주소로’ ‘채굴 중인 블록의 높이에 맞는 보상액 + 블록에 담은 거래들의 수수료’를 설정 한 거래다.<br>
      다음과 같은 거래를 채굴자가 직접 생성하여 본인이 만들고 있는 블록에 함께 담아 채굴에 성공할 경우, 보상과 거래들의 수수료를 얻게 되는 것이다. 즉, 분산화된 환경에서 보상과 수수료를 채굴자에게 주어지는 과정은 이러한 과정을 통해서 진행되는 것이다.<br><br>
      https://brunch.co.kr/@skkrypto/3<br>
   * "versionHex": (int) The block version formatted in hexadecimal<br>
      * 블록의 버전을 16진수 값 형태로 표현한 값입니다. <br>
   * "merkleroot": (hex, char[64]) The merkle root<br>
      * 블록에 포함되어 있는 거래들을 머클 트리구조에 대입하여 얻은 루트 해쉬값입니다. <br>
   * "tx": (hex, char[n][64]) The transaction ids{"transactionid": (string) The transaction id}<br>
      * 블록에 포함되어 있는 거래들의 목록을 나타내며, 문자열들의 목록 형태로 표현됩니다. 이때 각 문자열들은 거래의 고유 번호(Tx ID)를 나타냅니다.<br> 
   * "time": (int) The block time in seconds since epoch (Jan 1 1970 GMT)<br>
      * 유닉스 시간을 기준으로 해당 블록이 언제 생성되었는지 초로 나타냅니다.<br><br>
      > Each block contains a Unix time timestamp. In addition to serving as a source of variation for the block hash, they also make it more difficult for an adversary to manipulate the block chain.<br><br>
      A timestamp is accepted as valid if it is greater than the median timestamp of previous 11 blocks, and less than the network-adjusted time + 2 hours. "Network-adjusted time" is the median of the timestamps returned by all nodes connected to you. As a result block timestamps are not exactly accurate, and they do not need to be. Block times are accurate only to within an hour or two.<br><br>
      Whenever a node connects to another node, it gets a UTC timestamp from it, and stores its offset from node-local UTC. The network-adjusted time is then the node-local UTC plus the median offset from all connected nodes. Network time is never adjusted more than 70 minutes from local system time, however.<br><br>
      Bitcoin uses an unsigned integer for the timestamp, so the year 2038 problem is delayed for another 68 years.<br><br>
      https://en.bitcoin.it/wiki/Block_timestamp<br>
      * 각 블록에는 유닉스 시간 값을 저장하는 time(timestamp) 필드가 존재합니다. 이 필드는 블록 해쉬 값을 계산할 때 사용됩니다. time 값은 블록 해쉬 값을 보다 더 다양하게 해주는 소스(source)로써 사용되므로, 블록 체인을 조작하기에 더욱 어렵게 합니다. <br><br>
      time 값은 두 가지 조건을 만족해야 유효한 값으로 인정됩니다. 우선 생성될 블록 내에 들어갈 time 값은 이전 11 개의 블록들의 time 값의 중간값(median)보다 커야 합니다. 그리고 네트워크 조정 시간에 두 시간 더한 값보다 작아야 합니다. <br><br>
      여기서 말하는 네트워크 조정 시간(Network-adjusted time)이란 저와 연결되어 있는 모든 노드들의 time 값 중에서의 중간값(median)을 나타냅니다. 블록 내의 time 값은 블록 생성 시간과 정확히 일치하지 않으며, 정확히 일치해야 할 필요도 없습니다. 블록의 time 값들은 한 시간 또는 두 시간 이내에만 있다면 정확하다고 간주됩니다.<br><br>
      예를 들면, A 노드에 B 노드가 연결되면 B 노드로부터 UTC time 값을 얻습니다. 이 값과 A 노드의 local UTC time(해당 노드에서 사용하는 시간, 절대 시간이 아닌 상대 시간이 됨) 값과의 차이를 계산한 후, A 노드에 저장합니다. 이런식으로 모든 노드들과의 UTC time 값과 A 노드의 UTC time 값과의 차이를 계산하고, A 노드의 local UTC time 값에 차이값 중에서의 중간값(median)을 더한 값이 결국 네트워크 조정 시간이 됩니다. 그러나 이 시간은 A 노드의 local 시간에서 70분을 초과할 수 없습니다.<br><br>
      유닉스 시간을 사용하면 Y2K38(자료형의 범위가 초과되어 오류가 나는 문제) 문제가 발생할 수 있습니다. 비트코인에서는 unsigned integer 형을 사용하므로, Y2K38문제를 2038년에서 68년을 더한 2106년까지 지연시킬 수 있습니다.<br>
   * "mediantime": (int) The median block time in seconds since epoch (Jan 1 1970 GMT)<br>
      * 유닉스 시간을 기준으로 블록 시간의 중간값을 초로 나타냅니다.<br><br>
      > mediantime is the median timestamp of the previous 11 blocks. Check out BIP113 for more information.<br><br>
      https://github.com/bitcoin-studio/Bitcoin-Programming-with-BitcoinJS/blob/master/09_1_P2SH_CLTV.md<br>
      * 구체적으로 설명하자면, mediantime 값은 유닉스 시간을 기준으로 해당 블록의 이전 11개의 블록들의 time 값 중 중간값을 초로 나타낸 값입니다.<br>
   * "nonce": (unsigned int) The nonce<br>
      * 블록 해쉬를 계산하기 위해 사용되는 필드입니다. 블록 해쉬값 생성에 필요한 입력 값은 총 6개(version, previousblockhash, merklehash, time, bits, nonce)이며, 이 중에서 nonce를 제외한 값들은 고정되어 있습니다. 블록 해쉬 값은 정해진 목표에 만족하는 값을 생성해야 하는데, 목표를 만족하는 값이 나올 때까지 nonce 값을 0부터 1씩 차례대로 증가하여 해쉬 함수에 입력값으로 넣습니다.<br>
   * "bits": (unsigned int) The bits<br>
      * difficulty 값을 조절하는 값입니다.<br>
   * "difficulty": (double) The difficulty<br>
      * 블록을 채굴하는 난이도를 나타냅니다. 난이도를 계산하는 수식은 아래와 같습니다.<br>
      ~~~python
      '''
      [formula] target = coefficient * 2^(8 * (exponent — 3))
      
      0x1725bb76 = 388348790

      maximum_target = 0x00000000FFFF0000000000000000000000000000000000000000000000000000 
      difficulty  = maximum_target / current_target
      current_target : b * 2 ** (8 * (a - 3))
          example) 0x1725bb76
          a = 0x17
          b = 0x25bb76
      '''
      maximum_target = 0x00000000FFFF0000000000000000000000000000000000000000000000000000

      a = 0x17
      b = 0x25bb76
      current_target = b * 2 ** (8 * (a - 3))

      difficulty = maximum_target / current_target

      print(difficulty)       //  7459680720542.296
      print(current_target)   //  3614033401827878015998272335407144409231622422786998272
      hexStr = "%0.64X" % current_target
      print(hexStr)           //  00000000000000000025BB760000000000000000000000000000000000000000
      
      ~~~
      > 목표값은 2^256 / 난이도 이며, 난이도가 높아질수록 목표값이 작아지기 때문에, 마이닝시에는 더 작은 목표값을 만족하는 nonce 값을 찾기위해 더많은 hash 연산을 수행해야 하기 때문에 블록생성시간이 길어지게 됩니다.<br><br>
      즉, 난이도가 낮으면 목표값이 커지고(0x0011111111....) 목표값을 만족하는 nonce를 찾기가 쉬워져서 블록생성시간이 짧아집니다.<br><br>
      난이도가 높으면 목표값이 작아지고(0x00000000111...) 목표값을 만족하는 nonce를 찾기가 어려워져서 블록생생시간이 길어지게 됩니다.<br><br>
      https://ihpark92.tistory.com/51<br>
      * maximum_target 상수 값은 정해져있으며, 난이도 값에 따라 목표값이 정해진다. 수식<br>
      ~~~
      difficulty = maximum_target / current_target
      ~~~
      을 다시 표현하면 아래와 같이 표현할 수 있다.<br>
      ~~~
      current_target = maximum_target / difficulty
      ~~~
      난이도가 상승하면 목표값이 낮아지고, 난이도가 떨어지면 목표값이 높아진다. 목표값이 낮아지면 nonce를 발견할 가능성이 높아지며(즉 채굴이 쉬워지며), 목표값이 높아지면 nonce를 발견할 가능성이 낮아진다(즉 채굴이 어려워진다).<br>
   * "chainwork": (char[64]) Expected number of hashes required to produce the chain up to this block (in hex)<br>
      * 이 블록까지 체인을 생성하기 위해 필요한 해쉬들의 예상 값입니다.<br><br>
      > The chainwork value is really just the total amount of work in the chain.<br><br>
      It is the total number of hashes that are expected to have been necessary to produce the current chain, in hexadecimal.<br><br>
      Converting 0000000000000000000000000000000000000000000086859f7a841475b236fd to decimal, you get 635262017308958427068157, or 635262 exahashes.<br><br>
      At june 2014 hash rates (100 petahash/s), it would require only 73 days to perform that many hashes, while in reality it took over 5 years. The hash rate has been going up so fast however that the impact of more than a few months ago is negligible.<br><br>
      https://bitcoin.stackexchange.com/questions/26869/what-is-chainwork<br>
      * chainwork 값은 메인체인을 실제로 구성하는데 필요한 총 작업량을 나타냅니다. 이 값은 16진수 값으로 현재 메인 체인을 만들기 위해 필요한 해쉬의 총 수를 예상한 값입니다.<br><br>
        160000000000000000000000000000000000000000000086859f7a841475b236fd을 10진수로 변환하면, 635262017308958427068157를 얻을 수 있습니다(635262 exa-hashes). <br><br>
        2014년 6월 기준 해시레이트는 100 petahash/s인데, 이 해시레이트로 현재 메인 체인을 다시 생성한다고(즉, chainwork 값을 처리한다고) 가정하면 73일이 걸립니다. 하지만 실제로 이 메인 체인을 생성하기까지 5년 넘게 걸렸는데, 이는 해시 레이트가 매우 빠르게 상승하고 있기 때문에 몇 달 전의 해시 레이트로는 오늘날 해시 레이트에 영향을 거의 주지 못합니다.<br>
   * "nTx": (int) The number of transaction<br>
      * 블록에 포함되어 있는 총 거래 수를 나타냅니다. <br>
   * "previousblockhash": (char[64]) The hash of the previous block<br>
      * 이전 블록 해쉬값을 나타냅니다. 제네시스 블록이라면 이 필드는 나타나지 않습니다. <br>
   * "nextblockhash": (char[64]) The hash of the next block<br>
      * 다음 블록 해쉬값을 나타냅니다. 가장 최근에 생성된 블록(Best Block)이라면, 이 필드는 나타나지 않습니다. <br><br>
## getrawtransaction<br>
### getrawtransaction<br>
```
{
   "result":  
   {
      "txid":"0e3e2357e806b6cdb1f70b54c3a3a17b6714ee1f0e68bebb44a74b1efd512098",
      "hash":"0e3e2357e806b6cdb1f70b54c3a3a17b6714ee1f0e68bebb44a74b1efd512098",
      "version":1,
      "size":134,
      "vsize":134,
      "weight":536,
      "locktime":0,
      // 1. Coinbase인 경우
      "vin":[
         {
            "coinbase":"04ffff001d0104",
            "sequence":4294967295
         }
      ],
      // 2. 일반 거래인 경우
         {
            "txid": "4d214638d0843d696c9297590da3596ddf568bbd96308951703fe3b37dbc1dc4",
            "vout": 1,
            "scriptSig": {
               "asm": "3045022044d638560170fe50c6da56d00f15a4fcd2585b405dd5065259eda1ee036
               876b4022100da6f563e30d89dca68613f5549bbe6ab72bc8c1f7697d4fdbd92b83f0d67a6e0[ALL]
               0408ab86428401da35660a2ad2f8a797b9352bd04bff948d80ee81e4a5fbe7dca97c28e43d9
               9a1663badafa3774618ccbf3c5b20dd51897c88b6c3bba44b9a597f",
               "hex": "483045022044d638560170fe50c6da56d00f15a4fcd2585b405dd5065259eda1ee03
               6876b4022100da6f563e30d89dca68613f5549bbe6ab72bc8c1f7697d4fdbd92b83f0d67a6e0
               01410408ab86428401da35660a2ad2f8a797b9352bd04bff948d80ee81e4a5fbe7dca97c28e4
               3d99a1663badafa3774618ccbf3c5b20dd51897c88b6c3bba44b9a597f"
            },
            "sequence": 4294967295
         }
      ],
      "vout":[
         {
            "value":50.00000000,
            "n":0,
            "scriptPubKey":
            {
               "asm":"0496b538e853519c726a2c91e61ec11600ae1390813a627c66fb8be7947be63c
                     52da7589379515d4e0a604f8141781e62294721166bf621e73a82cbf2342c858ee OP_CHECKSIG",
               "hex":"410496b538e853519c726a2c91e61ec11600ae1390813a627c66fb8be7947be6
                     3c52da7589379515d4e0a604f8141781e62294721166bf621e73a82cbf2342c858eeac",
               "reqSigs":1,
               "type":"pubkey",
               "addresses":["12c6DSiU4Rq3P4ZxziKxzrL5LmMBrzjrJX"]
            }
         }
      ],
      "hex":"0100000001000000000000000000000000000000000000000000000000000000
            0000000000ffffffff0704ffff001d0104ffffffff0100f2052a010000004341
            0496b538e853519c726a2c91e61ec11600ae1390813a627c66fb8be7947be63c
            52da7589379515d4e0a604f8141781e62294721166bf621e73a82cbf2342c858
            eeac00000000",
      "blockhash":"00000000839a8e6886ab5951d76f411475428afc90947ee320161bbf18eb6048",
      "confirmations":582637,
      "time":1231469665,
      "blocktime":1231469665
   },
   "error":null,
   "id":null
}
```
* usage:<br>
   * getrawtransaction *txid*<br>
* result:<br>
   * "txid": (string) The transaction id (same as provided)<br>
      * 해당 거래의 ID를 나타냄<br>
   * "hash": (string) The transaction hash (differs from txid for witness transactions)<br>
      * 해당 거래의 ID를 나타내지만, 해당 거래가 segwit tx일 경우 witness data를 제외한 정보를 hashing한 값이므로 txid와 다른 값을 가진다. 반대로 해당 거래가 segwit tx<br>
      > **Summary: The difference is when the tx is a segwit tx, the calculation of hash does not include the witness data, whereas the txid does.**<br><br>
      The definition of transaction hash remains the same as txid for non-witness transactions (non Segwit). The reason the mainnet tx has a different hash than the txid is because it is a witness transaction. If you run bitcoin-cli help getrawtransaction it will show you:
      ```bash
      Result (if verbose is set to true):
      {
        ...
        "txid" : "id",        (string) The transaction id (same as provided)
        "hash" : "id",        (string) The transaction hash (differs from txid for witness transactions)
        ...
      ```
      > **Segregated Witness**<br>
      The following is from [BIP 141](https://github.com/bitcoin/bips/blob/master/bip-0141.mediawiki).<br><br>
      Note that the BIP states, "signature data is no longer part of the transaction hash."<br><br>
      **Transaction ID**<br>
      A new data structure, *witness*, is defined. Each transaction will have 2 IDs.<br><br>
      Definition of *txid* remains unchanged: the double SHA256 of the traditional serialization format:<br><br>
      ```bash
      [nVersion][txins][txouts][nLockTime]
      ```
      > A new *wtxid* is defined: the double SHA256 of the new serialization with witness data:<br><br>
      ```bash
      [nVersion][marker][flag][txins][txouts][witness][nLockTime]
      ```
      > Format of *nVersion, txins, txouts, and nLockTime* are same as traditional serialization.<br><br>
      The *marker* MUST be a 1-byte zero value: *0x00*.<br><br>
      The *flag* MUST be a 1-byte non-zero value. Currently, *0x01* MUST be used.<br><br>
      The *witness* is a serialization of all witness data of the transaction. Each txin is associated with a witness field. 
      A witness field starts with a *var_int* to indicate the number of stack items for the txin. It is followed by stack items, with each item starts with a *var_int* to indicate the length. Witness data is NOT script.<br><br>
      A non-witness program (defined hereinafter) txin MUST be associated with an empty witness field, represented by a *0x00*. If all txins are not witness program, a transaction's *wtxid* is equal to its *txid*.<br><br>
      [Whats the difference between “txid” and “hash” - getrawtransaction bitcoind](https://bitcoin.stackexchange.com/questions/77699/whats-the-difference-between-txid-and-hash-getrawtransaction-bitcoind)<br>

   * "version": (numeric) The version<br>
      * ..<br>
   * "size": (numeric) The serialized transaction size<br>
      * ..<br>
   * "vsize": (numeric) The virtual transaction size (differs from size for witness transactions)<br>
      * ..<br>
   * "weight": (numeric) The transaction's weight (between vsize*4-3 and vsize*4)<br>
      * ..<br>
   * "locktime": (numeric) The lock time<br>
      * ..<br>
   * "vin": (array of json objects)<br>
      * ..<br>
         *  Coinbase인 경우
         1. "Coinbase": The hex-encoded bytes of the signature script (coinbase txns only)<br>
            * <br>
         2. "sequence": (numeric) The script sequence number<br>
            * *현재 Sequence 값은 사용하지 않고, UNIT_MAX 값으로 고정해 사용 중이다. Sequence 값을 UNIT_MAX 값으로 사용하게 되면 locktime과는 상관 없이 바로 체인에 등록되며, 등록된 거래는 영원히 수정 불가 상태가 된다.*<br>
         * 일반 거래인 경우
         1. "txid": (string) The transaction id<br>
            * 이 계좌로 송금한 거래의 ID를 나타낸다.<br>
         2. "vout": (numeric) <br>
            * 한 거래 내에는 여러 계좌로 송금할 수 있기 때문에 여러 건의 송금이 존재할 수 있다. 각 송금마다 index가 부여되는데, vout은 이 계좌로 송금한 거래의 Index를 나타낸다. 만약 vout이 0이라면 아래의 "vout"의 n 값이 0인 거래를 의미하며, vout이 3이라면 "vout"의 n 값이 3인 거래를 의미한다. <br>
         3. "scriptSig": (json object) The script<br>
            * <br>
            1. "asm": (string) asm<br>
               * <br>
            2. "hex": (string) hex<br>
               * <br>
         4. "sequence": (numeric) The script sequence number<br>
            * *현재 Sequence 값은 사용하지 않고, UNIT_MAX 값으로 고정해 사용 중이다. Sequence 값을 UNIT_MAX 값으로 사용하게 되면 locktime과는 상관 없이 바로 체인에 등록되며, 등록된 거래는 영원히 수정 불가 상태가 된다.*<br>
         5. "txinwitness": ["hex", ...] (array of string) hex-encoded witness data (if any)<br>
            * <br>
   * "vout": (array of json objects)<br>
      * ..<br>
         * "value": (numeric) The value in BTC<br>
            * 현재 계좌로부터 다른 계좌로 전송할 BTC 값을 나타낸다. <br>
         * "n": (numeric) index<br>
            * 한 건의 거래 내부에는 여러 계좌로 BTC를 송금할 수 있기 때문에 여러 건의 송금이 포함될 수 있다. 따라서 각 송금 건마다 index가 부여되며, 이때 이 index가 n<br>
         * "scriptPubKey": (json object)<br>
            * <br>
               * "asm": (string) the asm<br>
                  * <br>
               * "hex": (string) the hex<br>
                  * <br>
               * "reqSigs": (numeric) The required sigs<br>
                  * <br>
               * "type": (string) The type, eg 'pubkeyhash'<br>
                  * <br>
               * "addresses": (json array of string)<br>
                  * <br>
                     * "address": (string) bitcoin address<br>
                        * <br>
                     * ...<br>
   * "hex": (string) The serialized, hex-encoded data for 'txid'<br>
      * ..<br>
   * "blockhash": (string) the block hash<br>
      * ..<br>
   * "confirmations": (numeric) The confirmations<br>
      * ..<br>
   * "time": (numeric) The transaction time in seconds since epoch (Jan 1 1970 GMT)<br>
      * ..<br>
   * "blocktime": (numeric) The block time in seconds since epoch (Jan 1 1970 GMT)<br>
      * ..<br><br>

## getblockchaininfo<br>
### getblockchaininfo<br>
```
{
   "chain":"main",
   "blocks":579172,
   "headers":579172,
   "bestblockhash":"0000000000000000000874611b720d16353a40ff91808388e2af963f4dcd3aba",
   "difficulty":7459680720542.296,
   "mediantime":1559636994,
   "verificationprogress":0.9999970393364542,
   "initialblockdownload":false,
   "chainwork":"00000000000000000000000000000000000000000684db32cc52a65468b5d969",
   "size_on_disk":253109128480,
   "pruned":false,
   "softforks":[
      {"id":"bip34","version":2,"reject":{"status":true}},
      {"id":"bip66","version":3,"reject":{"status":true}},
      {"id":"bip65","version":4,"reject":{"status":true}}],
   "bip9_softforks":{"csv":{"status":"active","startTime":1462060800,"timeout":1493596800,"since":419328},
   "segwit":{"status":"active","startTime":1479168000,"timeout":1510704000,"since":481824}},"warnings":""
}
```
* usage:<br>
   * getblockchaininfo<br>
* result:<br>
   * "txid": <br>
      * ..<br><br>
## getmempoolinfo<br>
### getmempoolinfo<br>
```
{
   "size":31894,
   "bytes":17553340,
   "usage":60861408,
   "maxmempool":300000000,
   "mempoolminfee":0.00001000,
   "minrelaytxfee":0.00001000
}
```
* usage:<br>
   * getmempoolinfo<br>
* result:<br>
   * "txid": <br>
      * ..<br><br>
## getmininginfo<br>
### getmininginfo<br>
```
{
   "blocks":579172,
   "difficulty":7459680720542.296,
   "networkhashps":4.702711524794712e+19,
   "pooledtx":32144,
   "chain":"main",
   "warnings":""
}
```
* usage:<br>
   * getmininginfo<br>
* result:<br>
   * "txid": <br>
      * ..<br><br>
## getnetworkinfo<br>
### 블록체인 네트워크 관련 정보를 제공합니다. <br>
```
{
   "version":180000,
   "subversion":"/Satoshi:0.18.0/",
   "protocolversion":70015,
   "localservices":"000000000000040d",
   "localrelay":true,
   "timeoffset":0,
   "networkactive":true,
   "connections":62,
   "networks":[
      {"name":"ipv4","limited":false,"reachable":true,"proxy":"","proxy_randomize_credentials":false},
      {"name":"ipv6","limited":false,"reachable":true,"proxy":"","proxy_randomize_credentials":false},
      {"name":"onion","limited":true,"reachable":false,"proxy":"","proxy_randomize_credentials":false}],
   "relayfee":0.00001000,
   "incrementalfee":0.00001000,
   "localaddresses":[
      {"address":"106.250.161.211","port":8333,"score":40671}],
   "warnings":""
}
```
* usage:<br>
   * getnetworkinfo<br>
* result:<br>
   * "txid": <br>
      * ..<br><br>
## getwalletinfo<br>
### 지갑과 관련한 정보를 제공합니다.<br>
* usage:<br>
   * getwalletinfo<br>
* result:<br>
   * "walletname": (string) the wallet name<br>
      * walletname은 지갑의 이름을 나타냅니다. <br>
   * "walletversion": (numeric) the wallet version<br>
      * walletversion은 지갑의 버전을 나타냅니다. <br><br>
      > The version of the wallet.dat file. Wallet.dat contains bitcoin addresses and public & private key pairs for these addresses. There is additional data on the wallet. Care must be taken to not restore from an old wallet backup. New addresses generated in the wallet since the old backup was made will not exist in the old backup!<br><br>
      https://bitcoin.stackexchange.com/questions/18033/what-does-getinfo-output-mean<br>
      * 정확하게는 wallet.dat 파일의 버전을 나타내며, wallet.dat 파일 안에는 비트코인 주소들과 그 주소들에 상응하는 공개 키와 개인 키 쌍들이 포함되어 있습니다. 예전에 백업한 지갑을 사용하여 복구를 하고자 할 때에는 주의해야 하는데, 이전 지갑으로 복구하게 되면 백업 이후에 생성된 새 주소는 사라지게 됩니다.<br>
   * "balance": (numeric) the total confirmed balance of the wallet in BTC<br>
      * 비트코인용 지갑의 승인된 전체 잔고를 나타냅니다. <br>
   * "unconfirmed_balance": (numeric) the total unconfirmed balance of the wallet in BTC<br>
      * 비트코인용 지갑의 미승인된 않은 전체 잔고를 나타냅니다. 여기서 말하는 미승인이란, 아직 블록이 메인 체인에 붙기 전 상태입니다. <br>
   * "immature_balance": (numeric) the total immature balance of the wallet in BTC<br>
      * 아직 승인이 이루어지지 않은 총 채굴한 블럭의 잔고를 나타냅니다. 즉, 채굴한 블록이 아직 메인 체인에 붙지 않았습니다. <br>
   * "txcount": (numeric) the total number of transactions in the wallet<br>
      * 지갑에서 이루어진 총 거래 횟수를 나타냅니다. <br>
   * "keypoololdest": (numeric) the timestamp (seconds since Unix epoch) of the oldest pre-generated key in the key pool<br>
      * 키들의 집합(keypool) 내에 미리 키를 생성해 놓는데, 이 중에서 가장 만들어진지 오래된 키의 생성 시간을 유닉스 시간으로 나타낸 값입니다.<br><br>
      > New public and private keys are pre-generated and stored in a queue before use.<br>
      This pooling feature was added so backups of the wallet would have a certain number of keys that would be used in the future. By default, the number of entries in the queue is 100.<br><br>
      https://en.bitcoin.it/wiki/Key_pool<br>
      * key pool이라는 기능은 새로운 공개 키와 개인 키 쌍(이하 키)들을 사용하기 전에 미리 생성하여 큐 안에 저장합니다. 큐 안에 저장되어 있는 키들은 차후에 지갑의 백업에 활용할 수 있습니다. 대기열의 항목 수는 기본값으로 100을 사용합니다.<br><br>
      > The date as Unix epoch time when the oldest key in the wallet key pool was created; useful for only scanning blocks created since this date for transactions<br><br>
      https://j1149.com/developers/pai_core_apis/get_wallet_info/<br>
      * keypoololdest은 생성된 여러 지갑 키들 중에서 가장 오래전에 생성된 키가 생성되었을 때의 UNIX 시간을 나타냅니다. 이 시간 이후에 생성된 블록들의 거래 내역을 검색할 경우에만 유용합니다.<br>
   * "keypoolsize": (numeric) how many new keys are pre-generated (only counts external keys)<br>
      * 키들의 집합에 몇 개의 키를 생성하여 저장할 것인지 나타냅니다. 이때 생성된 키는 외부 키의 갯수를 나타내며, 외부 키는 외부 체인을 생성할 때 사용됩니다. <br><br>
      > **m / purpose' / coin_type' / account' / change / address_index**<br><br>
      > * m : Seed로부터 생성된 마스터 키입니다.<br>
      > * purpose : BIP44의 44를 나타냅니다. BIP44는 계층 결정성 지갑(hierarchical deterministic wallet)이 어떻게 구현되는가에 대한 정의입니다.<br> 
      > * coin_type : 코인의 번호로써, BTC는 0번, ETH는 60번을 사용합니다. <br>
      > * account : 회계나 조직 목적을 위해 지갑을 하위 계좌로 세분화할 때 사용합니다. <br>
      > * change : 0은 내부 체인을 나타내고, 1은 외부 체인을 나타냅니다. 외부 체인은 지갑 외부에서 볼 수 있도록 의도된 주소(수신 주소, receiving address)를 뜻하며, 내부 체인은 지갑 외부에 표시 되지 않고 잔액 주소(change address)로 사용됩니다.<br>
      > * address : n번째 지갑을 나타냅니다.<br><br>
      https://medium.com/bitcraft/hd-wallets-explained-from-high-level-to-nuts-and-bolts-9a41545f5b0<br>
   * "keypoolsize_hd_internal": (numeric) how many new keys are pre-generated for internal use (used for change outputs, only appears if the wallet is using this feature, otherwise external keys are used)<br>
      * 키 집합에 내부에서 사용할 키를 몇 개 생성하여 저장해놓을 것인지 나타냅니다. 내부 키는 거래의 잔액들을 위해 사용되며, 지갑에서 이 기능을 사용하고자 할 때만 생깁니다. 사용하지 않는다면 외부 키들이 사용됩니다. <br>
      *(아직 확실하지 않지만, 이 기능은 거래 잔액들의 출력을 위해 사용되는 필드이며, 이 기능을 사용할 때 내부 키로써 사용되고 그렇지 않으면 이 키들이 전부 외부 키로 사용되는 것 같음.. 더 공부해서 이 부분 채워넣겠음)*
   * "paytxfee": (numeric) the transaction fee configuration, set in BTC/kB  "hdmasterkeyid": "<hash160>"     (string, optional) the Hash160 of the HD master pubkey (only present when HD is enabled)<br>
      * kb 당 몇 BTC를 거래 당 수수료로 사용할지 설정합니다.<br><br>
      > The minimum fee to pay per kilobyte of transaction; may be 0. Only returned if wallet suuport is enabled<br><br>
      http://doc.superbtc.org/en/developer-reference.html<br>
      * 구체적으로 표현하면, 거래의 kb 당 최소한의 거래 수수료를 나타내며, 0이 될 수도 있습니다. 지갑에서 이 기능을 지원해야만 이 필드 값을 반환합니다. <br>
   * "hdseedid": (string) the Hash160 of the HD seed (only present when HD is enabled)<br>
      * hdmasterkeyid 와 같으며, Hash160에 사용되는 HD seed 값입니다. 이때 HD 기능을 사용할 경우에만 나타납니다. 여기서 표현하는 seed는 난수를 생성할 때 사용하는 인자이며, 이 값이 바뀔 때마다 난수도 바뀝니다. <br><br>
      > The Hash160(Public Key) is a hashed version of your public key. It's the version of your public key that you give to other people so that they can send you bitcoins. It's shorter than the original public key, and it may provide an extra layer of security for your bitcoins compared to giving out your public key directly. It's also basically the "raw" version of an address.<br><br>
      https://learnmeabitcoin.com/glossary/public-key-hash160<br>
      * Hash160은 공개키를 해쉬 값으로 변환한 값입니다. 만약 제가 이 값을 다른 사람에게 보낸다면, 이 값을 사용하여 저에게 비트코인을 전송할 수 있습니다. 이 값을 사용하는 이유는 원래 공개 키보다 짧고, 또한 공개 키를 직접적으로 사용할 때보다 비트코인 보안에 사용되는 추가적인 계층을 제공받을 수도 있습니다. 이 값은 기본적으로 주소의 원본 값입니다.<br><br>
      > The hdseedid is the same as the hdmasterkeyid (the latter is retained for legacy reasons, it was renamed to hdseedid for better clarity). It is the HASH160 of the HD seed.<br><br>
      https://bitcoin.stackexchange.com/questions/81581/is-the-hdseedid-in-bitcoin-core-0-17-rpc-the-actual-seed-itself<br>
      * 보다 용어를 명확하게 하기 위해 hdmasterkeyid라는 이름을 hdseedid로 변경했습니다. <br>
   * "private_keys_enabled": (boolean) false if privatekeys are disabled for this wallet (enforced watch-only wallet)<br>
      * 이 지갑에서 개인 키들을 사용 못하게 할 경우 false 값으로 입력되며, 이 경우에는 읽기 전용 지갑이 됩니다. <br>
