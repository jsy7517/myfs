#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct superblock{
    unsigned freeinode[16];
    unsigned freedatablock[32];
} superblock;

typedef struct inode{
    char file_type;
    int file_creation_time[6];
    int file_size;
    int datablock_info[3];
} inode;

typedef struct datablock{
    char data[128];
} datablock;

typedef struct tree{
    char name[5];
    int inum;
    struct tree * left;
    struct tree * right;
}tree;

tree * root;
tree * now;
tree * parent;
tree * next;
tree * work;


void getdata_d(datablock *db, FILE * ifp, int tmp);
int getblock_d(datablock * db, superblock * sb, FILE * ifp);
void makedouble(datablock * db, int blocks, FILE * ifp);
void mytouch(char * tmp2);
void mycpfrom(char * tmp2, char * tmp3);
void mymkdir(char *tmp2);
void show_inode_info(inode in);
void myshowinode(char *tmp2);
void mycd(char *, int);
void myshowblock(char *tmp2);
void getroot(void);
void get_directory(superblock *sb, char * tmp2);
void myshowfile(char *tmp2, char *tmp3, char *tmp4);
void mystate(void);
void myshowfile(char *tmp2, char *tmp3, char *tmp4);
void classify(char *,char *,char *,char *,char *);
int getblock(datablock *, superblock *, FILE *);
void set_type(inode *);
void set_time(inode *);
void set_superblock_i(unsigned *a); // 슈퍼블록에 사용 정보 저장
void set_superblock_d(unsigned *a);
void freebit(unsigned *a, int mod); // 비트 1에서 0으로 바꿔주는 함수
int free_print(unsigned a);
void freesuperblock(int n, superblock *sb);
int find_free_inode(unsigned a);
int find_free_datablock(unsigned a);
void set_size(inode *in, char * sourcefile);
void set_datablock_info(int *, int);
void get_single(char *, int, int);
int bit_print(char *, int n);
int num_bit(char *, int n);
void freeinode(inode *in, superblock * sb);
void get_double(char *, int, int);
void getdata(datablock *db, FILE *);
void set_inode_info_f(superblock * sb, char * tmp2);
int set_inode_info_d(superblock * sb);
int makesingle(datablock * db, int blocks, FILE * ifp, int);
void init();
void mypwd();
void mycd(char * a, int num);
void myls(int);                      //!!
void myrmdir(char *a);               //비트열 초기화 아직 안함
void mytree(char *a);
void mymv(char *, char *);           //!!
void myfs_shell();                   //!!
void filesave();                     //!!
void fileload();                     //!!

inode in[512] = {0};
datablock db[1024] = {0};
superblock sb[1] = {0};

int main(void){
    myfs_shell();
        return 0;
}

void mycpfrom(char * tmp2, char * tmp3)
{
    char file_name[5] = {0};
    FILE * ifp;
    int blocks, remain, n, size = 0;
    int flag, flag2, tmp0, tmp1, s_flag;
    static int i, j;
    strncpy(file_name, tmp3, 4);
    
    ifp = fopen(tmp2, "rb");
    fseek(ifp, 0, SEEK_END);
    
    size = ftell(ifp);
    rewind(ifp);
    
    blocks = size / 128;
    remain = size % 128;
    while(1){
        flag = find_free_inode(sb -> freeinode[i]);
        if(flag <= 31){
            if(flag == 31)
                i++;
            break;
        }
        if(i == 16){
            i = 0;
            break;
        }
    }
    while(1){
        flag2 = find_free_datablock(sb -> freeinode[j]);
        if(flag2 <= 31){
            if(flag2 == 31)
                j++;
            break;
        }
        if(j == 32){
            j = 0;
            break;
        }
    }
    set_datablock_info(&(in[flag + (32 * i)].datablock_info[0]), flag2 + (32 * j));
    
    if(size <= 128){
        set_inode_info_f(sb, tmp2);
        getblock(db, sb, ifp);
    }
    else if(size > 128 && size <= 128 * 102){
        set_inode_info_f(sb, tmp2);
        getblock(db, sb, ifp);
        s_flag = makesingle(db, blocks, ifp, flag + (32 * i));
        set_datablock_info(&(in[flag + (32 * i)].datablock_info[1]), s_flag);
    }
    else{
        set_inode_info_f(sb, tmp2);
        getblock(db, sb, ifp);
        s_flag = makesingle(db, blocks, ifp, flag + (32 * i));
        set_datablock_info(&(in[flag + (32 * i)].datablock_info[1]), s_flag);
    }
}



int makesingle(datablock * db, int blocks, FILE * ifp, int flag)
{
    int single_member[102] = {0};
    int tmp, tmp2, tmp3, count1 = 0, block_num;
    static int j;
    bool sing = true;
    
    block_num = blocks;
    
    for(int i = 0; i < blocks; i++){
        tmp = getblock(db, sb, ifp);
        single_member[i] = tmp;
    }
    
    for(int i = 0; i < blocks; i++){
        if(count1 % 5 == 4){
            count1 += 1;
            i = i - 1;
        }
        get_single(db[tmp + 1].data, count1, single_member[i]);
        count1++;
    }
    return (tmp + 1);
}

void getroot(void){
    set_inode_info_d(sb);
    get_directory(sb, "/");
}

void myshowfile(char *tmp2, char *tmp3, char *tmp4)
{
    int argv_1, argv_2, c;
    FILE * ifp;
    
    argv_1 = atoi(tmp2);
    argv_2 = atoi(tmp3);
    
    ifp = fopen(tmp4, "r");
    fseek(ifp, argv_1 - 1, SEEK_SET);
    
    for(int i = 0; i < argv_2; i++){
        c = getc(ifp);
        putchar(c);
    }
    putchar('\n');
}


void set_datablock_info(int *n, int tmp)
{
    *n = tmp + 1;
}

void myshowblock(char *tmp2)
{
    int index = 0;
    index = atoi(tmp2);
    for(int i = 0; i < 128; i++)
        printf("%c", db[index - 1].data[i]);
    putchar('\n');
}

void myshowinode(char *tmp2)
{
    int index = 0;
    index = atoi(tmp2);
    show_inode_info(in[index - 1]);
}

void show_inode_info(inode in)
{
    if(in.file_type == 'd')
        printf("file type : directory file\n");
    else if(in.file_type == '-')
        printf("file type : regular file\n");
    printf("file size : %d\n", in.file_size);
    printf("modified time : %d/%d/%d %d:%d:%d\n", in.file_creation_time[0], in.file_creation_time[1], in.file_creation_time[2], in.file_creation_time[3], in.file_creation_time[4], in.file_creation_time[5]);
    printf("data block list : %d, %d, %d\n", in.datablock_info[0], in.datablock_info[1], in.datablock_info[2]);
}

void mymkdir(char *tmp2)
{
   	int num;
    tree * dir = NULL;
    dir = (tree *)malloc(sizeof(tree));
    num = set_inode_info_d(sb);
    get_directory(sb, tmp2);
    strncpy(dir -> name,tmp2,4);
    if(now -> left == NULL){
        now -> left = dir;
        now -> left -> right=NULL;
        now -> left -> left=NULL;
        now -> left -> inum = num+1;
    }
    else{
        next = now -> left;
        while(!(next -> right == NULL)){
            next = next -> right;}
        next -> right = dir;
        next -> right -> left =NULL;
        next -> right -> right = NULL;
        next -> right -> inum = num+1;
    }
}

void get_directory(superblock * sb, char * tmp2)
{
    int tmp = 0;
    static int i, j;
    int len = strlen(tmp2);
    while(1){
        tmp = find_free_datablock(sb -> freedatablock[i]);
        if(tmp <= 31){
            if(tmp == 31)
                j++;
            set_superblock_d(&(sb -> freedatablock[i]));
            set_datablock_info(&(in[tmp + (32 * i)].datablock_info[0]), tmp + (32 * i));
            strncpy(db[tmp + (32 * i)].data, tmp2, len);
            break;
        }
        if(i == 32){
            printf("no free datablocks\n");
            i = 0;
            break;
        }
    }
}


void mystate(void)
{
    int free_inodes = 0, free_datablocks = 0;
    for(int i = 0; i < 16; i++){
        free_inodes += free_print(sb -> freeinode[i]);
    }
    for(int i = 0; i < 32; i++){
        free_datablocks += free_print(sb -> freedatablock[i]);
    }
    printf("free inode : %d\n", free_inodes);
    printf("free data block : %d\n", free_datablocks);
    
}

void classify(char * tmp,char *tmp1,char *tmp2,char *tmp3,char *tmp4)
{
    int i=0,j=0,num=0;
    while(tmp[i]!='\0'){
        if(tmp[i]==' '){
            num++;
            j=0;
            i++;
        }
        if(num==0){
            tmp1[j]=tmp[i];
            i++;
            j++;
        }
        if(num==1){
            tmp2[j]=tmp[i];
            i++;
            j++;
        }
        if(num==2){
            tmp3[j]=tmp[i];
            i++;
            j++;
        }
        if(num==3){
            tmp4[j]=tmp[i];
            i++;
            j++;
        }
    }
}

int set_inode_info_d(superblock * sb)
{
    static int i, j;
    int tmp, tmp2;
    while(1){
        tmp2 = find_free_inode(sb -> freeinode[i]);
        if(tmp2 <= 31){
            set_superblock_i(&(sb -> freeinode[i]));
            if(tmp2 == 31)
                i++;
            break;
        }
        if(i == 16){
            printf("no free inodes\n");
            i = 0;
            break;
        }
    }
    
    while(1){
        tmp = find_free_datablock(sb -> freedatablock[j]);
        if(tmp <= 31){
            set_datablock_info(&(in[tmp + (32 * j)].datablock_info[0]), tmp + (32 * j));
            in[tmp + (32 * j)].file_type = 'd';
            set_time(&in[tmp + (32 * j)]);
            in[tmp + (32 * j)].file_size = 0;
            if(tmp == 31)
                j++;
            break;
        }
        if(j == 32){
            printf("no free datablocks\n");
            j = 0;
            break;
        }
        
    }
    return tmp2;
}

/*void mytouch(char * tmp2)
 {
	int tmp = 0;
	tmp = find_free_inode(sb -> freeinode[0]);
	set_superblock_i(&(sb -> freeinode[0]));
	set_superblock_d(&(sb -> freedatablock[0]));
	set_datablock_info(&(in[tmp].datablock_info[0]), tmp);
	in[tmp].file_type = '-';
	set_time(&in[tmp]);
	in[tmp].file_size = 0;
 }*/

void set_inode_info_f(superblock * sb, char * tmp2)
{
    int tmp = 0;
    static int j = 0;
    while(1){
        if(tmp <= 31){
            tmp = find_free_inode(sb -> freeinode[j]);
            tmp = tmp + (32 * j);
            set_superblock_i(&(sb -> freeinode[j]));
            in[tmp].file_type = '-';
            set_time(&in[tmp]);
            set_size(&in[tmp], tmp2);
            if(tmp == 31)
                j++;
            break;
        }
        if(j == 16){
            printf("no free inodes\n");
            j = 0;
            break;
        }
        
    }
    
}

void set_size(inode *in, char * sourcefile)
{
    FILE * ifp;
    int size;
    ifp = fopen(sourcefile, "rb");
    fseek(ifp, 0, SEEK_END);
    size = ftell(ifp);
    in -> file_size = size;
    return;
}


void freeinode(inode *in, superblock * sb)
{
    return;
}


int getblock(datablock * db, superblock * sb, FILE * ifp)
{
    int tmp;
    static int j;
    while(1){
        tmp = find_free_datablock(sb -> freedatablock[j]);
        if(tmp <= 31){
            db = db + tmp + (32 * j);
            getdata(db, ifp);
            set_superblock_d(&(sb -> freedatablock[j]));
            if(tmp == 31)
                j++;
            return tmp + (32 * j);
        }
        if(j == 32){
            printf("no free datablocks\n");
            j = 0;
            break;
        }
    }
}

void getdata(datablock *db, FILE * ifp)
{
    int c;
    if((c = fread(&db -> data, 128, 1, ifp)) == EOF){
        rewind(ifp);
        return;
    }
}




void set_time(inode * in)
{
    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);
    
    in -> file_creation_time[0] = t -> tm_year + 1900;
    in -> file_creation_time[1] = t -> tm_mon + 1;
    in -> file_creation_time[2] = t -> tm_mday;
    in -> file_creation_time[3] = t -> tm_hour;
    in -> file_creation_time[4] = t -> tm_min;
    in -> file_creation_time[5] = t -> tm_sec;
}

void freesuperblock(int n, superblock *sb)
{
    int div = 0, mod = 0;
    div = n / 32;
    mod = n % 32;
    freebit(&(sb -> freeinode[div]), mod);
}

int find_free_inode(unsigned a)
{
    int i, flag = 0;
    int n = sizeof(unsigned) * 8;
    int mask = 1 << (n-1);
    
    for(i = 1; i <= n; ++i){
        if((a & mask) == 0){
            return flag;
        }
        else{
            a <<= 1;
            flag++;
        }
    }
}

int find_free_datablock(unsigned a)
{
    int i, flag = 0;
    int n = sizeof(unsigned) * 8;
    int mask = 1 << (n-1);
    
    for(i = 1; i <= n; ++i){
        if((a & mask) == 0){
            return flag;
        }
        else{
            a <<= 1;
            flag++;
        }
    }
}

void set_superblock_i(unsigned *a){ // 슈퍼블록에 사용 정보 저장
    int tmp, sum = 0;
    static int n = 31;
    tmp = 1 << n;
    *a |= tmp;
    --n;
    if(n < 0)
        n = 31;
}

void set_superblock_d(unsigned *a){ // 슈퍼블록에 사용 정보 저장
    int tmp, sum = 0;
    static int n = 31;
    tmp = 1 << n;
    *a |= tmp;
    --n;
    if(n < 0)
        n = 31;
}

void freebit(unsigned *a, int mod){ // 비트 1에서 0으로 바꿔주는 함수
    int i, tmp;
    int n = sizeof(unsigned) * 8;
    int del = 32 - mod;
    tmp = 1 << del;
    *a &= tmp;
}

int free_print(unsigned a){
    int i, free = 0;
    bool check = false;
    int n = sizeof(unsigned) * 8;
    int mask = 1 << (n-1);
    for(i = 1; i <= n; ++i){
        if((a & mask) == 0){
            free++;
        }
        a <<= 1;
    }
    return free;
}

void get_single(char * a, int n, int num){
    // n은 data 인덱스 , 5로 나눴을때 나머지가 4인수들 못씀
    if(n%5 == 0){
        for(int i = 0; i < 8; i++)
            if(num&1<<(i+2))
                a[n]|=1<<i;
        for(int i = 6; i < 8; i++)
            if(num&1<<(7-i))
                a[n+1] |= 1<<(13-i);
        
    }
    else if(n%5 == 1){
        for(int i=0; i<6; i++)
            if(num&1<<(i+3))
                a[n]|= 1<<i;
        for(int i= 4; i<8; i++)
            if(num&1<<(i-4))
                a[n+1]|= 1<<i;
    }
    else if(n%5 == 2){
        for(int i= 0; i<4; i++)
            if(num&1<<(i+3))
                a[n]|= 1<<i;
        for(int i=2; i<8; i++)
            if(num&1<<(i-2))
                a[n+1]|= 1<<i;
    }
    else if(n%5 == 3){
        for(int i=0;i<2;i++)
            if(num&1<<(i+8))
                a[n]|= 1<<i;
        for(int i = 0; i <8; i++)
            if(num&1<<(i))
                a[n+1]|= 1<<i;
    }
}

void get_double(char * a, int n, int num){
    // n은 data 인덱스 , 5로 나눴을때 나머지가 4인수들 못씀
    if(n%5 == 0){
        for(int i = 0; i < 8; i++)
            if(num&1<<(i+2))
                a[n]|=1<<i;
        for(int i = 6; i < 8; i++)
            if(num&1<<(7-i))
                a[n+1] |= 1<<(13-i);
        
    }
    else if(n%5 == 1){
        for(int i=0; i<6; i++)
            if(num&1<<(i+3))
                a[n]|= 1<<i;
        for(int i= 4; i<8; i++)
            if(num&1<<(i-4))
                a[n+1]|= 1<<i;
    }
    else if(n%5 == 2){
        for(int i= 0; i<4; i++)
            if(num&1<<(i+3))
                a[n]|= 1<<i;
        for(int i=2; i<8; i++)
            if(num&1<<(i-2))
                a[n+1]|= 1<<i;
    }
    else if(n%5 == 3){
        for(int i=0;i<2;i++)
            if(num&1<<(i+8))
                a[n]|= 1<<i;
        for(int i = 0; i <8; i++)
            if(num&1<<(i))
                a[n+1]|= 1<<i;
    }
}

int bit_print(char * a, int n){
    char * c = (char *) a;
    int bit = c[n/8]&1<<(7-(n%8));
    return bit;
}

int num_bit(char * a, int n){
    int num=0,count=0;
    
    for(int i=(n-1)*10;i<(n-1)*10+10;i++,count++)
        if(bit_print(a,i))
            num |= 1 << (9-count);
    
    return num;
}
void init(){
    tree * start = NULL;
    start = (tree *)malloc(sizeof(tree));
    root = start;
    strcpy(root->name,"/");
    root -> left = NULL;
    root -> inum = 1;
    now = root;
    parent = root;
}   // tree 시작
void mypwd(){
    printf("%s\n",now->name);
}
void mycd(char * a, int num){
    int i=1;
    
    if(!num){
        now = root;
        return;
    }
    
    else if(num){
        if(!strcmp(a,"."))
            return;
        
        else if(!strcmp(a,".."))
            now = parent;
        
        else{
            if(now->left==NULL)
                printf("비어있습니다.\n");
            if(!strncmp(now->left->name,a,4))
                now = now->left;
            else{
                next = now->left;
                while(i){
                    if(next->right==NULL){
                        printf("없는 디렉터리입니다.\n");
                        return;
                    }
                    if(!strncmp(next->right->name,a,4)){
                        i--;
                        now = next -> right;
                    }
                    next = next -> right;
                }
            }
        }
    }
}
void myls(int op){
    char name[100][5],tmp[5];
    int num,i=0,inm[100]={0},itmp;
    
    if(now->left==NULL)
        return;
    
    else{
        strcpy(name[i],now->left->name);
        inm[i]=now->left->inum;
        i++;
        next = now->left;
        
        while(!(next->right==NULL)){
            strcpy(name[i],next->right->name);
            inm[i]=next->right->inum;
            // printf("namecopy\n");
            i++;
            next = next->right;
        }
    }
    num = i;
    for(int j=0;j<num;j++){
        for(int k = j+1; k<num;k++){
            if(strcmp(name[j],name[k])>0)
            {
                strcpy(tmp,name[j]);
                itmp = inm[j];
                strcpy(name[j],name[k]);
                inm[j] = inm[k];
                strcpy(name[k],tmp);
                inm[k] = itmp;
            }
        }
    }
    if(!op){
        printf(".\n");
        printf("..\n");
        for(int a=0;a<num;a++)
            printf("%s\n",name[a]);
    }
    if(op==1){
        printf("%d .\n",now->inum);
        printf("%d ..\n",parent->inum);
        for(int a=0;a<num;a++)
            printf("%d %s\n",inm[a],name[a]);
    }
    if(op==2){
        printf("%c %4d %d/%d/%d %d:%d:%d .\n",in[((now->inum)-1)].file_type,in[((now->inum)-1)].file_size,in[((now->inum)-1)].file_creation_time[0],in[((now->inum)-1)].file_creation_time[1],in[((now->inum)-1)].file_creation_time[2],in[((now->inum)-1)].file_creation_time[3],in[((now->inum)-1)].file_creation_time[4],in[((now->inum)-1)].file_creation_time[5]);
          printf("%c %4d %d/%d/%d %d:%d:%d ..\n",in[((parent->inum)-1)].file_type,in[((parent->inum)-1)].file_size,in[((parent->inum)-1)].file_creation_time[0],in[((parent->inum)-1)].file_creation_time[1],in[((parent->inum)-1)].file_creation_time[2],in[((parent->inum)-1)].file_creation_time[3],in[((parent->inum)-1)].file_creation_time[4],in[((parent->inum)-1)].file_creation_time[5]);
        for(int a=0;a<num;a++)
          printf("%c %4d %d/%d/%d %d:%d:%d %s\n",in[(inm[a]-1)].file_type,in[(inm[a]-1)].file_size,in[(inm[a]-1)].file_creation_time[0],in[(inm[a]-1)].file_creation_time[1],in[(inm[a]-1)].file_creation_time[2],in[(inm[a]-1)].file_creation_time[3],in[(inm[a]-1)].file_creation_time[4],in[(inm[a]-1)].file_creation_time[5],name[a]);
    }
    if(op==3){
        printf("%d %c %4d %d/%d/%d %d:%d:%d .\n",now->inum,in[((now->inum)-1)].file_type,in[((now->inum)-1)].file_size,in[((now->inum)-1)].file_creation_time[0],in[((now->inum)-1)].file_creation_time[1],in[((now->inum)-1)].file_creation_time[2],in[((now->inum)-1)].file_creation_time[3],in[((now->inum)-1)].file_creation_time[4],in[((now->inum)-1)].file_creation_time[5]);
        printf("%d %c %4d %d/%d/%d %d:%d:%d ..\n",parent->inum,in[((parent->inum)-1)].file_type,in[((parent->inum)-1)].file_size,in[((parent->inum)-1)].file_creation_time[0],in[((parent->inum)-1)].file_creation_time[1],in[((parent->inum)-1)].file_creation_time[2],in[((parent->inum)-1)].file_creation_time[3],in[((parent->inum)-1)].file_creation_time[4],in[((parent->inum)-1)].file_creation_time[5]);
        for(int a=0;a<num;a++)
            printf("%d %c %4d %d/%d/%d %d:%d:%d %s\n",inm[a],in[(inm[a]-1)].file_type,in[(inm[a]-1)].file_size,in[(inm[a]-1)].file_creation_time[0],in[(inm[a]-1)].file_creation_time[1],in[(inm[a]-1)].file_creation_time[2],in[(inm[a]-1)].file_creation_time[3],in[(inm[a]-1)].file_creation_time[4],in[(inm[a]-1)].file_creation_time[5],name[a]);
    }
}
void myrmdir(char * a){
    tree * pre;
    tree * con;
    if(!strcmp(now->left->name,a)){
        if(now->left->right==NULL)
            free(now->left);
        else{
            pre = now;
            con = now->left->right;
            free(now->left);
            pre->left=con;
        }
    }
    else{
        next = now->left;
        pre = next;
        while(!(next->right==NULL)){
            if(!strcmp(next->right->name,a)){
                if(next->right->right==NULL)
                    free(next->right);
                else{
                    con = next->right->right;
                    free(next->right);
                    pre->right =con;
                }
                break;
            }
            pre = next;
            next = next->right;
        }
    }
}
void mytree(char * a){
    int level[512]={0},j=1,count=0,ncheck[512]={0},go;
    tree *check[512];
    tree * next = NULL;
    tree * save = NULL;
    tree * turn = NULL;
    for(int i=0;i<512;i++)
        level[i]=i;
    
    if(a[0]=='\0'){
        printf("%s\n",now->name);
        if(now->left!=NULL){
            
            for(int i=0;i<3*level[j-1]+2;i++)
                printf("-");
            printf("* ");
            printf("%s\n",now->left->name);
            next = now -> left;
            
            if(next->right!=NULL){
                save = next;
                check[count] = save;
                ncheck[count]=j;
                count++;
            } // 첫번째 이동
            
            while(next->left!=NULL){
                j++;
                for(int i=0;i<3*level[j-1]+2;i++)
                    printf("-");
                printf("* ");
                printf("%s\n",next->left->name);
                next = next->left;
                
                if(next->right!=NULL){
                    save = next;
                    check[count] = save;
                    ncheck[count]=j;
                    count++;
                }
            } //맨 왼쪽 끝
            
            turn = next;
            while(turn->right!=NULL){
                for(int i=0;i<3*level[j-1]+2;i++)
                    printf("-");
                printf("* ");
                printf("%s\n",next->right->name);
                next = next-> right;
                count--;
                check[count]=NULL;
                ncheck[count]=0;
                
                if(next->left!=NULL){
                    if(next->right!=NULL){
                        save = next;
                        check[count] = save;
                        ncheck[count]=j;
                        count++;
                    }
                    while(next->left!=NULL){
                        j++;
                        for(int i=0;i<3*level[j-1]+2;i++)
                            printf("-");
                        printf("* ");
                        printf("%s\n",next->left->name);
                        next = next->left;
                        
                        if(next->right!=NULL){
                            save = next;
                            check[count] = save;
                            ncheck[count]=j;
                            count++;
                        }
                    }
                }
                turn = next;
            }
            
            
            
            while(ncheck[0]){
                for(int a = 511;a>-1;a--){
                    if(ncheck[a]){
                        j = ncheck[a];
                        turn = check[a];
                        count--;
                        check[a]=NULL;
                        ncheck[a]=0;
                        break;
                    }
                }
                while(turn->right!=NULL){
                    next = turn;
                    for(int i=0;i<3*level[j-1]+2;i++)
                        printf("-");
                    printf("* ");
                    printf("%s\n",next->right->name);
                    next = next-> right;
                    
                    if(next->left!=NULL){
                        if(next->right!=NULL){
                            save = next;
                            check[count] = save;
                            ncheck[count]=j;
                            count++;
                        }
                        
                        while(next->left!=NULL){
                            j++;
                            for(int i=0;i<3*level[j-1]+2;i++)
                                printf("-");
                            printf("* ");
                            printf("%s\n",next->left->name);
                            next = next->left;
                            
                            if(next->right!=NULL){
                                save = next;
                                check[count] = save;
                                ncheck[count]=j;
                                count++;
                            }
                        }
                    }
                    turn = next;
                }
            }
        }
    }
}
void mymv(char * a, char * b){
    tree * name;
    if(!strncmp(a,now->left->name,4))
        strncpy(now->left->name,b,4);
    else{
        next = now->left;
        while(next->right!=NULL){
            if(!strncmp(next->right->name,a,4)){
                strncpy(next->right->name,b,4);
                break;
            }
            next = next -> right;
        }
    }
}
void myfs_shell(){
    char *tmp, *tmp1, *tmp2, *tmp3, *tmp4;
    int len, my=0,check=0;
    bool check_fs = false;
    FILE * point;
    
    while(1){
        tmp = (char *)calloc(25, sizeof(char));
        tmp1 = (char *)calloc(25, sizeof(char));
        tmp2 = (char *)calloc(25, sizeof(char));
        tmp3 = (char *)calloc(25, sizeof(char));
        tmp4 = (char *)calloc(25, sizeof(char));
        printf("[%s ]$ : ",now->name);
        scanf("%[^\n]",tmp);
        getchar();
        classify(tmp,tmp1,tmp2,tmp3,tmp4);
        if(!strncmp(tmp,"my",2))
            my++;
        if(!strcmp(tmp,"byebye")){
            filesave();
            free(tmp);
            free(tmp1);
            free(tmp2);
            free(tmp3);
            free(tmp4);
            exit(1);
        }
        if(!my)
            system(tmp);
        if(my){
            if(!strncmp(tmp1, "mymkfs", 6)){
                if(check_fs){
                    printf("error : myfs exists\n");
                    continue;
                }
                if((point = fopen("myfs","rb+"))){
                fopen("myfs", "rb+");
                fileload();
                }
                init();
                getroot();
                check_fs = 1;
            }
            if(!strncmp(tmp1,"myls",4)){
                if(tmp[4]=='\0')
                    myls(0);
                
                else if((!strncmp(tmp2, "-li", 3))||(!strncmp(tmp2, "-il", 3))){
                    myls(3);
                }
                
                else if(!strncmp(tmp2,"-i",2)){
                    myls(1);
                }
                
                else if(!strncmp(tmp2,"-l",2)){
                    myls(2);
                }
            }
            
            else if(!strncmp(tmp1,"mycat",5))
                printf("mycat\n");
            
            else if(!strncmp(tmp1,"myshowfile",10)){
                myshowfile(tmp2, tmp3, tmp4);
            }
            
            else if(!strncmp(tmp1,"mypwd",5)){
                mypwd();
            }
            
            else if(!strncmp(tmp1,"mycd",4)){
                if(!(tmp[4]=='\0'))
                    check++;
                mycd(tmp2,check);
                check=0;
            }
            
            else if(!strncmp(tmp1,"mycp",4) && tmp[4] == '\0')
                printf("mycp\n");
            
            else if(!strncmp(tmp1,"mycpto",6))
                //mycpto(tmp2, tmp3);
                ;
            
            else if(!strncmp(tmp1,"mycpfrom",8)){
                mycpfrom(tmp2, tmp3);
            }
            else if(!strncmp(tmp1,"mymkdir",7)){
                mymkdir(tmp2);
            }
            
            else if(!strncmp(tmp1,"myrmdir",7))
                myrmdir(tmp2);
            
            else if(!strncmp(tmp1,"mymv",4))
                mymv(tmp2,tmp3);
            
            else if(!strncmp(tmp1,"mytouch",7))
                ;
            //mytouch(tmp2);
            else if(!strncmp(tmp1,"myshowinode",11)){
                myshowinode(tmp2);
            }
            
            else if(!strncmp(tmp1,"myshowblock",11)){
                myshowblock(tmp2);
            }
            
            else if(!strncmp(tmp1,"mystate",7)){
                mystate();
            }
            else if(!strncmp(tmp1,"mytree",6))
                mytree(tmp2);
            
            my--;
        }
        
    }
}
void filesave(){
    FILE * ifp;
    ifp = fopen("myfs","wb");
    fseek(ifp,2,SEEK_SET);
    fwrite(sb,sizeof(superblock),1,ifp);
    fwrite(in,sizeof(inode),512,ifp);
    fwrite(db,sizeof(datablock),1024,ifp);
    fclose(ifp);
}
void fileload(){
    FILE * ifp;
    ifp = fopen("myfs","rb");
    fseek(ifp,2,SEEK_SET);
    fread(sb,sizeof(superblock),1,ifp);
    fread(in,sizeof(inode),512,ifp);
    fread(db,sizeof(datablock),1024,ifp);
    fclose(ifp);
}






