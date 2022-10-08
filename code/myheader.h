
/* Function Delaration */
void initialize_semaphore();
void *thread(void *vargp);
void dojob(int );
void skip_requesthdrs(rio_t *);
int parse_uri(char *,char *,char *);
void serve_static(int ,char *,int );
void get_filetype(char *,char *);
void serve_dynamic(int ,char *,char *);
