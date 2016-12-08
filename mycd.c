#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(int argc, char ** argv){
	char dir[100] = {0};
		

	if(!strcmp(argv[1], "mycd")){
		if(argc == 2){
			strcpy(dir, "");
			printf("[/%s ]$", dir);
		}
		else{
			strcpy(dir, argv[2]);
			printf("[/%s ]$", dir);
		}
	}

	return 0;
}
