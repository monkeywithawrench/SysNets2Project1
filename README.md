# SysNets2Project1

GET requests look like: (replace <filename> with the name of the file you're requesting, if any. Note: no file name usually defaults to index.html on most websites)

GET /<filename> HTTP/1.1
Host: localhost:4444
Connection: keep-alive
Cache-Control: max-age=0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebK


POST requests look like: (Note: the Content-Length is the length of the body of the request in bytes!)

POST /<filename> HTTP/1.1
Host: 192.168.1.51:8080
Connection: keep-alive
Content-Length: 0
Cache-Control: no-cache
Origin: chrome-extension://fhbjgbiflinjbdggehcddcbncdddomop
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (

