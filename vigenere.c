#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_KEY_SIZE 10
#define MAX_PT_SIZE 500
#define MAX_FILENAME_SIZE 20

#ifndef DEBUG_MODE_RUN
#define DEBUG_MODE_RUN 0
#endif

const float letter_frequencies[26] = {
    8.2f,  // a
    1.5f,  // b
    2.8f,  // c
    4.3f,  // d
    12.7f, // e
    2.2f,  // f
    2.0f,  // g
    6.1f,  // h
    7.0f,  // i
    0.15f, // j
    0.8f,  // k
    4.0f,  // l
    2.4f,  // m
    6.7f,  // n
    7.5f,  // o
    1.9f,  // p
    0.10f, // q
    6.0f,  // r
    6.3f,  // s
    9.1f,  // t
    2.8f,  // u
    1.0f,  // v
    2.4f,  // w
    0.15f, // x
    2.0f,  // y
    0.07f  // z
};

char* vigenere_decode(char* ct, char* key);
void vigenere_encrypt();
void vigenere_keycrack(char* text, char** keys);
void vigenere_decrypt(char* filename);
float* frequency_analysis(char* text, int start, int interval);

int main(int argc, char* argv[]) {
    printf("E or D\n");
    char op;
    scanf("%c", &op);

    switch (op) {
    case 'E':
        vigenere_encrypt();
        break;
    case 'D':
        printf("Enter Filename: ");
        char* fn = malloc(MAX_FILENAME_SIZE);
        scanf("%s", fn);
        vigenere_decrypt(fn);
        break;
    default:
        printf("You're stupid\n");
        return 0;
    }
}

float* frequency_analysis(char* text, int start, int interval) {
    float* freq_anal = malloc(26 * sizeof(float));

    for (int j = start; j < strlen(text); j += interval) {
        freq_anal[text[j] - 65]++;
    }
    int sum_freqs = 0;
    for (int j = 0; j < 26; j++)
        sum_freqs += freq_anal[j];
    for (int j = 0; j < 26; j++)
        freq_anal[j] /= sum_freqs;
    return freq_anal;
}

void vigenere_keycrack(char* text, char** keys) {
    int i, j;

    int* freqs = malloc(20 * sizeof(int));
    char* curr = malloc(3);
    char* window = malloc(3);
    int spacing;

    for (i = 0; i < strlen(text) - 3; i++) {
        strncpy(curr, text + i, 3);
        for (j = i + 3; j < strlen(text) - 3; j++) {
            strncpy(window, text + j, 3);
            if (strcmp(curr, window) == 0) {
                spacing = j - i;
                for (int k = 2; k <= 20; k++) {
                    if (spacing % k == 0)
                        freqs[k]++;
                }
            }
        }
    }

    int key_lens[3] = {0, 0, 0};

    for (i = 0; i < 20; i++) {
        if (freqs[i] > freqs[key_lens[0]]) {
            key_lens[2] = key_lens[1];
            key_lens[1] = key_lens[0];
            key_lens[0] = i;
        } else if (freqs[i] < freqs[key_lens[0]] && freqs[i] > freqs[key_lens[1]]) {
            key_lens[2] = key_lens[1];
            key_lens[1] = i;
        } else if (freqs[i] < freqs[key_lens[1]] && freqs[i] >= freqs[key_lens[2]]) {
            key_lens[2] = i;
        }
    }

    for (i = 0; i < 3; i++) {
        keys[i] = malloc(key_lens[i]);
    }

    float min_dists[3] = {INFINITY, INFINITY, INFINITY};
    float* freq_anal;

    for (int l = 0; l < 3; l++) {
        for (i = 0; i < key_lens[l]; i++) {
            // Each character of key
            freq_anal = frequency_analysis(text, i, key_lens[l]);

            min_dists[l] = INFINITY;
            int min_ind = 0;
            float curr_dist = 0.0;

            for (j = 0; j < 26; j++) {
                for (int k = 0; k < 26; k++) {
                    curr_dist += fabs(letter_frequencies[k] - 100 * freq_anal[(k + j) % 26]);
                }
                curr_dist /= 26;
                if (curr_dist < min_dists[l]) {
                    min_dists[l] = curr_dist;
                    min_ind = j;
                }
            }

            keys[l][i] = (char)(min_ind + 65);
        }
    }
    free(freq_anal);
    free(freqs);
    free(curr);
    free(window);
}


char* vigenere_decode(char* ct, char* key) {
    // Rename to vigenere_decode
    int i;
    int key_len_1 = strlen(key);
    int count = 0;
    char* pt = malloc(strlen(ct));

    for (i = 0; i < strlen(ct); i++) {
        int temp = ct[i] - key[count % key_len_1];
        pt[i] = (char)(temp < 0) ? (temp + 26 + 'A') : (temp + 'A');
        count++;
    }

    return pt;
}

void vigenere_decrypt(char* filename){
    int i, j;
    //Read file for input
    FILE* fp;
    fp = fopen("input.txt", "r");
    if (fp == NULL) {
        printf("Fuck you no file bad bad bad\n");
        return;
    }

    char text[MAX_PT_SIZE];

    // Now we have text
    while (fgets(text, MAX_PT_SIZE, fp) != NULL);

    // Check with user
    printf("Decode with (K) or without key (N)?");
    char dop;
    scanf(" %c", &dop);
    if (dop=='K') {
        printf("Enter Key: ");
        char* key = malloc(MAX_KEY_SIZE);
        scanf("%s", key);
        char* pt = malloc(MAX_PT_SIZE);
        pt = vigenere_decode(text, key);
        printf("Decrypted text is\n\n %s\n", pt);
        free(pt);
        return;
    }

    // Decryption without key
    char** keys = malloc(3*sizeof(char*));
    vigenere_keycrack(text, keys);

    //Decryption
    char* decrypt_text_arr[3];
    // char* check = malloc(strlen(text));
    for (i = 0; i < 3; i++)
        decrypt_text_arr[i] = malloc(strlen(text));

    float decrypt_distances[3];

    float* freq_anal;

    for (int i = 0; i < 3; i++) {
        decrypt_text_arr[i] = vigenere_decode(text, keys[i]);
        // decrypt_text_arr[i] = check;
        // Frequency Analysis and Match
        decrypt_distances[i] = 0;
        int sum_freqs = 0;

        freq_anal = frequency_analysis(decrypt_text_arr[i], 0, 1);

        for (int k = 0; k < 26; k++) {
            decrypt_distances[i] += fabs(letter_frequencies[k] - 100 * freq_anal[k]);
        }
        decrypt_distances[i] /= 26;

        // Print the keys, min distances and then decrypted text for each key
        #if DEBUG_MODE_RUN
        printf("Key: %s \t \t Distance: %f\t\t DecrDist: %f\n", keys[i], min_dists[i],
               decrypt_distances[i]);
        #endif
    }

    float min = INFINITY;
    int min_idx = 0;
    for (i = 0; i < 3; i++) {
        if (decrypt_distances[i] < min) {
            min = decrypt_distances[i];
            min_idx = i;
        }
    }

    printf("Key is %s\n", keys[min_idx]);
    printf("Decrypted text is\n\n %s\n", decrypt_text_arr[min_idx]);
    for (i = 0; i<3; i++) free(decrypt_text_arr[i]);
    free(keys);
}

void vigenere_encrypt() {
    int i;
    printf("Enter Key: ");
    char* key = malloc(MAX_KEY_SIZE);
    scanf("%s", key);

    printf("Enter Plaintext: ");
    char* pt = malloc(MAX_PT_SIZE);
    scanf("%s", pt);

    int key_len_1 = strlen(key);
    char* ct = malloc(strlen(pt));
    int count = 0;

    for (i = 0; i < strlen(pt); i++) {
        ct[i] = (char)((pt[i] + key[count % key_len_1] - ('A' * 2)) % 26) + 'A';
        count++;
    }

    printf("Ciphertext is : \t %s \n", ct);

    free(key);
    free(pt);
    free(ct);
}
