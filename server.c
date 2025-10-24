#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 12345
#define MAX 1024
#define KEY 5  // Simple XOR key

void xor_encrypt_decrypt(char *msg) {
    for (int i = 0; i < strlen(msg); i++) {
        msg[i] ^= KEY;
    }
}

void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[MAX];
    
    recv(sock, buffer, MAX, 0);
    printf("Encrypted message from client: %s\n", buffer);
    
    xor_encrypt_decrypt(buffer);
    printf("Decrypted message: %s\n", buffer);
    
    char ack[] = "Message received securely!";
    xor_encrypt_decrypt(ack);
    send(sock, ack, strlen(ack), 0);
    
    close(sock);
    free(socket_desc);
    return NULL;
}

int main() {
    int server_fd, new_socket, *new_sock;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) { perror("socket failed"); exit(EXIT_FAILURE); }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address))<0) {
        perror("bind failed"); exit(EXIT_FAILURE);
    }
    
    listen(server_fd, 5);
    printf("Server listening on port %d...\n", PORT);
    
    while ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen))) {
        printf("Connected with client\n");
        pthread_t client_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;
        pthread_create(&client_thread, NULL, handle_client, (void*)new_sock);
        pthread_detach(client_thread);
    }
    
    if (new_socket < 0) { perror("accept failed"); exit(EXIT_FAILURE); }
    
    return 0;
}
