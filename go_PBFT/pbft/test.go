package main
type MsgType int

const (
	PrepareMsg MsgType = iota
	CommitMsg
)

func main() {
    println(PrepareMsg)
    println(CommitMsg)
}
