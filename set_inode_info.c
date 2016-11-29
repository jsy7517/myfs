#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

typedef struct superblock{
	unsigned inode[16];
	unsigned datablock[32];
}superblock;

typedef struct inode{
	char file_type;
	int file_creation_time[6];
	int file_size;
	int data_block_info[3];
} inode;

typedef struct inode_list{
	inode in;
	struct inode_list * next;
} inode_list;

typedef struct data_block{
	char data[128];
} data_block;

typedef struct data_block_list{
	data_block db;
	struct data_block_list * next;
} data_block_list;

void set_inode_info(inode_list *, superblock *);
bool file_or_dir(void);
void myshowinode(inode_list *);
void write_datablock(void);
void free_inode(inode_list *);
void get_inode(inode_list *, superblock *);
void set_superblock(unsigned *);
void bit_print(unsigned);

int main(){
	superblock * sb;
	sb = (superblock *)calloc(1, sizeof(superblock));
	FILE * fs;
	bool checkfs = false;
	char input[100] = {0};

	while(1){
		scanf("%s", input);
		if(checkfs){
			printf("error\n");
			break;
		}
		if(!strcmp(input, "mymkfs")){
			fs = fopen("myfs", "w+");
			checkfs = 1;
			continue;
		}
	}
		

	inode_list *root;
	root = (inode_list *)malloc(sizeof(inode_list));
	set_inode_info(root, sb);
	myshowinode(root);
	printf("\n");
	get_inode(root, sb);
	myshowinode(root -> next);

	fclose(fs);
	return 0;
}

void set_inode_info(inode_list *p, superblock * sb){
	struct tm *t;
	static int i;
	int count = 1;
	char c;

	if(file_or_dir)
		c = 'd';
	else
		c = '-';

	time_t now;
	now = time(NULL);
	t = localtime(&now);

	p -> in.file_type = c;
	p -> in.file_creation_time[0] = t -> tm_year + 1900;
	p -> in.file_creation_time[1] = t -> tm_mon + 1;
	p -> in.file_creation_time[2] = t -> tm_mday;
	p -> in.file_creation_time[3] = t -> tm_hour;
	p -> in.file_creation_time[4] = t -> tm_min;
	p -> in.file_creation_time[5] = t -> tm_sec;
	p -> in.file_size = 0;
	p -> next = NULL;
	set_superblock(&(sb -> inode[i]));

	/*if(getblock)
		p -> in.file_info[0] = 1;
	else if(getsingle)
		p -> in.file_info[1] = getsingle;
	else
		p -> in.file_info[2] = getdouble;*/
	count++;
	if(count == 16)
		++i;
	return;
}

bool file_or_dir(void){
	return true;
}

void myshowinode(inode_list *p){
	printf("%c ", p -> in.file_type);

	printf("%d/%d/%d %d:%d:%d ", p -> in.file_creation_time[0], p -> in.file_creation_time[1], p -> in.file_creation_time[2], p -> in.file_creation_time[3], p -> in.file_creation_time[4], p -> in.file_creation_time[5]);
	printf("%d", p -> in.file_size);
}

void free_inode(inode_list *p){
	inode_list * del;
	del = p -> next;
	p -> next = p -> next -> next;
	free(del);
}

void get_inode(inode_list * p, superblock * sb){
	inode_list * new;
	new = (inode_list *)malloc(sizeof(inode_list));
	new -> next = NULL;
	p -> next = new;
	set_inode_info(new, sb);
}

void set_superblock(unsigned * a){

	int tmp;
	static int i = 0;
	static int n = 31;
	tmp = 1 << n;
	*a += tmp;
	bit_print(*a);
	printf("\n");
	--n;
	++i;
}

void bit_print(unsigned a)
{
	int i;
	int n = sizeof(int) * 8;
	int mask = 1 << (n-1);

	for(i = 1; i <= n; ++i){
		putchar(((a & mask) == 0) ? '0' : '1');
		a <<= 1;
		if(i % 8 == 0 && i < n)
			putchar(' ');
	}
}

		
