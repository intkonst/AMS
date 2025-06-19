#include <cstddef>
#include <strstream>
#include <vector>
#include <ctime>
#include <cstring>
#include <sys/time.h>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <ostream>

#include <spdlog/logger.h>

#include "udp_server.h"



namespace message_keys {
    auto NewConnection = "S0VZX05FV19DT05ORUNUSU9O";
    auto Ping = "S0VZX1BJTkc=";
    auto GetDeviceTelemetry = "S0VZX0dFVF9ERVZJQ0VfVEVMRU1FVFJZ";
    auto GetDeviceStatus = "S0VZX0dFVF9ERVZJQ0VfU1RBVFVT";
    auto StatusFailedConnection = "S0VZX1NUQVRVU19GQUlMRURfQ09OTkVDVElPTg==";
    auto StatusSuccessfulConnection = "S0VZX1NUQVRVU19TVUNDRVNTRlVMX0NPTk5FQ1RJT04=";
    auto Exit = "S0VZX0VYSVQ=";
}  // namespace message_keys

namespace sock {

    int Device::id_gen_counter_ = 0;
    
    /*
    ############################
    # DEVICE CLASS REALISATION #
    ############################
    */

    Device::Device(struct sockaddr_in device_addr)
        : device_addr_(device_addr) {
            this->device_id_ = this->id_gen_counter_;
            id_gen_counter_++;
        }

    struct sockaddr_in Device::getAddr() {
        return this->device_addr_;
    }

    void Device::updateLastSession() {
        this->last_session_ = std::time(nullptr);
    }
    

    Device::~Device() {}

  
    std::ostream& operator<<(std::ostream& os, const Device& device) { 
        char time_buf[80];
        std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", std::localtime(&device.last_session_));
        
        os << "Device ID: " << device.device_id_ << "\n"
           << "IP Address: " << inet_ntoa(device.device_addr_.sin_addr) << "\n"
           << "Port: " << ntohs(device.device_addr_.sin_port) << "\n"
           << "Last session: " << time_buf << "\n"
           << "Buffer size: " << sizeof(device.recv_buffer_) << " bytes";
        
        return os;
    }
    
    /*
    ############################
    # SERVER CLASS REALISATION #
    ############################
    */

    Server::Server(int port, int count_of_devices, std::shared_ptr<spdlog::logger> logger)
        : port_(port), count_of_devices_(count_of_devices), logger_(logger) {
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
            
        if (sockfd_ < 0) {
            logger_->error("socket creating error");
            return;
        }

        memset(&serv_addr_, 0, sizeof(serv_addr_));
        serv_addr_.sin_family = AF_INET;
        serv_addr_.sin_addr.s_addr = INADDR_ANY;
        serv_addr_.sin_port = htons(port_);

        if (bind(sockfd_, (const struct sockaddr*) &serv_addr_, sizeof(serv_addr_)) < 0) {
            logger_->error("socket bind error");

            close(sockfd_);
            return;
        }

        logger_->info("server bind to port {}", port_);

        while (count_of_devices != 0) {
            socklen_t len = sizeof(device_addr_);
            int n = recvfrom(
                sockfd_, recv_buffer_, sizeof(recv_buffer_), 0, (struct sockaddr*) &device_addr_,
                &len
            );
            recv_buffer_[n] = '\0';

            logger_->info(
                "New connection | recv: {}, ip address: {}", recv_buffer_,
                inet_ntoa(device_addr_.sin_addr)
            );

            std::string str(recv_buffer_);

            bool is_in_list = false;
            
            bool is_valid = !std::strcmp(recv_buffer_, message_keys::NewConnection);
            const char* reply;

            if (is_valid) {
                reply = message_keys::StatusSuccessfulConnection;
                logger_->info("Device {} succsessful conected", inet_ntoa(device_addr_.sin_addr));
                count_of_devices--;

                sock::Device new_device(device_addr_);
                device_list_.push_back(new_device); 

            } else {
                reply = message_keys::StatusFailedConnection;
            }

            sendto(sockfd_, reply, strlen(reply), 0, (struct sockaddr*) &device_addr_, len);
        }
        return;
    }

    void Server::setTimeOut() {
        struct timeval tv;
        tv.tv_sec = SocketConnectionTimeout_;
        tv.tv_usec = 0;
        setsockopt(sockfd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    }

    bool Server::ping(Device obj){

        //send
        setTimeOut();
        struct sockaddr_in addr = obj.getAddr(); 
        socklen_t len = sizeof(addr);
        sendto(sockfd_, message_keys::Ping, strlen(message_keys::Ping), 0,\
         (struct sockaddr*) &addr, len);

        //recv
        int n = recvfrom(
            sockfd_, recv_buffer_, sizeof(recv_buffer_), 0, (struct sockaddr*) &device_addr_,
            &len
        );
        
        if (n == -1) {
            logger_->error("Failed ping {}, device not response", inet_ntoa(device_addr_.sin_addr));
            return false;
        }

        obj.updateLastSession();

        recv_buffer_[n] = '\0';

        return !std::strcmp(recv_buffer_, message_keys::StatusSuccessfulConnection);
    }

    bool Server::deviceExit(Device obj){

        //send
        setTimeOut();
        struct sockaddr_in addr = obj.getAddr(); 
        socklen_t len = sizeof(addr);
        sendto(sockfd_, message_keys::Exit, strlen(message_keys::Exit), 0,\
         (struct sockaddr*) &addr, len);

        //recv
        int n = recvfrom(
            sockfd_, recv_buffer_, sizeof(recv_buffer_), 0, (struct sockaddr*) &device_addr_,
            &len
        );
        
        if (n == -1) {
            logger_->error("Failed exit {}, device not response", inet_ntoa(device_addr_.sin_addr));
            return false;
        }

        obj.updateLastSession();

        recv_buffer_[n] = '\0';

        return !std::strcmp(recv_buffer_, message_keys::Exit);
    }

    char* Server::getTelemetry(Device obj){
        //send
        setTimeOut();
        struct sockaddr_in addr = obj.getAddr(); 
        socklen_t len = sizeof(addr);
        sendto(sockfd_, message_keys::GetDeviceTelemetry, strlen(message_keys::GetDeviceTelemetry), 0,\
            (struct sockaddr*) &addr, len);

        //recv
        int n = recvfrom(
            sockfd_, recv_buffer_, sizeof(recv_buffer_), 0, (struct sockaddr*) &device_addr_,
            &len
        );

        if (n == -1) {
            logger_->error("Failed ping {}, device not response", inet_ntoa(device_addr_.sin_addr));
            return (char *) message_keys::StatusFailedConnection;
        }

        recv_buffer_[n] = '\0';

        obj.updateLastSession();

        return recv_buffer_;
    }

    char* Server::getStatus(Device obj){
        //send
        setTimeOut();
        struct sockaddr_in addr = obj.getAddr(); 
        socklen_t len = sizeof(addr);
        sendto(sockfd_, message_keys::GetDeviceStatus, strlen(message_keys::GetDeviceStatus), 0,\
            (struct sockaddr*) &addr, len);

        //recv
        int n = recvfrom(
            sockfd_, recv_buffer_, sizeof(recv_buffer_), 0, (struct sockaddr*) &device_addr_,
            &len
        );

        if (n == -1) {
            logger_->error("Failed ping {}, device not response", inet_ntoa(device_addr_.sin_addr));
            return (char *) message_keys::StatusFailedConnection;
        }

        recv_buffer_[n] = '\0';

        obj.updateLastSession();

        return recv_buffer_;
    }

    void Server::showConnectionList() {
        std::ostrstream out;
        out << "All connections list: \n";
        for (size_t indx=0; indx<count_of_devices_; indx++) {
            out << "\n#" << indx+1 << "\n" << device_list_[indx] << "\n";
        }
        logger_->info(out.str());
        out.clear();
    }

    std::vector<bool> Server::pingAll() {
        std::vector<bool> all_ping;
        for (size_t indx=0; indx<count_of_devices_; indx++) {
            all_ping.push_back(Server::ping(device_list_[indx]));
        }
        return all_ping;
    }

    std::vector<bool> Server::exitAll() {
        std::vector<bool> all_exit;
        for (size_t indx=0; indx<count_of_devices_; indx++) {
            all_exit.push_back(Server::deviceExit(device_list_[indx]));
        }
        return all_exit;
    }

    std::vector<char*> Server::getTelemetryAll() {
        std::vector<char*> all_telemetry;
        for (size_t indx=0; indx<count_of_devices_; indx++) {
            all_telemetry.push_back(Server::getTelemetry(device_list_[indx]));
        }
        return all_telemetry;
    }

    Server::~Server() { close(sockfd_); }

}  // namespace sock
