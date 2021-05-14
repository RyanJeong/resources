#   Transactions<br>
##  Selecting batches of bitcoins and creating new ones from them.<br>

### What is a bitcoin transaction?<br>
>   A bitcoin transaction is a bunch of data.<br>
*   This data contains information about the amount being sent, the account it is being sent from, and the account it is being sent to.<br>
This is basic information, so it can be easily represented in a single line of data:<br>

![3-01](../img/3-01.png)

*   And when you “make a transaction”, you just send this transaction data in to the bitcoin network.<br>

![3-02](../img/3-02.png)

*   Eventually one of the nodes on the network will mine your transaction in to a block, and this block (with your transaction in it) will be added to the file of confirmed transactions (the blockchain).<br>
And that’s all a bitcoin transaction is – feeding a simple line of data in to the bitcoin network and waiting for it to be mined in to the blockchain.<br>

### How does a bitcoin transaction work?<br>
*   A bitcoin address is like an account number that holds bitcoins.<br>
But when you make a transaction, it’s not like taking an exact amount of coins out of a pot and moving them in to another.<br>
Instead, an address keeps track of each individual transaction it has received:<br>

![3-03](../img/3-03.png)

*   So when you want to send bitcoins to someone else, you grab whole amounts that you have already received, and use them to send a new amount to a new address:<br>

![3-04](../img/3-04.png)

*   And when that someone else wants to send bitcoins to another person, they will use up the whole amounts they have received in the same way:<br>

![3-05](../img/3-05.png)

*   So in effect you receive bitcoins in batches, and you use those batches to create new batches to send to other people.<br>
That’s how transactions work.<br>

### Hang on a minute…<br>
>   “What if the total of the batches adds up to more than I want to send?”<br>
*   Good question sir.<br>
In this instance (which it often is), you just add another output to the transaction and send the difference back to yourself:<br>

![3-06](../img/3-06.png)

*   This may seem awkward at first, I know, but it’s a **precise way of doing it from a programming perspective**, so get used to it.<br>

### Summary:<br>
1.  You have a bitcoin address. Bitcoins arrive at this address in batches, called outputs.<Br>
2.  A bitcoin transaction is the process of using these outputs (as inputs in the transaction) to create new outputs that belong to someone else’s address.<br>
3.  All of this can be represented by a single line of data.<br>

![3-07](../img/3-07.png)

### What prevents someone else from spending my bitcoins?<br>
*   Or in other words…<Br>
“If making a transaction is simply a case of feeding a line of data in to the bitcoin network, why can’t someone construct a transaction that includes my address and uses it to send bitcoins to their address?”<br>
**Answer**: Because each of the transaction outputs has a lock…<br>

![3-08](../img/3-08.png)

*   And if you create a transaction without first unlocking these outputs, nodes on the bitcoin network will reject the transaction.<br>

![3-09](../img/3-09.png)

*   But fortunately for you, your bitcoin address comes with a private key.<br>

![3-10](../img/3-10.png)

*   So if you want to send bitcoins somewhere, you use this private key to unlock the outputs located at your address.<br>

![3-11](../img/3-11.png)

*   After unlocking all of the outputs you want to use, the transaction will be accepted and propagated by nodes on the bitcoin network.<br>

![3-12](../img/3-12.png)

*   And that’s how bitcoin transactions work.<br>

##  Outputs<br>
### The new batches of bitcoins created in transactions.<br>

##  Output Locks<br>
### Locking mechanisms for batches of bitcoins.<br>


