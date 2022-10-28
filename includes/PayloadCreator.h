/* XTTP Was Made as an Alternative library to llhttp for anyone who Doesn't have NodeJs 
or needs a fast request writer and reader 
*/

#include <string>

// Used for Adding Conetent-Type and Content-Length quickly
static const char CONTENT_LEN_AND_CONTENT_TYPE[65] = "Content-Type: application/x-www-form-urlencoded\r\nContent-Length:";


// Used for passing along to other payloads and the inital socket requests and header requests
typedef struct Postdata
{
    std::string payload;
    size_t size;
    // Resizes the payloadsize as it gets bigger 
    inline void resize(){
        size = payload.size();
    }
    
    inline void add_header(std::string key, short value){
       payload += '&' + key + '=' + std::to_string(value);
    }

    inline void add_header(std::string key, int value){
        payload += '&' + key + '=' + std::to_string(value);
    }

    inline void add_header(std::string key, long value){
       payload += '&' + key + '=' + std::to_string(value);
    }

    inline void add_header(std::string key, size_t value){
       payload += '&' + key + '=' + std::to_string(value);
    }

    inline void add_header(std::string key , std::string value){
        payload += '&' + key + '=' + value;
    }

    // returns with the result for making the request data 
    std::string result(){
        return CONTENT_LEN_AND_CONTENT_TYPE + std::to_string(size) + "\r\n\r\n" + payload;
    }

    // returns with the result , used for making multiple dynamic paramters and multiple http and https requests quickly
    std::string result(std::string key , std::string value){
        std::string data = payload + "&" + key + ":" + value;
        return CONTENT_LEN_AND_CONTENT_TYPE + std::to_string(data.size()) + data;
    }

} Postdata;




/* -- OVERLOADS USED FOR OPTIMIZING PERFORMANCE -- */

Postdata add_first_header(std::string key, long value){
    Postdata pd;
    pd.payload = key + '=' + std::to_string(value);
    return pd;
}

Postdata add_first_header(std::string key, short value){
    Postdata pd;
    pd.payload = key + '=' + std::to_string(value);
    return pd;
}

Postdata add_first_header(std::string key, int value){
    Postdata pd;
    pd.payload = key + '=' + std::to_string(value);
    return pd;
}

Postdata add_first_header(std::string key, size_t value){
    Postdata pd;
    pd.payload = key + '=' + std::to_string(value);
    return pd;
}



Postdata add_first_header(std::string key, std::string value){
    Postdata pd;
    pd.payload = key + '=' + value;
    return pd;
}


// Finalizes headers and allows for multiple headers to be made without altering the orginal Postdata object
std::string add_final_header(Postdata pd,std::string key, std::string value){
    std::string load = pd.payload;
    load += "&" + key + "=" + value;
    return CONTENT_LEN_AND_CONTENT_TYPE + std::to_string(load.size())  + "\r\n\r\n" + load;
}



// Finalizes headers and allows for multiple headers to be made without altering the orginal Postdata object
std::string add_final_header(Postdata pd,std::string key, int value){
    std::string load = pd.payload;
    load += "&" + key + "=" + std::to_string(value);
    return CONTENT_LEN_AND_CONTENT_TYPE + std::to_string(load.size())  + "\r\n\r\n" + load;
}


enum http_flags{
    // HTTP/1.0
    http_10,
    // HTTP/1.1
    http_11,
    // HTTP/2
    http_2};


enum action_flags { Get, Post, Options, Put, Patch, Delete, Head};

typedef action_flags action_flags;
typedef http_flags http_flags;



typedef struct Headers{
    // These variables should be locked to prevent unwanted things from happending 
  
   
    std::string action;
    std::string http_type;
    std::string endpoint;

    /* the parameters to take into the http request 
    for example

    Connection : keep_alive
    Content-Type: application/x-www-form-urlencoded
    User-Agent: Xttp 1.0
    */
    std::string parameters;

    std::string host;

    bool keep_alive;

    inline short set_http_type(http_flags flag){
        switch (flag)
        {
        case http_10:
            http_type = "HTTP/1.0";
            break;
        case http_11:
            http_type = "HTTP/1.1";
            break;
        case http_2:
            http_type = "HTTP/2";
            break;
        default:
            return -1;
        }
        return 1;
    };

    /* Adds in a parameter into the headers */
    void add_parameter(std::string key, std::string value){
        parameters += "\r\n" + key + ':' + value;
    }
    /*Uses an already made haeader and simple appends it such as "Sec-Fetch-User: ?1"  
    because at the end of the day , the more precompiled headers you use , the faster the 
    Http message will be built*/
    void add_parameter(std::string param){
       parameters += "\r\n" + param;
    }


    // Returns -1 if the flag is invalid , returns 1 if the flag exists and has been set.
    inline short set_action(action_flags flag){
        switch (flag)
        {
        case Get:
            action = "GET";
            break;
        case Post:
            action = "POST";
            break;
        case Options:
            action = "OPTIONS";
            break;
        case Put:
            action = "PUT";
            break;
        case Patch:
            action = "PATCH";
            break;
        case Delete:
            action = "DELETE";
            break;
        case Head:
            action = "HEAD";
            break;
        default:
            return -1;
        }
        return 1;

    };


    /* 3 Overloads for different types of string objects to optimize web-api communication */

    // Attaches string to the endpoint object including the host site if included 
    void add_url(std::string url){
        if (url[0] == '/'){
            endpoint = url;
        }
        else {
            int first_delim = url.find("/");

            host = url.substr(0,first_delim);

            endpoint = url.substr(first_delim);
        }
    }


    // Parses The Current request together
    std::string parse_together(){
        std::string data;
        data += action + " " + endpoint + " " + http_type + "\r\nHost:" + host;
            
        if (keep_alive){
            data += "\r\nConnection: keep_alive";
        }
        if (parameters.size()){
            data += "\r\n" + parameters;
        }
        return data;
    }

    std::string parse_together(std::string extra_headers){
        std::string data;
        data += action + " " + endpoint + " " + http_type + "\r\nHost:" + host;
            
        if (keep_alive){
            data += "\r\nConnection: keep_alive";
        }

        if (parameters.size()){
            data += "\r\n" + parameters;
        }

        return data +"\r\n" + extra_headers;
    }


    std::string parse_with_post_data(Postdata pd, std::string final_key, std::string final_value){
        return parse_together(add_final_header(pd,final_key,final_value));
    }

    std::string parse_with_post_data(Postdata pd, std::string final_key, int final_value){
        return parse_together(add_final_header(pd,final_key,final_value));
    }



} Headers;
