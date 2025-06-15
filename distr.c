#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 70
#define MAX_LINE 128
#define AMINO_ACIDS 21  // 20 standard + 'X'

// Amino acid characters in order
const char amino_acid_list[AMINO_ACIDS] = {
    'A','C','D','E','F','G','H','I','K','L',
    'M','N','P','Q','R','S','T','V','W','Y','X'
};

// Get index of amino acid in the list
int aa_index(char aa) {
    for (int i = 0; i < AMINO_ACIDS; i++) {
        if (amino_acid_list[i] == aa) return i;
    }
    return -1; // Invalid character
}

// Process a single file
void process_file(const char *filename, int length_counts[], int distributions[MAX_LENGTH + 1][MAX_LENGTH][AMINO_ACIDS]) {

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        size_t len = strcspn(line, "\r\n"); // Remove newline
        line[len] = '\0';

        int length = strlen(line);
        if (length > MAX_LENGTH) continue;

        length_counts[length]++;
        for (int i = 0; i < length; i++) {
            int idx = aa_index(line[i]);
            if (idx >= 0) {
                distributions[length][i][idx]++;
            }
        }
    }

    fclose(file);
}

// Print results
void print_results(const char *cdr_name, int length_counts[], int distributions[MAX_LENGTH + 1][MAX_LENGTH][AMINO_ACIDS]) {
    printf("Results for %s:\n", cdr_name);
    long long total_records = 0;
    for (int len = 1; len <= MAX_LENGTH; len++) {
        total_records += length_counts[len];
    }
    printf("Total records: %lld\n", total_records);

    for (int len = 1; len <= MAX_LENGTH; len++) {
        if (length_counts[len] == 0) continue;
        printf("Length %d: %d (%.4%%) records\n", len, length_counts[len], 
            100.0*length_counts[len]/ total_records);
        for (int pos = 0; pos < len; pos++) {
            printf("  Position %d: ", pos + 1);
            for (int aa = 0; aa < AMINO_ACIDS; aa++) {
                int count = distributions[len][pos][aa];
                if (count > 0) {
                    printf("%c(%.4%%) ", amino_acid_list[aa], 100.0*count/length_counts[len]);
                }
            }
            printf("\n");
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    //const char *files[3] = {"cdr1.txt", "cdr2.txt", "cdr3.txt"};
    if (argc < 2 || argc > 6) {
        fprintf(stderr, "Usage: %s <cdr1.txt> <cdr2.txt> <cdr3.txt> [<cdr4.txt> <cdr5.txt>]\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *names[5] = {"CDR1", "CDR2", "CDR3", "CDR4", "CDR5"};

    for (int i = 0; i < argc; i++) {
        int length_counts[MAX_LENGTH + 1] = {0};
        int distributions[MAX_LENGTH + 1][MAX_LENGTH][AMINO_ACIDS] = {{{0}}};

        process_file(argv[i+1], length_counts, distributions);
        print_results(names[i], length_counts, distributions);
    }

    return 0;
}

