#/bin/bash

if [ "$#" -lt 1 ]; then
    echo "Error: Target missing."
    echo "Usage: $0 test"
    echo "Usage: $0 <TARGET.go>"
    echo "Usage: $0 <TARGET.go> check"
    exit 1
fi

if [[ $2 = "check" ]]; then
    gofmt -w $1
    goimports -w $1
elif [[ $1 = "test" ]]; then
    go test
else
    gofmt -w $1
    goimports -w $1
    go run $1
fi
