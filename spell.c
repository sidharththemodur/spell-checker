#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef char *string;

// You may use this hash function in your Hashtable.c implementation.
// Return hash of string S into [0, SIZE)
int hash(string s, long size) {
	unsigned long sum;
	const long prime = 3141592661;

	for (sum = 0; *s; s++) {
		sum = (sum << 4) ^ *s;
	}

	return prime * sum % size;
}

#define DEFAULT_ALLOC_SIZE 10

/* Read the next line of the standard input into a newly allocated
 * buffer. Returns a pointer to that buffer if successful. If unable
 * to allocate memory, or if STDIN has ended (getchar() == EOF),
 * getLine will return NULL. It is the caller's responsiblity to free
 * the allocated memory when it is no longer needed. */
char *getLine() {
	int size = DEFAULT_ALLOC_SIZE; 
	char *str = malloc(size);
	if (!str) return NULL;

	int i = 0;
	char c;
	while ((c = getchar()) != '\n' && c != EOF) {
		if (i == size - 2) {
			size = size * 2;
			char *temp = realloc(str, size);
			if (!temp) {
				*str = '\0';
				return str;
			}
			else {
				str = temp;
			}
		}
		str[i] = c;
		i++;
	}
	str[i] = '\0';
    if (i == 0) {
        free(str);
        str = NULL;
    }
    return str;
}

#define DICTSIZE 200000


void addToHash(string *arr, string toAdd) {
	int startingIndex = hash(toAdd, DICTSIZE);
	int index = startingIndex;
	while (arr[index] != NULL && strcmp(arr[index], toAdd) != 0) {
		index = (index + 1) % DICTSIZE;
		if (index == startingIndex) {
			fprintf(stderr, "Too many words!\n");
			exit(1);
		}
	}
	arr[index] = toAdd;
}

bool charcmp(char c, char d) {
    if (c >= 'a' && c <= 'z') {
        c -= 32;
    }
    if (d >= 'a' && d <= 'z') {
        d -= 32;
    }
    return (c == d);
}

bool strcmpcasein(string s1, string s2) {
    while (*s1 && *s2) {
        if (!charcmp(*s1, *s2)) {
            return false;
        }
        s1++;
        s2++;
    }
    return true;
}

bool dictContains(string *dict, string word) {
	int startingIndex = hash(word, DICTSIZE);
	int index = startingIndex;
	while (dict[index] != NULL && !strcmpcasein(dict[index], word)) {
		index = (index + 1) % DICTSIZE;
		if (index == startingIndex) {
			return false;
		}
	}
	return dict[index] != NULL;
}

void printDeletes(string *dict, string word, int len) {
    char *permutation = malloc(len * sizeof(char));
    permutation[len - 1] = '\0';
    for (int i = 0; i < len; i++) {
        int currIdx = 0;
        for (int j = 0; j < len; j++) {
            if(j != i) {
                permutation[currIdx] = word[j];
                currIdx++;
            }
        }
        if (dictContains(dict, permutation)) printf("%s, ", permutation);
    }
    free(permutation);
}

// will set word[index] to 'Z'
void printReplacements(string *dict, int index, int length, char *word) {
    for (char c = 'A'; c <= 'Z'; c++) {
        word[index] = c;
        if (dictContains(dict, word)) printf("%s, ", word);
    }
}

void printAdds(string *dict, string word, int len) {
    int newlen = len + 1;
    string permutation = malloc((newlen + 1) * sizeof(char));
    strcpy(permutation + 1, word);
    for (int i = 0; i < len; i++) {
        printReplacements(dict ,i, newlen, permutation);
        permutation[i] = permutation[i + 1];
    }
    free(permutation);
}

void printMisspellings(string *dict, string word) {
    int len = strlen(word);
    printDeletes(dict, word, len);
    printAdds(dict, word, len);
    for (int i = 0; i < len; i++) {
        char oldChar = permutation[i];
        printReplacements(dict, i, len, word);
        permutation[i] = oldChar;
    }
}

// one option: create a set of all English words.
// for each word in input: check the set.
int main(int argc, char *argv[]) {
	string *dict = calloc(DICTSIZE, sizeof(string));
	
	// Fill the hashtable
	string curLine;
	while ((curLine = getLine()) != NULL) {
		addToHash(dict, curLine);
	}
	
	// Check each argument
	for (int i = 1; i < argc; i++) {
		if (!dictContains(dict, argv[i])) {
            printf("%s not found, suggestions:", argv[i]);
            printMisspellings(dict, argv[i]);
            printf("\n");
		}
	}
}
