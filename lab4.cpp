#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef char* charArr[];

#define MAX_LEN (1000)

bool is_empty_char(char c) {
    return c < '\x0e' || c == ' ';
}

int find_cmd_length(const char *cmd) {
    const char *p = cmd;
    while(!is_empty_char(*p)) p++;
    return (int)(p-cmd);
}

bool splitParams(char** dest, char *str, char delimiter = ' ') {
    char quote_char = '\0';
    int lastIdx = 0, idx = 0;
    char *p = str;
    while(true) {
        if(!(*p)) {
            dest[idx++] = &str[lastIdx];
            dest[idx++] = NULL;
            break;
        }
        if(!quote_char) {
            if(*p == delimiter) {
                dest[idx++] = &str[lastIdx];
                lastIdx = p-str+1;
                *p = '\0';
            } else {
                if(*p == '\'' || *p == '"') quote_char = *p;
            }
        } else {
            if(*p == quote_char) quote_char = '\0';
        }
        p++;
    }
    if(quote_char) {
        return false;
    }
    return true;
}

char myGetChar(char *dest, int *len) {
    char tmp = getchar();
    dest[(*len)++] = tmp;
    return tmp;
}

struct HistoryCmdNode {
    HistoryCmdNode *next;
    char cmd[MAX_LEN+1];
    int pid;
};

int main(void) {

    char cmd[MAX_LEN+1], tmp; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    int cmd_len = 0, len = 0, retCode = 0, i;
    bool background_flag = false;
    char *paramsArr[MAX_LEN];

    HistoryCmdNode history_head, *history_p;
    history_head.next = NULL;

    while(should_run) {
        printf("osh %s$ ", get_current_dir_name());
        fflush(stdout);

        background_flag = false;

        len = 0;
        while(myGetChar(cmd, &len) != '\n');
        cmd[len] = '\0';

        while(len && is_empty_char(cmd[len-1])) len--;
        if(len == 0) continue;
        cmd[len] = '\0';

        history_p = &history_head;
        while(history_p->next) {
            history_p = history_p->next;
        }
        strcpy(history_p->cmd, cmd);
        history_p->next = (HistoryCmdNode*)malloc(sizeof(HistoryCmdNode));
        history_p->next->next = NULL;
        history_p->next->pid = 0;

        if(cmd[len-1] == '&') {
            background_flag = true;
            cmd[--len] = '\0';
        }

        if(!splitParams(paramsArr, cmd)) {
            printf("syntax error. Missing ' or \".\n");
            continue;
        }

        if(strcmp(cmd, "exit") == 0) {
            return 0;
        }

        if(strcmp(cmd, "cd") == 0) {
            if(chdir(paramsArr[1]))
                printf("cd %s: path does not exist\n", paramsArr[1]);
            continue;
        }

        if(strcmp(cmd, "history") == 0) {
            i = 0;
            history_p = &history_head;
            while(history_p->next) {
                printf("%d: %s\n", ++i, history_p->cmd);
                history_p = history_p->next;
            }
            continue;
        }

        pid_t pid = fork();

        if(pid == -1) {
            printf("Can not fork.\n");
            continue;
        }

        if(pid == 0) {
            // child proces
            retCode = execvp(cmd, paramsArr);
            return retCode;
        } else {
            // parent
            if(!background_flag) wait(NULL);
            else history_p->pid = pid;
        }

    }

    history_p = &history_head;
    while(history_p->next) {
        if(history_p->pid) waitpid(history_p->pid, NULL, 0);
        history_p = history_p->next;
    }

    return 0;
}
