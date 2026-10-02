#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

static void
usage(void)
{
	printf("You must need enter a file name. \nFor example: `./preprocessing file.p` \nThis program does not have flags.");
}

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		usage();
		return -1;
	}

	char _file_name_out[30];

	snprintf(_file_name_out, sizeof(_file_name_out), "%s.tmp", argv[1]);

	char *_file_name = argv[1];

	struct stat _file;
	if (stat(_file_name, &_file) != 0) {
		printf("%s: file not found \n", _file_name);
		return -1;
	}

	FILE *file_temp = fopen(_file_name, "r");
	FILE *file_out = fopen(_file_name_out, "w");

	if (file_temp == NULL || file_out == NULL) {
		printf("%s: error to open the file", _file_name);
		return -1;
	}

	long size_file = _file.st_size;
	char *buffer = malloc(size_file + 1); /* +1 for '\0' */

	size_t bytes = fread(buffer, 1, size_file, file_temp);
	buffer[bytes] = '\0';
	rewind(file_temp);

	char *token = strtok(buffer, " \n");
	char *content = malloc(size_file + 1);

	if (content == NULL) {
		free(buffer);
		fclose(file_temp);
		fclose(file_out);
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

	DIR *dir = opendir("include/");
	if (dir == NULL) {
		perror("Error to open include dict \n");
		return -1;
	}

	struct dirent *dir_content;

	char line[1024];
	
	rewind(file_temp);

	while (fgets(line, sizeof(line), file_temp)) {
		int found = 0;
		while ((dir_content = readdir(dir)) != NULL) { /* read all dict content */
			if (strcmp(dir_content->d_name, ".") == 0 || strcmp(dir_content->d_name, "..") == 0) {
				continue;
			}
			/* search for the line who contains the 'dir_content->d_name' and replace it */
			if (strstr(line, dir_content->d_name) != NULL) {
				char path[30];
				snprintf(path, sizeof(path), "include/%s", dir_content->d_name);
				FILE *header = fopen(path, "r");
				if (header == NULL) {
					fprintf(stderr, "Error to open: %s file \n", dir_content->d_name);
					return -1;
				}
				fseek(header, 0, SEEK_END);
				long file_size = ftell(header);
				rewind(header);
				char *_buffer = malloc(file_size + 1); /* for the NULL char */
				size_t _bytes = fread(_buffer, 1, file_size, header);
				_buffer[_bytes] = '\0';
				fputs(_buffer, file_out);
				fclose(header);
				free(_buffer);
				found = 1;
				break;
			}
		}
		rewinddir(dir);
		
		if (!found) {
			fputs(line, file_out);
		}
	}

	closedir(dir);
	remove(_file_name);
	rename(_file_name_out, _file_name);

	free(content);
	fclose(file_temp);
	fclose(file_out);
	free(buffer);
	return 0;
}
