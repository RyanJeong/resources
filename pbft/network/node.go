package network

import (
	"encoding/json"
	"errors"
	"fmt"
	"strings"
	"time"

	"../consensus"
}

/*
 *  View    :   primary node p가 primary 노드로서 활동하는 기간의 단위를 나타내며,
 *              view v의 primary node는 (v mod N)번 노드가 담당함
 *  ------------------------
 *  id      :   클라이언트를 식별할 수 있는 ID.
 *  primary :   클라이언트로부터 메시지를 받고,
 *              이 메시지의 순서를 정해주는 역할을 한다.
 */
type View struct {
	id      int64
	primary string
}

type MsgBuffer struct {
	ReqMsgs             []*consensus.RequestMsg
	PrePrepareMsgs      []*consensus.PrePrepareMsg
	PrepareMsgs         []*consensus.VoteMsg
	CommitMsgs          []*consensus.VoteMsg
	ViewChangeMsgs      []*consensus.ViewChangeMsg
	ViewChangeClameMsgs []*consensus.ViewChangeClameMsg
}

type Node struct {
	nodeId      string
	nodeMap     map[string]string // key=nodeID, value=url
	view        *View
	currState   *consensus.State
	isByzantine int
	committed   []*consensus.RequestMsg // kinda block.
	msg         *MsgBuffer
	msgEntrance chan interface{}
	msgDelivery chan interface{}
	alarm       chan bool
}

