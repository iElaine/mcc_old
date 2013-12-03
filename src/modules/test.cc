#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct fileinfo {
	char name[256];
	struct fileinfo* next;
};

struct fileinfo* getfiles(const char *path)
{
	struct fileinfo *head = NULL; 
	struct fileinfo *temp= NULL; 
	DIR* pDir = NULL;
	struct dirent *ent = NULL;
	pDir = opendir(path);
	if(NULL == pDir) {
		perror("opendir");
		return NULL;
	}
	while(NULL != (ent = readdir(pDir))) {
		if(ent->d_type != DT_DIR)
		{
			temp = (struct fileinfo*)malloc(sizeof(struct fileinfo));
			if(temp == NULL)
				goto error_return;
			temp->next = head;
			strcpy(temp->name, ent->d_name);
			head = temp;
		}
	}
	closedir(pDir);
	pDir = NULL;
	return head;
error_return:
	while(head)
	{
		temp = head;
		head = head->next;
		free(temp);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	DIR* pDir = NULL;
	struct dirent *ent = NULL;
	pDir = opendir(".");
	if(NULL == pDir) {
		perror("opendir");
		exit(0);
	}
	while(NULL != (ent = readdir(pDir))) {
		printf("%s ", ent->d_name);
	}
	closedir(pDir);
	pDir = NULL;
	struct fileinfo* fi = getfiles(".");
	struct fileinfo* tmp = fi;
	while(tmp) {
		printf("%s\n", tmp->name);
		tmp = tmp->next;
	}
	return 0;
}
