# HTTP

## Notes

* HTTP is a complex protocol, [RFC2616 -- Hypertext Transfer Protocol -- HTTP/1.1](https://datatracker.ietf.org/doc/html/rfc2616) is 175 pages long.

* Server-side programs in this directory could be inefficient, insecure or unstable--they are just a few simple
  experiments roughly demonstrating how HTTP works.

## How do some common requests look like from the server-side

### Plain browser request

* Just go to `http://localhost:8081/` with Firefox
* Data received by `server.out`
```
GET / HTTP/1.1
Host: localhost:8081
User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:78.0) Gecko/20100101 Firefox/78.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Connection: keep-alive
Upgrade-Insecure-Requests: 1

```

### Text file upload request
* Test request from `cURL`
```
curl -F 'form_name=@/tmp/text-upload.test' http://localhost:8081/upload
```
* Data received by `server.out`
```
POST /upload HTTP/1.1
Host: localhost:8081
User-Agent: curl/7.74.0
Accept: */*
Content-Length: 255
Content-Type: multipart/form-data; boundary=------------------------60f12d911c990df6

--------------------------60f12d911c990df6
Content-Disposition: form-data; name="form_name"; filename="text-upload.test"
Content-Type: application/octet-stream

Hello world!
Hello world!!
Hello world!!!

--------------------------60f12d911c990df6--
```

### Binary file upload request
* Test request from `cURL`
```
curl -F 'form_name=@/tmp/binary-upload.png' http://localhost:8081/upload
```
* Data received by `server.out`
```
POST /upload HTTP/1.1
Host: localhost:8081
User-Agent: curl/7.74.0
Accept: */*
Content-Length: 26399
Content-Type: multipart/form-data; boundary=------------------------3c1b05f6dd05799d

--------------------------3c1b05f6dd05799d
Content-Disposition: form-data; name="form_name"; filename="binary-upload.png"
Content-Type: image/png

\89PNG

\00\00\00
IHDR\00\00\00d\00\00\00P\00\00\00`kG\B8\00\00#zTXtRaw profile type exif\00\00xڭ\9Bg\8Ed\B9r\85\FFsZ]\D0,\87\D0\B4|}\87\99\D5o\BAg
[...More binary data omitted...]
{\8B0\8F_`\CF\AF'\DDq\E5\F3\D5/\ED\D6많s?\9FM\F5<\AF\F5\D3wnC\B5,WV\99Gҍ\9A0\[\B4>\ED\B6\B9\C7p\AE\95U\F8?\F2\B9\F0`\F1ݕ\00\00\00\00IEND\AEB`\82
--------------------------3c1b05f6dd05799d--

```

### RESTful Request

* Test request from `cURL`
```
curl --header "Content-Type: application/json" \
     --request POST \
     --data '{"username":"hello","password":"world"}' \
    http://localhost:8081/api/login
```
* Data received by `server.out`
```
POST /api/login HTTP/1.1
Host: localhost:8081
User-Agent: curl/7.74.0
Accept: */*
Content-Type: application/json
Content-Length: 39

{"username":"hello","password":"world"}
```