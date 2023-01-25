// Z dedykacją dla Halszki, mojej przyciółki, która wierzy że tego nie zawale, chociaż może być różnie

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>

#define TAB_SIZE 1000
#define LEN 3

char last_dir[TAB_SIZE];
char input[TAB_SIZE], device[TAB_SIZE], user[TAB_SIZE], curr_path[TAB_SIZE];
char ext[] = "exit", help[] = "help", cd[] = "cd";
char * polecenia[LEN] = {cd, ext, help};
char * in_sep[10];
int res;

int changed(char *zmien_dir);
void pomoc();
char *trim(char * string);
void clean(char * string, char *usun);
int exe();

int main() {
    int uid = getlogin_r(user, sizeof(user));
    int host = gethostname(device, sizeof(device));
    while(!0){
        int wyn;
        int liczydlo=0;
        char * path = getcwd(curr_path, TAB_SIZE);
        printf("%s@%s:[%s]$ ", user, device, curr_path); fgets(input, TAB_SIZE, stdin);
        input[strcspn(input, "\n")] = 0;
        strcpy(input,trim(input));
        char * token = strtok(input, " ");
        while(token!=NULL && liczydlo<10){
            clean(token, " ");
            in_sep[liczydlo] = token; liczydlo++;
            token = strtok(NULL, " ");
        }
        if(strcmp(input, ext) == 0){
            return 0;
        } else if(strcmp(input, help) == 0){
            pomoc();
        } else if(strcmp(input, cd) == 0){
            wyn = changed(in_sep[1]);
            if(wyn){
                perror("cd");
            }
        }
        else {
            int cpid = fork();
            if(cpid==-1){
                perror("fork()");
            }
            if(cpid == 0){
                res = exe();
                exit(EXIT_SUCCESS);
            } else{
                wait(NULL);
            }
            if(res == -1){
                perror("execvp()");
            }
        }
        for(int i=0;i<10;i++){
            in_sep[i] = NULL;
        }
    }
}

int changed(char *zmien_dir) {
    char pop[] = "-", dom[] = "~";
    if (!strcmp(zmien_dir, pop)) {
        zmien_dir = last_dir;
    } else if (!strcmp(zmien_dir, dom)) {
        char * homedir = getenv("HOME");
        zmien_dir = homedir;
    }
    int zmiana = chdir(zmien_dir);
    if (strcmp(zmien_dir, curr_path)!=0) {
        strcpy(last_dir, curr_path);
    }
    return zmiana;
}

 void pomoc(){
    printf("Autor: Jakub Markil\n");
    printf("Ten microshell stara sie udawac basha. Zawiera recznie zaimplementowane polecenia:\n");
    for (int i=0;i<LEN;i++){
        printf(" - %s\n", polecenia[i]);
    }
}

char *trim(char *string){
    size_t dlugo= strlen(string);
    char *start = string, *koniec = NULL;
    koniec = string + dlugo;
    while(isspace((unsigned char)*start)) start++;
    if(start!=koniec){
        while( isspace((unsigned char) *(--koniec)) && koniec != start );
    }
    if( start != string && koniec == start ){
        *string = '\0';
    } else if( string + dlugo - 1 != koniec ){
        *(koniec + 1) = '\0';
    }
    koniec = string;
    if( start != string ){
        while( *start ){
            *koniec++ = *start++;
        }
        *koniec = '\0';
    }
    return string;
}

void clean(char * string, char *usun){
    int podpisz=0, nadpisz=0;
    while(string[nadpisz]){
        if(string[nadpisz]!=usun[0]){
            string[podpisz++] = string[nadpisz];
        }
        nadpisz++;
    }
    string[podpisz] = 0;
}

int exe(){
    char *table[10];
    for(int i=1;i<=sizeof(in_sep);i++){
        if(i!=sizeof(in_sep)){
            table[i-1]= in_sep[i];
        } else {table[i]=NULL;}
    }
    int wynik = execvp(input, table);
    return wynik;
}