#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// Named Piep
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

char *put_words(char *text, char *words);
void write_result_to_file(char decoded_text[]);
void write_in_fifo(char *path, char *content);
void read_from_fifo(char *path, int amount, char output[]);

int main()
{
	// FIFO file path
	char *main_placer_fifo = "/tmp/mpfifo";
	char *finder_placer_fifo = "/tmp/fpfifo";

	// read main process message
	char text[100];
	read_from_fifo(main_placer_fifo, 100, text);
	// printf("(placer) text: %s\n", text);

	// read finder message
	char words[100];
	read_from_fifo(finder_placer_fifo, 100, words);
	// printf("(placer) words: %s", words);

	// put words in text
	char *complete_text = put_words(text, words);
	write_result_to_file(complete_text);
	// printf("\ncomplete text: %s", complete_text);

	// send clear text to main process
	write_in_fifo(main_placer_fifo, complete_text);

	return 0;
}

void write_in_fifo(char *path, char *content)
{
	int key = open(path, O_WRONLY);
	write(key, content, strlen(content) + 1);
	close(key);
}

void read_from_fifo(char *path, int amount, char output[])
{
	int key = open(path, O_RDONLY);
	read(key, output, amount);
	close(key);
}

void write_result_to_file(char complete_text[])
{
	char *filename = "replace_result.txt";
	FILE *fp = fopen(filename, "a");
	if (fp == NULL)
	{
		printf("Error opening the file %s", filename);
		return;
	}

	fprintf(fp, "\n%s", complete_text);
	fclose(fp);
}

char *put_words(char *text, char *words)
{
	int complete_text_length = strlen(text) + strlen(words);
	char *complete_text = (char *)malloc(complete_text_length);
	int j = 0;
	int k = 0;
	for (int i = 0; text[i] != '\0'; i++)
	{
		if (text[i] != '$')
		{
			complete_text[j] = text[i];
			j++;
		}
		else
		{
			while (words[k] != '\n' && words[k] != '\0')
			{
				complete_text[j] = words[k];
				k++;
				j++;
			}
			k++;
		}
	}
	// complete_text[j] = '\0';
	return complete_text;
}
