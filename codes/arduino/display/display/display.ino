int a=22;
int b=21;
int c=2;
int d=4;
int e=5;
int f=19;
int g=18;
void setup() {
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
}

void loop() {
 // for 0
 digitalWrite(a,1);
 digitalWrite(b,0);
 digitalWrite(c,0);
 digitalWrite(d,0);
 digitalWrite(e,0);
 digitalWrite(f,0);
 digitalWrite(g,0);
 delay(150);
 digitalWrite(a,0);
 digitalWrite(b,1);
 digitalWrite(c,0);
 digitalWrite(d,0);
 digitalWrite(e,0);
 digitalWrite(f,0);
 digitalWrite(g,0);
 delay(150);
  digitalWrite(a,0);
 digitalWrite(b,0);
 digitalWrite(c,1);
 digitalWrite(d,0);
 digitalWrite(e,0);
 digitalWrite(f,0);
 digitalWrite(g,0);
 delay(150); 
 digitalWrite(a,0);
 digitalWrite(b,0);
 digitalWrite(c,0);
 digitalWrite(d,1);
 digitalWrite(e,0);
 digitalWrite(f,0);
 digitalWrite(g,0);
 delay(150); 
 digitalWrite(a,0);
 digitalWrite(b,0);
 digitalWrite(c,0);
 digitalWrite(d,0);
 digitalWrite(e,1);
 digitalWrite(f,0);
 digitalWrite(g,0);
 delay(150); 
 digitalWrite(a,0);
 digitalWrite(b,0);
 digitalWrite(c,0);
 digitalWrite(d,0);
 digitalWrite(e,0);
 digitalWrite(f,1);
 digitalWrite(g,0);
 delay(150);
 }
