#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// Named Piepe
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

struct word_data
{
	int index;
	int length;
};

void write_in_fifo(char *path, char *content);
void read_from_fifo(char *path, int amount, char output[]);
char *find_words(char *text, char *words_data_str);
void write_result_to_file(char words[]);

int main()
{
	// FIFO file path
	char *main_finder_fifo = "/tmp/mffifo";
	char *decoder_finder_fifo = "/tmp/fdfifo";
	char *placer_finder_fifo = "/tmp/fpfifo";

	const int PERMISSION = 0666;
	mkfifo(placer_finder_fifo, PERMISSION);

	// read main process message
	char words_data_str[80];
	read_from_fifo(main_finder_fifo, 80, words_data_str);

	// read decoder process message
	char text[80];
	read_from_fifo(decoder_finder_fifo, 80, text);

	char *words = find_words(text, words_data_str);
	write_result_to_file(words);
	// printf("\n(finder) words:\n%s", words);

	write_in_fifo(placer_finder_fifo, words);

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

void write_result_to_file(char words[])
{
	char *filename = "find_result.txt";
	FILE *fp = fopen(filename, "a");
	if (fp == NULL)
	{
		printf("Error opening the file %s", filename);
		return;
	}

	fprintf(fp, "\n%s", words);
	fclose(fp);
}

int calculate_words_chars_count(struct word_data wd[], int word_count)
{
	int counter = word_count;
	for (int i = 0; i < word_count; i++)
	{
		counter += wd[i].length;
	}
	return counter;
}

int get_word_count(char *str)
{
	int counter = 0;
	while (*str != '\0')
	{
		if (*str == '$')
		{
			counter++;
		}
		str++;
	}
	return counter;
}

int cast_str_to_int(char *str)
{
	char *ptr;
	int val = strtol(str, &ptr, 10);
	return val;
}

void substring(char *str, int i1, int i2, char *sub)
{
	int j = 0;
	for (int i = i1; i <= i2; i++)
	{
		sub[j] = str[i];
		j++;
	}
}

int index_of(char *str, char c)
{
	char *e;
	int index;
	e = strchr(str, c);
	return (int)(e - str);
}

void extract_word_data(char *str, int words_count, struct word_data wd[])
{
	for (int i = 0; *str != '\0'; i++)
	{
		int space_index = index_of(str, ' ');
		int dollar_index = index_of(str, '$');

		char *word_index_str = (char *)malloc(space_index);
		substring(str, 0, space_index - 1, word_index_str);

		char *word_length_str = (char *)malloc(dollar_index - space_index - 1);
		substring(str, space_index + 1, dollar_index - 1, word_length_str);

		wd[i].index = cast_str_to_int(word_index_str);
		wd[i].length = cast_str_to_int(word_length_str);
		str += dollar_index + 1;
	}
}

char *find_words(char *text, char *words_data_str)
{
	int word_count = get_word_count(words_data_str);
	struct word_data wd[word_count];
	extract_word_data(words_data_str, word_count, wd);
	int words_chars_count = calculate_words_chars_count(wd, word_count);

	char *words = (char *)malloc(words_chars_count);
	int k = 0;
	for (int i = 0; i < word_count; i++)
	{
		int end_word_index = wd[i].index + wd[i].length - 1;
		for (int j = wd[i].index; j <= end_word_index; j++)
		{
			words[k] = text[j];
			k++;
		}
		words[k] = '\n';
		k++;
	}
	words[k] = '\0';
	return words;
}