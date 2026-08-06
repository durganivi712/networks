#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct Network
{
    char url[50];
    char srcIP[20];
    char destIP[20];
    char srcMAC[20];
    char destMAC[20];
};

struct Network table[3] =
{
    {"www.google.com","172.16.16.20","142.250.190.78","AA:BB:CC:DD:EE:01","11:22:33:44:55:66"},
    {"www.youtube.com","173.16.1.20","142.250.182.46","AA:BB:CC:DD:EE:02","22:33:44:55:66:77"},
    {"www.gmail.com","172.16.16.30","142.250.183.37","AA:BB:CC:DD:EE:03","33:44:55:66:77:88"}
};

void decimalToBinary(int n,char bin[])
{
    int i;

    for(i=7;i>=0;i--)
    {
        bin[i]=(n%2)+'0';
        n=n/2;
    }

    bin[8]='\0';
}

void portToBinary(int n,char bin[])
{
    int i;

    for(i=15;i>=0;i--)
    {
        bin[i]=(n%2)+'0';
        n=n/2;
    }

    bin[16]='\0';
}

void messageToBinary(char msg[],char binary[])
{
    int i;
    char temp[9];

    binary[0]='\0';

    for(i=0;msg[i]!='\0';i++)
    {
        decimalToBinary((int)msg[i],temp);
        strcat(binary,temp);
    }
}

void ipToBinary(char ip[],char binary[])
{
    int i=0,j;
    int num=0;
    char temp[9];

    binary[0]='\0';

    while(1)
    {
        if(ip[i]=='.' || ip[i]=='\0')
        {
            decimalToBinary(num,temp);
            strcat(binary,temp);

            num=0;

            if(ip[i]=='\0')
                break;
        }
        else
        {
            num=num*10+(ip[i]-'0');
        }

        i++;
    }
}

void macToBinary(char mac[],char binary[])
{
    int i,j,value;
    char temp[9];

    binary[0]='\0';

    for(i=0;mac[i]!='\0';)
    {
        if(mac[i]==':')
        {
            i++;
            continue;
        }

        value=0;

        for(j=0;j<2;j++)
        {
            value=value*16;

            if(mac[i]>='0' && mac[i]<='9')
                value=value+(mac[i]-'0');
            else
                value=value+(mac[i]-'A'+10);

            i++;
        }

        decimalToBinary(value,temp);
        strcat(binary,temp);
    }
}
int main()
{
    int i, j;
    int found = -1;

    char url[50];
    char message[200];

    char msgBinary[500];
    char srcPortBinary[55];
    char destPortBinary[55];
    char transport[600];

    char srcIPBinary[40];
    char destIPBinary[40];

    char srcMACBinary[60];
    char destMACBinary[60];

    int srcPort, destPort;

    srand(time(NULL));

    printf("Enter URL : ");
    scanf("%s", url);

    for(i=0; i<3; i++)
    {
        if(strcmp(url, table[i].url)==0)
        {
            found=i;
            break;
        }
    }

    if(found==-1)
    {
        printf("\nURL Not Found!\n");
        return 0;
    }

    printf("\n===== URL DETAILS =====\n");

    printf("Source IP : %s\n", table[found].srcIP);
    printf("Destination IP : %s\n", table[found].destIP);
    printf("Source MAC : %s\n", table[found].srcMAC);
    printf("Destination MAC: %s\n", table[found].destMAC);

    getchar();

    printf("\nEnter Message : ");
    fgets(message,sizeof(message),stdin);

    message[strcspn(message,"\n")] = '\0';

    srcPort = rand()%50000 + 1000;
    destPort = rand()%50000 + 1000;

    portToBinary(srcPort,srcPortBinary);
    portToBinary(destPort,destPortBinary);

    messageToBinary(message,msgBinary);

    strcpy(transport,msgBinary);
    strcat(transport,srcPortBinary);
    strcat(transport,destPortBinary);

    printf("\n===== TRANSPORT LAYER =====\n");

    printf("Source Port : %d\n",srcPort);
    printf("Destination Port : %d\n",destPort);

    printf("\nMessage Binary :\n%s\n",msgBinary);

    printf("\nSource Port Binary : %s\n",srcPortBinary);
    printf("Destination Port Binary : %s\n",destPortBinary);

    printf("\nTransport Segment :\n%s\n",transport);

    ipToBinary(table[found].srcIP,srcIPBinary);
    ipToBinary(table[found].destIP,destIPBinary);

    macToBinary(table[found].srcMAC,srcMACBinary);
    macToBinary(table[found].destMAC,destMACBinary);

    int len = strlen(transport);

    int totalPackets = (len + 23) / 24;

    char packet[25];

    int totalFrames = 0;
    
    printf("\n==================== NETWORK LAYER ====================\n\n");

    printf("%-25s %-35s %-35s %-20s %-20s\n",
           "Packet Data",
           "Source IP(Binary)",
           "Destination IP(Binary)",
           "Source Port(Binary)",
           "Destination Port(Binary)");

    for(i=0; i<totalPackets; i++)
    {
        int start = i * 24;

        for(j=0; j<24; j++)
        {
            if(start + j < len)
                packet[j] = transport[start + j];
            else
                packet[j] = '0';
        }

        packet[24] = '\0';

        printf("%-25s %-35s %-35s %-20s %-20s\n",
               packet,
               srcIPBinary,
               destIPBinary,
               srcPortBinary,
               destPortBinary);

        totalFrames = totalFrames + 3;
    }

    printf("\n==================== DATA LINK LAYER ====================\n\n");

    printf("%-15s %-50s %-50s %-35s %-35s %-10s %-20s %-20s\n",
           "Frame Data",
           "Source MAC(Binary)",
           "Destination MAC(Binary)",
           "Source IP(Binary)",
           "Destination IP(Binary)",
           "Error",
           "Source Port(Binary)",
           "Destination Port(Binary)");
           for(i=0; i<totalPackets; i++)
    {
        int start = i * 24;

        for(j=0; j<24; j++)
        {
            if(start + j < len)
                packet[j] = transport[start + j];
            else
                packet[j] = '0';
        }

        packet[24] = '\0';

        char frame[11];
        int k, m;

        for(k=0; k<3; k++)
        {
            int pos = k * 10;

            for(m=0; m<10; m++)
            {
                if(pos + m < 24)
                    frame[m] = packet[pos + m];
                else
                    frame[m] = '0';
            }

            frame[10] = '\0';

            printf("%-15s %-50s %-50s %-35s %-35s %-10s %-20s %-20s\n",
                   frame,
                   srcMACBinary,
                   destMACBinary,
                   srcIPBinary,
                   destIPBinary,
                   "00000000",
                   srcPortBinary,
                   destPortBinary);
        }
    }

    printf("\n==================== TRANSMISSION SUMMARY ====================\n");

    printf("Total Bits : %d\n", len);
    printf("Total Packets : %d\n", totalPackets);
    printf("Total Frames : %d\n", totalFrames);

    return 0;
}

