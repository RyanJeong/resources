<pre><code>
</code></pre>
# UTXO (Unspent Transaction Outputs)

Bitcoin Core 내부에는 트랜잭션 자료구조가 구현되어 있으며, 이 자료구조 내에 CTxIn 클래스와 CTxOut 클래스가 있습니다. 

<pre><code>
class CTxIn
{
public:
    COutPoint prevout;
    CScript scriptSig;
    uint32_t nSequence;
    CScriptWitness scriptWitness; //! Only serialized through CTransaction

    /* Setting nSequence to this value for every input in a transaction
     * disables nLockTime. */
    static const uint32_t SEQUENCE_FINAL = 0xffffffff;

    /* Below flags apply in the context of BIP 68*/
    /* If this flag set, CTxIn::nSequence is NOT interpreted as a
     * relative lock-time. */
    static const uint32_t SEQUENCE_LOCKTIME_DISABLE_FLAG = (1 << 31);

    /* If CTxIn::nSequence encodes a relative lock-time and this flag
     * is set, the relative lock-time has units of 512 seconds,
     * otherwise it specifies blocks with a granularity of 1. */
    static const uint32_t SEQUENCE_LOCKTIME_TYPE_FLAG = (1 << 22);

    /* If CTxIn::nSequence encodes a relative lock-time, this mask is
     * applied to extract that lock-time from the sequence field. */
    static const uint32_t SEQUENCE_LOCKTIME_MASK = 0x0000ffff;

    /* In order to use the same number of bits to encode roughly the
     * same wall-clock duration, and because blocks are naturally
     * limited to occur every 600s on average, the minimum granularity
     * for time-based relative lock-time is fixed at 512 seconds.
     * Converting from CTxIn::nSequence to seconds is performed by
     * multiplying by 512 = 2^9, or equivalently shifting up by
     * 9 bits. */
    static const int SEQUENCE_LOCKTIME_GRANULARITY = 9;

    CTxIn()
    {
        nSequence = SEQUENCE_FINAL;
    }

    explicit CTxIn(COutPoint prevoutIn, CScript scriptSigIn=CScript(), uint32_t nSequenceIn=SEQUENCE_FINAL);
    CTxIn(uint256 hashPrevTx, uint32_t nOut, CScript scriptSigIn=CScript(), uint32_t nSequenceIn=SEQUENCE_FINAL);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(prevout);
        READWRITE(scriptSig);
        READWRITE(nSequence);
    }

    friend bool operator==(const CTxIn& a, const CTxIn& b)
    {
        return (a.prevout   == b.prevout &&
                a.scriptSig == b.scriptSig &&
                a.nSequence == b.nSequence);
    }

    friend bool operator!=(const CTxIn& a, const CTxIn& b)
    {
        return !(a == b);
    }

    std::string ToString() const;
};

/** An output of a transaction.  It contains the public key that the next input
 * must be able to sign with to claim it.
 */
class CTxOut
{
public:
    CAmount nValue;
    CScript scriptPubKey;

    CTxOut()
    {
        SetNull();
    }

    CTxOut(const CAmount& nValueIn, CScript scriptPubKeyIn);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(nValue);
        READWRITE(scriptPubKey);
    }

    void SetNull()
    {
        nValue = -1;
        scriptPubKey.clear();
    }

    bool IsNull() const
    {
        return (nValue == -1);
    }

    friend bool operator==(const CTxOut& a, const CTxOut& b)
    {
        return (a.nValue       == b.nValue &&
                a.scriptPubKey == b.scriptPubKey);
    }

    friend bool operator!=(const CTxOut& a, const CTxOut& b)
    {
        return !(a == b);
    }

    std::string ToString() const;
};
</code></pre>
철수(송금자)가 영희(수금자)에게 5 BTC를 송금하는 것으로 예를 들어보겠습니다.
철수는 영희의 주소를 알고 있으며, 아래와 같이 트랜잭션을 만들 수 있습니다.

<pre><code>
class CTxIn
{
public:
    COutPoint prevout;  //  철수의 공개키가 입력된 CTxOut을 가리키는 포인터
    CScript scriptSig;  //  철수의 서명(primary key)
    ...
}

class CTxOut
{
public:
    CAmount nValue;       //  전송할 비트코인의 양(5 BTC)
    CScript scriptPubKey; //  영희의 공개키
    ...
}
</code></pre>
위의 트랜잭션이 블록 내에 포함되면 영희는 후에 이 CTxOut을 참조해 5 BTC를 사용할 수 있습니다.
<br>
<br>
CTxIn은 가리킬 수만 있고 비트코인의 양을 지정할 수 없습니다. 즉 CTxOut은 무조건 한 번에 소비됩니다. 만약 영희가 민수에게 3 BTC만 주고 싶다면, [5 BTC의 CTxIn] -> [3 BTC의 CTxOut], [2 BTC의 CTxOut]의 트랜잭션을 만들어 3 BTC는 민수에게, 2 BTC는 자기 자신에게 돌려주면 됩니다. 
<br>
<br>
영희의 입장에서 2 BTC 거스름돈을 받을 때 공개키를 재사용할 이유가 없습니다. 따라서 영희는 새로운 키쌍을 생성해 해당 키를 사용하여 돌려 받습니다. 이 과정에서 이 거래와 상관 없는 영선은 블록체인에 기록된 거래 내역을 봤을 때 무엇이 거스름돈인지 알 수가 없습니다. 심지어 거스름돈 없이 수금자가 2명인 가능성도 배제할 수 없습니다. 이 UTXO 모델은 프라이버시 측면에서 장점이 있습니다.
<br>
<br>
비트코인 지갑이나 BlockExplorer에서 보여주는 잔고(balance)는 하나 이상의 UTXO의 합이라 할 수 있습니다. 비트코인 트랜잭션은 여러 UTXO를 조합하여 만들 수 있습니다. 예를 들어, 철수가 10 BTC, 5 BTC, 3 BTC의 UTXO를 가지고 있고, 영희에게 6 BTC를 줘야 하는 상황이라면, 
    *   10 BTC를 사용한다
        *   Balance: (10 - 4) + 5 + 3 BTC
    *   5 BTC + 3 BTC를 사용한다
        *   Balance: 10 + (5 + 3 - 6) BTC
    *   10 BTC + 5 BTC + 3 BTC를 사용한다
        *   Balance: (10 + 5 + 3 - 6) BTC
        
<br>
UTXO 조합에 따라 트랜잭션 바이트 크기가 달라지고, 바이트 크기가 작을수록 채굴자에게 지불하는 수수료를 낮출 수 있기 때문에 위의 조합 문제를 풀어야 하는 요구 사항이 있습니다. 
<br>

# 결론
비트코인을 보유하는 사람은 공개키를 여러 개 소유할 수 있습니다. 이 공개키들은 하나 이상의 UTXO와 연결될 수 있으나, 프라이버시 측면에서 하나의 공개키 당 하나의 UTXO만 연결되는 것이 바람직합니다. 보유자는 이 UTXO를 조합하여 사용할 수 있습니다.
<br>
<br>
사용자 입작에서 UTXO 모델은 이해하기 더 어렵고, 위에서 설명한 조합 문제나 UTXO set을 관리하는 문제 등 기술적인 문제들도 존재합니다. 하지만 트랜잭션의 병렬 처리나 프라이버스 측면에서 이더리움의 어카운트(Account)처럼 주소가 재사용되는 구조에 비해 장점이 분명히 존재합니다.
<br>

### 참고
*   https://joojis.tistory.com/entry/%EB%B9%84%ED%8A%B8%EC%BD%94%EC%9D%B8-%EC%A3%BC%EC%86%8C%EC%97%90-%EB%8C%80%ED%95%9C-%EC%9D%B4%ED%95%B4-1-UTXO-%EB%AA%A8%EB%8D%B8?category=776808
