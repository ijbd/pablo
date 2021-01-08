#include <Servo.h>

// pins
#define X_STEP_PIN 6
#define X_DIR_PIN 7
#define Y_STEP_PIN 3
#define Y_DIR_PIN 4
#define SERVO_PIN 9

// parameters
#define NEMA_17_DEGS_PER_STEP 1.8
#define NEMA_17_MM_PER_DEG .11111

// constraints
#define X_MIN 0.0
#define X_MAX 140.0
#define Y_MIN 0.0
#define Y_MAX 230.0
#define STEP_DELAY 1500
#define SERVO_DELAY 100

// other
#define STEP true
#define NO_STEP false
#define FORWARD true
#define REVERSE false
#define UP true
#define DOWN false
#define ON true
#define OFF false

class Linear_Axis;
class Pablone;

Servo pen;


class Pablone{
  public:
    Pablone(){
      // Wait for program start
      while(state == OFF) parseCommand();
    }

    void start_pablone(){
      state = ON;
      pen_up();
      zero();
    }

    void end_pablone(){
      state = OFF;
      pen_up();
      move_to(0,0);
    }

    void zero(){
      theta_x = 0;
      theta_y = 0;
      updatePosition();
    }

    void updatePosition(){
      x = theta_x * NEMA_17_MM_PER_DEG;
      y = theta_y * NEMA_17_MM_PER_DEG;
    }

    void pen_up(){
      pen.write(105);
      pen_state = UP;
      delay(SERVO_DELAY);
    }

    void pen_down(){
      pen.write(80);
      pen_state = DOWN;
      delay(SERVO_DELAY);
    }

    void move_step(int const step_pin, int const dir_pin, bool stp, bool dir, double &theta){
      if(stp){
        digitalWrite(dir_pin,dir);
        digitalWrite(step_pin,HIGH);
        delayMicroseconds(STEP_DELAY);
        digitalWrite(step_pin,LOW);
        if(dir) theta += NEMA_17_DEGS_PER_STEP;
        else theta -= NEMA_17_DEGS_PER_STEP;
      }
      else{
        delayMicroseconds(STEP_DELAY);
      }
    }

    void move_motors(bool always_x, bool x_dir, bool y_dir, bool * other_steps,  int num_steps){
      
      if(always_x){
        for(int i = 0; i < num_steps; i++){
          move_step(X_STEP_PIN, X_DIR_PIN, STEP, x_dir, theta_x);
          move_step(Y_STEP_PIN, Y_DIR_PIN, other_steps[i], y_dir, theta_y);
        }
      }
      else{
        for(int i = 0; i < num_steps; i++){
          move_step(X_STEP_PIN, X_DIR_PIN, other_steps[i], x_dir, theta_x);
          move_step(Y_STEP_PIN, Y_DIR_PIN, STEP, y_dir, theta_y);
        }
      }
      updatePosition();
    }

    // Bessenham Algorithm
    void plotLineLow(int x0, int y0, int x1, int y1, bool * y_steps){
      int dx = x1 - x0;
      int dy = y1 - y0;
      int yi = 1;
      
      if(dy < 0){
        yi = -1;
        dy = -dy;
      }
      
      int D = (2 * dy) - dx;
      int y_c = y0;
      
      for(int x_c = x0; x_c < x1; x_c++){
        if(D > 0){
          y_c += yi;
          D = D + (2 * (dy - dx));
          y_steps[x_c-x0] = STEP;
        }
        else{
          D = D + 2*dy;
          y_steps[x_c-x0] = NO_STEP;
        }
      }
    }

    // Bessenham Algorithm
    void plotLineHigh(int x0, int y0, int x1, int y1, bool * x_steps){
      int dx = x1 - x0;
      int dy = y1 - y0;
      int xi = 1;
      if(dx < 0){
          xi = -1;
          dx = -dx;
      }
      int D = (2 * dx) - dy;
      int x_c = x0;
  
      for(int y_c = y0; y_c < y1; y_c++){
        if(D > 0){
          x_c += xi;
          D = D + (2 * (dx - dy));
          x_steps[y_c-y0] = STEP;
        }
        else{
          D = D + 2*dx;
          x_steps[y_c-y0] = NO_STEP;
        }
      }
    }

    void move_to(double x_final, double y_final){

      double theta_x_final = min(max(x_final,X_MIN),X_MAX) / NEMA_17_MM_PER_DEG;
      double theta_y_final = min(max(y_final,Y_MIN),Y_MAX) / NEMA_17_MM_PER_DEG;

      // Switch to ticks for integer arithmetic; follow formulation of Bresenham Algorithm
      int x0 = round(theta_x / NEMA_17_DEGS_PER_STEP);
      int y0 = round(theta_y / NEMA_17_DEGS_PER_STEP);
      int x1 = round(theta_x_final / NEMA_17_DEGS_PER_STEP);
      int y1 = round(theta_y_final / NEMA_17_DEGS_PER_STEP);

      // Fill Array 
      int num_steps = max(abs(y1-y0),abs(x1-x0));
      bool * other_steps = new bool[num_steps];

      // Directions
      bool always_x = (abs(x1-x0) > abs(y1-y0));
      bool x_dir = (x1 >= x0);
      bool y_dir = (y1 >= y0);
      
      // Bessenham Algorithm
      if(abs(y1 - y0) < abs(x1 - x0)){
        if(x0 > x1){          
          plotLineLow(x1, y1, x0, y0, other_steps);
        }
        else{
          plotLineLow(x0, y0, x1, y1, other_steps);
        }
      }
      else{
        if(y0 > y1){
          plotLineHigh(x1, y1, x0, y0, other_steps);
        }
        else{
          plotLineHigh(x0, y0, x1, y1, other_steps);
        }
      }


      // Move motors
      move_motors(always_x, x_dir, y_dir, other_steps, num_steps);
      delete[] other_steps;
    }

    void circle(double x_c, double y_c, double r){
      if(pen_state == DOWN) pen_up();
      double x_final = x_c + r;
      double y_final = y_c;
      move_to(x_final, y_final);
      pen_down();
      for(int phi = 1; phi <360; phi++){
        x_final = x_c + r*cos(phi*3.14/180.0);
        y_final = y_c + r* sin(phi*3.14/180.0);
        move_to(x_final, y_final);
      }
      pen_up();
    }

    void arc(double x_c, double y_c, double r, double phi_s, double phi_e){
      if(pen_state == DOWN) pen_up();
      double x_final = x_c + r*cos(phi_s);
      double y_final = y_c + r*sin(phi_s);
      move_to(x_final, y_final);
      pen_down();
      for(int phi = phi_s+1; phi <= phi_e; phi++){
        x_final = x_c + r*cos(phi*3.14/180.0);
        y_final = y_c + r* sin(phi*3.14/180.0);
        move_to(x_final, y_final);
      }
      pen_up();
    }

    void rectangle(double x_c1, double y_c1, double x_c2, double y_c2){

      if(x_c1 > x_c2){
        double tmp = x_c1;
        x_c1 = x_c2;
        x_c2 = tmp;
      }

      if(y_c1 > y_c2){
        double tmp = y_c1;
        y_c1 = y_c2;
        y_c2 = tmp;
      }
      
      if(pen_state == DOWN) pen_up();
      move_to(x_c1,y_c1);
      pen_down();
      move_to(x_c1,y_c2);
      move_to(x_c2,y_c2);
      move_to(x_c2,y_c1);
      move_to(x_c1,y_c1);
      pen_up();
    }

    void fill_rectangle(double x_c1, double y_c1, double x_c2, double y_c2){

      if(x_c1 > x_c2){
        double tmp = x_c1;
        x_c1 = x_c2;
        x_c2 = tmp;
      }

      if(y_c1 > y_c2){
        double tmp = y_c1;
        y_c1 = y_c2;
        y_c2 = tmp;
      }
      
      if(pen_state == DOWN) pen_up();
      move_to(x_c1,y_c1);
      pen_down();
      move_to(x_c1,y_c2);
      move_to(x_c2,y_c2);
      move_to(x_c2,y_c1);
      move_to(x_c1,y_c1);

      for(int x_final = x_c1+1; x_final < x_c2; ++x_final){
        move_to(x_final,y_c1+.5);
        move_to(x_final,y_c2-.5);
        ++x_final;
        move_to(x_final,y_c2-.5);
        move_to(x_final,y_c1+.5);
      }
      
      pen_up();
    }

    void line(double x_c1, double y_c1, double x_c2, double y_c2){
      if(pen_state == DOWN) pen_up();
      move_to(x_c1,y);
      move_to(x_c1,y_c1);
      pen_down();
      move_to(x_c2,y_c2);
      pen_up();
    }

    void dot(double x_c, double y_c){
      if(pen_state == DOWN) pen_up();
      move_to(x_c,y);
      move_to(x_c,y_c);
      pen_down();
      pen_up();
    }

    void failure(){
      end_pablone();
      Serial.write((byte)'b');
      Serial.write((byte)'\n');
    }

    void parse_e(){
      end_pablone();
      Serial.write((byte)'e');
      Serial.write((byte)'\n');
    } 

    void parse_p(String &sdata){
      if(sdata.length() != 3){
        failure();
      }
      switch(sdata.charAt(1)){
        case 'u':
          pen_up();
          break;
        case 'd':
          pen_down();
          break;
        default:
          failure();
          break;
      }
    }

    void parse_m(String &sdata){
      if(sdata.charAt(1) != ' ') failure();
      int i = 2;
      while(sdata.charAt(i) != ' ' && i < sdata.length()) i++;
      if(i >= sdata.length()-1) failure();
      double x_final = sdata.substring(2,i).toDouble();
      double y_final = sdata.substring(i+1).toDouble();
      move_to(x_final,y_final);
    }

    void parse_l(String &sdata){
      if(sdata.charAt(1) != ' ') failure();

      // find origin coordinates
      int i = 2;
      while(sdata.charAt(i) != ' ' && i < sdata.length()) i++;
      int j = i+1;
      while(sdata.charAt(j) != ' ' && j < sdata.length()) j++;

      // store origin
      double x_c1 = sdata.substring(2,i).toDouble();
      double y_c1 = sdata.substring(i+1,j).toDouble();

      // find destination coordinates
      i = j+1;
      while(sdata.charAt(i) != ' ' && i < sdata.length()) i++;

      if(i >= sdata.length()-1) failure();

      // store destination
      double x_c2 = sdata.substring(j+1,i).toDouble();
      double y_c2 = sdata.substring(i+1).toDouble();
      line(x_c1, y_c1, x_c2, y_c2);
    }

    void parse_r(String &sdata){
      if(sdata.charAt(1) != ' ') failure();

      // find bottom left coordinates
      int i = 2;
      while(sdata.charAt(i) != ' ' && i < sdata.length()) i++;
      int j = i+1;
      while(sdata.charAt(j) != ' ' && j < sdata.length()) j++;

      // store origin
      double x_c1 = sdata.substring(2,i).toDouble();
      double y_c1 = sdata.substring(i+1,j).toDouble();

      // find top right coordinates
      i = j+1;
      while(sdata.charAt(i) != ' ' && i < sdata.length()) i++;

      if(i >= sdata.length()-1) failure();

      // store destination
      double x_c2 = sdata.substring(j+1,i).toDouble();
      double y_c2 = sdata.substring(i+1).toDouble();
      rectangle(x_c1,y_c1,x_c2,y_c2);
    }

    void parse_c(String &sdata){
      if(sdata.charAt(1) != ' ') failure();

      // find spaces
      int i = 2;
      while(sdata.charAt(i) != ' ' && i < sdata.length()) i++;
      int j = i+1;
      while(sdata.charAt(j) != ' ' && j < sdata.length()) j++;

      if(j >= sdata.length()-1) failure();

      // store coordinates
      double x_c = sdata.substring(2,i).toDouble();
      double y_c = sdata.substring(i+1,j).toDouble();
      double r = sdata.substring(j+1).toDouble();

      circle(x_c,y_c,r);
    }

    void parse_a(String &sdata){
      if(sdata.charAt(1) != ' ') failure();

      // find spaces
      int i = 2;
      while(sdata.charAt(i) != ' ' && i < sdata.length()) i++;
      int j = i+1;
      while(sdata.charAt(j) != ' ' && j < sdata.length()) j++;

      // store origin
      double x_c = sdata.substring(2,i).toDouble();
      double y_c = sdata.substring(i+1,j).toDouble();

      // find radius
      i = j+1;
      while(sdata.charAt(i) != ' ' && i < sdata.length()) i++;
      double r = sdata.substring(j+1,i).toDouble();

      // find start/end angles
      j = i+1;
      while(sdata.charAt(j) != ' ' && j < sdata.length()) j++;

      if(j >= sdata.length()-1) failure();

      // store angles
      double start_angle = sdata.substring(i+1,j).toDouble();
      double end_angle = sdata.substring(j+1).toDouble();

      arc(x_c, y_c, r, start_angle, end_angle);
    }

    void parse_d(String &sdata){
      if(sdata.charAt(1) != ' ') failure();
      int i = 2;
      while(sdata.charAt(i) != ' ' && i < sdata.length()) i++;
      if(i >= sdata.length()-1) failure();
      double x_final = sdata.substring(2,i).toDouble();
      double y_final = sdata.substring(i+1).toDouble();
      dot(x_final,y_final);
    }

    void parseCommand(){
      String sdata = "";
      byte ch;
      bool received = false;

      // Read in command
      while(!received){
        if(Serial.available() > 0){
          ch = Serial.read();

          sdata += (char)ch;

          if(ch == '\n'){
            received = true;   
          }
          else if(sdata.length() >= 32){
            failure();
          }
        }      
      }

      // wait for on
      if(state == OFF){
        if(sdata.charAt(0) == 's' && sdata.length() == 2){
          start_pablone();
          Serial.write((byte) 's');
          Serial.write((byte)'\n');
          return;
        }
        else{
          failure();
          return;
        }
      }

      // Switch
      switch((char) sdata.charAt(0)){
        case 'e':
          parse_e();
          break;
          
        case 'p':
          parse_p(sdata);
          break;
          
        case 'm':
          parse_m(sdata);
          break;
          
        case 'l':
          parse_l(sdata);
          break;

        case 'r':
          parse_r(sdata);
          break;
                    
        case 'c':
          parse_c(sdata);
          break;
          
        case 'a':
          parse_a(sdata);
          break;
          
        case 'd':
          parse_d(sdata);
          break;
          
        default:
          failure();
          break;         
      }

      if(state == ON){
        for(int i = 0; i < sdata.length(); i++){
          Serial.write((byte)sdata.charAt(i));
        }
      }
    }
    
  private:
    double theta_x = 0;
    double theta_y = 0;
    double x = 0;
    double y = 0;
    bool pen_state = UP;
    bool state = OFF;
};

void setup() {
  // Pins
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(Y_STEP_PIN, OUTPUT);
  pinMode(Y_DIR_PIN, OUTPUT);
  
  // Serial 
  Serial.begin(9600);
  Serial.flush();

  pen.attach(SERVO_PIN);

  // Create an Instance
  Pablone p;

  while(true){
    p.parseCommand();
  }
  
}

void loop() {
  
}
