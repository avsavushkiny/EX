/* Icon class */
class Icon
{
public:
unsigned char MyConsole[128] = { //32x32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 
  0x00, 0x31, 0x00, 0x00, 0x00, 0xC7, 0x00, 0x00, 0x00, 0x19, 0x03, 0x00, 
  0x00, 0x65, 0x0C, 0x00, 0x00, 0x9D, 0x31, 0x00, 0x00, 0x7D, 0xC6, 0x00, 
  0x00, 0xFD, 0x19, 0x01, 0x00, 0xFD, 0xE7, 0x01, 0x00, 0xFD, 0xDF, 0x01, 
  0x00, 0xFD, 0xDF, 0x01, 0x00, 0xFD, 0xDF, 0x01, 0x00, 0xF3, 0xDF, 0x01, 
  0x00, 0xCF, 0xDF, 0x01, 0x00, 0x3C, 0xDF, 0x01, 0x00, 0xF0, 0xDC, 0x01, 
  0x00, 0xC0, 0xD3, 0x01, 0x00, 0x00, 0xCF, 0x01, 0x00, 0x00, 0xFC, 0x01, 
  0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyNullApp[128] = { //32x32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0x0F, 0x10, 0x00, 0x00, 0x08, 
  0xD0, 0xFF, 0xFF, 0x0B, 0xD0, 0xFF, 0x49, 0x0A, 0xD0, 0xFF, 0x49, 0x0A, 
  0xD0, 0xFF, 0xFF, 0x0B, 0x10, 0x00, 0x00, 0x08, 0xD0, 0xFF, 0xFF, 0x0B, 
  0x50, 0x00, 0x00, 0x0A, 0x50, 0x00, 0x00, 0x0A, 0x50, 0x12, 0x09, 0x0A, 
  0x50, 0xAA, 0x0A, 0x0A, 0x50, 0xAA, 0x0A, 0x0A, 0x50, 0x12, 0x09, 0x0A, 
  0x50, 0x00, 0x00, 0x0A, 0x50, 0x00, 0x00, 0x0A, 0x50, 0x00, 0x00, 0x0A, 
  0x50, 0x00, 0x00, 0x0A, 0x50, 0x86, 0x61, 0x0A, 0x50, 0x49, 0x92, 0x0A, 
  0x50, 0x49, 0x92, 0x0A, 0xD0, 0x30, 0x0C, 0x0B, 0x10, 0x00, 0x00, 0x08, 
  0xF0, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyTaskManager[128] = { //32x32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0x0F, 0x10, 0x00, 0x00, 0x08, 
  0x10, 0x00, 0x01, 0x08, 0x10, 0x00, 0x01, 0x08, 0x10, 0xC0, 0x07, 0x08, 
  0x10, 0x80, 0x03, 0x08, 0x10, 0x00, 0x01, 0x08, 0x10, 0x00, 0x00, 0x08, 
  0x10, 0xFE, 0xFF, 0x08, 0x10, 0x02, 0x80, 0x08, 0x10, 0xDA, 0xBF, 0x0B, 
  0x10, 0xDA, 0xBF, 0x02, 0x10, 0x02, 0x80, 0x0E, 0x10, 0xFA, 0xBF, 0x0A, 
  0x10, 0x0A, 0xA0, 0x0A, 0x10, 0x0A, 0xA0, 0x0A, 0x10, 0xCA, 0xA6, 0x0A, 
  0x10, 0x3A, 0xB9, 0x0A, 0x10, 0x02, 0x80, 0x0A, 0x10, 0xFE, 0xFF, 0x0A, 
  0x10, 0x08, 0x00, 0x0A, 0x10, 0xF8, 0xFF, 0x0B, 0x10, 0x20, 0x00, 0x08, 
  0xF0, 0xEF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyUserWorkSpace[128] = { //32x32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0x03, 0x00, 0x01, 0x00, 0x04, 
  0x00, 0x6D, 0xFE, 0x05, 0x00, 0x6D, 0xFE, 0x0D, 0x00, 0x01, 0x00, 0x0C, 
  0x00, 0x01, 0x00, 0x0C, 0x00, 0x01, 0x48, 0x0C, 0xC0, 0x7F, 0x48, 0x0C, 
  0xE0, 0xFF, 0x48, 0x0C, 0xF0, 0xFF, 0x48, 0x0C, 0xF0, 0x8F, 0x30, 0x0C, 
  0xF0, 0x47, 0x00, 0x0C, 0xF0, 0x73, 0x00, 0x0C, 0xF0, 0x81, 0x00, 0x0C, 
  0xF0, 0x80, 0x01, 0x0C, 0xF0, 0xC9, 0x00, 0x0C, 0xE0, 0xF1, 0xFF, 0x0F, 
  0xE0, 0x40, 0xFF, 0x07, 0xC0, 0x38, 0x00, 0x00, 0x40, 0x10, 0x00, 0x00, 
  0xE0, 0x11, 0x00, 0x00, 0xF0, 0x33, 0x00, 0x00, 0xC0, 0x7F, 0x00, 0x00, 
  0xC0, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyADCData[128] = { //32x32
  0x80, 0xFF, 0xFF, 0x0F, 0x40, 0x00, 0x00, 0x18, 0x20, 0x00, 0x00, 0x1C, 
  0x10, 0x00, 0x00, 0x1E, 0x08, 0x00, 0x00, 0x1E, 0xC8, 0xFF, 0x7F, 0x1E, 
  0xC8, 0xFF, 0x7F, 0x1E, 0xC8, 0xFF, 0x7F, 0x1E, 0xC8, 0xFF, 0x7E, 0x1E, 
  0xC8, 0x7F, 0x7D, 0x1E, 0xC8, 0xBF, 0x7B, 0x1E, 0xC8, 0xBF, 0x7B, 0x1E, 
  0xC8, 0xDD, 0x77, 0x1E, 0xC8, 0xDD, 0x77, 0x1E, 0xC8, 0xEB, 0x7F, 0x1E, 
  0xC8, 0xF7, 0x7F, 0x1E, 0xC8, 0xFF, 0x7F, 0x1E, 0xC8, 0xFF, 0x7F, 0x1E, 
  0x08, 0x00, 0x00, 0x1E, 0x08, 0x00, 0x00, 0x7E, 0xF8, 0xFF, 0xFF, 0xDF, 
  0xF8, 0xFF, 0xFF, 0xEF, 0xFE, 0xFF, 0xFF, 0xF7, 0x01, 0x00, 0x00, 0xF8, 
  0x01, 0x00, 0x00, 0xF8, 0x01, 0x80, 0xFF, 0xF9, 0x31, 0x80, 0xFF, 0xF9, 
  0x31, 0x00, 0x20, 0xF8, 0x01, 0x00, 0x00, 0x78, 0x01, 0x00, 0x00, 0x38, 
  0xFF, 0xFF, 0xFF, 0x1F, 0xFE, 0xFF, 0xFF, 0x0F, };

unsigned char MyCalc[128] = { //32x32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xFC, 0xFF, 0xFF, 0x7F, 0x02, 0x00, 0x00, 0xC0, 0xFA, 0xFF, 0xFF, 0xFF, 
  0x3A, 0x00, 0xF0, 0xFF, 0x3A, 0x00, 0xF0, 0xFF, 0x3A, 0x00, 0xF0, 0xFF, 
  0xFA, 0xFF, 0xFF, 0xFF, 0x9A, 0x99, 0x99, 0xF9, 0x9A, 0x99, 0x99, 0xF9, 
  0xFA, 0xFF, 0xFF, 0xFF, 0xFA, 0xFF, 0xFF, 0xFF, 0x9A, 0x99, 0x99, 0xF9, 
  0x9A, 0x99, 0x99, 0xF9, 0xFA, 0xFF, 0xFF, 0xFF, 0xFA, 0xFF, 0xFF, 0xFF, 
  0x9A, 0x99, 0x19, 0xF8, 0x9A, 0x99, 0x19, 0xF8, 0xFA, 0xFF, 0xFF, 0xFF, 
  0xFA, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0x7F, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyCalendar[128] = { //32x32
  0xFC, 0xBF, 0xFF, 0x0F, 0x04, 0xC0, 0x00, 0x18, 0x06, 0xC0, 0x00, 0x68, 
  0x86, 0xC1, 0x70, 0xE8, 0xC6, 0x21, 0xF9, 0xE8, 0xC6, 0xE1, 0xD9, 0xE8, 
  0x86, 0xC1, 0xC0, 0xE8, 0x86, 0xC1, 0xE0, 0xE8, 0x86, 0xC1, 0x70, 0xE8, 
  0x86, 0xC1, 0x38, 0xE8, 0xC6, 0x23, 0xF9, 0xE8, 0xC6, 0xE3, 0xF9, 0xE8, 
  0x06, 0xC0, 0x00, 0xE8, 0x06, 0xC0, 0x00, 0xE8, 0x86, 0xFF, 0xFF, 0xE9, 
  0x46, 0x00, 0x00, 0xEA, 0x26, 0x00, 0x00, 0xEC, 0x26, 0x55, 0x01, 0xEC, 
  0x26, 0xAA, 0x02, 0xEC, 0x26, 0x00, 0x00, 0xEC, 0x3E, 0x00, 0x00, 0xEC, 
  0x2E, 0xE7, 0x7F, 0xE4, 0x3E, 0x00, 0x00, 0xFC, 0x3E, 0xE7, 0x7F, 0xFC, 
  0x3C, 0x00, 0x00, 0x7C, 0x20, 0xE7, 0x7F, 0x04, 0x20, 0x00, 0x00, 0x04, 
  0x20, 0x10, 0x08, 0x04, 0x20, 0x28, 0x14, 0x04, 0x20, 0x28, 0x14, 0x04, 
  0x20, 0x28, 0x14, 0x04, 0xE0, 0xEF, 0xF7, 0x07, };

unsigned char MyComPort[128] = { //32x32
  0x00, 0x80, 0xFF, 0x07, 0x00, 0x40, 0x55, 0x0D, 0x00, 0xC0, 0xFF, 0x0F, 
  0x00, 0xE0, 0xFF, 0x3F, 0x00, 0xD0, 0xFF, 0x6F, 0x00, 0x08, 0x00, 0x70, 
  0x00, 0x08, 0x00, 0x30, 0x00, 0xF0, 0xFF, 0x1F, 0x00, 0xE0, 0xFF, 0x1F, 
  0x00, 0x20, 0x00, 0x1C, 0x00, 0x20, 0xFF, 0x1C, 0x00, 0x20, 0x00, 0x1C, 
  0x00, 0x20, 0xFF, 0x1C, 0x00, 0x60, 0x00, 0x0C, 0x00, 0xC0, 0x00, 0x06, 
  0x00, 0x80, 0x01, 0x03, 0x00, 0x00, 0x83, 0x01, 0x00, 0x00, 0xFE, 0x00, 
  0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0xE4, 0x00, 0x00, 0x00, 0x7C, 0x00, 
  0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x00, 0x68, 0x00, 
  0xF0, 0x03, 0x68, 0x00, 0x0C, 0x1C, 0x34, 0x00, 0xF2, 0xE3, 0x3B, 0x00, 
  0xFA, 0x1F, 0x1C, 0x00, 0x1D, 0xFE, 0x0F, 0x00, 0x0D, 0xF0, 0x03, 0x00, 
  0x0D, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00, };

unsigned char MyDoc[128] = { //32x32
0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0xC0, 
  0xFD, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0x9F, 0xE4, 0xFD, 0xFF, 0x9F, 0xE4, 
  0xFD, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0xC0, 0xFD, 0xFF, 0xFF, 0xDF, 
  0x05, 0x00, 0x00, 0xC0, 0x05, 0x00, 0x00, 0xC0, 0x05, 0x00, 0x00, 0xC0, 
  0x05, 0x00, 0x00, 0xC0, 0x05, 0x60, 0x00, 0xC0, 0x05, 0x60, 0x00, 0xC0, 
  0x85, 0x67, 0xE3, 0xC1, 0x45, 0xEC, 0x36, 0xC3, 0x05, 0x6F, 0x36, 0xC0, 
  0x85, 0x6D, 0x36, 0xC0, 0xC5, 0x6C, 0x36, 0xC0, 0xC5, 0xEC, 0x36, 0xC3, 
  0x85, 0x6F, 0xE3, 0xC1, 0x05, 0x00, 0x00, 0xC0, 0x05, 0x00, 0x00, 0xC0, 
  0x05, 0x00, 0x00, 0xC0, 0x05, 0x00, 0x00, 0xC0, 0x05, 0x00, 0x00, 0xC0, 
  0x01, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyGames[128] = { //32x32
  0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x3F, 0x71, 0x00, 
  0xC0, 0xC0, 0x60, 0x00, 0x30, 0x00, 0x71, 0x00, 0x08, 0x8C, 0x61, 0x00, 
  0x04, 0xEC, 0x71, 0x00, 0x04, 0xE0, 0x61, 0x00, 0x02, 0x8C, 0x71, 0x00, 
  0x62, 0x8C, 0x61, 0x00, 0xD2, 0xC0, 0x71, 0x00, 0xC1, 0x60, 0x61, 0x00, 
  0xD1, 0x38, 0x70, 0x00, 0x61, 0x1E, 0x61, 0x00, 0x81, 0x07, 0x71, 0x00, 
  0xE1, 0x01, 0x7E, 0x00, 0x31, 0xC0, 0xF2, 0x03, 0x32, 0x10, 0x73, 0x0C, 
  0x32, 0x84, 0xF2, 0x70, 0x32, 0x81, 0xFC, 0xC6, 0x1C, 0x00, 0x61, 0xCD, 
  0x80, 0x00, 0x3E, 0xE7, 0x80, 0x00, 0x00, 0xF8, 0x80, 0x00, 0x00, 0xFE, 
  0x80, 0x00, 0x00, 0xFF, 0x80, 0x00, 0x40, 0x7B, 0x00, 0x03, 0x40, 0x3F, 
  0x00, 0x1C, 0xA0, 0x0E, 0x00, 0xE0, 0x80, 0x03, 0x00, 0x00, 0xE7, 0x00, 
  0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyGfx[128] = { //32x32
  0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0xC0, 
  0xFD, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0x9F, 0xE4, 0xFD, 0xFF, 0x9F, 0xE4, 
  0xFD, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0xC0, 0xFD, 0xFF, 0xFF, 0xDF, 
  0x05, 0x00, 0x00, 0xC0, 0x05, 0x00, 0x00, 0xC0, 0x05, 0x00, 0xF8, 0xC3, 
  0x05, 0x00, 0xFC, 0xC2, 0x05, 0x00, 0x7E, 0xC3, 0x05, 0x00, 0x81, 0xC3, 
  0x05, 0xFC, 0xFD, 0xC3, 0x05, 0xE6, 0xD7, 0xC3, 0x05, 0xD3, 0xDF, 0xC3, 
  0x05, 0xB9, 0xB7, 0xC3, 0x05, 0x69, 0xDF, 0xC3, 0x05, 0xD9, 0xBF, 0xC1, 
  0x05, 0xB9, 0xEF, 0xC0, 0x05, 0xD3, 0x7F, 0xC0, 0x05, 0xFE, 0x01, 0xC0, 
  0x05, 0xFC, 0x00, 0xC0, 0x05, 0x00, 0x00, 0xC0, 0x05, 0x00, 0x00, 0xC0, 
  0x01, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyHelp[128] = { //32x32
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0xC0, 0x0F, 0x00, 
  0x00, 0xE0, 0x3F, 0x00, 0x00, 0xF0, 0xFF, 0x00, 0x00, 0xF8, 0xF3, 0x03, 
  0x00, 0xFC, 0xC4, 0x0F, 0x00, 0xFE, 0xEF, 0x0F, 0x00, 0xFF, 0xCF, 0x07, 
  0x80, 0xFF, 0xE9, 0x07, 0xC0, 0xFF, 0xFE, 0x07, 0xE0, 0xFF, 0xFF, 0x05, 
  0xF0, 0xBF, 0xFF, 0x04, 0xF0, 0xBF, 0x7F, 0x0C, 0x30, 0xFF, 0x3F, 0x0E, 
  0x30, 0xFC, 0x1F, 0x07, 0x30, 0xF0, 0x8F, 0x03, 0xF0, 0xC1, 0xC7, 0x01, 
  0xE0, 0x07, 0xE3, 0x00, 0x80, 0x1F, 0x70, 0x00, 0x00, 0x7E, 0x38, 0x00, 
  0x00, 0xF8, 0x1D, 0x00, 0x00, 0xE0, 0x0F, 0x00, 0x00, 0x80, 0x07, 0x00, 
  0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyMessage[128] = { //32x32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xFC, 0xFF, 0xFF, 0x3F, 0x04, 0x00, 0x00, 0x34, 0x0C, 0x82, 0x20, 0x39, 
  0x5C, 0x10, 0x04, 0x3C, 0x34, 0x04, 0x81, 0x36, 0x64, 0x40, 0x50, 0x33, 
  0xC4, 0x08, 0x90, 0x31, 0x84, 0x01, 0xC1, 0x34, 0x04, 0x13, 0x74, 0x30, 
  0x14, 0x46, 0x10, 0x35, 0x84, 0x0E, 0x39, 0x30, 0x44, 0x59, 0x4C, 0x35, 
  0x84, 0xF0, 0x97, 0x30, 0x54, 0xA4, 0x45, 0x35, 0x24, 0x00, 0x10, 0x32, 
  0x14, 0x45, 0x04, 0x34, 0x4C, 0x10, 0x91, 0x38, 0x04, 0x44, 0x44, 0x34, 
  0x14, 0x11, 0x01, 0x31, 0xFC, 0xFF, 0xFF, 0x3F, 0xFC, 0xFF, 0xFF, 0x3F, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyPc[128] = { //32x32
  0x00, 0xFE, 0xFF, 0x07, 0x00, 0x01, 0x00, 0x0C, 0x80, 0x00, 0x00, 0x0E, 
  0x40, 0x00, 0x00, 0x0E, 0x40, 0xFE, 0x7F, 0x0E, 0x40, 0xFE, 0x7F, 0x0E, 
  0x40, 0xFE, 0x7F, 0x0E, 0x40, 0xFE, 0x7F, 0x0E, 0x40, 0x1E, 0x7E, 0x0E, 
  0x40, 0xDE, 0x7F, 0x0E, 0x40, 0x5E, 0x78, 0x0E, 0x40, 0x7E, 0x78, 0x0E, 
  0x40, 0x7E, 0x78, 0x0E, 0x40, 0xFE, 0x7F, 0x0E, 0x40, 0xFE, 0x7F, 0x0E, 
  0x40, 0x00, 0x00, 0x0E, 0x40, 0x00, 0x00, 0x3E, 0xC0, 0xFF, 0xFF, 0x5F, 
  0xC0, 0xFF, 0xFF, 0x6F, 0xF0, 0xFF, 0xFF, 0x77, 0x08, 0x00, 0x00, 0x78, 
  0x08, 0x00, 0x00, 0x78, 0x08, 0x80, 0xFF, 0x79, 0x88, 0x81, 0xFF, 0x79, 
  0x88, 0x01, 0x20, 0x78, 0x08, 0x00, 0x00, 0x78, 0x78, 0xDB, 0xB6, 0x3D, 
  0xB4, 0x6D, 0xDB, 0x1E, 0xFA, 0xFF, 0xFF, 0x0F, 0x01, 0x00, 0x00, 0x06, 
  0x01, 0x00, 0x00, 0x03, 0xFE, 0xFF, 0xFF, 0x01, };

unsigned char MyTablet[128] = { //32x32
  0x00, 0x70, 0x00, 0x00, 0x00, 0x88, 0x01, 0x00, 0x00, 0x38, 0x06, 0x00, 
  0x00, 0xC8, 0x18, 0x00, 0x00, 0x28, 0x63, 0x00, 0x00, 0xE8, 0x8C, 0x01, 
  0x00, 0xE8, 0x33, 0x06, 0x00, 0xE8, 0xCF, 0x08, 0x00, 0xE8, 0x3F, 0x0F, 
  0x00, 0xE8, 0xFF, 0x0E, 0x00, 0xE8, 0xFF, 0x0E, 0x00, 0xE8, 0xFF, 0x0E, 
  0x00, 0x98, 0xFF, 0x0E, 0x00, 0x7C, 0xFE, 0x0E, 0x00, 0xE2, 0xF9, 0x0E, 
  0x00, 0x91, 0xE7, 0x0E, 0x80, 0x48, 0x9E, 0x0E, 0x40, 0x24, 0x79, 0x0E, 
  0x20, 0x92, 0xE4, 0x0F, 0x10, 0x49, 0x92, 0x07, 0x88, 0x24, 0x49, 0x06, 
  0x64, 0x92, 0x24, 0x07, 0x8C, 0x49, 0x92, 0x07, 0x34, 0x26, 0xC9, 0x03, 
  0xCC, 0x98, 0xE4, 0x01, 0x30, 0x63, 0xF2, 0x00, 0xC0, 0x8C, 0x79, 0x00, 
  0x00, 0x33, 0x3C, 0x00, 0x00, 0xCC, 0x1E, 0x00, 0x00, 0x30, 0x0F, 0x00, 
  0x00, 0xC0, 0x07, 0x00, 0x00, 0x00, 0x03, 0x00, };

unsigned char MySecure[128] = { //32x32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x01, 0x00, 
  0x00, 0x02, 0x03, 0x00, 0x00, 0x71, 0x06, 0x00, 0x80, 0xF8, 0x0E, 0x00, 
  0x40, 0x98, 0x18, 0x00, 0x40, 0x9C, 0x1C, 0x00, 0x40, 0x70, 0x18, 0x00, 
  0x40, 0x04, 0x19, 0x00, 0x40, 0x20, 0x1C, 0x00, 0x80, 0x04, 0x0D, 0x00, 
  0x00, 0x21, 0x06, 0x00, 0x00, 0x82, 0x03, 0x00, 0x00, 0xA4, 0x01, 0x00, 
  0x00, 0x88, 0x01, 0x00, 0x00, 0x88, 0x01, 0x00, 0x00, 0xC4, 0x01, 0x00, 
  0x00, 0x84, 0x01, 0x00, 0x00, 0xC8, 0x01, 0x00, 0x00, 0x90, 0x01, 0x00, 
  0x00, 0xC8, 0x01, 0x00, 0x00, 0x84, 0x01, 0x00, 0x00, 0xC8, 0x01, 0x00, 
  0x00, 0x90, 0x01, 0x00, 0x00, 0x88, 0x01, 0x00, 0x00, 0xA4, 0x01, 0x00, 
  0x00, 0x84, 0x01, 0x00, 0x00, 0xC8, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 
  0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MySecure2[128] = { //32x32
  0x00, 0x1F, 0x00, 0x00, 0x80, 0x31, 0x00, 0x00, 0xF0, 0xF7, 0x01, 0x00, 
  0x88, 0x0D, 0x06, 0x00, 0xC4, 0xF5, 0x09, 0x00, 0xE2, 0xFB, 0x17, 0x00, 
  0x61, 0xBF, 0x2B, 0x00, 0x61, 0x9D, 0x27, 0x00, 0xC1, 0xFD, 0x4F, 0x00, 
  0x81, 0xFD, 0x57, 0x00, 0x41, 0xFD, 0x4B, 0x00, 0x42, 0xFD, 0x55, 0x00, 
  0x44, 0xBA, 0x4A, 0x00, 0x48, 0xFA, 0x57, 0x00, 0x50, 0xE4, 0xAF, 0x00, 
  0xA0, 0x18, 0x50, 0x01, 0x20, 0xE1, 0xA7, 0x02, 0x10, 0x03, 0x4B, 0x05, 
  0x10, 0x85, 0x91, 0x0A, 0x20, 0xCD, 0x11, 0x15, 0x40, 0x4D, 0xE1, 0x2A, 
  0x20, 0x45, 0x01, 0x55, 0x10, 0x45, 0x01, 0xA9, 0x20, 0x4D, 0x01, 0xCF, 
  0x40, 0x55, 0x01, 0xA8, 0x20, 0x4D, 0x01, 0x88, 0x20, 0x45, 0x01, 0x70, 
  0x20, 0x4E, 0x01, 0x00, 0xC0, 0x5F, 0x01, 0x00, 0x80, 0x89, 0x01, 0x00, 
  0x00, 0xF0, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, };

unsigned char MyTech[128] = { //32x32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 
  0x00, 0x42, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x60, 0x7A, 0x06, 0x00, 
  0x90, 0xD3, 0x09, 0x00, 0x10, 0x72, 0x08, 0x00, 0x30, 0x62, 0x0C, 0x00, 
  0x30, 0xD3, 0x0C, 0x00, 0x9C, 0x72, 0x39, 0x00, 0x02, 0xD3, 0x40, 0x00, 
  0x1E, 0x7E, 0x78, 0x00, 0x96, 0x00, 0x86, 0x00, 0x1C, 0xC3, 0xCC, 0x0C, 
  0xB0, 0x27, 0xF7, 0x13, 0xF0, 0x25, 0xA4, 0x10, 0xD0, 0x7E, 0xE4, 0x1C, 
  0x60, 0x6C, 0xC6, 0x19, 0x00, 0x3C, 0xA5, 0x72, 0x00, 0x84, 0xE6, 0x81, 
  0x00, 0x3C, 0xFC, 0xF4, 0x00, 0x3C, 0x00, 0xF0, 0x00, 0x38, 0xC7, 0x71, 
  0x00, 0x60, 0xCF, 0x1B, 0x00, 0xE0, 0xCB, 0x1F, 0x00, 0xA0, 0xFD, 0x16, 
  0x00, 0xC0, 0x58, 0x0C, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyTechFunc[128] = { //32x32
  0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0xC0, 
  0xFD, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0x9F, 0xE4, 0xFD, 0xFF, 0x9F, 0xE4, 
  0xFD, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0xC0, 0xFD, 0xFF, 0xFF, 0xDF, 
  0x05, 0x00, 0x00, 0xC0, 0x05, 0x00, 0x00, 0xC0, 0x05, 0x80, 0x03, 0xC0, 
  0x05, 0x40, 0x04, 0xC0, 0x05, 0x78, 0x3F, 0xC0, 0x05, 0x44, 0x4E, 0xC0, 
  0x05, 0x4C, 0x67, 0xC0, 0x05, 0x6F, 0xCE, 0xC0, 0x05, 0x65, 0x0F, 0xC1, 
  0x05, 0xCF, 0xE7, 0xC1, 0x05, 0x1F, 0xF0, 0xC1, 0x05, 0xE7, 0xCE, 0xC0, 
  0x05, 0xF4, 0x7A, 0xC0, 0x05, 0xF4, 0x7F, 0xC0, 0x05, 0x98, 0x32, 0xC0, 
  0x05, 0x80, 0x03, 0xC0, 0x05, 0x00, 0x00, 0xC0, 0x05, 0x00, 0x00, 0xC0, 
  0x01, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyTechInfo[128] = { //32x32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0xAC, 0x00, 0x00, 0x80, 0xDB, 0x01, 
  0x00, 0xC0, 0xB4, 0x02, 0x00, 0xB8, 0x6A, 0x07, 0x00, 0x0C, 0xA5, 0x0D, 
  0x80, 0x43, 0x54, 0x1F, 0xC0, 0x00, 0xD1, 0x3A, 0x38, 0x00, 0x88, 0x6A, 
  0x0C, 0x80, 0x50, 0x7D, 0x02, 0x00, 0x22, 0x7F, 0x02, 0x20, 0xA8, 0x6F, 
  0x02, 0x00, 0xE8, 0x7F, 0xE2, 0x00, 0xFA, 0x3E, 0x32, 0x01, 0xFF, 0x3F, 
  0xB4, 0xC0, 0xEF, 0x3B, 0xB8, 0xE0, 0xFF, 0x13, 0xB0, 0xF8, 0xBE, 0x03, 
  0xC0, 0xFC, 0x3F, 0x01, 0x80, 0xEC, 0x3B, 0x00, 0x80, 0xFC, 0x13, 0x00, 
  0x00, 0xBD, 0x03, 0x00, 0x00, 0x3E, 0x01, 0x00, 0x00, 0x38, 0x00, 0x00, 
  0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyTerminal[128] = { //32x32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xFE, 0xFF, 0xFF, 0xFF, 0xFA, 0xAF, 0xFA, 0xED, 0xFE, 0xFF, 0xFF, 0xFF, 
  0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xDE, 0xFF, 0xFF, 0xFF, 
  0xBE, 0xFF, 0xFF, 0xFF, 0x7E, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFF, 0xFF, 
  0x7E, 0xFF, 0xFF, 0xFF, 0xBE, 0xFF, 0xFF, 0xFF, 0xDE, 0xC3, 0xFF, 0xFF, 
  0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 
  0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 
  0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 
  0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 
  0xFE, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyExplorer[128] = { //32x32
  0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0xC0, 0x61, 0x00, 0x00, 0x38, 0xC0, 
  0x00, 0xE0, 0x7F, 0x80, 0x00, 0xFC, 0xDD, 0x81, 0x00, 0x5F, 0x6F, 0x87, 
  0x80, 0xFF, 0xDD, 0x8A, 0xC0, 0xEF, 0x6F, 0x9F, 0xE0, 0x73, 0xDB, 0x4A, 
  0xF0, 0xDD, 0xBF, 0x7D, 0xF0, 0x7E, 0x58, 0x29, 0x78, 0x37, 0xF0, 0x3A, 
  0xF8, 0x1F, 0x60, 0x35, 0xFC, 0x15, 0xE0, 0x55, 0xFC, 0xFD, 0x7F, 0x7F, 
  0x7C, 0xB7, 0xDB, 0x6A, 0xFC, 0x6F, 0xBF, 0x77, 0xBE, 0xDB, 0x7B, 0x6D, 
  0xFE, 0xF7, 0xFF, 0x7F, 0xDF, 0x1F, 0x00, 0x00, 0xEF, 0x17, 0xC0, 0x5B, 
  0xFF, 0x3F, 0x60, 0x7F, 0xFF, 0x6F, 0xF0, 0x2E, 0xF7, 0xDF, 0xAF, 0x3B, 
  0xFF, 0x7F, 0x7F, 0x17, 0xD7, 0xFF, 0xD5, 0x0E, 0x8F, 0xFF, 0xBF, 0x07, 
  0x0E, 0xFE, 0xDF, 0x01, 0x0E, 0xF8, 0x7F, 0x00, 0x1E, 0xFC, 0x0F, 0x00, 
  0xFC, 0x0F, 0x00, 0x00, 0xF0, 0x03, 0x00, 0x00, };

unsigned char MyConnect[128] = { //32x32
  0xFC, 0xFF, 0x03, 0x00, 0x02, 0x00, 0x06, 0x00, 0x01, 0x00, 0x07, 0x00, 
  0x01, 0x00, 0x07, 0x00, 0xF9, 0x3F, 0x07, 0x00, 0xF9, 0x3F, 0x07, 0x00, 
  0x59, 0x35, 0x07, 0x00, 0xB9, 0x2D, 0x07, 0x00, 0x69, 0x3B, 0x07, 0x00, 
  0xD9, 0xF6, 0xFF, 0x1F, 0xB9, 0x1D, 0x00, 0x30, 0x59, 0x0B, 0x00, 0x38, 
  0xF9, 0x0F, 0x00, 0x38, 0x01, 0xC8, 0xFF, 0x39, 0x01, 0xC8, 0xFF, 0x39, 
  0xFE, 0xCF, 0xFD, 0x39, 0xFF, 0xCF, 0xFA, 0x39, 0x01, 0xC8, 0xFD, 0x39, 
  0x01, 0xCC, 0xFF, 0x39, 0x19, 0xC8, 0xFF, 0x39, 0x19, 0xCE, 0xFF, 0x3D, 
  0x81, 0xCF, 0xFF, 0x7D, 0xFE, 0x0F, 0x00, 0xFC, 0xC0, 0x09, 0x80, 0xFF, 
  0xC0, 0xF1, 0xFF, 0xFF, 0xC0, 0xF9, 0xFF, 0xFF, 0xC0, 0x09, 0x00, 0xF0, 
  0xFF, 0x0F, 0x80, 0xFF, 0x9F, 0xCF, 0x80, 0xFF, 0xDF, 0xCF, 0x40, 0x7C, 
  0xF0, 0x09, 0xFC, 0x3F, 0x00, 0xF0, 0xFF, 0x1F, };

unsigned char MyDataPort[128] = { //32x32
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x3F, 0x00, 0x00, 0xAA, 0x6A, 0x00, 
  0x00, 0xFE, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0x01, 0x80, 0xFE, 0x7F, 0x03, 
  0x40, 0x00, 0x80, 0x03, 0x40, 0x00, 0x80, 0x01, 0x80, 0xFF, 0xFF, 0x00, 
  0x00, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0xF9, 0xE7, 0x00, 
  0x00, 0x01, 0xE0, 0x00, 0x00, 0xF9, 0xE7, 0x00, 0x00, 0x03, 0x60, 0x00, 
  0x00, 0x06, 0x30, 0x00, 0x00, 0x0C, 0x18, 0x00, 0x00, 0x18, 0x0C, 0x00, 
  0x00, 0xF0, 0x07, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x20, 0x07, 0x00, 
  0x00, 0xE0, 0x03, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x40, 0x03, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char MyWiFiClient[128] = { //32x32
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0xC0, 0x81, 0x03, 0x00, 
  0x20, 0x04, 0x04, 0x00, 0x10, 0x12, 0x08, 0x00, 0x10, 0x4A, 0x08, 0x00, 
  0x10, 0x12, 0x08, 0x00, 0x20, 0x04, 0x04, 0x00, 0xC0, 0x81, 0x03, 0x00, 
  0x00, 0x7F, 0xFE, 0x01, 0x80, 0x01, 0x01, 0x03, 0x80, 0x81, 0x80, 0x03, 
  0x80, 0x89, 0xBE, 0x03, 0x80, 0x9B, 0xBA, 0x03, 0x00, 0xBF, 0x96, 0x03, 
  0x00, 0xBE, 0xBE, 0x03, 0x00, 0x98, 0x80, 0x07, 0x00, 0x88, 0xFF, 0x05, 
  0x00, 0x40, 0x80, 0x06, 0x00, 0xC0, 0xFF, 0x07, 0x00, 0x60, 0x55, 0x09, 
  0x00, 0x10, 0x80, 0x0B, 0x00, 0xF0, 0x7F, 0x0C, 0x00, 0x00, 0x40, 0x07, 
  0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

private:
};

/*EX logo */
#define ex_width 45
#define ex_height 50
static unsigned char ex_bits[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x40, 0x91, 0x00, 0x00, 0x00, 
  0x00, 0xA8, 0xEA, 0x00, 0x00, 0x00, 0x00, 0x44, 0xE4, 0x00, 0x00, 0x00, 
  0x00, 0xAA, 0x3A, 0x00, 0xA0, 0x00, 0x00, 0x11, 0x1E, 0x00, 0x50, 0x00, 
  0xA0, 0xAA, 0x0F, 0x00, 0xA8, 0x00, 0x40, 0xC4, 0x01, 0x04, 0xC0, 0x00, 
  0xA8, 0xEA, 0x00, 0x2A, 0xA8, 0x00, 0x10, 0x39, 0x00, 0x14, 0x54, 0x00, 
  0xA8, 0x0E, 0x00, 0xAA, 0x6A, 0x00, 0x44, 0x04, 0x00, 0x44, 0x64, 0x00, 
  0xAE, 0x02, 0x28, 0xAA, 0x2A, 0x00, 0x1C, 0x01, 0x54, 0x14, 0x31, 0x00, 
  0xB8, 0x82, 0xAA, 0xA8, 0x2A, 0x00, 0x40, 0x44, 0xC4, 0x58, 0x14, 0x00, 
  0xA0, 0xAA, 0xEA, 0xB8, 0x1A, 0x00, 0x40, 0x11, 0x71, 0x30, 0x11, 0x00, 
  0xA0, 0xAA, 0x3C, 0xA0, 0x0A, 0x00, 0x40, 0x44, 0x0F, 0x40, 0x04, 0x00, 
  0xA0, 0xAA, 0x03, 0xA0, 0x2A, 0x00, 0x10, 0xE1, 0x01, 0x00, 0x11, 0x00, 
  0xA8, 0xFA, 0x80, 0xA0, 0x2A, 0x00, 0x50, 0x1C, 0x40, 0x40, 0x44, 0x00, 
  0xB8, 0x0A, 0xA8, 0xA2, 0xAA, 0x00, 0x30, 0x01, 0x14, 0x12, 0x16, 0x00, 
  0x80, 0x8A, 0xAA, 0xAB, 0xAE, 0x00, 0x40, 0x44, 0xC4, 0x41, 0x59, 0x00, 
  0x80, 0xAA, 0xFA, 0xA8, 0xB9, 0x02, 0x40, 0x11, 0x3D, 0x90, 0x31, 0x01, 
  0x80, 0xAA, 0x0E, 0xA8, 0xE0, 0x02, 0x40, 0xC4, 0x07, 0xC0, 0xC0, 0x01, 
  0xA0, 0xEA, 0x01, 0x68, 0x80, 0x03, 0x00, 0x71, 0x00, 0x78, 0x00, 0x01, 
  0xA0, 0x3A, 0x00, 0x38, 0x00, 0x00, 0x60, 0x08, 0x00, 0x00, 0x00, 0x00, 
  0xE0, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  };

/* network */
#define icon_network_1616_width 16
#define icon_network_1616_height 16
static unsigned char icon_network_1616_bits[] = {
0x00, 0x00, 0x80, 0x3F, 0xC0, 0x5F, 0xC0, 0x5D, 0xFC, 0x5D, 0xFE, 0x5E, 
  0xF6, 0x5E, 0xF6, 0x5E, 0xFE, 0x42, 0xFE, 0x7E, 0xFE, 0xDA, 0x02, 0xFE, 
  0xFE, 0x7F, 0xC1, 0x06, 0xFF, 0x07, 0xFE, 0x03, };

/* cursor */
#define cursor_w 7
#define cursor_h 10
const uint8_t cursor_bits[] = {
0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0x1F, 0x1B, 0x30, 0x30, };

/* logotype terminal */
#define trm_l_w 11
#define trm_l_h 9
const uint8_t trm_l_bits[] = {
  0xFE, 0x03, 0x01, 0x04, 0x05, 0x04, 0x09, 0x04, 0x11, 0x04, 0x09, 0x04, 
  0xC5, 0x05, 0x01, 0x04, 0xFE, 0x03, };

/* icon: icon */
#define icon_w 8
#define icon_h 8
const uint8_t icon_bits[] = {
  0xFF, 0x81, 0xBD, 0xB9, 0xBD, 0xAD, 0x81, 0xFF, };

/* shortcut */
#define shortcut_width 11
#define shortcut_height 11
static unsigned char shortcut_bits[] = {
0xFF, 0x07, 0x01, 0x04, 0xF1, 0x04, 0xE1, 0x04, 0xF1, 0x04, 0xB9, 0x04, 
  0x19, 0x04, 0x09, 0x04, 0x11, 0x04, 0x01, 0x04, 0xFF, 0x07, };

/* wait */
#define wait_width 13
#define wait_height 22
static unsigned char wait_bits[] = {
0xFF, 0x1F, 0xC3, 0x1F, 0xFE, 0x0F, 0x02, 0x0E, 0x02, 0x0E, 0x02, 0x0E, 
  0x02, 0x0E, 0x52, 0x0F, 0xA4, 0x06, 0x48, 0x03, 0x90, 0x01, 0x90, 0x01, 
  0x08, 0x03, 0x44, 0x06, 0x02, 0x0E, 0x42, 0x0E, 0xA2, 0x0E, 0x52, 0x0F, 
  0xAA, 0x0E, 0xFE, 0x0F, 0xC3, 0x1F, 0xFF, 0x1F, };