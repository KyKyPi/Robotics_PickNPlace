
#include "project.h"
#include "math.h"

int Theta1(float Angle)
{
    int Compare;
//    int min_comp = 1500;
//    int max_comp = 6500;
    int min_comp = 1800;
    int max_comp = 6500;
    int min_angle = 0;
    int max_angle = 180;
    Compare = ((max_comp - min_comp)/(max_angle - min_angle)) * (Angle - min_angle) + min_comp;
    return Compare;
}

int Theta2(float Angle)
{
    int Compare;
//    int min_comp = 1550;
//    int max_comp = 6200;
    int min_comp = 2300;
    int max_comp = 7100;
    int min_angle = 0;
    int max_angle = 180;
    Compare = ((max_comp - min_comp)/(max_angle - min_angle)) * (Angle - min_angle) + min_comp;
    return Compare;
}

void rack_pin_onoff(char position)
{
    float pos;
    if(position == 'd')
    {
        pos = 1750.0;
    }
    else{
        pos = 0.0;
    }
    float time = 0;
    while (time < 1000)
    {
        float Count = QuadDec_1_GetCounter();
        if (Count < (pos - 30))
        {
            PWM_2_WriteCompare1(70);
            PWM_2_WriteCompare2(0);
        }
        else if (Count > (pos + 30))
        {
            PWM_2_WriteCompare1(0);
            PWM_2_WriteCompare2(70);
        }
        else
        {
            PWM_2_WriteCompare1(0);
            PWM_2_WriteCompare2(0);
        }
        CyDelay(10);
//        LCD_Char_1_ClearDisplay();
//        LCD_Char_1_Position(0,0);
//        LCD_Char_1_PrintNumber(Count);
        time = time + 10;
    }
}

void rack_pin_pid(char position)
{
    float time = 0;
    int Count;
    int Target_Count1 = 0;
//    int Target_Count2 = 1500;
    int Error;
    float Kp = 0.21;
    int Speed;
    int max_speed = 80;
    uint8 val1;
    uint8 val2;
//    float pos;
    if(position == 'd')
    {
        Target_Count1 = 1900.0;
    }
    else{
        Target_Count1 = 550.0;
    }
    while (time < 2000)
    {
        Count = QuadDec_1_GetCounter();
        Error = Target_Count1 - Count;
        if(Error > 0)
        {
            Speed = Kp * Error;
            if (Speed > max_speed)
            {
                Speed = max_speed;
            }
            PWM_2_WriteCompare1(Speed);
            PWM_2_WriteCompare2(0);
        } 
        else
        {
            Speed = -Kp * Error;
            if (Speed > max_speed)
            {
                Speed = max_speed;
            }
            PWM_2_WriteCompare1(0);
            PWM_2_WriteCompare2(Speed);               
        }
        
        LCD_Char_1_ClearDisplay();
        LCD_Char_1_Position(0,0);
        LCD_Char_1_PrintNumber(Count);
        val1 = Count / 256;
        val2 = Count - (val1 * 256);
        UART_1_PutChar(val1);
        CyDelay(10);
        UART_1_PutChar(val2);
        time = time + 10;
    }
}

void angles(float T1, float T2) { //, float curr_x, float curr_y){
    PWM_1_WriteCompare1(Theta1(T1));
    PWM_1_WriteCompare2(Theta2(T2));
    LCD_Char_1_ClearDisplay();
    LCD_Char_1_Position(0,0);
    LCD_Char_1_PrintNumber(T1);
    LCD_Char_1_Position(1,0);
    LCD_Char_1_PrintNumber(T2);
}

float find_angle(float x, float y, int a) { //, float curr_x, float curr_y){
    float X = x;
    float Y = y;
    float r1 = 0.0;
    float a2 = 6.0;  // Measured in cm
    float a4 = 5.5;  // Measured in cm
    float phi1 = 0.0;
    float phi2 = 0.0;
    float phi3 = 0.0;
    float T1 = 0.0;  // T1 is theta 1 in radians
    float T2 = 0.0; // T2 is theta 2 in radians

    r1 = sqrt((X * X) + (Y * Y));  // Eq. 1
    
    phi1 = acos(((a4 * a4) - (a2 * a2) - (r1 * r1)) / (-2.0 * a2 * r1));  // Eq. 2
    
    phi2 = atan(Y / X);  // Eq. 3
    
    T1 = phi2 - phi1; // Eq. 4 - T1 in radians
    
    phi3 = acos(((r1 * r1) - (a2 * a2) - (a4 * a4)) / (-2.0 * a2 * a4));  // Eq. 5
    
    T2 = 3.14159 - phi3;
    
    if(a == 1){
        return T1;
    }
    else{
        return T2;
    }
}

void location(float x, float y) { //, float curr_x, float curr_y){
    float X = x;
    float Y = y;
    float r1 = 0.0;
    float a2 = 6.0;  // Measured in cm
    float a4 = 5.5;  // Measured in cm
    float phi1 = 0.0;
    float phi2 = 0.0;
    float phi3 = 0.0;
    float T1 = 0.0;  // T1 is theta 1 in radians
    float T2 = 0.0; // T2 is theta 2 in radians
//    
//    float incrementTheta1;
//    float Theta1dot; // Degrees per second velcoity of the first servo
//    float incrementTheta2;
//    float Theta2dot; // Degrees per second velcoity of the first servo
//    
//    float Xdot = 0.0; // cm/ s
//    float Ydot = 1.0; // cm/s

    r1 = sqrt((X * X) + (Y * Y));  // Eq. 1
    
    phi1 = acos(((a4 * a4) - (a2 * a2) - (r1 * r1)) / (-2.0 * a2 * r1));  // Eq. 2
    
    phi2 = atan(Y / X);  // Eq. 3
    
    T1 = phi2 - phi1; // Eq. 4 - T1 in radians
    
    phi3 = acos(((r1 * r1) - (a2 * a2) - (a4 * a4)) / (-2.0 * a2 * a4));  // Eq. 5
    
    T2 = 3.14159 - phi3;
    
    PWM_1_WriteCompare1(Theta1((T1 * 180.0) / 3.1415926)); // Send servo 1 to clockwise position
    PWM_1_WriteCompare2(Theta2((T2 * 180.0) / 3.1415926)); // Send servo 2 to clockwise position
//    CyDelay(2000);
    LCD_Char_1_ClearDisplay();
    LCD_Char_1_Position(0,0);
    LCD_Char_1_PrintNumber((T1 * 180.0) / 3.1415926);
    LCD_Char_1_Position(1,0);
    LCD_Char_1_PrintNumber((T2 * 180.0) / 3.1415926);
}

void location_speed(float x, float y, float curr_x, float curr_y){
    float X = x;
    float Y = y;
    float r1 = 0.0;
    float a2 = 6.0;  // Measured in cm
    float a4 = 5.5;  // Measured in cm
    float T1 = find_angle(curr_x, curr_y, 1);  // T1 is theta 1 in radians
    float T2 = find_angle(curr_x, curr_y, 2); // T2 is theta 2 in radians
    float v = 1.0; // cm/s
    
    float incrementTheta1;
    float Theta1dot; // Degrees per second velcoity of the first servo
    float incrementTheta2;
    float Theta2dot; // Degrees per second velcoity of the first servo
    
    float angle = atan(fabs(Y - curr_y) / fabs(X - curr_x));
    float Xdot = v * cos(angle);
    if(X < curr_x){
        Xdot = -Xdot;
    }
    float Ydot = v * sin(angle);
    if(Y < curr_y){
        Ydot = -Ydot;
    }
    
    float multiplier = 1.0;
    float J11inv;
    float J12inv;
    float J21inv;
    float J22inv;
    float J11;
    float J12;
    float J21;
    float J22;
    float time = 0;
    
    float t = sqrt(pow((X - curr_x), 2) + pow((Y - curr_y), 2));
    LCD_Char_1_ClearDisplay();
    LCD_Char_1_Position(0,0);
    LCD_Char_1_PrintString("Starting");
    CyDelay(2000);
    while (time < t)
    {
         // Convert to radians
        T1 = (T1 / 180.0) * 3.14159;
        T2 = (T2 / 180.0) * 3.14159;
        
        J11 = -a4 * sin(T1) * cos(T2) - a4 * cos(T1) * sin(T2) - a2  * sin(T1);
        J21 = a4 * cos(T1) * cos(T2) - a4 * sin(T1) * sin(T2) + a2 * cos(T1);
        J12 = -a4 * sin(T1) * cos(T2) - a4 * cos(T1) * sin(T2);
        J22 = a4 * cos(T1) * cos(T2) - a4 * sin(T1) * sin(T2);
        
        if(J11 * J22 - J12 * J21 != 0){
            multiplier = 1.0 / (J11 * J22 - J12 * J21);
        }
        else{
            multiplier = 1.0;
        }
        J11inv = multiplier * J22;
        J21inv = multiplier * (-J21);
        J12inv = multiplier * (-J12);
        J22inv = multiplier * J11;
        
        Theta1dot = J11inv * Xdot + J12inv * Ydot; // degrees per second
        incrementTheta1 = Theta1dot / 100.0;
        Theta2dot = J21inv * Xdot + J22inv * Ydot; // degrees per second
        incrementTheta2 = Theta2dot / 100.0;
        
            
        T1 = T1 + incrementTheta1;
        T2 = T2 + incrementTheta2;
        
        // Convert back to degrees
        T1 = (T1 / 3.14159) * 180.0;
        T2 = (T2 / 3.14159) * 180.0;
        
        
        PWM_1_WriteCompare1(Theta1(T1));
        PWM_1_WriteCompare2(Theta2(T2));
        CyDelay(10);
        time =time+0.01;
        LCD_Char_1_ClearDisplay();
        LCD_Char_1_Position(0,0);
        LCD_Char_1_PrintNumber(J21);
        LCD_Char_1_Position(1,0);
        LCD_Char_1_PrintNumber(time);
    }
    
    
//    PWM_1_WriteCompare1(Theta1((T1 * 180.0) / 3.1415926)); // Send servo 1 to clockwise position
//    PWM_1_WriteCompare2(Theta2((T2 * 180.0) / 3.1415926)); // Send servo 2 to clockwise position
//    LCD_Char_1_ClearDisplay();
//    LCD_Char_1_Position(0,0);
//    LCD_Char_1_PrintNumber((T1 * 180.0) / 3.1415926);
//    LCD_Char_1_Position(1,0);
//    LCD_Char_1_PrintNumber((T2 * 180.0) / 3.1415926);
//    }
}

int main(void)
{
    PWM_1_Start();
    PWM_2_Start();
    QuadDec_1_Start();
    LCD_Char_1_Start();
    UART_1_Start();
    
    for(;;)
    {
        uint8 Receive = 0;
        // Wait for a value to come over UART
        // Print the value on the LCD screen
        LCD_Char_1_ClearDisplay();
        LCD_Char_1_Position(0,0);
        LCD_Char_1_PrintString("Python Rec");
        CyDelay(1000);
        Receive = UART_1_GetChar();
        while (Receive == 0)
        {
            Receive = UART_1_GetChar();
        }
        float x1 = Receive / 10.0;
        
        LCD_Char_1_ClearDisplay();
        LCD_Char_1_Position(0,0);
        LCD_Char_1_PrintNumber(x1);
//        CyDelay(3000);
        
        Receive = 0;
        while (Receive == 0)
        {
            Receive = UART_1_GetChar();
        }
        float x2 = Receive / 10;
        // Print the value to the LCD screen

        LCD_Char_1_ClearDisplay();
        LCD_Char_1_Position(0,0);
        LCD_Char_1_PrintNumber(x2);
//        CyDelay(3000);
        
        float home_x = 11.0;
        float home_y = 0.0;
        float curr_x = 11.0;
        float curr_y = 0.0;
//        float X = 5.0;
//        float Y = 6.0;
        float X = x1;
        float Y = x2;
        rack_pin_pid('u');
        CyDelay(2000);
        angles(0,0);
        CyDelay(1000);
        location_speed(X, Y, curr_x, curr_y);
//        curr_x = X;
//        curr_y = Y;
        location(X, Y);
        CyDelay(2000);
        solenoid_1_Write(0);
        rack_pin_pid('d');
        CyDelay(2000);
        rack_pin_pid('u');
        CyDelay(2000);
//        location_speed(10, 0, curr_x, curr_y);
        angles(0,0);
        CyDelay(1000);
//        rack_pin_pid('d');
        solenoid_1_Write(1);
        CyDelay(2000);
    }
}

