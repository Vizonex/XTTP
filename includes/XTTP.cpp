
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <unordered_map>



namespace XTTP{
    class SOCKETERROR : public std::exception
    {
        std::string what_message;
    public:
        SOCKETERROR(std::string msg){what_message = msg;}
        const char* what(){return what_message.c_str();}
    };
}

#define END_CHUNK "0"
#define DEFAULT_BUFFER_SIZE 8192

#ifdef _WIN32
    #include <WinSock2.h>
    #include <windows.h>

    #pragma comment(lib, "ws2_32.lib")
    

    namespace XTTP {
        /*
        Used for helping with socket creation on Windows
        */
        SOCKET create_socket(){
            WSADATA wsaData = {0};
            int iResult = 0;
            iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (iResult != 0){
                throw XTTP::SOCKETERROR("SocketError WINDOWS REALATED [" + std::to_string(iResult) + "] Socket couldn't be made");
            }
            SOCKET sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
            if (sock == INVALID_SOCKET){
                throw XTTP::SOCKETERROR("INVALID SOCKET FORMATION");
            }
            return sock;
        }


        int CloseSocket(SOCKET sock){
            int iResult = closesocket(sock);
            if (iResult == SOCKET_ERROR) {
                wprintf(L"closesocket failed with error = %d\n", WSAGetLastError() );
                WSACleanup();
                return 1;
            } else {
                WSACleanup();
                return 0;
            }
        }

    }


#else 

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>

namespace XTTP {
        /*
        Used for helping with socket creation
        */
        int create_socket(){
            
            int sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
            return sock;
        }


        int CloseSocket(int sock){
            int iResult = closesocket(sock);
            if (iResult == SOCKET_ERROR) {
                puts("closesocket failed with error = SOCKETERROR\n")
                return 1;
            }
        }
    }


// "I will be adding the same functions for the different archetextures of linux and Mac soon Even though I'm a Windows developer" - Vizonex 




#endif 




// IF you would to like to take a deeper dive into http you should read this https://www.rfc-editor.org/rfc/rfc2616

namespace XTTP{

    class XTTPSocket{  
        private:
            #ifdef _WIN32
                SOCKET s;
            #else
                int s;
            #endif

        public:
            XTTPSocket(){s = XTTP::create_socket();};

            int Connect(const char* host, u_short port){

                // TODO Make Sure this can also work on other systems and not just windows 

                SOCKADDR_IN SockAddr;
                struct hostent* localHost = gethostbyname(host);
                char* localIP = inet_ntoa(*(struct in_addr *)*localHost->h_addr_list);
                SockAddr.sin_port = htons(80);
                SockAddr.sin_family = AF_INET;
                SockAddr.sin_addr.s_addr = inet_addr(localIP);
                return connect(s,reinterpret_cast<SOCKADDR*>(&SockAddr),sizeof(SockAddr));
            };
            /* Sends an Http message to the server it's communicating with */
            int Send(const char* a){
                // TODO MAKE A WAY TO USE std::string by specifying an XTTP MACRO  
                return send(s , a , (int)strlen(a),0);
            }

            /*
            Default Recive , returns the int len of bytes left to be recived back
            */
            int _Recv(char* buffer, int buffersize = DEFAULT_BUFFER_SIZE){

                int iResult = recv(s,buffer,buffersize,0);
                #ifdef _WIN32
                    while ((iResult < 0) && (WSAGetLastError() == WSAENOBUFS)){
                    

                       // This will attempt to bruteforce windows into giving us buffers if we failed the first time

                        Sleep(1);
                        int iResult = recv(s,buffer,buffersize,0); 
                    }
                #else
                #endif 

                return iResult;
            }

            /*
            Used for reading non fixed byte sizes such as http responses in chunks 
            */
            std::string Recv_as_string(int buffersize = DEFAULT_BUFFER_SIZE){
                char* buff = new char [buffersize];
                _Recv(buff,buffersize);
                return std::string(buff);
            }

            int Close(){return XTTP::CloseSocket(s);};
    };
    
    

    /* 
    Automatically Reads Recvieves string when it runs out of lines to read to find very simillar to 
    Python's Socketio Class , Asks XTTPSocket for more data to read 
    when a line is not found , used to simplify Response Parsing and downloading data in chunks  
    */
    class XTTPReader{
        public:
            size_t start = 0;
            size_t end; 

            int buffersize;

            std::string buffer;

            XTTPSocket sock;

            
            XTTPReader(){}
            
            XTTPReader(XTTPSocket s,int buffsize = DEFAULT_BUFFER_SIZE){
                sock = s;
                buffersize = buffsize;
            }

            /* 
            *Starts reading off the first payload which is required for starting to read the 
            *response as using this prevents NULL bytes and allows for faster allocation 
            */
            void Begin_Read(){
                buffer += sock.Recv_as_string(buffersize);
            }
            
            /* 
            *Cleans out the buffer variable , 
            *Used when data sizes get too big to handle and is automatically done for memeory safety*/
            void BufferReset(){
                buffer = buffer.substr(start);
                // Reset Buffer start variable to 0 since were cutting the string up to the beginning again
                start = 0;
            }

            /* Automatically changes the buffersize being used */ 
            void ChangeBufferSize(int buffsize){
                buffersize = buffsize;
            }

            /* Reads data line by line when the \\r\\n delimiter is not found it will simply tell the socket to go fetch more data to be read */
            std::string Readline(){
                // This while loop will keep fecthing buffers until \\r\\n is found in the next chunk
                while((end = buffer.find_first_of("\r\n",start)) == std::string::npos){
                    buffer.append(sock.Recv_as_string(buffersize));
                }
                std::string b = buffer.substr(start,end - start);
                start = end + 2;
                end += 3;
                // Use bufferreset to rest our buffer variable so that we can make up for already used string and for memory safety
                BufferReset();
                return b;
            }

            /* 
            *Reads data line by line when the \\r\\n delimiter is not found it will simply tell the socket to go fetch more data to be read 
            *
            *This will also request a custom buffersize that is different from the function that doesn't do this
            *useful for increasing your bandwith as a download is happening 
            */
            std::string Readline(int buffsize){
                // This while loop will keep fecthing buffers until \\r\\n is found in the next chunk
                while((end = buffer.find_first_of("\r\n",start)) == std::string::npos){
                    buffer.append(sock.Recv_as_string(buffsize));
                }

                std::string b = buffer.substr(start,end - start);
                start = end + 2;
                end += 3;
                BufferReset();
                return b;
            }

    };


    /*
    * The Response parser use for parsing http responses 
    */
    class XTTPResponse{

        public:
            XTTPReader reader;
            std::unordered_map<std::string,std::string> headers;

            bool ok;
            int status;
            bool chunked;
            std::string httptype;

            std::string statusreason;
            std::string text;

            XTTPResponse(){};

            XTTPResponse(XTTPSocket sock,int buffersize = DEFAULT_BUFFER_SIZE){
                reader = XTTPReader(sock,buffersize);
            }

            /*
            Used for giving text it's inital chunks back 
            */ 
            void read_chunks(){
                std::string buff;
                while (buff != END_CHUNK){
                    buff = reader.Readline();
                    text += reader.Readline();
                }
            };


            /* 
            Returns True if Chunks must be read 
            */
            void Begin(){
                size_t start = 0, end , delim;
                reader.Begin_Read();
                std::string buff = reader.Readline();
                end = buff.find(' ');

                httptype = buff.substr(start, end);
                start = end + 1;
                status = std::stoi(buff.substr(start,(end = buff.find(' ',start))));
                ok = (status == 200);
                start = end + 1;
                // Get the last part of the line 
                statusreason = buff.substr(start);

                // Parse the headers 
                buff = reader.Readline();

                while ((delim = buff.find(':')) != std::string::npos){
                    headers.emplace(buff.substr(0,delim),buff.substr(delim + 2));
                    buff = reader.Readline();
                }

                if (headers.count("Transfer-Encoding") && headers["Transfer-Encoding"] == "chunked"){
                    buff = reader.Readline();
                    text += reader.Readline();
                    chunked = true;
                }

                else {
                    text += reader.Readline();
                    chunked = false;
                }

            }

            /* Reads the response Warning! this should not be used for dowloading files! */
            void Read(){
                Begin();
                if (chunked){
                    read_chunks();
                }
            }
    };
}
