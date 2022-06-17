# Running Kafka behind a NAT network

* Kafka runs well on Linux, but what if you are given only a Windows server? It turns out that Kafka has issues here and there on Windows platform. Therefore, running
Kafka inside a Linux virtual machine on top of a Windows host can be considered.

* It would be easier if you can select bridged network mode since you can treat the Linux VM as a separate machine.
Unfortunately, it is usually not pracitcal in a corporate network... 

* So we document the more challenging approach, i.e., the NAT network mode, in thie page.

* The settings are pretty tricky--you need to get everything just right to make it work. However, when something is 
broken, the cause is mostly obscure...

## `Hyper-V` NAT settings

* `Hyper-V Manager` -> `Virtual Switch Manager...` -> `Internal` -> `Create Virtual Switch`.

* `Network settings` -> `Change adapter options` -> right click the Adapter with Internet connection -> `Sharing` -> `Allow other network users to connect through this computer's Internet connection` -> pick the virtual switch just created.

* Right click the virtual switch just created -> `Internet Protocol Version 4 (TCP/IPv4)` -> `Use the following IP address` -> `IP address: 192.168.137.1` (example) -> `Subnet mask: 255.255.255.0` -> Seems we can leave DNS addresses empty.

* Install Linux in `Hyper-V` as usual -> sometimes Linux's DHCP auto-configuration works -> if not, you pick an IP in the same network, e.g., `192.168.137.7`

## Port forwarding on Windows Host with `PowerShell`

* The idea is that you need to expose some Liunx port to the Internet so that other computers can access the service on it.

* Add a record, the example opens `2222` on Host and redirects traffic to `22` on Guest. Note that this example is selected for
a reason--you can check if port forwarding is really working by SSH'ing to the guest system.
```
netsh interface portproxy add v4tov4 listenport=2222 listenaddress=[Host IP] connectport=22 connectaddress=192.168.137.7
```

* List all records:
```
netsh interface portproxy show all
```

* Remove a record:
```
netsh interface portproxy delete v4tov4 listenport=2222 listenaddress=[Host IP]
```

* Typically, you need to forward two sets of ports. The reason is that, say the hostname of Windows host is `my.superserver.com`, if you resolve
`my.superserver.com` on another computer, the result is usually the IP of the host's WAN interface. However, if you resolve `my.superserver.com`
on Windows host itself, the result could be `192.168.137.1` (i.e., the Windows host's LAN interface's IP), which will not be redirected to Linux
host if you only have the first set of ports.
```
# Windows host's WAN address:port to Linux guest's address
netsh interface portproxy add v4tov4 listenport=9092 listenaddress=[Host IP of WAN interface] connectport=9092 connectaddress=192.168.137.7
netsh interface portproxy add v4tov4 listenport=9093 listenaddress=[Host IP of WAN interface] connectport=9093 connectaddress=192.168.137.7
netsh interface portproxy add v4tov4 listenport=9094 listenaddress=[Host IP of WAN interface] connectport=9094 connectaddress=192.168.137.7
# Windows host's LAN address:port to Linux guest's address
netsh interface portproxy add v4tov4 listenport=9092 listenaddress=192.168.137.1 connectport=9092 connectaddress=192.168.137.7
netsh interface portproxy add v4tov4 listenport=9093 listenaddress=192.168.137.1 connectport=9093 connectaddress=192.168.137.7
netsh interface portproxy add v4tov4 listenport=9094 listenaddress=192.168.137.1 connectport=9094 connectaddress=192.168.137.7
```

## `Kafka`'s special settings on Linux Guest

* First, you make sure the `Kafka` instance is 100% working within the Linux environment, and then
we focus on making it work beyond NAT.

* The core idea is that, if you need to connect from both your LAN and WAN, you'll need two listeners; one for LAN and one for WAN.

* Revise the `server.properties` accordingly based on the below example
```
listeners=LISTENER_LAN://0.0.0.0:9092,LISTENER_WAN://0.0.0.0:9094
advertised.listeners=LISTENER_LAN://:9092,LISTENER_WAN://PUBLIC_IP_ADDRESS_HERE:9094
listener.security.protocol.map: LISTENER_LAN:PLAINTEXT,LISTENER_WAN:PLAINTEXT
inter.broker.listener.name=LISTENER_LAN
```

* How about we only keep `LISTENER_WAN` and remove `LISTENER_LAN` so that all traffic is through the `WAN` interface
of the Windows host? It does not work due to the fact that, somehow, Linux guest does *NOT* have the access to the 
`WAN` IP of the Windows host for some reason.

* It basically follows this link: https://stackoverflow.com/questions/61101236/accessing-kafka-broker-from-outside-my-lan

