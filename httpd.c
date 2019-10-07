/*************************************************************************
	> File Name: http.c
	> Author: 
	> Mail: 
	> Created Time: 2019年09月30日 星期一 16时32分14秒
 ************************************************************************/
 
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<ctype.h>
#include<string.h>
#include<sys/stat.h>
#include<pthread.h>
#include<sys/wait.h>
 
# define SERVER_STRING "Server: jdhttpd/0.1.0\r\n"
# define ISspace(x) isspace((int)(x))
void *accept_request(void *);
void bad_request(int);
void cat(int, FILE *);
void cannot_excute(int);
void error_dis(const char *);
void execute_cgi(int, const char *, const char *, const char *);
void headers(int, const char *);
void not_found(int);
void serve_file(int, const char *);
void not_found(int);
void serve_file(int, const char *);
int startup(int *);
void unimplemented(int);
int get_line(int, char *, int);
int pthread_create(pthread_t *thread,const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
 
void* accept_request(void *arg){
    
    char buf[1024];
    char method[255];
    char url[255];
    char path[512];
    int client = *(int *)arg;
    int cgi = 0;
    struct stat st;
    char *query_string = NULL;
    int numchars = 0;
    int i, j;
 
    numchars = get_line(client, buf, sizeof(buf));
    i = 0, j = 0;
    while(!ISspace(buf[i]) && (i < sizeof(method) - 1)){
        method[i] = buf[i];
        i++;
    }
    j = i;
    method[i] = '\0';
 
    if(strcasecmp(method, "GET") && strcasecmp(method, "POST")){
        unimplemented(client);
        return NULL;	
    }
    
    if((strcasecmp, "POST") == 0){
        cgi = 1;
    }
    
    i = 0;
    while(ISspace(buf[j]) && (j < numchars)){
        j++;
    }
    while(!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < numchars)){
        url[i] = buf[j];
        i++;
        j++;
    }
    url[i] = '\0';
 
    if(strcasecmp(method, "GET") == 0){
        query_string = url;
        while((*query_string != '?') && (*query_string != '\0')){
            query_string++;
        }
        if(*query_string == '?'){
            cgi = 1;
            *query_string = '\0';
            query_string++;
        }
    }
 
    sprintf(path, "htdocs%s", url);
    if(path[strlen(path) - 1] == '/'){
        strcat(path, "index.html");
    }
    if(stat(path, &st) == -1){
        while((numchars > 0) && strcmp("\n", buf)){
            numchars = get_line(client, buf, sizeof(buf));
        }
        not_found(client);
    }
    else {
        if((st.st_mode & S_IFMT) == S_IFDIR){
            strcat(path, "/index.html");
        }
        // 如果不是cgi, 就直接把服务器文件返回
        if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || 
           (st.st_mode & S_IXOTH))
        cgi = 1;
        if(!cgi)
        serve_file(client, path);
        else 
        // client 客户端对应的socket,　path 是对应的路径，method是请求方法
        // query_string 是请求的内容
        execute_cgi(client, path, method, query_string);
    }
    close(client);
    return NULL;
}
 
void execute_cgi(int client, const char *path, const char* method, const char * query_string ){
    
    char buf[1024];
    char c;
    int numchars = 1, content_length = -1;
    buf[0] = 'A';
    buf[1] = '\0';
    int cgi_output[2];
    int cgi_input[2];
    int status;
    pid_t pid;
 
    if(strcasecmp(method, "GET") == 0){
        while((numchars > 0 ) && (strcmp("\n", buf))) {
            numchars = get_line(client, buf, sizeof(buf));
        }
    }
    else if(strcasecmp(method, "POST") == 0){
        numchars = get_line(client, buf, sizeof(buf));
        
        while((numchars > 0) && strcmp("\n", buf)){
            buf[15] = '\0';
        if(strcasecmp(buf, "Content-Length:") == 0){
            content_length = atoi(&buf[16]);
        }
        
         numchars = get_line(client, buf, sizeof(buf));
        }
 
        if(content_length == -1){
            bad_request(client);
            return ;
        }
    }
    
    if(pipe(cgi_output) < 0){
        cannot_excute(client);
        return ;
    }
 
    if(pipe(cgi_input) < 0){
        cannot_excute(client);
    }
 
    
    if((pid = fork()) < 0 ){
        cannot_excute(client);
    }
 
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, sizeof(buf), 0);
 
    if(pid == 0){
        
        char meth_env[255];
        char query_env[255];
        char length_env[255];
 
        dup2(cgi_input[0], 0);
        dup2(cgi_output[1], 1);
        close(cgi_input[1]);
        close(cgi_output[0]);
        
        sprintf(meth_env, "REQUEST_METHOD=%s", method);
        putenv(meth_env);
        
        if(strcasecmp(method, "GET") == 0){
            sprintf(query_env, "QUERY_STRING=%s", query_string);
            putenv(query_env);
        }
        else {
            sprintf(query_env, "QUERY_STRING=%s", query_string);
            putenv(length_env);
        }
 
        execl(path, path, NULL);
        exit(0);
    }
    else{
        close(cgi_output[1]);
        close(cgi_input[0]);
        
        if(strcasecmp(method, "POST") == 0){
	    int i;
            for(i = 0; i < content_length; i++){
            recv(client, &c, 1, 0);
            write(cgi_input[1], &c, 1);
            }
        }
 
        while(read(cgi_output[0], &c, 1) > 0){
            send(client, &c, 1, 0);
        }
 
        close(cgi_output[0]);
        close(cgi_input[1]);
 
        waitpid(pid, &status, 0);
    }
 
    return ;
}
 
void cannot_excute(int client){
    
    char buf[1024];
 
    sprintf(buf, "HTTPS/1.0 500 Internal Server ERror\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Cpntent-type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
    send(client, buf, strlen(buf), 0);
    return ;
}
 
void bad_request(int client){ 
    char buf[1024];
 
    sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "<P>Your browser sent a bad request. ");
    send(client, buf, sizeof(buf), 0);;
    sprintf(buf, "such as a POST without a content_length.\r\n");
    send(client, buf, sizeof(buf), 0);
    return ;
}
void serve_file(int client, const char *filename){
    
    FILE *resource = NULL;
    int numchars = 1;
    char buf[1024];
 
    buf[0] = 'A';
    buf[1] = '\0';
    while((numchars > 0) && strcmp("\n", buf)){
        numchars = get_line(client, buf, sizeof(buf));
    }
 
    resource = fopen(filename, "r");
    if(resource == NULL){
        not_found(client);
    }
    else {
        // 先把http头部信息发送过去，再把文件中的内容通过socket发送出去
        headers(client, filename);
        cat(client, resource);
    }
    return ;
}
 
void headers(int client, const char *filename){
    char buf[1024];
    (void)filename;
 
    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    return ;
}
 
void cat(int client, FILE *resource){
    char buf[1024];
 
    fgets(buf, sizeof(buf), resource);
    while(!feof(resource)){
       send(client, buf, strlen(buf), 0);
       fgets(buf, sizeof(buf), resource);
    }
    return ;
} 
 
void not_found(int client){
    char buf[1024];
 
    sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><TITLe>Not Found</TITLE>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>The server could notfullfill\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "your request because the resource specified\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "is unavauilable or nonexistent/\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
    return ;
 
}
void unimplemented(int client){
    
    char buf[1024];
 
    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf,SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><HEAD><TITLE>Method not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</TITLE></HEAD>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
    return ;
}
 
int get_line(int sock, char *buf, int size){
    int i = 0;
    char c = '\0';
    int n;
    
    while((i < size - 1) && (c != '\n')){
        n = recv(sock, &c, 1, 0);
        if(n > 0){
            if(c == '\r'){
                n = recv(sock, &c, 1, MSG_PEEK);
                if((n > 0) && (c == '\n')){
                    recv(sock, &c, 1, 0);
                }
                else {
                    c = '\n';
                }
            }
            buf[i] = c;
            i++;
        }
        else c = '\n';
    }
 
    buf[i] = '\0';
 
    return i;
}
 
int  startup(int *port){
    int httpd = -1, on = 1;
    struct sockaddr_in name;
 
    httpd = socket(AF_INET,SOCK_STREAM, 0);
    if(httpd == -1){
        perror("create socket error");
        exit(1);
    }
    // SO_REUSEADDR允许地址重用，每次bind的时候，
    // 如果这个端口正在被使用，bind就会把端口抢过来  
    if((setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)){
        perror("setsockopt error");
        exit(1);
    }
 
    memset(&name, 0, sizeof(name));
    name.sin_family =  AF_INET;
    name.sin_port = htons(*port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(httpd, (struct sockaddr *)&name,sizeof(name)) < 0){
        perror("bind error");
        exit(1);
    }
 
    if(*port == 0){
        socklen_t namelen = sizeof(name);
        if(getsockname(httpd, (struct sockaddr *)&name, &namelen) < 0){
            perror("get sock error");
            exit(1);
        }
        *port = ntohs(name.sin_port);
    }
 
     if(listen(httpd, 5) < 0){
        perror("set listen error");
        exit(1);
        }
    return httpd;
}
 
int main(){
    int serv_sock = -1;
    int port = 0;
    int clie_sock = -1;
    struct sockaddr_in client_name;
    pthread_t newthread;
    socklen_t len_client_name;
 
    serv_sock = startup(&port);
    printf("httpd running on port lalallal;a %d\n", port);
    
    len_client_name = sizeof(client_name);
    while(1){
        clie_sock = accept(serv_sock, (struct sockaddr *)&client_name, &len_client_name);
        if(clie_sock == -1){
            perror("create accept error");
            exit(1);
        }  
 
        if(pthread_create(&newthread, NULL, accept_request, (void *)&clie_sock) != 0){
            perror("create pthread error");
            exit(1);
        }
        
    }
    close(serv_sock);
    return 0;
}
