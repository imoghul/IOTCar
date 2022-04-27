#define ALLOW_MULTIPLE_CONNECTIONS      ("AT+CIPMUX=1\r\n")
#define START_SERVER                    ("AT+CIPSERVER=1,4764\r\n")
#define SSID_COMMAND                    ("AT+CWJAP?\r\n")
#define IP_COMMAND                      ("AT+CIFSR\r\n")
#define PING_COMMAND                    ("AT+PING=\"www.google.com\"\r\n")
#define OK_RESPONSE                     ("OK\r\n")
#define BOOT_RESPONSE                   ("WIFI GOT IP\r\n")
#define SSID_RESPONSE                   ("+CWJAP:")
#define IP_RESPONSE                     ("+CIFSR:STAIP,")
#define DISCONNECTED_RESPONSE           ("DISCONNECT")
#define SSID_RESPONSE_LEN               (7)
#define IP_RESPONSE_LEN                 (13)
#define BOOT_UP                         ('B')
#define CIPMUX_Tx                       ('M')
#define CIPMUX_Rx                       ('m')
#define CIPSERVER_Tx                    ('S')
#define CIPSERVER_Rx                    ('s')
#define IOT_SETUP_FINISHED              ('F')
#define GET_SSID_Tx                     ('N')
#define GET_SSID_Rx                     ('n')
#define GET_IP_Tx                       ('I')
#define GET_IP_Rx                       ('i')
#define SSID_LEN                        (10)
#define IP_LEN                          (13)

#define SECURITY_CODE                   ("9A73")
#define CARET_SECURITY_CODE             ("^9A73")
#define CARET_SECURITY_CODE_LEN         (5)

#define COMMAND_BUFFER_LEN              (5)

#define FORWARD_COMMAND                 ('f')
#define REVERSE_COMMAND                 ('b')
#define RIGHT_COMMAND                   ('r')
#define LEFT_COMMAND                    ('l')
#define STOP_COMMAND                    ('s')
#define LINEFOLLOW_COMMAND              ('o')
#define DISPLAY_NUMBER_COMMAND          ('d')
#define EXIT_COMMAND                    ('e')
#define TURN_CONSTANT                   (3)

typedef struct {
    char comm;
    unsigned int duration;
} command;

int Init_IOT(void);
void waitForReady(void);
void SendIOTCommand(char* command, char nextState);
void getSSID(void);
void getIP(void);
void centerStringToDisplay(unsigned int line, char * s);
void IOTBufferCommands(void);
void ProcessCommands(void);
void pushCB(command c);
void displayNetworkInfo(void);
void displayIP(int);
command popCB(void);
