#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

static void usage(void)
{
	printf("You must need enter a file name. \nFor example: `./preprocessing file.p` \nThis program does not have flags.");
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		usage();
		return -1;
	}

	char *_file_name = argv[1];

	struct stat _file;
	if (stat(_file_name, &_file) != 0) {
		printf("%s: file not found \n", _file_name);
		return -1;
	}

	FILE *file = fopen(_file_name, "r");
	if (file == NULL) {
		printf("%s: error to open the file", _file_name);
		return -1;
	}
	
	long size_file = _file.st_size;
	char *buffer = malloc(size_file + 1); /* +1 for '\0' */

	size_t bytes = fread(buffer, 1, size_file, file);
	buffer[bytes] = '\0';

	char *token = strtok(buffer, " \n");
	char *content = malloc(size_file + 1);

	if (content == NULL) {
		free(buffer);
		fclose(file);
		return -1;
	}

	content[0] = '\0';

	while (token != NULL) {
		printf("Token: %s \n", token);
		if (strcmp(token, "#include") == 0) { /* remove the #include token */
			token = strtok(NULL, " \n");
			continue;
		}
		strcat(content, token);
		token = strtok(NULL, " \n");
	}

	/* remove the <.h> chars and add .h file content in _file_name */

		


	printf("%s \n", content);

	printf("File: %s | Bytes: %ld \n", _file_name, size_file);

	free(content);
	fclose(file);
	free(buffer);
	return 0;
}
