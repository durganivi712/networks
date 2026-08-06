#include <stdio.h>
#include <string.h>
void charToBinary(char ch, char binary[])
{
    int i;

    for (i = 7; i >= 0; i--)
    {
        if ((ch >> i) & 1)
            binary[7 - i] = '1';
        else
            binary[7 - i] = '0';
    }

    binary[8] = '\0';
}
void byteStuffing(char binaryData[], char stuffedData[])
{
    char flag[] = "01111110";
    char escape[] = "11100000";

    int i, j = 0,k;
    char byte[9];

    for (i = 0; binaryData[i] != '\0'; i = i + 8)
    {
        strncpy(byte, &binaryData[i], 8);
        byte[8] = '\0';
        if (strcmp(byte, flag) == 0 ||
            strcmp(byte, escape) == 0)
        {
            for  (k = 0; k < 8; k++)
            {
                stuffedData[j] = escape[k];
                j++;
            }
        }
        int k;
        for ( k = 0; k < 8; k++)
        {
            stuffedData[j] = byte[k];
            j++;
        }
    }

    stuffedData[j] = '\0';
}
void addBinary(char a[], char b[], char result[])
{
    int i;
    int carry = 0;

    for (i = 7; i >= 0; i--)
    {
        int x = a[i] - '0';
        int y = b[i] - '0';

        int sum = x + y + carry;

        result[i] = (sum % 2) + '0';
        carry = sum / 2;
    }
    if (carry == 1)
    {
        for (i = 7; i >= 0; i--)
        {
            if (result[i] == '0')
            {
                result[i] = '1';
                carry = 0;
                break;
            }
            else
            {
                result[i] = '0';
            }
        }
    }

    result[8] = '\0';
}
void calculateChecksum(char data[], char checksum[])
{
    int length;
    int blocks;
    int i, j;

    char sum[9] = "00000000";
    char block[9];
    char result[9];
    length = strlen(data);
    blocks = (length + 7) / 8;
    printf("\nCHECKSUM CALCULATION\n");
    printf("--------------------\n");
    for (i = 0; i < blocks; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if ((i * 8 + j) < length)
                block[j] = data[i * 8 + j];
            else
                block[j] = '0';
        }
        block[8] = '\0';
        printf("Block %d  : %s\n", i + 1, block);
        addBinary(sum, block, result);
        strcpy(sum, result);
        printf("Sum      : %s\n\n", sum);
    }
    for (i = 0; i < 8; i++)
    {
        if (sum[i] == '0')
            checksum[i] = '1';
        else
            checksum[i] = '0';
    }
    checksum[8] = '\0';
    printf("Final Sum      : %s\n", sum);
    printf("1's Complement : %s\n", checksum);
}
void printFrame(char data[], char checksum[])
{
    printf("\n\n                    PPP FRAME\n");
    printf("+----------+----------+----------+------------------------------------------+----------+----------+\n");
    printf("|   Flag   | Address  | Control  |                  Data                    | Checksum |   Flag   |\n");

    printf("+----------+----------+----------+------------------------------------------+----------+----------+\n");
    printf("| 01111110 | 11111111 | 00000011 | %-40s | %-8s | 01111110 |\n",
           data, checksum);
    printf("+----------+----------+----------+------------------------------------------+----------+----------+\n");
}
int main()
{
    FILE *fp;
    FILE *out;
    char message[100];
    char binaryData[1000] = "";
    char stuffedData[1000] = "";
    char checksum[9];
    char binary[9];
    char choice;
    int position;
    fp = fopen("message.txt", "r");
    if (fp == NULL)
    {
        printf("message.txt file not found!\n");
        return 0;
    }
    fgets(message, sizeof(message), fp);
    fclose(fp);
    message[strcspn(message, "\n")] = '\0';
    printf("MESSAGE FROM FILE : %s\n", message);
    int i;
    for ( i = 0; message[i] != '\0'; i++)
    {
        charToBinary(message[i], binary);
        strcat(binaryData, binary);
    }
    printf("\nBINARY DATA:\n");
    printf("%s\n", binaryData);
    byteStuffing(binaryData, stuffedData);
    printf("\nBYTE STUFFED DATA:\n");
    printf("%s\n", stuffedData);
    calculateChecksum(stuffedData, checksum);
    printf("\nCHECKSUM = %s\n", checksum);
    printFrame(stuffedData, checksum);
    printf("\nDo you want to change a bit in DATA? (Y/N): ");
    scanf(" %c", &choice);
    if (choice == 'N' || choice == 'n')
    {
        out = fopen("sender_output.txt", "w");
        fprintf(out, "01111110\n");
        fprintf(out, "11111111\n");
        fprintf(out, "00000011\n");
        fprintf(out, "%s\n", stuffedData);
        fprintf(out, "%s\n", checksum);
        fprintf(out, "01111110\n");
        fclose(out);
        printf("\nNo bit changed.\n");
        printf("Data is ready for transmission.\n");
    }
    else if (choice == 'Y' || choice == 'y')
    {
        printf("Enter bit position to change (0 to %d): ",
               (int)strlen(stuffedData) - 1);
        scanf("%d", &position);
        if (position >= 0 && position < strlen(stuffedData))
        {
            if (stuffedData[position] == '0')
                stuffedData[position] = '1';
            else
                stuffedData[position] = '0';
            out = fopen("sender_output.txt", "w");
            fprintf(out, "01111110\n");
            fprintf(out, "11111111\n");
            fprintf(out, "00000011\n");
            fprintf(out, "%s\n", stuffedData);
            fprintf(out, "%s\n", checksum);
            fprintf(out, "01111110\n");
            fclose(out);
            printf("\n*** BIT CHANGED SUCCESSFULLY ***\n");
            printf("\nCHANGED PPP FRAME\n");
            printFrame(stuffedData, checksum);
        }
        else
        {
            printf("Invalid position!\n");
            return 0;
        }
    }
    printf("\nFrame stored in sender_output.txt\n");
    return 0;
}
#include <stdio.h>
#include <string.h>
void addBinary(char a[], char b[], char result[])
{
    int i;
    int carry = 0;
    for (i = 7; i >= 0; i--)
    {
        int x = a[i] - '0';
        int y = b[i] - '0';
        int sum = x + y + carry;
        result[i] = (sum % 2) + '0';
        carry = sum / 2;
    }
    if (carry == 1)
    {
        for (i = 7; i >= 0; i--)
        {
            if (result[i] == '0')
            {
                result[i] = '1';
                carry = 0;
                break;
            }
            else
            {
                result[i] = '0';
            }
        }
    }
    result[8] = '\0';
}
void printFrame(char data[], char checksum[])
{
    printf("\n\n                 RECEIVED PPP FRAME\n");
    printf("+----------+----------+----------+------------------------------------------+----------+----------+\n");
    printf("|   Flag   | Address  | Control  |                  Data                    | Checksum |   Flag   |\n");
    printf("+----------+----------+----------+------------------------------------------+----------+----------+\n");
    printf("| 01111110 | 11111111 | 00000011 | %-40s | %-8s | 01111110 |\n",
           data, checksum);
    printf("+----------+----------+----------+------------------------------------------+----------+----------+\n");
}
int main()
{
    FILE *fp;
    char flag1[20];
    char address[20];
    char control[20];
    char data[1000];
    char checksum[20];
    char flag2[20];
    char sum[9] = "00000000";
    char block[9];
    char result[9];
    int length;
    int blocks;
    int i, j;
    fp = fopen("sender_output.txt", "r");
    if (fp == NULL)
    {
        printf("sender_output.txt file not found!\n");
        return 0;
    }
    fscanf(fp, "%s", flag1);
    fscanf(fp, "%s", address);
    fscanf(fp, "%s", control);
    fscanf(fp, "%s", data);
    fscanf(fp, "%s", checksum);
    fscanf(fp, "%s", flag2);
    fclose(fp);
    printFrame(data, checksum);
    length = strlen(data);
    blocks = (length + 7) / 8;
    printf("\n\n              CHECKSUM VERIFICATION\n");
    printf("+----------+----------------+\n");
    printf("| Block    | 8-bit Data     |\n");
    printf("+----------+----------------+\n");
    for (i = 0; i < blocks; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if ((i * 8 + j) < length)
                block[j] = data[i * 8 + j];
            else
                block[j] = '0';
        }
        block[8] = '\0';
        printf("| Block %-2d | %-14s |\n", i + 1, block);
        addBinary(sum, block, result);
        strcpy(sum, result);
    }
    printf("+----------+----------------+\n");
    printf("| Checksum | %-14s |\n", checksum);
    printf("+----------+----------------+\n");
    addBinary(sum, checksum, result);
    printf("\nFinal Sum + Checksum = %s\n", result);
    if (strcmp(result, "11111111") == 0)
    {
        printf("\n====================================\n");
        printf("       RESULT : NO ERROR\n");
        printf("       Final Answer = 11111111\n");
        printf("====================================\n");
    }
    else
    {
        printf("\n====================================\n");
        printf("       RESULT : ERROR DETECTED\n");
        printf("       Final Answer = %s\n", result);
        printf("====================================\n");
    }
    return 0;
}

