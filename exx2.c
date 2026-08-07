char* addBinary(char* a, char* b) {
    int i = strlen(a) - 1;
    int j = strlen(b) - 1;
    int carry = 0;

    int maxLen = (strlen(a) > strlen(b) ? strlen(a) : strlen(b)) + 2;
    char *res = (char *)malloc(maxLen);

    int k = maxLen - 1;
    res[k--] = '\0';

    while (i >= 0 || j >= 0 || carry) {
        int sum = carry;

        if (i >= 0)
            sum += a[i--] - '0';
        if (j >= 0)
            sum += b[j--] - '0';

        res[k--] = (sum % 2) + '0';
        carry = sum / 2;
    }

    char *ans = strdup(res + k + 1);
    free(res);
    return ans;
}

#include <stdlib.h>
#include <stdbool.h>

int subarrayBitwiseORs(int* arr, int arrSize) {
    bool *seen = (bool *)calloc(1 << 20, sizeof(bool));

    int prev[32], curr[32];
    int prevSize = 0;
    int count = 0;

    for (int i = 0; i < arrSize; i++) {
        int currSize = 0;

        curr[currSize++] = arr[i];

        for (int j = 0; j < prevSize; j++) {
            int val = prev[j] | arr[i];

            bool exists = false;
            for (int k = 0; k < currSize; k++) {
                if (curr[k] == val) {
                    exists = true;
                    break;
                }
            }

            if (!exists)
                curr[currSize++] = val;
        }

        for (int j = 0; j < currSize; j++) {
            if (!seen[curr[j]]) {
                seen[curr[j]] = true;
                count++;
            }
        }

        prevSize = currSize;
        for (int j = 0; j < currSize; j++)
            prev[j] = curr[j];
    }

    free(seen);
    return count;
