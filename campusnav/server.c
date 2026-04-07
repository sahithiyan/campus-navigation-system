/*
 * ================================================================
 *  CAMPUS PATHFINDER - C Backend Server
 *  Amrita Vishwa Vidyapeetham – Nagercoil Campus
 *  HTTP server serving graph algorithm results as JSON
 *  Compile: gcc -o server server.c
 *  Run:     ./server
 *  Serves on: http://localhost:8080
 * ================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32.lib")
  typedef int socklen_t;
#else
  #include <unistd.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #define closesocket close
#endif

#define PORT        8080
#define BUF_SIZE    8192
#define MAX_NODES   12
#define MAX_EDGES   18
#define MAX_NAME    60
#define INF         INT_MAX
#define QUEUE_SIZE  64

/* ── DATA STRUCTURES ── */

typedef struct {
    int  id;
    char name[MAX_NAME];
    char abbr[16];
    int  x, y;   /* SVG canvas coordinates */
} Location;

typedef struct {
    int from, to, dist_m, time_min;
} Edge;

typedef struct AdjNode {
    int to, dist_m, time_min;
    struct AdjNode *next;
} AdjNode;

typedef struct { AdjNode *head; } AdjList;

/* ── CAMPUS DATA ── */

Location locations[MAX_NODES] = {
    { 0,  "Main Gate",                    "Gate",      350,  55 },
    { 1,  "Admin Block",                  "Admin",     350, 160 },
    { 2,  "AVVP Block",                   "AVVP",      155, 215 },
    { 3,  "CSE Block",                    "CSE",       520, 215 },
    { 4,  "Science & Humanities Block",   "S&H",       155, 335 },
    { 5,  "Civil Block",                  "Civil",     350, 305 },
    { 6,  "ECE Block",                    "ECE",       540, 335 },
    { 7,  "Library",                      "Library",    85, 425 },
    { 8,  "Canteen",                      "Canteen",   350, 425 },
    { 9,  "Girls Hostel",                 "G.Hostel",  555, 445 },
    {10,  "Anugraha Bhavan (Boys)",        "Anugraha",  185, 500 },
    {11,  "Vyasa Bhavan (Boys)",           "Vyasa",     475, 500 }
};

Edge edges[MAX_EDGES] = {
    { 0, 1,150,2},{ 1, 2,200,3},{ 1, 3,200,3},{ 1, 5,180,2},
    { 2, 4,150,2},{ 2, 5,220,3},{ 3, 5,250,3},{ 3, 6,180,2},
    { 4, 7,200,3},{ 4, 8,250,3},{ 5, 6,200,3},{ 5, 8,150,2},
    { 6, 9,200,3},{ 6,11,220,3},{ 7,10,160,2},{ 8,10,200,3},
    { 8,11,180,2},{ 9,11,180,2}
};

AdjList graph[MAX_NODES];

/* ── GRAPH BUILD ── */

void addEdge(int u, int v, int d, int t) {
    AdjNode *n = malloc(sizeof(AdjNode));
    n->to=v; n->dist_m=d; n->time_min=t; n->next=graph[u].head;
    graph[u].head = n;
}

void buildGraph(void) {
    for(int i=0;i<MAX_NODES;i++) graph[i].head=NULL;
    for(int i=0;i<MAX_EDGES;i++){
        addEdge(edges[i].from,edges[i].to,edges[i].dist_m,edges[i].time_min);
        addEdge(edges[i].to,edges[i].from,edges[i].dist_m,edges[i].time_min);
    }
}

/* ── PATH UTILITIES ── */

int getEdgeTime(int u, int v){
    for(int i=0;i<MAX_EDGES;i++)
        if((edges[i].from==u&&edges[i].to==v)||(edges[i].from==v&&edges[i].to==u))
            return edges[i].time_min;
    return 0;
}

int getEdgeDist(int u, int v){
    for(int i=0;i<MAX_EDGES;i++)
        if((edges[i].from==u&&edges[i].to==v)||(edges[i].from==v&&edges[i].to==u))
            return edges[i].dist_m;
    return 0;
}

int reconstructPath(int prev[], int src, int dst, int path[]){
    if(dst!=src && prev[dst]==-1) return 0;
    int tmp[MAX_NODES], len=0, cur=dst;
    while(cur!=-1){tmp[len++]=cur; cur=prev[cur];}
    for(int i=0;i<len;i++) path[i]=tmp[len-1-i];
    return len;
}

/* ── ALGORITHMS ── */

/* Bellman-Ford */
int bellmanFord(int src, int dst, int path[], int *dist_out, int *time_out,
                int visited[], int *vc){
    int dist[MAX_NODES], prev[MAX_NODES];
    for(int i=0;i<MAX_NODES;i++){dist[i]=INF;prev[i]=-1;}
    dist[src]=0; *vc=0;
    int vis_set[MAX_NODES]={0};

    for(int iter=0;iter<MAX_NODES-1;iter++){
        int upd=0;
        for(int i=0;i<MAX_EDGES;i++){
            int u=edges[i].from,v=edges[i].to,w=edges[i].dist_m;
            if(dist[u]!=INF && dist[u]+w<dist[v]){
                dist[v]=dist[u]+w; prev[v]=u; upd=1;
                if(!vis_set[v]){vis_set[v]=1; visited[(*vc)++]=v;}
            }
            if(dist[v]!=INF && dist[v]+w<dist[u]){
                dist[u]=dist[v]+w; prev[u]=v; upd=1;
                if(!vis_set[u]){vis_set[u]=1; visited[(*vc)++]=u;}
            }
        }
        if(!upd) break;
    }
    if(!vis_set[src]){visited[(*vc)++]=src;}

    int len=reconstructPath(prev,src,dst,path);
    if(!len) return 0;
    *dist_out=dist[dst];
    *time_out=0;
    for(int i=0;i<len-1;i++) *time_out+=getEdgeTime(path[i],path[i+1]);
    return len;
}

/* BFS */
int bfs(int src, int dst, int path[], int *dist_out, int *time_out,
        int visited[], int *vc){
    int prev[MAX_NODES], vis[MAX_NODES], q[QUEUE_SIZE];
    for(int i=0;i<MAX_NODES;i++){prev[i]=-1;vis[i]=0;}
    int front=0,rear=0; *vc=0;
    vis[src]=1; q[rear++]=src;
    while(front<rear){
        int u=q[front++]; visited[(*vc)++]=u;
        if(u==dst) break;
        for(AdjNode*c=graph[u].head;c;c=c->next)
            if(!vis[c->to]){vis[c->to]=1;prev[c->to]=u;q[rear++]=c->to;}
    }
    int len=reconstructPath(prev,src,dst,path);
    if(!len) return 0;
    *dist_out=0; *time_out=0;
    for(int i=0;i<len-1;i++){
        *dist_out+=getEdgeDist(path[i],path[i+1]);
        *time_out+=getEdgeTime(path[i],path[i+1]);
    }
    return len;
}

/* DFS */
static int dfsVis[MAX_NODES], dfsPrev[MAX_NODES];
static int dfsFound, dfsVList[MAX_NODES], dfsVC;

void dfsHelper(int u, int dst){
    dfsVis[u]=1; dfsVList[dfsVC++]=u;
    if(u==dst){dfsFound=1;return;}
    for(AdjNode*c=graph[u].head;c&&!dfsFound;c=c->next)
        if(!dfsVis[c->to]){dfsPrev[c->to]=u; dfsHelper(c->to,dst);}
}

int dfs(int src, int dst, int path[], int *dist_out, int *time_out,
        int visited[], int *vc){
    for(int i=0;i<MAX_NODES;i++){dfsVis[i]=0;dfsPrev[i]=-1;}
    dfsFound=0; dfsVC=0;
    dfsHelper(src,dst);
    *vc=dfsVC;
    for(int i=0;i<dfsVC;i++) visited[i]=dfsVList[i];
    int len=reconstructPath(dfsPrev,src,dst,path);
    if(!dfsFound||!len) return 0;
    *dist_out=0; *time_out=0;
    for(int i=0;i<len-1;i++){
        *dist_out+=getEdgeDist(path[i],path[i+1]);
        *time_out+=getEdgeTime(path[i],path[i+1]);
    }
    return len;
}

/* ── JSON BUILDERS ── */

/* Escape strings for JSON */
void jsonStr(char *out, size_t sz, const char *s){
    size_t j=0;
    for(size_t i=0;s[i]&&j<sz-2;i++){
        if(s[i]=='"'||s[i]=='\\'){out[j++]='\\'; out[j++]=s[i];}
        else out[j++]=s[i];
    }
    out[j]='\0';
}

/* Build full graph JSON for frontend initialization */
void buildGraphJSON(char *buf, int sz){
    char tmp[128];
    int pos=0;
    pos+=snprintf(buf+pos,sz-pos,"{\"nodes\":[");
    for(int i=0;i<MAX_NODES;i++){
        char esc[64]; jsonStr(esc,sizeof(esc),locations[i].name);
        char esca[20]; jsonStr(esca,sizeof(esca),locations[i].abbr);
        pos+=snprintf(buf+pos,sz-pos,
            "%s{\"id\":%d,\"name\":\"%s\",\"abbr\":\"%s\",\"x\":%d,\"y\":%d}",
            i?",":"", i, esc, esca, locations[i].x, locations[i].y);
    }
    pos+=snprintf(buf+pos,sz-pos,"],\"edges\":[");
    for(int i=0;i<MAX_EDGES;i++){
        pos+=snprintf(buf+pos,sz-pos,
            "%s{\"from\":%d,\"to\":%d,\"dist\":%d,\"time\":%d}",
            i?",":"", edges[i].from, edges[i].to,
            edges[i].dist_m, edges[i].time_min);
    }
    pos+=snprintf(buf+pos,sz-pos,"]}");
    (void)tmp;
}

/* Build route result JSON */
void buildRouteJSON(char *buf, int sz, const char *algo,
                    int src, int dst, int path[], int len,
                    int dist, int time_v, int visited[], int vc){
    int pos=0;
    char esc_algo[32]; jsonStr(esc_algo,sizeof(esc_algo),algo);

    pos+=snprintf(buf+pos,sz-pos,
        "{\"algo\":\"%s\",\"src\":%d,\"dst\":%d,\"dist\":%d,\"time\":%d,"
        "\"found\":%s,",
        esc_algo, src, dst, dist, time_v, len>0?"true":"false");

    pos+=snprintf(buf+pos,sz-pos,"\"path\":[");
    for(int i=0;i<len;i++)
        pos+=snprintf(buf+pos,sz-pos,"%s%d",i?",":"",path[i]);
    pos+=snprintf(buf+pos,sz-pos,"],");

    pos+=snprintf(buf+pos,sz-pos,"\"visited\":[");
    for(int i=0;i<vc;i++)
        pos+=snprintf(buf+pos,sz-pos,"%s%d",i?",":"",visited[i]);
    pos+=snprintf(buf+pos,sz-pos,"]}");
}

/* ── HTTP SERVER ── */

void sendResponse(int sock, int status, const char *ctype,
                  const char *body, int cors){
    char hdr[512];
    const char *stxt = (status==200)?"OK":(status==404)?"Not Found":"Bad Request";
    int blen = (int)strlen(body);
    int hlen = snprintf(hdr, sizeof(hdr),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "%s"
        "Connection: close\r\n\r\n",
        status, stxt, ctype, blen,
        cors?"Access-Control-Allow-Origin: *\r\n":"");
    send(sock, hdr, hlen, 0);
    send(sock, body, blen, 0);
}

/* Parse query param: ?key=value */
int getQueryInt(const char *qs, const char *key, int def){
    if(!qs) return def;
    char search[32]; snprintf(search,sizeof(search),"%s=",key);
    const char *p = strstr(qs, search);
    if(!p) return def;
    return atoi(p + strlen(search));
}

const char *getQueryStr(const char *qs, const char *key, char *out, int sz){
    if(!qs){out[0]='\0';return out;}
    char search[32]; snprintf(search,sizeof(search),"%s=",key);
    const char *p = strstr(qs, search);
    if(!p){out[0]='\0';return out;}
    p += strlen(search);
    int i=0;
    while(*p && *p!='&' && i<sz-1) out[i++]=*p++;
    out[i]='\0';
    return out;
}

void handleRequest(int sock, char *req){
    /* Parse method and path */
    char method[8]="", path_buf[256]="", qs[256]="";
    sscanf(req, "%7s %255s", method, path_buf);

    /* Split path and query string */
    char *q = strchr(path_buf,'?');
    if(q){ *q='\0'; strncpy(qs, q+1, sizeof(qs)-1); }

    /* OPTIONS preflight */
    if(strcmp(method,"OPTIONS")==0){
        char resp[] = "HTTP/1.1 204 No Content\r\n"
                      "Access-Control-Allow-Origin: *\r\n"
                      "Access-Control-Allow-Methods: GET\r\n"
                      "Connection: close\r\n\r\n";
        send(sock, resp, (int)strlen(resp), 0);
        return;
    }

    /* GET /graph – return nodes + edges */
    if(strcmp(path_buf,"/graph")==0){
        char body[4096]; buildGraphJSON(body, sizeof(body));
        sendResponse(sock, 200, "application/json", body, 1);
        return;
    }

    /* GET /route?algo=bellman&src=0&dst=9 */
    if(strcmp(path_buf,"/route")==0){
        char algo_s[16]; getQueryStr(qs,"algo",algo_s,sizeof(algo_s));
        int src = getQueryInt(qs,"src",0);
        int dst = getQueryInt(qs,"dst",1);

        if(src<0||src>=MAX_NODES||dst<0||dst>=MAX_NODES){
            sendResponse(sock,400,"application/json","{\"error\":\"invalid node\"}",1);
            return;
        }

        int path[MAX_NODES], visited[MAX_NODES], vc=0, dist=0, time_v=0, len=0;

        if(strcmp(algo_s,"bfs")==0)
            len = bfs(src,dst,path,&dist,&time_v,visited,&vc);
        else if(strcmp(algo_s,"dfs")==0)
            len = dfs(src,dst,path,&dist,&time_v,visited,&vc);
        else { /* default bellman-ford */
            strcpy(algo_s,"bellman");
            len = bellmanFord(src,dst,path,&dist,&time_v,visited,&vc);
        }

        const char *algo_name = (strcmp(algo_s,"bfs")==0)?"BFS":
                                 (strcmp(algo_s,"dfs")==0)?"DFS":"Bellman-Ford";

        char body[2048];
        buildRouteJSON(body,sizeof(body),algo_name,src,dst,
                       path,len,dist,time_v,visited,vc);
        sendResponse(sock, 200, "application/json", body, 1);
        return;
    }

    sendResponse(sock, 404, "application/json", "{\"error\":\"not found\"}", 1);
}

int main(void){
#ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2,2),&wsa);
#endif
    buildGraph();

    int srv = (int)socket(AF_INET, SOCK_STREAM, 0);
    if(srv < 0){ perror("socket"); return 1; }

    int opt=1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(PORT);

    if(bind(srv,(struct sockaddr*)&addr,sizeof(addr))<0){
        perror("bind"); return 1;
    }
    listen(srv, 10);

    printf("\n");
    printf("  ================================================\n");
    printf("  CAMPUS PATHFINDER - C Backend Server\n");
    printf("  Amrita Vishwa Vidyapeetham, Nagercoil\n");
    printf("  ================================================\n");
    printf("  Server running at http://localhost:%d\n", PORT);
    printf("  Open index.html in your browser to use the app.\n");
    printf("  Press Ctrl+C to stop.\n");
    printf("  ================================================\n\n");

    while(1){
        struct sockaddr_in cli; socklen_t clen=sizeof(cli);
        int csock=(int)accept(srv,(struct sockaddr*)&cli,&clen);
        if(csock<0) continue;

        char buf[BUF_SIZE]={0};
        recv(csock, buf, sizeof(buf)-1, 0);
        handleRequest(csock, buf);
        closesocket(csock);
    }

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
