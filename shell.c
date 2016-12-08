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

typedef struct name{
    char name[5];
} name;

void mymkdir(char *tmp2);
void show_inode_info(inode in);
void myshowinode(char *tmp2);
void mycd(char *, char *);
void set_inode_info_d(inode * in, superblock *sb);
void myshowblock(char *tmp2);
void getroot(void);
void get_directory(datablock * db, superblock *sb, char *);
void myshowfile(char *tmp2, char *tmp3, char *tmp4);
void mystate(void);
void myshowfile(char *tmp2, char *tmp3, char *tmp4);
void classify(char *,char *,char *,char *,char *);
void set_inode_info(inode *, superblock *);
int getblock(datablock *, superblock *);
int getdata(datablock *);
void set_type(inode *);
void set_time(inode *);
void set_superblock_i(unsigned *a); // 슈퍼블록에 사용 정보 저장
void set_superblock_d(unsigned *a);
void freebit(unsigned *a, int mod); // 비트 1에서 0으로 바꿔주는 함수
int free_print(unsigned a);
void freesuperblock(int n, superblock *sb);
int find_free_inode(unsigned a);
int find_free_datablock(unsigned a);
void set_size(inode *);
void set_datablock_info(int *, int);
void get_single(char *, int, int);
int bit_print(char *, int n);
int num_bit(char *, int n);
void freeinode(inode *in, superblock * sb);
void get_double(char *, int, int);

inode in[512] = {0};
datablock db[1024] = {0};
superblock sb[1] = {0};
name * nm;

int main(void){
	char tmp[25]={0},tmp1[25]={0},tmp2[25]={0},tmp3[25]={0},tmp4[25]={0};
    char dir[4] = {'/'};
	int len, my=0;
	bool check_fs = false;
    
	while(1){
		printf("[%s ]$ : ",dir);
		scanf("%[^\n]",tmp);
		getchar();
		classify(tmp,tmp1,tmp2,tmp3,tmp4);
		if(!strncmp(tmp,"my",2))
			my++;
		if(!strcmp(tmp,"byebye"))
			exit(1);
		if(!my)
			system(tmp);
		if(my){
			if(!strncmp(tmp1, "mymkfs", 6)){
				if(check_fs){
					printf("error : myfs exists\n");
					continue;
				}
				fopen("myfs", "rb+");
				getroot();
				check_fs = 1;
			}
			if(!strncmp(tmp1,"myls",4)){
				if(tmp[4]=='\0')
					printf("myls\n");
                
                else if(!strncmp(tmp2,"-i",2)){
                    if(!strncmp(tmp3,"-l",2))
                    printf("myls -l\n");
					else
                    printf("myls -i\n");
                }
                
                else if(!strncmp(tmp2,"-l",2)){
                    if(!strncmp(tmp3,"-i",2))
					printf("myls -i\n");
                    else
                    printf("myls -l\n");
                }
				else if(!strncmp(tmp2, "-li", 3)){
					printf("myls -li\n");
				}
				else if(!strncmp(tmp2, "-il", 3)){
					printf("myls -il\n");
				}
                
			}

			else if(!strncmp(tmp1,"mycat",5))
				printf("mycat\n");

			else if(!strncmp(tmp1,"myshowfile",10)){
				myshowfile(tmp2, tmp3, tmp4);
			}

			else if(!strncmp(tmp1,"mypwd",5)){
				putchar('\n');
			}

			else if(!strncmp(tmp1,"mycd",4)){
				printf("mycd\n");
			}

			else if(!strncmp(tmp1,"mycp",4))
				printf("mycp\n");

			else if(!strncmp(tmp1,"mycpto",6))
				printf("mycpto\n");

			else if(!strncmp(tmp1,"mycpfrom",8))
				printf("mycpfrom\n");

            else if(!strncmp(tmp1,"mymkdir",7)){
                
				mymkdir(tmp2);
            }

			else if(!strncmp(tmp1,"myrmdir",7))
				printf("myrmdir\n");

			else if(!strncmp(tmp1,"mymv",4))
				printf("mymv\n");

			else if(!strncmp(tmp1,"mytouch",7))
				printf("mytouch\n");

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
				printf("mytree\n");

			my--;
		}

	}    
	return 0;
}


void getroot(void){
		set_inode_info_d(in, sb);
		get_directory(db, sb, "/");
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



void get_directory(datablock * db, superblock *sb, char * tmp2)
{
    static int i;
    int tmp = 0, count = 0, max = 128;
    tmp = find_free_datablock(sb -> freedatablock[i]);
    db = db + tmp;
	int len;
	len = strlen(tmp2);
    while(1){
        if(tmp != 1023){
            set_superblock_d(&(sb -> freedatablock[i]));
			set_datablock_info(&(in -> datablock_info[0]), tmp);
			strncpy(db -> data, tmp2, len);
			break;
        }
        else
            ++i;
        if(i == 32){
            printf("no free datablocks\n");
            break;
        }
    }
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
	set_inode_info_d(in, sb);
	get_directory(db, sb, tmp2);
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


void classify(char * tmp,char *tmp1,char *tmp2,char *tmp3,char *tmp4){
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

void set_inode_info_d(inode * in, superblock *sb)
{
    static int i;
    int tmp = 0;
    tmp = find_free_inode(sb -> freeinode[i]);
    in = in + tmp;
    while(1){
        if(tmp != 512){
            set_superblock_i(&(sb -> freeinode[i]));
			set_datablock_info(&(in -> datablock_info[0]), tmp);
            in -> file_type = 'd';
            set_time(in);
            in -> file_size = 1;
            break;
        }
        else
            ++i;
        if(i == 16){
            printf("no free inodes\n");
            i = 0;
            break;
        }
    }
}
void set_inode_info(inode * in, superblock *sb)
{
    static int i;
    int tmp = 0;
    tmp = find_free_inode(sb -> freeinode[i]);
    in = in + tmp;
    while(1){
        if(tmp != 512){
            set_superblock_i(&(sb -> freeinode[i]));
            in -> file_type = '-';
            set_time(in);
            set_size(in);
            break;
        }
        else
            ++i;
        if(i == 16){
            printf("no free inodes\n");
            i = 0;
            break;
        }
    }
}

void set_size(inode *in)
{
    FILE * ifp;
    int size;
    ifp = fopen("appl", "r");
    fseek(ifp, 0, SEEK_END);
    size = ftell(ifp);
    in -> file_size = size;
    return;
}

void set_datablock_info(int * n, int tmp)
{
    *n = tmp + 1;
    return;
}

void freeinode(inode *in, superblock * sb)
{
    return;
}
int getblock(datablock * db, superblock * sb)
{
    static int i;
    int tmp = 0, count = 0, max = 128;
    tmp = find_free_datablock(sb -> freedatablock[i]);
    db = db + tmp;
    while(1){
        if(tmp != 1023){
            getdata(db);
            set_superblock_d(&(sb -> freedatablock[i]));
            return tmp;
        }
        else
            ++i;
        if(i == 32){
            printf("no free datablocks\n");
            break;
        }
    }
}

int getdata(datablock *db)
{
    FILE * ifp;
    int c;
    ifp = fopen("appl", "r");
    rewind(ifp);
    for(int i = 0; i < 128; i++){
        if((c = getc(ifp)) == EOF)
            return 1;
        db -> data[i] = c;
    }
}


void set_time(inode * in){
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
    int i, count = 0;
    int n = sizeof(unsigned) * 8;
    int mask = 1 << (n-1);
    
    for(i = 1; i <= n; ++i){
        if((a & mask) == 0){
            return count;
        }
        else{
            a >>= 1;
            count++;
        }
    }
}

int find_free_datablock(unsigned a)
{
    int i, count = 0;
    int n = sizeof(unsigned) * 8;
    int mask = 1 << (n-1);
    
    for(i = 1; i <= n; ++i){
        if((a & mask) == 0){
            return count;
        }
        else{
            a >>= 1;
            ++count;
        }
    }
}

void set_superblock_i(unsigned *a){ // 슈퍼블록에 사용 정보 저장
    int tmp, sum = 0;
    static int n = 31;
    tmp = 1 << n;
    *a += tmp;
    --n;
    if(n < 0)
        n = 31;
    for(int i = 0; i < 16; i++){
        sum += free_print(*a);
        a = a + 1;
    }
}

void set_superblock_d(unsigned *a){ // 슈퍼블록에 사용 정보 저장
    int tmp, sum = 0;
    static int n = 31;
    tmp = 1 << n;
    *a += tmp;
    --n;
    if(n < 0)
        n = 31;
    for(int i = 0; i < 32; i++){
        sum += free_print(*a);
        a = a + 1;
    }
}

void freebit(unsigned *a, int mod){ // 비트 1에서 0으로 바꿔주는 함수
    int i, tmp;
    int n = sizeof(unsigned) * 8;
    int del = 32 - mod;
    tmp = 1 << del;
    *a -= tmp;
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

