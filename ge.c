#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

typedef struct inode{
	char file_type;
	int file_creation_time[6];
	int file_size;
	int datablock_info[3];
} inode;

typedef struct datablock{
	char data[128];
} datablock;

typedef struct superblock{
	unsigned freeinode[16];
	unsigned freedatablock[32];
} superblock;

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
inode * in;
datablock * db;
superblock * sb;

int main(){
	int tmp, blocks = 0, count1 = 0, count2 = 0, tmp2;
	int single[102] = {0};
	int double_ind[102] = {0};
	bool direct = true;
	bool sing = true;
	sb = (superblock *)calloc(1, sizeof(superblock));
	in = (inode *)calloc(512, sizeof(inode));
	db = (datablock *)calloc(1024, sizeof(datablock));

	set_inode_info(in, sb);

	if(in -> file_size > 128)
		blocks = in -> file_size / 128;

	if(blocks > 1){
		for(int i = 0; i < blocks; i++){
			tmp = getblock(db, sb);
			single[i] = tmp;
			if(direct){
				set_datablock_info(&(in -> datablock_info[0]), tmp);
				direct = false;
				}	
			}
		db = db + blocks - 1;
		tmp2 = blocks - 1;
		for(int i = 0; i < blocks; i++){
				if(count1 % 5 == 4){
					count1 + 1;
					i = i - 1;
				}
				get_single(db -> data, count1, single[i]);
				if(sing){
					set_datablock_info(&(in -> datablock_info[1]), tmp2);
					sing = false;
				}
				count1++;
		}
		if(blocks > 103){
				set_datablock_info(&(in -> datablock_info[2]), tmp2 + 1);
				get_double(db -> data, count2, tmp2 + 1);
		}
	}

	for(int i = 0; i < 3; i++)
		printf("%d", in -> datablock_info[i]);
	return 0;
}

void set_inode_info(inode * in, superblock *sb)
{
	static int i;
	int tmp = 0;
	tmp = find_free_inode(sb -> freeinode[i]);
	in = in + tmp;
	while(1){
			if(tmp != 511){
			set_superblock_i(&(sb -> freeinode[i]));
			set_type(in);
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
	*n = tmp;
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
		if(i == 31){
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

void set_type(inode * in)
{
	char c;
	bool file_or_dir = true;
	if(file_or_dir == true)
		c = 'd';
	else
		c = '-';

	in -> file_type = c;
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
	int i, tmp, count = 0;
	int n = sizeof(unsigned) * 8;
	int mask = 1 << (n-1);
	tmp = a << 31;

	for(i = 1; i <= n; ++i){
		if((tmp & mask) == 0){
			return count;
		}
		else{
			tmp >>= 1;
			count++;
		}
	}
}

int find_free_datablock(unsigned a)
{
	int i, tmp, count = 0;
	int n = sizeof(unsigned) * 8;
	int mask = 1 << (n-1);
	tmp = a << 31;

	for(i = 1; i <= n; ++i){
		if((tmp & mask) == 0){
			return count;
		}
		else{
			tmp >>= 1;
			count++;
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
		if(i == 15)
			printf("%d\n", sum);
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
		if(i == 31)
			printf("%d\n", sum);
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
			check = true;
			free++;
			check = false;
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
