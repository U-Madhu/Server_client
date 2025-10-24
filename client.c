#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define MAX 1024
#define KEY 5  // Simple XOR key

void xor_encrypt_decrypt(char *msg) {
    for (int i = 0; i < strlen(msg); i++) {
        msg[i] ^= KEY;
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAX];
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("Socket creation error"); return -1; }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported"); return -1;
    }
    
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed"); return -1;
    }
    
    printf("Enter message: ");
    fgets(buffer, MAX, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    
    xor_encrypt_decrypt(buffer);
    send(sock, buffer, strlen(buffer), 0);
    
    char ack[MAX];
    recv(sock, ack, MAX, 0);
    xor_encrypt_decrypt(ack);
    printf("Decrypted acknowledgment from server: %s\n", ack);
    
    close(sock);
    return 0;
}
