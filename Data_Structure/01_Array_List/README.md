#   Array List<br>
##  Abstract Data Type(ADT)<br>
### An ADT is composed of<br>
*   A collection of data<br>
*   A set of operations on that data<br>

### Specifications of an ADT indicate<br>
*   What the ADT operations do, not how to implement them<br>

### Implementation of an ADT<br>
*   Includes choosing a particular data structure<br>
*   In other words, data structures are part of an ADT’s implementation<br>

### Data structure<br>
*   A construct that is defined within a programming language to store a collection of data<br>
*   Example: arrays<br>

### ADTs and data structures are not the same<br>

### Data abstraction<br>
*   Results in a wall of ADT operations between data structures and the program that accesses the data within these data structures<br>

### A wall of ADT operations isolates a data structure from the program that uses it<br>

##  Specifying ADTs<br>
### Array list is slower than linked list because a lot of shifting needs to occur if any node is added or removed from the array list.
*   0-based index<br>

| Operation              | Description                                                  | Input                         | Output                                 |
|:-----------------------|:-------------------------------------------------------------|:------------------------------|:---------------------------------------|
| createArrayList()      | Create an empty array list                                   | Maximum number of nodes n     | List l                                 |
| deleteArrayList()      | Delete the array list                                        | List l                        | N/A                                    |
| isArrayListFull()      | Determine whether an array list is full                      | List l                        | True or False                          |
| isArrayListEmpty()     | Determine whether an array list is empty                     | List l                        | True or False                          |
| addNode()              | Add an item at a given position in the array list            | List l, Position p, Node n    | Returns whether it succeeded or failed |
| addNodeFirst()         | Add an item at first position in the array list           | List l, Node n                | Returns whether it succeeded or failed |
| addNodeLast()          | Add an item at last position in the array list            | List l, Node n                | Returns whether it succeeded or failed |
| removeNode()           | Remove the item at a given position in the array list        | List l, Position p            | Returns whether it succeeded or failed |
| removeAll()            | Remove all the items from the array list                     | List l                        | N/A                                    |
| getArrayListLength()   | Determine the number of items in an array list               | List l                        | Number of nodes in the list n          |
| getArrayListCapacity() | Determine the number of items in the underlying array list   | List l                        | Number of nodes in the list n          |
| getNode()              | Retrieve(get) the item at a given position in the array list | List l, Position p            | Node n                                 |
| printArrayList()       | Print all nodes of the array list                            | List l                        | N/A                                    |
