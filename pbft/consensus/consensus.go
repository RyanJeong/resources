package consensus

type msg int

/*  Request - (REQUEST, o, t, c)s_c
 *  ------------------------
 *  o   string  `json:"o"`  :   operation, 요청하고 싶은 명령
 *  t   int64   `json:"t"`  :   timestamp, 클라이언트 자신의 타임스탬프
 *  c   string  `json:"c"`  :   client ID, 클라이언트를 식별할 수 있는 ID
 */
type Request struct {
    o   string  `json:"o"`
    t   int64   `json:"t"`
    c   string  `json:"c"`
}

/*  PrePrepare - ((PRE-PREPARE, v, n, d)s_p, m)
 *  ------------------------
 *  v   int64       `json:"v"`  :   view number, primary node p가 primary 노드로서 활동하는 기간의 단위를 나타내며,
 *                                  view v의 primary node는 (v mod N)번 노드가 담당함
 *  n   int64       `json:"n"`  :   sequence number, 몇 번째 request인지 나타내는 숫자값
 *  d   string      `json:"d"`  :   digest, 클라이언트로부터 도착한 메세지의 hash digest값
 *                                  (a short written report providing the most important parts of a larger piece
 *                                  of writing, or one containing recent news, cambridge)
 *  m   *Request    `json:"m"`  :   클라이언트로부터 도착한 메세지
 */
type PrePrepare struct {
	v   int64       `json:"v"`
	n   int64       `json:"n"`
	d   string      `json:"d"`
	m   *Request    `json:"r"`
}

const PrepareMsg msg = iota
const CommitMsg

type PBFT interface {
	Request(request *RequestMsg) (*PrePrepareMsg, error)
	PrePrepare(prePrepareMsg *PrePrepareMsg) (*VoteMsg, error)
	Prepare(prepareMsg *VoteMsg) (*VoteMsg, *ViewChangeMsg, error)
	Commit(commitMsg *VoteMsg) (*ReplyMsg, *RequestMsg, error)
	ViewChange(viewChangeMsg *ViewChangeMsg) (*ViewChangeMsg, error)
	NewView(viewChangeMsgClame *ViewChangeClameMsg) error
}

type ViewChangeMsg struct {
	ClientID      string `json:"clientID"`
	SequenceID    int64  `json:"sequenceID"`
	Digest        string `json:"digest"`
	ViewID        int64  `json:"viewID"`
	NextPrimaryID string `json:"nextPrimaryID"`
}

type ViewChangeClameMsg struct {
	ClientID   string `json:"clientID"`
	SequenceID int64  `json:"sequenceID"`
	Digest     string `json:"digest"`
	ViewID     int64  `json:"viewID"`
}

type ReplyMsg struct {
	ViewID    int64  `json:"viewID"`
	Timestamp int64  `json:"timestamp"`
	ClientID  string `json:"clientID"`
	NodeID    string `json:"nodeID"`
	Result    string `json:"result"`
}


type VoteMsg struct {
	ViewID     int64  `json:"viewID"`
	SequenceID int64  `json:"sequenceID"`
	Digest     string `json:"digest"`
	NodeID     string `json:"nodeID"`
	MsgType    `json:"msgType"`
}


const (
)
