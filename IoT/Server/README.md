# KEM_Webserver<br>
## Web server application written in golang<br>
```Text
$ ssh -x root@kem.ourplan.kr -p 22
```
## [How to prevent SSH from disconnecting if it's been idle for a while](https://superuser.com/questions/699676/how-to-prevent-ssh-from-disconnecting-if-its-been-idle-for-a-while)<br>
* The problem is that there is something (usually a firewall or load-balancer), which is dropping idle sessions. If you configure session keepalives, the keepalives will prevent network devices from considering the session as idle.<br>

### Linux / Unix / Cygwin OpenSSH fix:<br>
* The simplest fix is to enable ssh client keepalives; this example will send an ssh keepalive every 60 seconds:<br>
```bash
ssh -o "ServerAliveInterval 60" <SERVER_ADDRESS>
```

* If you want to enable this on all your sessions, put this in your /etc/ssh/ssh_config or ~/.ssh/config:<br>
```bash
ServerAliveInterval 60
```

* For more information, see the [ssh_config manpage](https://linux.die.net/man/5/ssh_config)<br>

### Putty Fix:<br>
* Save this to your PuTTY "Default Settings"...<br>
    * Click on Connection<br>
    * Type 60 into "Seconds between keepalives"<br>
    
![putty](https://i.stack.imgur.com/Tgrlr.png)

