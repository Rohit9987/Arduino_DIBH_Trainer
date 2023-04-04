// LCD
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

LCDWIKI_KBV my_lcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset

//define some colour values
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


// Distance Sensor

#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;
#define HIGH_ACCURACY

void versionText()
{
  my_lcd.Set_Text_Mode(0);
  
  my_lcd.Fill_Screen(BLACK);
  my_lcd.Set_Text_Back_colour(BLACK);
  my_lcd.Set_Text_colour(WHITE);
  my_lcd.Set_Text_Size(3);
  my_lcd.Print_String("DIBH TRAINER!", 10, 20);


  my_lcd.Set_Text_Size(2);
  my_lcd.Print_String("SQCCCCRC v1.0!", 10, 70);
}

#define GREEN_AREA_UPPER_BOUND 30
#define GREEN_AREA_LOWER_BOUND 50
void initGraph()
{
  my_lcd.Fill_Screen(BLACK);

  my_lcd.Set_Text_Mode(0);
  my_lcd.Set_Text_Back_colour(BLACK);
  my_lcd.Set_Text_colour(WHITE);
  my_lcd.Set_Text_Size(1);

  my_lcd.Print_String("0", 0, 0);
  my_lcd.Print_String("2.5", 0, 79);
  my_lcd.Print_String("5", 0, 158);
  my_lcd.Print_String("7.5", 0, 237);
  my_lcd.Print_String("10", 0, 316); 

  my_lcd.Set_Draw_color(0, 255, 255); 
  my_lcd.Draw_Fast_VLine(10, 16, my_lcd.Get_Display_Height()- 32);
  my_lcd.Draw_Fast_HLine(10, 304, my_lcd.Get_Display_Width()-12);

  my_lcd.Set_Draw_color(GREEN);

  my_lcd.Fill_Rectangle(15, GREEN_AREA_UPPER_BOUND, 480, GREEN_AREA_LOWER_BOUND);
}

void setup() 
{
  Serial.begin(9600);
  my_lcd.Init_LCD();
  my_lcd.Set_Rotation(1);
  //versionText();
  //delay(2000);

  initGraph();

  Wire.begin();
  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }

#if defined HIGH_ACCURACY
  // increase timing budget to 200 ms
  sensor.setMeasurementTimingBudget(200000);
#endif
}


#define resolution 10
#define h_offset 15
const int buffsize = 450 / resolution;  
int buff[buffsize];
#define MAXDISTANCE 100
int scale = 320 / MAXDISTANCE;

void plotGraph(int distance)
{
  for(int i = 0; i < buffsize - 1; i++)
  {
    buff[i] = buff[i+1]; 
  }
  buff[buffsize-1] = distance * scale;

  for(int i = 1; i < buffsize; i++)
  {
    // draw graphs lines
    if (buff[i-1] <= GREEN_AREA_LOWER_BOUND && buff[i-1] >= GREEN_AREA_UPPER_BOUND)
      my_lcd.Set_Draw_color(RED);
    else
      my_lcd.Set_Draw_color(WHITE);
    my_lcd.Draw_Fast_HLine(i*resolution + h_offset ,buff[i], resolution); 
    my_lcd.Draw_Fast_HLine(i*resolution + h_offset ,buff[i]-1, resolution);
    my_lcd.Draw_Fast_HLine(i*resolution + h_offset ,buff[i]-2, resolution);                

    // remove previous lines
    if (buff[i-1] <= GREEN_AREA_LOWER_BOUND && buff[i-1] >= GREEN_AREA_UPPER_BOUND)
      my_lcd.Set_Draw_color(GREEN);
    else
      my_lcd.Set_Draw_color(BLACK);
    my_lcd.Draw_Fast_HLine(i*resolution + h_offset, buff[i-1], resolution);
    my_lcd.Draw_Fast_HLine(i*resolution + h_offset, buff[i-1]-1, resolution);
    my_lcd.Draw_Fast_HLine(i*resolution + h_offset, buff[i-1]-2, resolution);
  }
}



void loop() 
{
  int distance = sensor.readRangeSingleMillimeters() - 70;  // offset 70 has to be tested
  if (distance > MAXDISTANCE) 
    distance = MAXDISTANCE - 5;
   
  plotGraph(distance);    
}
