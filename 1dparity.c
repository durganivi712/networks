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
char findParity(char data[])
{
    int count = 0;
    int i;
    for (i = 0; i < 7; i++)
    {
        if (data[i] == '1')
            count++;
    }
    if (count % 2 == 0)
        return '0';
    else
        return '1';
}
int verifyParity(char block[])
{
    int count = 0;
    int i;
    for (i = 0; i < 8; i++)
    {
        if (block[i] == '1')
            count++;
    }
    /* Returns 1 if error exists (odd count), 0 if no error (even count) */
    return (count % 2 != 0);
}
void displayBlocks(char blocks[][9], int blockCount)
{
    int i;
    printf("\n");
    printf("+----------+------------------+\n");
    printf("|  Block   |  Data + Parity   |\n");
    printf("+----------+------------------+\n");
    for (i = 0; i < blockCount; i++)
    {
        printf("| Block %-2d |     %s       |\n", i + 1, blocks[i]);
    }
    printf("+----------+------------------+\n");
}

int main()
{
    char message[100];
    char binaryData[1000] = "";
    char binary[9];
    char blocks[200][9];
    int length;
    int blockCount;
    int i, j;
    int numberOfChanges;
    int position;
    int totalError = 0;
    printf("\n============================================\n");
    printf("                  SENDER\n");
    printf("============================================\n");

    printf("\nEnter message: ");
    scanf(" %[^\n]", message);
    for (i = 0; message[i] != '\0'; i++)
    {
        charToBinary(message[i], binary);
        strcat(binaryData, binary);
    }

    printf("\nOriginal Message : %s\n", message);
    printf("\nBinary Message:\n");
    printf("%s\n", binaryData);
    length = strlen(binaryData);
    blockCount = (length + 6) / 7;
    for (i = 0; i < blockCount; i++)
    {
        for (j = 0; j < 7; j++)
        {
            if ((i * 7 + j) < length)
                blocks[i][j] = binaryData[i * 7 + j];
            else
                blocks[i][j] = '0';
        }
        blocks[i][7] = findParity(blocks[i]);
        blocks[i][8] = '\0';
    }
    printf("\n7-BIT DATA + EVEN PARITY BIT");
    displayBlocks(blocks, blockCount);
    printf("\nEach block = 7 Data Bits + 1 Parity Bit\n");
    printf("\n\n============================================\n");
    printf("                 RECEIVER\n");
    printf("============================================\n");

    printf("\nReceived Data:");
    displayBlocks(blocks, blockCount);
    printf("\nHow many bits do you want to change? ");
    scanf("%d", &numberOfChanges);

    for (i = 0; i < numberOfChanges; i++)
    {
        printf("Enter bit position to change (0 to %d): ", blockCount * 8 - 1);
        scanf("%d", &position);

        if (position < 0 || position >= blockCount * 8)
        {
            printf("Invalid position! Enter again.\n");
            i--;
            continue;
        }

        int blockNumber = position / 8;
        int bitNumber = position % 8;
        if (blocks[blockNumber][bitNumber] == '0')
            blocks[blockNumber][bitNumber] = '1';
        else
            blocks[blockNumber][bitNumber] = '0';
    }
    if (numberOfChanges > 0)
    {
        printf("\nDATA AFTER BIT CHANGE:");
        displayBlocks(blocks, blockCount);
    }
    printf("\n============================================\n");
    printf("             PARITY VERIFICATION\n");
    printf("============================================\n");
    
    printf("\n+----------+------------------+-------------------+\n");
    printf("|  Block   | Received Data    | Result            |\n");
    printf("+----------+------------------+-------------------+\n");

    int errors[200];
    int errorCount = 0;

    for (i = 0; i < blockCount; i++)
    {
        int hasError = verifyParity(blocks[i]);
        
        printf("| Block %-2d |     %s       | %-17s |\n", 
               i + 1, blocks[i], hasError ? "ERROR DETECTED" : "NO ERROR");
        
        if (hasError)
        {
            errors[errorCount] = i + 1;
            errorCount++;
            totalError = 1;
        }
    }
    printf("+----------+------------------+-------------------+\n");
    printf("\n============================================\n");
    printf("             FINAL RESULT\n");
    if (totalError)
    {
        printf("             ERROR DETECTED\n\n");
        printf("Error detected in block(s): ");
        for (i = 0; i < errorCount; i++)
        {
            printf("Block %d  ", errors[i]);
        }
        printf("\n");
    }
    else
    {
        printf("             NO ERROR DETECTED\n");
    }
    printf("============================================\n");

    return 0;
}

