#include <SD.h>
#include <SPI.h>
//#include <Streaming.h>
#include <stdint.h>
#include <TFTv2.h>
#include <SeeedTouchScreen.h>
#include <math.h>

#include "eval.h"
#include "yard.h"
#include "float.h"

void bmpdraw(File f, int x, int y);
void limpiar();
uint16_t read16(File f);
uint32_t read32(File f);

const int PIN_SD_CS = 4;                        // pin of sd card

const int __Gnbmp_height = 320;                 // bmp hight
const int __Gnbmp_width  = 240;                 // bmp width

unsigned char __Gnbmp_image_offset  = 0;        // offset

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 54   // can be a digital pin, this is A0
#define XP 57   // can be a digital pin, this is A3 

#elif defined(__AVR_ATmega32U4__) // leonardo
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 18   // can be a digital pin, this is A0
#define XP 21   // can be a digital pin, this is A3 

#else //168, 328, something else
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 14   // can be a digital pin, this is A0
#define XP 17   // can be a digital pin, this is A3 

#endif

//Measured ADC values for (0,0) and (210-1,320-1)
//TS_MINX corresponds to ADC value when X = 0
//TS_MINY corresponds to ADC value when Y = 0
//TS_MAXX corresponds to ADC value when X = 240 -1
//TS_MAXY corresponds to ADC value when Y = 320 -1

#define TS_MINX 116*2
#define TS_MAXX 890*2
#define TS_MINY 83*2
#define TS_MAXY 913*2


// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// The 2.8" TFT Touch shield has 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM);


#define numCol 4
#define numFil 5

#define X11 18
#define X12 58
#define X21 61
#define X22 101
#define X31 104
#define X32 144
#define X41 147
#define X42 187
#define X51 190
#define X52 230

#define Y11 57
#define Y12 105
#define Y21 108
#define Y22 156
#define Y31 159
#define Y32 207
#define Y41 210
#define Y42 258
#define Y51 261
#define Y52 309

int multi = 10;
int divi = 0;
boolean coma = false;
double primerOp = 0;
double actualOp = 0;
double mostrar = 0;
int decMostrar = 0;
char operacion = ' ';
boolean error = false;

 // ^ for power
 // S for sin-1 s for sin
 // [ for sqrt, ] for any root, C for cos-1, c for cos
 // T for tan-1, t for tan, $ and # TODO
 // ! for factorial, p for pi, l for ln
 // o is the switch key

char teclas_bas[5][5] = {
	{ 'o', '(', ')', '@', 'A' },
	{ '7', '8', '9', '^', '&' },
	{ '4', '5', '6', '*', '/' },
	{ '1', '2', '3', '+', '-' },
	{ '0', '.', 'a', '=', '=' },
};

char teclas_adv[5][5] = {
	{ 'o', '(', ')', '@', 'A' },
	{ 'z', 'l', 'n', 's', 'S' },
	{ 'R', 'r', '!', 'c', 'C' },
	{ 'k', 'e', 'E', 't', 'T' },
	{ 'p', '.', 'a', '=', '=' },
};

char (*teclas)[5][5] = &teclas_bas;

char eval_str[100];
int eval_str_i;

File bmpFile;
File bmpFile_adv;

#pragma GCC diagnostic ignored "-Wwrite-strings"
void setup() {
	Serial.begin(9600);

	pinMode(PIN_SD_CS, OUTPUT);
	digitalWrite(PIN_SD_CS, HIGH);

	Tft.TFTinit();

	Sd2Card card;
	card.init(SPI_FULL_SPEED, PIN_SD_CS); 

	if (!SD.begin(PIN_SD_CS)) { 
		Serial.println("failed!");
		while(1);                               // init fail, die here
	}

	Serial.println("SD OK!");

	TFT_BL_ON;

	int r = 115;                          
	
	Tft.drawCircle(119, 160, r, RED);
	Tft.drawCircle(119, 160, r-1, RED);   
	Tft.drawCircle(119, 160, r-3, BLUE);
	Tft.drawCircle(119, 160, r-4, BLUE);
	Tft.drawCircle(119, 160, r-6, GREEN);
	Tft.drawCircle(119, 160, r-7, GREEN);
	Tft.drawString("J.A.D", 50, 135, 4, BLUE);
	Tft.drawString("3000", 65, 170, 4, BLUE);
	delay(1000);
	

	Serial.println("gonna draw init");
	bmpFile = SD.open("calcu.bmp");
	bmpdraw(bmpFile, 0, 0);
	bmpFile.close();
	Serial.println("drawn init");
	//Tft.drawString("0", 22, 18, 3, BLUE);
}

int get_col_in_grid(int x) {
	int columna;
	if (x>=X11 && x<=X12) {
		columna = 1;
	}
	if (x>=X21 && x<=X22) {
		columna = 2;
	}
	if (x>=X31 && x<=X32) {
		columna = 3;
	}
	if (x>=X41 && x<=X42) {
		columna = 4;
	}
	if (x>=X51 && x<=X52) {
		columna = 5;
	}
	return columna;
}

int get_row_in_grid(int y) {
	int fila;
	if (y>=Y11 && y<=Y12) {
		fila = 1;
	}
	if (y>=Y21 && y<=Y22) {
		fila = 2;
	}
	if (y>=Y31 && y<=Y32) {
		fila = 3;
	}
	if (y>=Y41 && y<=Y42) {
		fila = 4;
	}
	if (y>=Y51 && y<=Y52) {
		fila = 5;
	}
	return fila;
}

static int dbglinei = 1;
extern "C"{
void dbg(char* a) {
	//Tft.drawString(a, 5, 10*dbglinei, 1, BLUE);
	Serial.write(a);
	Serial.print("\r\n");
	dbglinei++;
	delay(500);
}
}

void draw_eval_str() { // DOS line endings coz ino serial
	Serial.print("Eval str: ");
	Serial.print(eval_str);
	Serial.print("\r\n");
	//Serial.print("Eval str ptr: ");
	//Serial.print((int)eval_str);
	Serial.print("\r\n");
	Tft.drawString(eval_str, 22, 18, 3, BLUE);
}

bool is_digit(char c) {
	return (c >= '0' && c <= '9');
}

void loop() {
	TFT_BL_ON;
	Point p = ts.getPoint();
	int columna = 0;
	int fila = 0;
	static char tecla;
	double result;

	if (p.z > __PRESURE) {
		p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
		p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
		fila = get_row_in_grid(p.y);
		columna = get_col_in_grid(p.x);

		tecla = (*teclas)[fila-1][columna-1];
		Serial.write("pressed: ");
		Serial.print(tecla);
		Serial.write("\r\n");

		switch (tecla) {
		case 'A' :
			eval_str[0] = 0;
			eval_str_i = 0;
			limpiar();
			Serial.println("clear");
			break;
		case '@' :
			if (eval_str_i == 0)
				break;
			eval_str_i--;
			eval_str[eval_str_i] = 0;
			Serial.println("backspace");
			limpiar();
			draw_eval_str();
			break;
		case 'o' :
			if (teclas == &teclas_bas) {
				Serial.println("gonna draw 2");
				teclas = &teclas_adv;
				bmpFile = SD.open("calcu2.bmp");
				bmpdraw(bmpFile, 0, 0);
				bmpFile.close();
				Serial.println("drawn 2");
			} else {
				Serial.println("gonna draw 1");
				teclas = &teclas_bas;
				bmpFile = SD.open("calcu.bmp");
				bmpdraw(bmpFile, 0, 0);
				bmpFile.close();
				Serial.println("drawn 1");
			}
			draw_eval_str();
			break;
		case '=' :
			limpiar();
			
			result = yard(eval_str);
			fmtDouble(result, 6, eval_str, 10);
			cut_trailing_zeroes(eval_str);
			/*if (eval(eval_str, eval_str)) {
				strcpy(eval_str, "Error");
				Serial.println("Eval failed");
			} else {
				Serial.println("Eval ok");
			}
			*/
			eval_str_i = strlen(eval_str);
			draw_eval_str();
			break;
		default :
			eval_str[eval_str_i] = tecla;
			eval_str_i++;
			// Tanquem la string
			eval_str[eval_str_i] = 0;
			draw_eval_str();
		}
		// ???
		//Tft.fillRectangle(23, 10, 195, 40, BLACK);
		delay(200);
	}
}

double redondear(double dbVal, int nPlaces /* = 0 */)
{
	const double dbShift = pow(10.0, nPlaces);
	return  floor(dbVal * dbShift + 0.5) / dbShift; 
}

void limpiar() {
	Tft.fillRectangle(22, 18, 250, 30, BLACK);
}

/*********************************************/
// This procedure reads a bitmap and draws it to the screen
// its sped up by reading many pixels worth of data at a time
// instead of just one pixel at a time. increading the buffer takes
// more RAM but makes the drawing a little faster. 20 pixels' worth
// is probably a good place

#define BUFFPIXEL       20                      // must be a divisor of 240 
#define BUFFPIXEL_X3    BUFFPIXEL*3                     // BUFFPIXELx3

void bmpdraw(File f, int x, int y)
{
	bmpFile.seek(__Gnbmp_image_offset);

	uint32_t time = millis();

	uint8_t sdbuffer[BUFFPIXEL_X3];                 // 3 * pixels to buffer

	for (int i=0; i< __Gnbmp_height; i++)
	{

		for(int j=0; j<(240/BUFFPIXEL); j++)
		{
			bmpFile.read(sdbuffer, BUFFPIXEL_X3);
			uint8_t buffidx = 0;
			int offset_x = j*BUFFPIXEL;

			unsigned int __color[BUFFPIXEL];

			for(int k=0; k<BUFFPIXEL; k++)
			{
				__color[k] = sdbuffer[buffidx+2]>>3;                        // read
				__color[k] = __color[k]<<6 | (sdbuffer[buffidx+1]>>2);      // green
				__color[k] = __color[k]<<5 | (sdbuffer[buffidx+0]>>3);      // blue

				buffidx += 3;
			}

			Tft.setCol(offset_x, offset_x+BUFFPIXEL);
			Tft.setPage(i, i);
			Tft.sendCMD(0x2c);                                                  

			TFT_DC_HIGH;
			TFT_CS_LOW;

			for(int m=0; m < BUFFPIXEL; m++)
			{
				SPI.transfer(__color[m]>>8);
				SPI.transfer(__color[m]);
			}

			TFT_CS_HIGH;
		}

	}

	Serial.print(millis() - time, DEC);
	Serial.println(" ms");
}

boolean bmpReadHeader(File f) 
{
	// read header
	uint32_t tmp;
	uint8_t bmpDepth;

	if (read16(f) != 0x4D42) {
		// magic bytes missing
		return false;
	}

	// read file size
	tmp = read32(f);
	Serial.print("size 0x");
	Serial.println(tmp, HEX);

	// read and ignore creator bytes
	read32(f);

	__Gnbmp_image_offset = read32(f);
	Serial.print("offset ");
	Serial.println(__Gnbmp_image_offset, DEC);

	// read DIB header
	tmp = read32(f);
	Serial.print("header size ");
	Serial.println(tmp, DEC);


	int bmp_width = read32(f);
	int bmp_height = read32(f);

	if(bmp_width != __Gnbmp_width || bmp_height != __Gnbmp_height)      // if image is not 320x240, return false
	{
		return false;
	}

	if (read16(f) != 1)
		return false;

	bmpDepth = read16(f);
	Serial.print("bitdepth ");
	Serial.println(bmpDepth, DEC);

	if (read32(f) != 0) {
		// compression not supported!
		return false;
	}

	Serial.print("compression ");
	Serial.println(tmp, DEC);

	return true;
}

/*********************************************/
// These read data from the SD card file and convert them to big endian
// (the data is stored in little endian format!)

// LITTLE ENDIAN!
uint16_t read16(File f)
{
	uint16_t d;
	uint8_t b;
	b = f.read();
	d = f.read();
	d <<= 8;
	d |= b;
	return d;
}

// LITTLE ENDIAN!
uint32_t read32(File f)
{
	uint32_t d;
	uint16_t b;

	b = read16(f);
	d = read16(f);
	d <<= 16;
	d |= b;
	return d;
}


