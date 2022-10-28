![alt-text](https://github.com/Vizonex/XTTP/blob/main/XTTP.png)
# XTTP
A powerful Http Socket python library made with C++ as an alternative library for http parsing if you were unable to use llhttp with nodejs and didn't want to download someting you weren't very familliar of , really care about speed and performance and you need to go low level but you also desire some stability.  


## Completed so far

- [x] make XTTP Socket in C++
- [x] program XTTP Reader in C++ 
- [x] Make XTTPResponse Parser
- [ ] make portable with Windows and linux
- [x] Works On Windows
- [ ] Create Readthedocs documentation 



## Features

- Post Data writer for x-www-form-urlencoded data along with the ability to loop the last header added so you can have multiple post payloads made extremely fast
- Minimal memory managment 
- No External third party libraries 
- To my knowlege I've tested Xttp.cpp on windows and it works great. 


## Includes 

- C++ Libraries 
- A super fast http parser library which gets parsed as the data is being read
- HTTP/1.1 support 


## Future Plans
- SSL support
- Make learning about socket messages and proxy configuation easy to understand while also getting a little deeper into how it all works
- Sperate github repository for a Python/Cython port of this library
- HTTP/2 support 

