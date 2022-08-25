#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//Named Piep
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

char *process_parent_message(char message[]);
void write_result_to_file(char plain_text[]);
void write_in_fifo(char *path, char *content);
void read_from_fifo(char *path, int amount, char output[]);

int main()
{
    // FIFO file path
    char *main_decoder_fifo = "/tmp/mdfifo";
    char *finder_decoder_fifo = "/tmp/fdfifo";

    //make fifo
    const int PERMISSION = 0666;
    mkfifo(finder_decoder_fifo, PERMISSION);

    // read main prcess message
    char cipherText[100];
    read_from_fifo(main_decoder_fifo, 100, cipherText);

    // decode the message
    char *plain_text = process_parent_message(cipherText);
    write_result_to_file(plain_text);

    // send decoded text to finder
    write_in_fifo(finder_decoder_fifo, plain_text);
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

void write_result_to_file(char plain_text[])
{
    char *filename = "decode_result.txt";
    FILE *fp = fopen(filename, "a");
    if (fp == NULL)
    {
        printf("Error opening the file %s", filename);
        return;
    }

    fprintf(fp, "\n%s", plain_text);
    fclose(fp);
}

void decode(char *cipherText)
{
    while (*cipherText != '\0')
    {
        if (*cipherText >= 97 && *cipherText <= 122)
        {
            int target_alphabet = *cipherText - 3;
            if (target_alphabet < 97)
            {
                *cipherText = (char)(122 - (97 - target_alphabet - 1));
            }
            else
            {
                *cipherText = (char)target_alphabet;
            }
        }

        if (*cipherText >= 65 && *cipherText <= 90)
        {
            int target_alphabet = *cipherText - 3;
            if (target_alphabet < 65)
            {
                *cipherText = (char)(90 - (65 - target_alphabet - 1));
            }
            else
            {
                *cipherText = (char)target_alphabet;
            }
        }
        cipherText++;
    }
}

char *remove_whitespaces(char str[])
{
    int str_length = strlen(str);
    char *temp = (char *)malloc(str_length);

    int j = 0;
    for (int i = 0; i < str_length; i++)
    {
        while (str[i] == ' ' || str[i] == '\n')
        {
            i++;
        }
        temp[j] = str[i];
        j++;
    }
    return temp;
}

char *process_parent_message(char message[])
{
    char *prepared_message = remove_whitespaces(message);
    decode(prepared_message);
    return prepared_message;
}
