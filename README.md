# Network Protocols

* Implementing a few common network protocols from scratch.
* "Scratch" generally means from L3 upwards in OSI model.

### Review of OSI and TCP/IP models

<table>
    <thead>
        <tr>
            <th>TCP/IP Model</th>
            <th>OSI Model</th>
            <th>Protocols</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td rowspan=3>Application layer</td>
            <td>L7: Application layer</td>
            <td>FTP,HTTP,Telnet</td>
        </tr>
        <tr>
            <td>L6: Presentation layer</td>
            <td>JPEG,MPEG</td>
        </tr>
        <tr>
            <td>L5: Session layer</td>
            <td>NFS,SQL</td>
        </tr>
        <tr>
            <td>Transport layer</td>
            <td>L4: Transport layer</td>
            <td>TCP, UDP</td>
        </tr>
        <tr>
            <td>Internet layer</td>
            <td>L3: Network layer</td>
            <td>IPv4, IPv6</td>
        </tr>
        <tr>
            <td rowspan=2>Network Access layer</td>
            <td>L2: Data link layer</td>
            <td>ARP</td>
        </tr> 
        <tr>
            <td>L1: Physical layer</td>
            <td>Ethernet, Wi-Fi</td>
        </tr>
    </tbody>
</table>

* The `socket()` is primarily a concept used in the transport layer of the Internet protocol suite or session layer of the OSI model.