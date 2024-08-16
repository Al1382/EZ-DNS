#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"


void flush_dns() {

    printf(YELLOW "\nClearing DNS cache: \n" RESET);

    FILE *pf;

    pf = popen("ipconfig /flushdns > NUL", "r");
    if (pf == NULL) {
        perror("popen failed for flushdns");
        printf(RED "Error! \n" RESET);
    } else {
        pclose(pf);
        printf(GREEN "Done! \n" RESET);
    }
}

void executer(char *dnsServer1, char *dnsServer2) {

    system("cls");

    FILE *pf;
    
    char WiFi1[256];
    char WiFi2[256];
    char Lan1[256];
    char Lan2[256];

    // Construct the command strings
    sprintf(WiFi1, "netsh interface ipv4 set dnsservers \"Wi-Fi\" static %s primary", dnsServer1);
    sprintf(WiFi2, "netsh interface ipv4 add dnsservers \"Wi-Fi\" %s index=2", dnsServer2);

    sprintf(Lan1, "netsh interface ipv4 set dnsservers \"Ethernet\" static %s primary", dnsServer1);
    sprintf(Lan2, "netsh interface ipv4 add dnsservers \"Ethernet\" %s index=2", dnsServer2);
  
    // Execute the commands for Wi-Fi
    printf(YELLOW "\nSetting Wi-Fi primary DNS: \n" RESET);
    pf = popen(WiFi1, "r");
    if (pf == NULL) {
        perror("popen failed for WiFi1");
        printf(RED "Error! \n" RESET);
    } else {
        pclose(pf);
        printf(GREEN "Done! \n" RESET);
    }

    printf(YELLOW "\nSetting Wi-Fi secondary DNS: \n" RESET);
    pf = popen(WiFi2, "r");
    if (pf == NULL) {
        perror("popen failed for WiFi2");
        printf(RED "Error! \n" RESET);
    } else {
        pclose(pf);
        printf(GREEN "Done! \n" RESET);
    }

    // Execute the commands for Ethernet
    printf(YELLOW "\nSetting Ethernet primary DNS: \n" RESET);
    pf = popen(Lan1, "r");
    if (pf == NULL) {
        perror("popen failed for Lan1");
        printf(RED "Error! \n" RESET);
    } else {
        pclose(pf);
        printf(GREEN "Done! \n" RESET);
    }

    printf(YELLOW "\nSetting Ethernet secondary DNS: \n" RESET);
    pf = popen(Lan2, "r");
    if (pf == NULL) {
        perror("popen failed for Lan2");
        printf(RED "Error! \n" RESET);
    } else {
        pclose(pf);
        printf(GREEN "Done! \n" RESET);
    }

    // Flush DNS
    flush_dns();

}



void remove_dns() {

    system("cls");

    FILE *pf;

    // Remove DNS servers for Wi-Fi
    printf(YELLOW "\nRemoving Wi-Fi DNS: \n" RESET);
    pf = popen("netsh interface ipv4 delete dnsservers \"Wi-Fi\" all > NUL", "r");
    if (pf == NULL) {
        perror("popen failed for remove Wi-Fi");
        printf(RED "Error! \n" RESET);
    } else {
        pclose(pf);
        printf(GREEN "Done! \n" RESET);
    }

    // Remove DNS servers for Ethernet
    printf(YELLOW "\nRemoving Ethernet DNS: \n" RESET);
    pf = popen("netsh interface ipv4 delete dnsservers \"Ethernet\" all > NUL", "r");
    if (pf == NULL) {
        perror("popen failed for remove Ethernet");
        printf(RED "Error! \n" RESET);
    } else {
        pclose(pf);
        printf(GREEN "Done! \n" RESET);
    }

    // Flush DNS
    flush_dns();

}

void show_current_dns() {

    system("cls");

    FILE *pf;

    // Display current DNS settings for Ethernet
    printf(YELLOW "\nCurrent DNS settings for Ethernet:\n" RESET);
    pf = popen("netsh interface ipv4 show dnsservers \"Ethernet\"", "r");
    if (pf) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pf) != NULL) {
            printf("%s", buffer);
        }
        pclose(pf);
    } else {
        perror("popen failed for show Ethernet DNS");
    }

    // Display current DNS settings for Wi-Fi
    printf(YELLOW "\nCurrent DNS settings for Wi-Fi:\n" RESET);
    pf = popen("netsh interface ipv4 show dnsservers \"Wi-Fi\"", "r");
    if (pf) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pf) != NULL) {
            printf("%s", buffer);
        }
        pclose(pf);
    } else {
        perror("popen failed for show Wi-Fi DNS");
    }
}

void check_current_dns() {
    FILE *pf;
    char buffer[256];
    char current_dns[64] = "";
    const char *dns_list[][2] = {
        {"78.157.42.100", "Electro"},
        {"10.202.10.10", "Radar"},
        {"178.22.122.100", "Shecan"},
        {"10.202.10.102", "403"},
        {"1.1.1.1", "Cloudflare"},
        {"64.6.64.6", "Verisign"},
        {"77.88.8.8", "Yandex"},
        {"8.8.8.8", "Google"},
        {"94.156.65.130", "Shelter"},
    };
    int dns_list_size = sizeof(dns_list) / sizeof(dns_list[0]);

    // Use nslookup to find the current DNS server and suppress unwanted output
    pf = popen("nslookup www.google.com 2>&1", "r"); // Redirect stderr to stdout
    if (pf == NULL) {
        perror("popen failed for nslookup");
        printf("Error!\n");
        return;
    }

    // Read the output of nslookup to find the DNS server in use
    while (fgets(buffer, sizeof(buffer), pf) != NULL) {
        if (strstr(buffer, "Address:") != NULL) {
            // Extract the DNS server IP address
            sscanf(buffer, "Address: %63s", current_dns);
            break;
        }
    }
    pclose(pf);

    // Check if the current DNS was found
    if (strlen(current_dns) == 0) {
        printf(RED "\nNo DNS detected\n" RESET);
        return;
    }

    // Compare the current DNS with the predefined DNS list
    int found = 0;
    for (int i = 0; i < dns_list_size; i++) {
        if (strcmp(current_dns, dns_list[i][0]) == 0) {
            printf(GREEN "\nCurrent set DNS: %s\n" RESET, dns_list[i][1]);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf(RED "\nCurrent set DNS is unknown\n" RESET);
    }
}



int main() {
    char *Electro1 = "78.157.42.100";
    char *Electro2 = "78.157.42.101";
    char *Radar1 = "10.202.10.10";
    char *Radar2 = "10.202.10.11";
    char *Shecan1 = "178.22.122.100";
    char *Shecan2 = "185.51.200.2";
    char *FourZeroThree1 = "10.202.10.102";
    char *FourZeroThree2 = "10.202.10.202";
    char *Cloudflare1 = "1.1.1.1";
    char *Cloudflare2 = "1.0.0.1";
    char *Verisign1 = "64.6.64.6";
    char *Verisign2 = "64.6.65.6";
    char *Yandex1 = "77.88.8.8";
    char *Yandex2 = "77.88.8.1";
    char *Google1 = "8.8.8.8";
    char *Google2 = "8.8.4.4";
    char *ShelterOne1 = "94.156.65.130";
    char *ShelterOne2 = "91.92.249.227";
    char *ShelterTwo1 = "94.156.65.130";
    char *ShelterTwo2 = "91.92.253.197";
    int mode;

start:
    system("cls"); // Clear screen (for Windows, use "clear" for Linux/macOS)

    printf(BLUE "~\"*^=.~\"*^- EZ-DNS -^*\"~.=^*\"~\n\n" RESET);
    printf(RED "*Always run the program as administrator*\n\n" RESET);
    printf(YELLOW "*Setting up some of the options might take a while, please be patient and do not close the app mid-process*\n\n" RESET);
    check_current_dns();
    printf("\n\nSelect the DNS number you want to use:\n\n");
    printf("[1]  - Electro          (Reducing the ping of the game and removal of sanctions)\n");
    printf("[2]  - Radar            (Reducing the ping of the game and removal of sanctions)\n");
    printf("[3]  - Shecan           (Removal of sanctions)\n");
    printf("[4]  - 403              (Removal of sanctions)\n");
    printf("[5]  - Cloudflare       (For download)\n");
    printf("[6]  - Verisign         (For download)\n");
    printf("[7]  - Yandex           (For download)\n");
    printf("[8]  - Google           (For download)\n");
    printf("[9]  - Shelter1         (Removal of sanctions and experience some games without problems)\n");
    printf("[10] - Shelter2         (Removal of sanctions and experience some games without problems)\n");
    printf("[11] - Clear all DNS\n");
    printf("[12] - Show current DNS\n");
    printf(YELLOW "\nEnter your choice:\n" RESET);

    if (scanf("%d", &mode) != 1) {
        printf(RED "\nError in input. Please enter an integer.\n" RESET);
        goto start;
    }

    switch (mode) {
        case 1:
            executer(Electro1, Electro2);
            break;
        case 2:
            executer(Radar1, Radar2);
            break;
        case 3:
            executer(Shecan1, Shecan2);
            break;
        case 4:
            executer(FourZeroThree1, FourZeroThree2);
            break;
        case 5:
            executer(Cloudflare1, Cloudflare2);
            break;
        case 6:
            executer(Verisign1, Verisign2);
            break;
        case 7:
            executer(Yandex1, Yandex2);
            break;
        case 8:
            executer(Google1, Google2);
            break;
        case 9:
            executer(ShelterOne1, ShelterOne2);
            break;
        case 10:
            executer(ShelterTwo1, ShelterTwo2);
            break;
        case 11:
            remove_dns();
            break;
        case 12:
            show_current_dns();
            break;
        default:
            printf(RED "\nPlease select your number among the options\n" RESET);
            break;
    }

    printf(YELLOW "\nPress Enter to restart the program...\n" RESET);
    getchar(); // Consume the newline left in the buffer by scanf
    getchar(); // Wait for Enter key

    goto start;

    return 0;
}
