#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include<sys/socket.h>
#include<arpa/inet.h>


static char *ngx_http_kinoi(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_kinoi_handler(ngx_http_request_t *r);

static ngx_command_t ngx_http_kinoi_commands[] = {

    { ngx_string("kinoi"),
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      ngx_http_kinoi,
      0, 0, NULL},
    ngx_null_command
};

static ngx_http_module_t ngx_http_kinoi_module_ctx = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

ngx_module_t ngx_http_kinoi_module = {
    NGX_MODULE_V1,
    &ngx_http_kinoi_module_ctx,
    ngx_http_kinoi_commands,
    NGX_HTTP_MODULE,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NGX_MODULE_V1_PADDING
};

char *socket_path = "/tmp/kinoi.socket";
static ngx_int_t ngx_http_kinoi_handler(ngx_http_request_t *r)
{
    ngx_buf_t *b;
    ngx_chain_t out;

    struct sockaddr_un addr;
    u_char buf[1024];
    int fd,rc;

    if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
      perror("socket error");
      exit(-1);
    }

    memset(buf, 0, sizeof(buf));
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
      perror("1, connect error");
    }

    while( (rc=read(fd, buf, sizeof(buf))) > 0) {
      break;
    }
    close(fd);

    r->headers_out.content_type.len = sizeof("text/plain") - 1;
    r->headers_out.content_type.data = (u_char *) "text/plain";

    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));

    out.buf = b;
    out.next = NULL;

    b->pos = buf;
    b->last = buf + rc;
    b->memory = 1;
    b->last_buf = 1;

    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = rc;
    ngx_http_send_header(r);
    return ngx_http_output_filter(r, &out);
}

static char *ngx_http_kinoi(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_kinoi_handler;

    return NGX_CONF_OK;
}
