package consensus

/*  
 *  Words
 *  View    :   primary node p가 primary node로서 활동하는 기간의 단위를 의미한다.
 *              view v의 primary는 (v mod N)번 node가 담당하며, Raft의 term number와 유사한 개념이다. 
 *  Primary :   client로부터 message를 받고, 이 message의 순서를 정해주는 역할을 수행한다. 
 *  Backup  :   cluster의 primary를 제외한 다른 모든 node들을 의미한다. 
 *
 *  Notation
 *  (MSG)s  :   MSG(message) signed by ~
 *
 *  Simple sequence
 *  Request -> to total order requests(Pre-prepare -> Prepare) -> prepared -> Commit -> Committed -> reply
 *  CLIENT -> PRIMARY
 *  RequestMsg          MsgRequest
 *      (REQUEST, o, t, c)sc    
 *              : A client sends a service request request to the primary
 *      o       :   requested operation
 *      t       :   timestamp
 *      c       :   client identity
 *      sc      :   signed by 'c'
 * 
 *  PRIMARY -> BACKUPS
 *  PrePrepareMsg       MsgPrePrepare
 *      ((PRE-PREPARE, v, n, d)sp, m)   
 *          : The primary assigns a unique sequence number and multicasts this message to all backups
 *            * A backup will accept the message iff
 *              1) v, n, d are valid
 *              2) n is between h and H
 *                  - h is lower bound and H is upper bound
 *              3) digest(m) is different from digest of other messages
 *      v   :   view number
 *      n   :   sequence number
 *      d   :   m's digest(hash)
 *      m   :   client's requested message
 *      sp  :   signed by 'p'
 *
 *  BACKUP i -> BACKUPS
 *  PrepareMsg          MsgPrepare
 *      (PREPARE, v, n, d, i)si
 *          : If backup 'i' accepts PRE-PREPARE message,
 *            it enters the prepare phase by multicasting PREPARE message to all other backups
 *      v   :   view number
 *      n   :   sequence number
 *      d   :   m's digest(hash)
 *      i   :   node 'i' (backup number)
 *      si  :   signed by 'i'
 *  
 *      Prepared(m, v, n, i) 
 *            * Prepared(m, v, n, i) is true iff backup 'i'
 *              1) PRE-PREPARE for message m has been received
 *              2) 2f distinct and valid PREPARE messages received
 *      m   : message
 *      v   : view number
 *      n   : sequence number
 *      i   : node 'i' (backup number)
 *  
 *  BACKUP i -> BACKUPS
 *  CommitMsg           MsgConmmit
 *      (COMMIT, v, n, d, i)si
 *          : Backup 'i' multicasts a COMMIT message to the other backups when prepared(m, v, n, i) becomes true
 *      v   : view number
 *      n   : sequence number
 *      d   : m's digest(hash)
 *      i   : node 'i' (backup number)
 *      si  : signed by 'i'
 *
 *      Committed(m, v, n)
 *            * Committed(m, v, n) is true iff
 *              1) Prepared(m, v, n, i) is true for all i in some set of f + 1 non-faulty backups
 *      m   : message
 *      v   : view number
 *      n   : sequence number 
 *
 *      Committed-local(m, v, n, i)
 *            * Committed-local(m, v, n, i) is true iff
 *              1) Prepared(m, v, n, i) is true
 *              2) i has accepted 2f + 1 commits (including itself) from different backups
 *      m   : message
 *      v   : view number
 *      n   : sequence number 
 *      i   : node 'i'
 *      
 *  ReplyMsg            MsgReply
 *      (REPLY, v, t, c, i, r)si
 *          : Each backup i executes the operation requested by client
 *          : After executing the operation, backups send a reply to the client
 *          : Client waits for f + 1 replies
 *      v   : view number
 *      t   : timestamp
 *      c   : client identity
 *      i   : node 'i' (backup number)
 *      r   : execution result
 *
 *  Every node saves a log of Pre-prepare, Prepare, Commit in their storage
 *      Reason      : Nodes may miss some messages
 *      Problem     : Limited storage size
 *      Solution    : make a checkpoint and discard the message below the checkpoint  
 *                  : Multicast (CHECKPOINT, n, d, i)si
 *
 *  ?                   MsgCheckPoint
 *      (CHECKPOINT, n, d, i)si
 *          : Collect 2f + 1 CHECKPOINT messages
 *          : After completing each checkpoint, discard the messages below n
 *          : Update the bound of sequence number;
 *              1) lower bound h = n
 *              2) upper bound H = n + k, k = some constant k
 *      n   : sequence number
 *      d   : m's digest(hash)
 *      i   : node 'i' (backup number)
 *      si  : signed by 'i'
 *      
 *  ViewChangeMsg       MsgViewChange
 *      (VIEW-CHANGE, v + 1, n, C, P, Q, i)si
 *              : Backups monitor the primary to prevent faulty primaries
 *              : Backups propose a view change when a timer expires
 *                  1) View change protocol is started if 2f + 1 backups do not have a valid message from the primary v within the timer
 * 
 *      v + 1   : change view number from v to (v + 1)
 *      n       : sequence number of current checkpoint
 *      C       : 2f checkpoint messages
 *      P       : set of Pre-prepared(m, v, n', i), n' > n (n = i's current checkpoint)
 *      Q       : set of prepared(m, v, n', i),     n' > n (n = i's current checkpoint)
 *      i       : node 'i' (backup number)
 *      si      : signed by 'i'
 *
 *  viewChangeClameMsg  MsgViewChange? Maybe New-view state..
 *      (New-view, v+1, V, O)sp
 * 
 *  VoteMsg             MsgVote
 *
 *  MsgType             Msg
 * 
 */
