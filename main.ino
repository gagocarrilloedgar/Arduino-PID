#include <TimerOne.h>

//maqueta 3

//pines Arduino
#define fan 9 // definim el pin d'entrada de l'heli del motor. 
#define pot 0 // salida del potenciometro 

//varibales
float posicion = 0.0;

//Controlador
float error =0.0;
float errorprev= 0.0;
float kp =1.0; // constante del controlodar parte proporcional
float ki= 0.0; //parte integral
float kd=0.0; // parte derivativa
float Integrador=0.0;
float Proporcional = 0.0; 
float Derivador = 0.0;
float suma =0.0; 

//máximos y mínimos para la posición 
float  maximo = 1.0; 
float minimo =0.0;

//Definimos la entrada y salida para variar la velocidad de nuestra hélice.
int entrada = 0.0;
float salida = 0.0; 

// cada 2 ms ejecuta esta función que lo que hace es fijar un valor de posicón a través de una inclinación
//en % dada que la convierte a un valor entre 0-1023, se multiplica pr 10.23 ya que el valor dado es en %

void fposicion(){
  posicion = analogRead(pot);
  posicion=map(posicion,minimo,maximo,0,1023);
  entrada = map(entrada,0,100,0,1023);
  
  errorprev=error;
  error =  entrada - posicion;
  suma = suma + error;

  //KP // simplemente con este controlador ya podemos hacerlo estable pero frente a una perturbación, no podria recuperar su posición. 
  if (entrada>0 && entrada<50) {kp=100;}
  if (entrada>=50 && entrada<=70) {kp=60;}
  if (entrada>70) {kp=40;}

  //KI // COn el integrador lo que conseguimos es darle estabildad al contralador, de esta manera podemos controlar cualquier tipo de perturbación del medio (hay que limitar el integrador ya que sino podria empeorar el control y no mejorarlo en ciertas situaciones.
  if (entrada>0 && entrada<50) {ki=16.875;}
  if (entrada>=50 && entrada<=70) {ki=20.25;}
  if (posicion>70) {kp=13.3875;}

  //KD
 // if (entrada>0 && entrada<50) {kd=16.875;}
  //if (entrada>=50 && entrada<=70) {kd=20.25;}
 // if (entrada>70) {kd=13.3875;}
  
  //proporcional
  Proporcional= kp*error;
  
  //integrador 
  Integrador= ki*(suma)*0.2;
  if (Integrador>900) {Integrador=900;}
  
  //Derivador
  Derivador= kd*((error-errorprev)/2000);
 
  salida = Proporcional;
  
  if(salida<0) {salida =0;}
  if(salida>1023) {salida =1023; }
  Timer1.setPwmDuty(fan, salida);
  }
  
  
void setup() {
 
  Serial.begin(9600);
  pinMode(fan,OUTPUT);

 // Calibración del brazo para poder leer el % de inclinación del brazo. 

 Serial.print("Calibrando brazo... ");
  digitalWrite(13, HIGH);
  // leemos durante 5 segundos la salida del potenciometro que será un valor entre 0-1023 en función
  // de la tensión que de. El led se encederá para indicar el calibrado. 
  while (millis() < 5000) {
   posicion = analogRead(pot);
    if (posicion> maximo)
    maximo = posicion;
    else {
      minimo=analogRead(pot);
    }
  }
  // hasta este punto hemos registrado el maximo y el mínmo angulo del brazo
  digitalWrite(13, LOW);
  Serial.print("El máximo es:");
  Serial.println(maximo);

  //setup de la lectura cada 500Hz

   Timer1.initialize(2000); // 2000 ya que el tiempo está en microsegundos
   Timer1.attachInterrupt(fposicion); // ésta función se ejecuta cada 500 hz aunque no este en el loop
   Timer1.pwm(fan,salida);
  }
void loop() {

  posicion = analogRead(pot);
  posicion= map(posicion,minimo,maximo,0,100); // utilizamos la función map para cambiar directamente del sistema en el que estamos trabajando %, para pasar a binario ( 10 bits).

  //se muestran los valores de posicón del brazo en tanto por ciento( deberia ser el valor que se le ha indicado).
  Serial.print(" Sensor : ");
  Serial.print( posicion );
  Serial.print(" ");
  Serial.println(" % ");
  delay (2000) ;
  
  //nos va pidiendo el valor de entrada de posición
   Serial.println("Introduce un valor de posicion para el brazo");
  if (Serial.available()>0){
    entrada = Serial.parseInt();
    if(entrada>100) entrada =100; 
    if (entrada<0) entrada =0;
  
}}
