#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

//Named Pipe
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

//FIFO file path
#define MAIN_DECODER_FIFO "/tmp/mdfifo"
#define MAIN_FINDER_FIFo "/tmp/mffifo"
#define MAIN_PLACER_FIFO "/tmp/mpfifo"

void write_in_fifo(char *path, char *content);
void read_from_fifo(char *path, int amount, char output[]);
struct input_parts extract_input_parts(char *str);

struct input_parts
{
    char first[100];
    char second[20];
    char third[100];
};

void execute_childs()
{
    int pid1;
    int pid2;
    int pid3;

    pid1 = fork();
    if (pid1 != 0)
    {
        pid2 = fork();
        if (pid2 != 0)
        {
            pid3 = fork();
        }
    }

    if (pid1 == 0)
    {
        char *args[] = {"./decoder", NULL};
        execvp(args[0], args);
    }

    if (pid2 == 0)
    {
        char *args[] = {"./finder", NULL};
        execvp(args[0], args);
    }

    if (pid3 == 0)
    {
        char *args[] = {"./placer", NULL};
        execvp(args[0], args);
    }
}

void make_named_pipes()
{
    const int PERMISSION = 0666;
    mkfifo(MAIN_DECODER_FIFO, PERMISSION);
    mkfifo(MAIN_FINDER_FIFo, PERMISSION);
    mkfifo(MAIN_PLACER_FIFO, PERMISSION);
}

int main()
{
    execute_childs();
    // tests
    // char str[] = "dddvdohk\nadylddd###3 5$8 4$###$ isn't a football player.\n$ is a football player.";
    char str[] = "gggggPDQcadas\nEODFNabcdef abcdeJRQH###5 3$13 5$29 4$###the maderer was a $. he was wearing a $ coat and had a small $.";

    struct input_parts parts = extract_input_parts(str);
    // printf("%s\n",parts.first);
    // printf("%s\n",parts.second);
    // printf("%s\n", parts.third);

    // Creating the named file(FIFO)
    make_named_pipes();

    // decoder
    write_in_fifo(MAIN_DECODER_FIFO, parts.first);

    // finder
    write_in_fifo(MAIN_FINDER_FIFo, parts.second);

    // placer
    write_in_fifo(MAIN_PLACER_FIFO, parts.third);

    // recieve decoded text from placer process
    char decodec_text[100];
    read_from_fifo(MAIN_PLACER_FIFO, 100, decodec_text);

    // result
    printf("(main) decoded text:\n%s\n", decodec_text);

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

struct input_parts extract_input_parts(char *str)
{
    struct input_parts result;

    char *target = "###";
    char *first_part_end;

    //end of first part
    first_part_end = strstr(str, target);
    int first_part_length = (int)(first_part_end - str);

    result.first[first_part_length] = '\0';
    for (int i = 0; i < first_part_length; i++)
    {
        result.first[i] = str[i];
    }
    char *second_part_begining = first_part_end + 3;
    char *second_part_end = strstr(second_part_begining, target);
    int second_part_length = (int)(second_part_end - second_part_begining);

    result.second[second_part_length] = '\0';
    for (int j = 0; j < second_part_length; j++)
    {
        result.second[j] = second_part_begining[j];
    }

    char *third_part_beginig = second_part_end + 3;
    int k;
    for (k = 0; *third_part_beginig != '\0'; k++)
    {
        result.third[k] = *third_part_beginig;
        third_part_beginig++;
    }
    result.third[k] = '\0';
    return result;
}