# Network Protocols

* Implementing/testing a few common network protocols from scratch.
* "From scratch" generally means from L4 upwards as defined in OSI model with Linux's `socket()` functions.

### Review of OSI and TCP/IP models

<table>
    <thead>
        <tr>
            <th>TCP/IP Model</th>
            <th>OSI Model</th>
            <th>Protocol:Type</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td rowspan=3>Application layer</td>
            <td>L7: Application layer</td>
            <td>FTP:binary/text, HTTP:text, Protobuf:binary, Kafka:binary</td>
        </tr>
        <tr>
            <td>L6: Presentation layer</td>
            <td>&#60;no common examples...&#62;</td>
        </tr>
        <tr>
            <td>L5: Session layer</td>
            <td>L2TP</td>
        </tr>
        <tr>
            <td>Transport layer</td>
            <td>L4: Transport layer</td>
            <td>TCP:binary, UDP:binary</td>
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
