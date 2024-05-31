#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <string>
#include <vector>
#include <termios.h>

class SerialPort
{
public:
    SerialPort(const std::string &portName);
    SerialPort();
    ~SerialPort();

    bool openPort();
    void closePort();
    bool configurePort(int baudRate, int dataBits, int stopBits, char parity);

    ssize_t sendData(unsigned char *buf, int bufSize);

    ssize_t receiveData(unsigned char *buf, int bufSize);

    std::string portName;
    int baudRate;
    int dataBit;
    int stopBit;
    char parity;

private:
    int fd;
    struct termios tty;

    bool configureTermios(int baudRate, int dataBits, int stopBits, char parity);
};

#endif // SERIALPORT_H
