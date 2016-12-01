#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

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

typedef struct inode_list{
	inode in;
	struct inode_list * next;
} inode_list;

typedef struct datablock{
	char data[128];
} datablock;

typedef struct datablock_list{
	datablock db;
	struct datablock_list * next;
} datablock_list;

void makeinode(void);
void makeblock(void);
void setinodeinfo(inode_list *, superblock *);
void set_time(inode_list *);
void set_size();
void set_type(inode_list *);
void set_datablock_info();
void set_superblock(unsigned *);
bool find_free_inode(unsigned);
void freeinode(inode_list *, superblock *);
void freeblock(datablock_list *);
void freesuperblock(int, superblock *);
void freebit(unsigned *, int);
void bit_print(unsigned);
int main(){
	makeinode();
	makeblock();
	return 0;
}

void makeinode(void){
	inode_list * i_head, *p, *q;
	i_head = (inode_list *)calloc(1, sizeof(inode_list));
	p = i_head;
	superblock *sb;
	sb = (superblock *)calloc(1, sizeof(superblock));

	setinodeinfo(p, sb);

	for(int i = 0; i < 6; i++){
		printf("%d ", p -> in.file_creation_time[i]);
	}

	freeinode(p, sb);

	for(int i = 2; i <= 512; i++){
		q = (inode_list *)calloc(1, sizeof(inode_list));
		q -> next = NULL;
		p -> next = q;
		p = q;
	}
}

void makeblock(void){
	datablock_list * d_head, *p, *q;
	d_head = (datablock_list *)calloc(1, sizeof(datablock));
	p = d_head;

	for(int i = 2; i <= 1024; i++){
		q = (datablock_list *)calloc(1, sizeof(datablock));
		q -> next = NULL;
		p -> next = q;
		p = q;
	}
}


void setinodeinfo(inode_list *p, superblock *sb){ // 아이노드 정보 입력
	static int i;
	static int j = 1;
	while(1){
		if(find_free_inode(sb -> freeinode[i])){
			set_superblock(&(sb -> freeinode[i]));
			set_type(p);
			set_time(p);
			set_size(p);
			set_datablock_info();
			j++;
			break;
		}
		else{
			++i;
		}
		if(i == 15)
			printf("no free inodes\n");
			break;
	}
}

void set_type(inode_list *p){
	char c;
	bool file_or_dir = true;
	if(file_or_dir == true)
		c = 'd';
	else
		c = '-';

	p -> in.file_type = 'c';
}

void set_time(inode_list *p){
	struct tm *t;
	time_t now;
	now = time(NULL);
	t = localtime(&now);

	p -> in.file_creation_time[0] = t -> tm_year + 1900;
	p -> in.file_creation_time[1] = t -> tm_mon + 1;
	p -> in.file_creation_time[2] = t -> tm_mday;
	p -> in.file_creation_time[3] = t -> tm_hour;
	p -> in.file_creation_time[4] = t -> tm_min;
	p -> in.file_creation_time[5] = t -> tm_sec;
}

void set_size(inode_list *p){
}

void set_datablock_info(inode_list *p){
}

void freeinode(inode_list *p, superblock *sb){ // 아이노드 제거
	/*inode_list * del;
	del = p -> next;
	p -> next = p -> next -> next;
	free(del);*/
	freesuperblock(1, sb);
}

void freedatablock(datablock_list *p){
	datablock_list * del;
	del = p -> next;
	p -> next = p -> next;
	free(del);
}

void freesuperblock(int n, superblock *sb){ // 슈퍼블록에 저장된 정보 제거
	int div = 0, mod = 0;
	div = n / 32;
	mod = n % 32;
	freebit(&(sb -> freeinode[div]), mod);
}

bool find_free_inode(unsigned a){ // 가용 아이노드 검색
	int i;
	int n = sizeof(unsigned) * 8;
	int mask = 1 << (n-1);

	for(i = 1; i <= n; ++i){
		if(a & mask == 0){
			return true;
		}
	}
}

void set_superblock(unsigned *a){ // 슈퍼블록에 사용 정보 저장
	int tmp;
	static int n = 31;
	tmp = 1 << n;
	*a += tmp;
	--n;
	if(n < 0)
		n = 31;
	bit_print(*a);
}

void freebit(unsigned *a, int mod){ // 비트 1에서 0으로 바꿔주는 함수
	int i, tmp;
	int n = sizeof(unsigned) * 8;
	int delete = 32 - mod;
	tmp = 1 << delete;
	*a -= tmp;
	bit_print(*a);
}

void bit_print(unsigned a){
	int i;
	int n = sizeof(unsigned) * 8;
	int mask = 1 << (n-1);
	for(i = 1; i <= n; ++i){
		putchar(((a & mask) == 0) ? '0' : '1');
		a <<= 1;
		if(i % 16 == 0)
			putchar(' ');
	}
}
