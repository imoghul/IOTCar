#define BASE_10         (10)
#define HEX_BCD_BASES   {1000,100,10}
#define HEX_BCD_SIZE    (3)
void centerStringToDisplay(unsigned int line, char * s);
char* subStringPos(const char* str, char * subString);
char* charInString(const char* str, char c);
int stoi(char* str, int len) ;
unsigned int absVal(int n) ;
void HEXtoBCD(int hex_value, int line, int start);
int additionSafe(int val, int max, int min, int increment);
int multSafe(int a, int b);