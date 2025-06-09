#pragma once

#define BUFFER_SIZE 1024

#include <string>
#include <sys/time.h>
#include <ctime>
#include <vector>
#include <netinet/in.h>

#include <spdlog/logger.h>

namespace sock {
    class Device {
      private:
        static int id_gen_counter_;
        int device_id_;
        struct sockaddr_in device_addr_;
        std::time_t last_session_;
        char recv_buffer_[BUFFER_SIZE];

      public:
        Device(struct sockaddr_in);
        void updateLastSession();
        struct sockaddr_in getAddr();
        ~Device();

        friend std::ostream& operator<<(std::ostream& os, const Device& device);
    };

    class Server {
      private:
        long int SocketConnectionTimeout_;
        std::vector<Device> device_list_;
        struct sockaddr_in serv_addr_, device_addr_;
        int port_;
        int sockfd_;
        int count_of_devices_;
        char recv_buffer_[BUFFER_SIZE];
        std::shared_ptr<spdlog::logger> logger_;

        friend class Device;

      public:
        Server(int, int, std::shared_ptr<spdlog::logger>);

        void setTimeOut();

        void showConnectionList();
        bool deviceExit(Device);
        bool ping(Device);
        char* getTelemetry(Device);
        char* getStatus(Device);

        std::vector<bool> pingAll();
        std::vector<bool> exitAll();
        std::vector<char*> getTelemetryAll();
        ~Server();
    };
}  // namespace sock
