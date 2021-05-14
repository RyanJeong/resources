#   Bitcoin Network<br>
###  The network of computers running the bitcoin program.<br>

####    What is the Bitcoin Network?<br>
*   The Bitcoin Network is made up of everyone running the bitcoin software (better known as “bitcoin clients”).<br>

####    What does the network do?<br>
*   People (well, bitcoin clients) on the network talk to each other.<br>
*   And by “talk to each other” I mean pass on information about what’s going on in other parts of the network.<br>
This is done by sending each other messages.<br>
For example, a message could be information about a new transaction.<br>

![1-01](../img/1-01.png)

*   This sharing of information (e.g. transactions) is what allows the everyone on the network to keep up-to-date, which is pretty important if you want to run a digital currency on the Internet.<br>

![1-02](../img/1-02.png)

>   The Bitcoin Network is described as a [peer-to-peer network](https://en.wikipedia.org/wiki/Peer-to-peer), because:<br>
1. Everyone is connected to each other, so it’s a network.<br>
2. Everyone on the network is equal, so we are peers.<br>

####    Who makes up the network?<br>
*   As mentioned, anyone with an active Internet connection and running a [bitcoin client](https://bitcoin.org/en/download).<br>
Seriously, anyone can join the bitcoin network.<br>
All you need is an internet connection and a bitcoin client, which is a piece of software like any other.<br>
And once you’re up and running you’ll be referred to as a node on the bitcoin network.<br>

##  Nodes<br>
### Computers running the bitcoin program.<br>

####    What is a node?<br>

![1-01-01](../img/1-01-01.png)

*   A node is just a computer that is running the Bitcoin program. More importantly, it is connected to other computers (running the same program) to create a [Network](#bitcoin-network).<br>

####    What does a node do?<br>
*   A node has three jobs:<br>
    1.  Follow rules<br>
    2.  Share information<br>
    3.  Keep a copy of confirmed transactions<br>

1.  Follow rules<br>
*   Each node (bitcoin client) has been programmed to follow a set of rules.<br>
By following these rules a node is able to check the transactions it receives and only relay them if everything is cool.<br>
If there are any problems, the transaction isn’t passed on.<br>

![1-01-02](../img/1-01-02.png)

*   For example, one rule is that a person must own an equal or greater amount of bitcoins than they are trying to send.<br>
So if your node receives a transaction where someone has tried to send more bitcoins than they own, the transaction won’t be passed on to other nodes.<br>

2.  Share information<br>
*   A node’s main job is to share information with other nodes, and the quintessential information a node shares is transactions.<br>
Now, there are two types of transactions that nodes share:<br>
    1.  Fresh transactions – transactions that have recently entered the network.<br>
    2.  Confirmed transactions – transactions that have been “confirmed” and written to a file. These are shared in blocks of transactions, and not individually.<br>

![1-01-03](../img/1-01-03.png)

3.  Keep a copy of confirmed transactions<br>
*   As mentioned, each node also keeps blocks of confirmed transactions. These are held together in a file called the [blockchain](../2_mining/README.md#the-blockchain).<br>

![1-01-04](../img/1-01-04.png)

*   Fresh transactions are bounced around the network until they are etched into the blockchain, which is a ledger of confirmed transactions.<br>
Each node has a copy of the blockchain for safe keeping, and shares it with other nodes if their copy isn’t up to date.<br>

*   The process of adding fresh transactions to the blockchain is called [Mining](../2_mining).<br>

>   I wasn’t to mention that each node is autonomous.<br>
By that I mean that when you run a bitcoin client, the network doesn’t “tell you what to do”.<br>
Instead, your bitcoin client already knows what to do, and it makes it’s own decisions.<br>
So the entire bitcoin network is made up of nodes making their own decisions, but they each make the same decisions as one another, which makes it a completely decentralised yet powerful network.<br>
>>  If every other node went offline, your node would be upholding the entire bitcoin network.<br>

####    Do I have to be a node to use bitcoin?<br>
*   **No.**<br>
You can send and receive bitcoins without having to be a node. You just need to get the transaction into the bitcoin network and you’re good to go.<br>

![1-01-05](../img/1-01-05.png)

*   If you’re using a web wallet for example, they will feed the transactions you make into the network for you.<br>
