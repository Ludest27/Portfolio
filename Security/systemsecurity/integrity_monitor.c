#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

#define BUFFER_SIZE 4096
#define HASH_HEX_LENGTH (SHA256_DIGEST_LENGTH * 2 + 1)
#define MAX_FILENAME 256
#define BASELINE_FILE "baseline.txt"

// Compute the SHA-256 hash of a file.
int compute_sha256(const char *filename, unsigned char hash[SHA256_DIGEST_LENGTH]) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror(filename);
        return -1;
    }
    
    SHA256_CTX sha256;
    if (SHA256_Init(&sha256) != 1) {
        fclose(file);
        return -1;
    }
    
    unsigned char buffer[BUFFER_SIZE];
    size_t bytesRead = 0;
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (SHA256_Update(&sha256, buffer, bytesRead) != 1) {
            fclose(file);
            return -1;
        }
    }
    
    if (SHA256_Final(hash, &sha256) != 1) {
        fclose(file);
        return -1;
    }
    
    fclose(file);
    return 0;
}

// Convert the binary hash to a hexadecimal string.
void hash_to_hex(const unsigned char hash[SHA256_DIGEST_LENGTH], char hexstr[HASH_HEX_LENGTH]) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hexstr + (i * 2), "%02x", hash[i]);
    }
    hexstr[HASH_HEX_LENGTH - 1] = '\0';
}

// Structure to hold baseline entry for a file.
typedef struct {
    char filename[MAX_FILENAME];
    char hash[HASH_HEX_LENGTH];
} baseline_entry;

// Load baseline entries from BASELINE_FILE.
// On success, returns a dynamically allocated array and sets *count to the number of entries.
baseline_entry* load_baseline(size_t *count) {
    FILE *fp = fopen(BASELINE_FILE, "r");
    if (!fp) {
        perror("Error opening baseline file");
        return NULL;
    }
    
    baseline_entry *entries = NULL;
    size_t capacity = 0;
    *count = 0;
    char line[512];
    
    while (fgets(line, sizeof(line), fp)) {
        char fname[MAX_FILENAME];
        char hash[HASH_HEX_LENGTH];
        if (sscanf(line, "%s %64s", fname, hash) == 2) {
            if (*count >= capacity) {
                capacity = (capacity == 0) ? 10 : capacity * 2;
                baseline_entry *tmp = realloc(entries, capacity * sizeof(baseline_entry));
                if (!tmp) {
                    free(entries);
                    fclose(fp);
                    return NULL;
                }
                entries = tmp;
            }
            strncpy(entries[*count].filename, fname, MAX_FILENAME - 1);
            entries[*count].filename[MAX_FILENAME - 1] = '\0';
            strncpy(entries[*count].hash, hash, HASH_HEX_LENGTH - 1);
            entries[*count].hash[HASH_HEX_LENGTH - 1] = '\0';
            (*count)++;
        }
    }
    fclose(fp);
    return entries;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <init|check> <file1> [file2 ...]\n", argv[0]);
        return 1;
    }
    
    const char *mode = argv[1];
    
    if (strcmp(mode, "init") == 0) {
        // Initialize baseline: compute hash for each file and write to baseline file.
        FILE *fp = fopen(BASELINE_FILE, "w");
        if (!fp) {
            perror("Error opening baseline file for writing");
            return 1;
        }
        
        for (int i = 2; i < argc; i++) {
            const char *filename = argv[i];
            unsigned char hash[SHA256_DIGEST_LENGTH];
            if (compute_sha256(filename, hash) != 0) {
                fprintf(stderr, "Failed to compute hash for %s\n", filename);
                continue;
            }
            char hexstr[HASH_HEX_LENGTH];
            hash_to_hex(hash, hexstr);
            fprintf(fp, "%s %s\n", filename, hexstr);
            printf("Computed hash for %s: %s\n", filename, hexstr);
        }
        fclose(fp);
        printf("Baseline saved to %s\n", BASELINE_FILE);
        
    } else if (strcmp(mode, "check") == 0) {
        // Load baseline entries.
        size_t count = 0;
        baseline_entry *entries = load_baseline(&count);
        if (!entries) {
            fprintf(stderr, "Failed to load baseline.\n");
            return 1;
        }
        
        // For each file provided, compute its current hash and compare with the baseline.
        for (int i = 2; i < argc; i++) {
            const char *filename = argv[i];
            unsigned char hash[SHA256_DIGEST_LENGTH];
            if (compute_sha256(filename, hash) != 0) {
                fprintf(stderr, "Failed to compute hash for %s\n", filename);
                continue;
            }
            char hexstr[HASH_HEX_LENGTH];
            hash_to_hex(hash, hexstr);
            
            int found = 0;
            for (size_t j = 0; j < count; j++) {
                if (strcmp(entries[j].filename, filename) == 0) {
                    found = 1;
                    if (strcmp(entries[j].hash, hexstr) == 0) {
                        printf("%s: OK\n", filename);
                    } else {
                        printf("%s: CHANGED\n", filename);
                        printf("  Baseline: %s\n", entries[j].hash);
                        printf("  Current:  %s\n", hexstr);
                    }
                    break;
                }
            }
            if (!found) {
                printf("%s: No baseline entry found.\n", filename);
            }
        }
        free(entries);
        
    } else {
        fprintf(stderr, "Unknown mode: %s\n", mode);
        fprintf(stderr, "Usage: %s <init|check> <file1> [file2 ...]\n", argv[0]);
        return 1;
    }
    
    return 0;
}
