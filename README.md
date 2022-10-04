![alt-text](https://github.com/Vizonex/XTTP/blob/main/XTTP.png)
# XTTP
A powerful Http Socket python library made with Cython / C++ as an alternative library for http parsing if you were unable to use llhttp with nodejs and didn't want to download someting you weren't very familliar of and really care about speed and performance.


- [x] make XTTP Writer in C++
- [ ] program XTTP Reader in C++
- [ ] Build Cython library 
- [ ] Make a stable bridge to python for python users to use
- [ ] Add to pypi when first stable version releases 
- [ ] Create Readthedocs documentation 

## Features

- Post Data writer for x-www-form-urlencoded data along with the ability to loop the last header added so you can have multiple post payloads made extremely fast
- Minimal memory managment 
- No External third party libraries 
- To my knowlege I've tested Xttp.h on windows and it works 
- No Need to Use Cmake to compile header files so the Stress can be cut on having to find out what arch-id matches your computer when compiling. 
- you can make your own premade http-headers to merge with the struct variables in C++

## Includes 

- Cython / Python and C++ Libraries 
- A super fast http parser library 
