#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdbool.h>

void read_records_from_database(char *, char *, int *);

char *commentsDetector(char *, char *, char *);

char *subString(char *, int, int);

char *replaceWord(const char *, const char *, const char *);

int detectComment(char *, char *);

char *commentsDetector(char *, char *, char *);

bool isDelimiter(char);

bool isKeyword(char *);

int findEndOfTheWord(char [], int);

void startAnalyzer(char [], int);

bool isInteger(char *);

bool isOperator(char);

bool isConnectedOperator(char);

bool isValidIdentifier(char *);

int findEndOfTheString(char *, int, int);

void toUpperCase(char *);

int main(int argc, char *argv[]) {

    char str[400];
    int iSize = -1;

    char sourceFileName[] = "code.ceng";

    read_records_from_database(sourceFileName, str, &iSize);

    char *source = commentsDetector(str, "(*", "*)");

    startAnalyzer(source, iSize);

    return 0;
}

void startAnalyzer(char source[], int iSize) {
    FILE *outputFile = fopen("code.lex","w");
    for (int i = 0; i < iSize && source[i] != '\0'; ++i) {
        if (source[i] == '"' || source[i] == '\'') {
            int getEndOfString = findEndOfTheString(source, i, source[i]);
            char *detectedWord = subString(source, i, getEndOfString);
            i = getEndOfString ;
            printf("ConstString(%s)\n",detectedWord);
            fprintf(outputFile,"ConstString(%s)\n",detectedWord);
        }else if(source[i] == ';'){
            printf("EndOfLine\n");
            fprintf(outputFile,"EndOfLine\n");
        }else if (source[i] == '[') {
            printf("LeftSquareBracket\n");
            fprintf(outputFile,"LeftSquareBracket\n");
        }
         else if (source[i] == ']') {
             printf("RightSquareBracket\n");
             fprintf(outputFile, "RightSquareBracket\n");
         }
         else if (source[i] == '{') {
             printf("LeftCurlyBracket\n");
             fprintf(outputFile, "LeftCurlyBracket\n");
         }
         else if (source[i] == '}') {
             printf("RightCurlyBracket\n");
             fprintf(outputFile, "RightCurlyBracket\n");
         }
         else if (source[i] == '(') {
             printf("LeftPar\n");
             fprintf(outputFile, "LeftPar\n");
         }
         else if (source[i] == ')') {
             printf("RightPar\n");
             fprintf(outputFile, "RightPar\n");
         }
         else if (isalnum(source[i])) {
            int getEndOfWord = findEndOfTheWord(source, i);
            char *detectedWord = subString(source, i, getEndOfWord);
            i = getEndOfWord;
            if (isKeyword(detectedWord)) {
                printf("Keywords(%s)\n", detectedWord);
                fprintf(outputFile, "Keywords(%s)\n", detectedWord);
            } else if (isValidIdentifier(detectedWord)) {
                printf("Identifier(%s)\n", detectedWord);
                fprintf(outputFile, "Identifier(%s)\n", detectedWord);
            } else if (isInteger(detectedWord)){
                printf("ConstInt(%s)\n", detectedWord);
                fprintf(outputFile, "ConstInt(%s)\n", detectedWord);
            }
        } else if (isOperator(source[i])) {
            if (isConnectedOperator(source[i]) && (source[i + 1] == '+' || source[i + 1] == '-' || source[i + 1] == '=')) {
                i++;
                printf("Operator(%c%c)\n", source[i - 1], source[i]);
                fprintf(outputFile, "Operator(%c%c)\n", source[i - 1], source[i]);
            } else {
                printf("Operator(%c)\n", source[i]);
                fprintf(outputFile, "Operator(%c)\n", source[i]);
            }
        }

    }
}

void read_records_from_database(char *argFileName, char *str, int *iSize) {
    FILE *fileReader;
    if ((fileReader = fopen(argFileName, "rb+")) == NULL) {
        //Error
        printf("File is not opened or founded ...");
        exit(0);
    } else {
        rewind(fileReader);
        int i = 0;
        // I read all line in dat file.
        while (!feof(fileReader)) {
            fread(str, sizeof(char), 1, fileReader);
            str++;
            i++;
        }
        *iSize = i;
    }
    fclose(fileReader);
}

char *subString(char *str, int left, int right) {
    int i;
    char *subStr = (char *) malloc(sizeof(char) * (right - left + 2));

    for (i = left; i <= right; i++)
        subStr[i - left] = str[i];
    subStr[right - left + 1] = '\0';
    return (subStr);
}

char *replaceWord(const char *s, const char *oldW, const char *newW) {
    char *result;
    int i, cnt = 0;
    int newWlen = strlen(newW);
    int oldWlen = strlen(oldW);

    // Counting the number of times old word
    // occur in the string
    for (i = 0; s[i] != '\0'; i++) {
        if (strstr(&s[i], oldW) == &s[i]) {
            cnt++;

            // Jumping to index after the old word.
            i += oldWlen - 1;
        }
    }

    // Making new string of enough length
    result = (char *) malloc(i + cnt * (newWlen - oldWlen) + 1);

    i = 0;
    while (*s) {
        // compare the substring with the result
        if (strstr(s, oldW) == s) {
            strcpy(&result[i], newW);
            i += newWlen;
            s += oldWlen;
        } else
            result[i++] = *s++;
    }

    result[i] = '\0';
    return result;
}

int detectComment(char *source, char *foundWord) {
    char *dest = strstr(source, foundWord);
    int pos;
    pos = dest - source;
    return (dest == 0) ? -1 : pos;
}

char *commentsDetector(char *source, char *commentStarter, char *commentFinisher) {
    //printf("%d",detectComment(source,"(*"));
    while (detectComment(source, "(*") > 0) {
        int getCommentStartPosition = detectComment(source, commentStarter);
        int getCommentFinishPosition = detectComment(source, commentFinisher);
        getCommentFinishPosition += strlen(commentFinisher) - 1;
        char *allComment = subString(source, getCommentStartPosition, getCommentFinishPosition);
        source = replaceWord(source, allComment, "");
    }
    return source;
}

int findEndOfTheWord(char *source, int startPosition) {
    while (!isDelimiter(source[startPosition])) {
        startPosition++;
    }
    return (startPosition - 1);
}

int findEndOfTheString(char *source, int startPosition, int finisherCharacter) {
    do {
        startPosition++;
    } while (source[startPosition] != finisherCharacter);
    return (startPosition);
}

bool isDelimiter(char ch) {
    if (ch == ' ' || ch == '+' || ch == '-' || ch == '*' ||
        ch == '/' || ch == ',' || ch == ';' || ch == '>' ||
        ch == '<' || ch == '=' || ch == '(' || ch == ')' ||
        ch == '[' || ch == ']' || ch == '{' || ch == '}' ||
        ch == ':')
        return (true);
    return (false);
}

bool isKeyword(char *str) {
    if (!strcmp(str, "break") || !strcmp(str, "case") || !strcmp(str, "char") || !strcmp(str, "const") ||
        !strcmp(str, "continue") || !strcmp(str, "do") || !strcmp(str, "else") ||
        !strcmp(str, "enum") || !strcmp(str, "float") || !strcmp(str, "for") || !strcmp(str, "goto") ||
        !strcmp(str, "if") || !strcmp(str, "int") || !strcmp(str, "long") || !strcmp(str, "record") ||
        !strcmp(str, "return") ||
        !strcmp(str, "static") || !strcmp(str, "while"))
        return (true);
    return (false);
}

bool isInteger(char *str) {
    int i, len = strlen(str);

    if (len == 0)
        return (false);
    for (i = 0; i < len; i++) {
        if (str[i] != '0' && str[i] != '1' && str[i] != '2'
            && str[i] != '3' && str[i] != '4' && str[i] != '5'
            && str[i] != '6' && str[i] != '7' && str[i] != '8'
            && str[i] != '9' || (str[i] == '-' && i > 0))
            return (false);
    }
    return (true);
}

bool isOperator(char ch) {
    if (ch == '+' || ch == '-' || ch == '*' ||
        ch == '/' || ch == '>' || ch == '<' ||
        ch == '=' || ch == ':')
        return (true);
    return (false);
}

bool isConnectedOperator(char ch) {
    if (ch == ':' || ch == '+' || ch == '-')
        return (true);
    return (false);
}

/*bool isValidIdentifier(char *str) {

    if (strlen(str) > 20) {
        printf("Maximum identifier size must be 20 characters.\n");
    }

    bool is_first_letter = false;
    bool is_contains = true;
    for (char c = 'A'; c <= 'Z'; ++c) {
        if (str[0] == c) {
            is_first_letter = true;
        }

        for (int i = 0; i < strlen(str); i++) {
            if (str[i] != c) {
                for (int j = 0; j < 10; j++) {
                    if (str[i] != j) {
                        if (str[i] != '_') {
                            is_contains = false;
                        }
                    }
                }
            }
        }
    }

    return (is_first_letter && is_contains) ? 1 : 0;
}*/

bool isValidIdentifier(char* str)
{
    bool result = true;
    if (str[0] == '0' || str[0] == '1' || str[0] == '2' ||
        str[0] == '3' || str[0] == '4' || str[0] == '5' ||
        str[0] == '6' || str[0] == '7' || str[0] == '8' ||
        str[0] == '9' || isDelimiter(str[0]) == true ||
        str[0] == '_' )
        return false;

    for (int i = 1; i < strlen(str); ++i) {

        if (!isalnum(str[i])) result = false;
        if (str[i] == '_') result = true;

        if (!result) break;
    }
    return result;
}

void toUpperCase(char *str){
    char * s = str;
    while(*s) {
        *s = (*s >= 'a' && *s <= 'z') ? *s-32 : *s;
        s++;
    }
}
