void sendTestUDP(char* serializedPacket) {
    int sockfd;
    struct sockaddr_in client_addr, server_addr;

    // Create a socket for UDP communication
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(CLIENT_PORT);
    client_addr.sin_addr.s_addr = INADDR_ANY;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, "192.168.007.002", &(server_addr.sin_addr)) <= 0) {
        perror("Invalid IP address\r\n");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_port = htons(SERVER_PORT);

    // Example message to be sent
    if (bind(sockfd, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1) {
        perror("Bind failed\r\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    clock_t start_time = clock();
    time_t current_time = time(NULL);
    struct tm* local_time = localtime(&current_time);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

    if (sendto(sockfd, (const char*)serializedPacket, strlen(serializedPacket), 0, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        perror("Sendto failed\r\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP packet send successfully!\r\n");
    size_t bytes_received = 0;
    char receivedPacket[5] = {0};

    if (bytes_received = recfrom(sockfd, receivedPacket, sizeof(receivedPacket), 0, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        perror("Receiving failed!\r\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Send the message to the server
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC);
    printf("UDP packet received successfully!\r\n");
    packet_to_client received_packet = {0};
    memcpy(received_packet.TestID, receivedPacket, 4);
    memcpy(received_packet.TestResult, receivedPacket[4], 1);
    char test_result[10] = {0};
    (received_packet.Test == 0x01U) ? strcpy(test_results, "Success", 8) : strcpy(test_results, "Failure", 8);

    // Take input and put it in a file
    FILE* file;
    if ((file = fopen("Test_Results.txt", "a")) == NULL) {
        perror("Error opening the file!");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "Date: [%s], Time Taken: %.6f seconds, Test ID: %d, Result: %s \r\n", time_str, elapsed_time, received_packet.TestID, test_result);

    fclose(file);
    // Close the socket
    close(sockfd);
}
