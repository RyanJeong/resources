* Sort Program<br>
```Text
Usage: sort [OPTION]
-n: numeric sort
-r: sorting in reverse order
```

```Text
$./sort
come
get
give
go
keep
let
make
put
seem
take
be
do
have
say
see
send


be
come
do
get
give
go
have
keep
let
make
put
say
see
seem
send
take
```

```Text
./sort -r
come
get
give
go
keep
let
make
put
seem
take
be
do
have
say
see
send

take
send
seem
see
say
put
make
let
keep
have
go
give
get
do
come
be
```

```Text
./sort -n
816
1239
1685
931
1148
1114
1679
940
556
1317
102
642
1335
829
1850
539


102
539
556
642
816
829
931
940
1114
1148
1239
1317
1335
1679
1685
1850
```

```Text
./sort -rn
816
1239
1685
931
1148
1114
1679
940
556
1317
102
642
1335
829
1850
539

1850
1685
1679
1335
1317
1239
1148
1114
940
931
829
816
642
556
539
102
```
