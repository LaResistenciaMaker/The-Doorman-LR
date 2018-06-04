/*

  Doorman 1.0 ==O== LA RESISTENCIA ==O==

  Este script va a hacer que el doorman, cuando detecte movimiento al frente suyo,
  abra los ojos y mire en distintas direcciones elegidas aleatoriamente. En el
  último movimiento, mueve los ojos lentamente hacia el centro para mirar al
  maleante que está tocando la puerta.

  Escrito por Rafael Fernández
  Última edición 19/03/2018 11:46 am.
*/

#include <Servo.h>

// Configuracion básica de pines y coordenadas.
const int pir = 8;
const int pines_servo[] = {3, 4, 5, 6, 7};
int cords_servo[] = {90, 90, 100}; // {Eje x Ojos, Y Ojos, Y Parpados}
//

bool estado_pir;
int vel_random;
Servo obj_servo[6];

void actualizarServos() {
  int cords[] = {cords_servo[0], cords_servo[1], cords_servo[0], cords_servo[1], cords_servo[2]};
  for (int x = 0; x < 5; x++) {
    if(x == 1){
      obj_servo[x].write(180 - cords[x]); // Invertir eje y en un servo.
    }
    else{
      obj_servo[x].write(cords[x]);
    }
  }
}


// Funcion que permite elegir el objeto que se movera ("ojos" o "parpado"), una coordenada x
// e y, y un tiempo total que se demorará el movimiento, en ms. Para la opcion parpado solo
// se aplica la coordenada Y entregada a la función.
void mover_servo(String tipo, int x, int y, int tiempo){
  if (tipo == "ojos") {
    Serial.println("Funcion servo llamada");
    Serial.print("X: ");
    Serial.println(x);
    Serial.print("Y: ");
    Serial.println(y);
    Serial.print("tiempo: ");
    Serial.println(tiempo);

    int ejes[] = {x, y};
    int dif[] = {abs(cords_servo[0] - x), abs(cords_servo[1] - y)};
    int espera_por_grado[] = {tiempo / dif[0], tiempo / dif[1]};

    bool terminado = false;
    long tiempos[] = {millis() + espera_por_grado[0], millis() + espera_por_grado[1]};
    while (terminado == false) {
      for (int k = 0; k < 2; k++) {
        if (millis() > tiempos[k]) {
          if (cords_servo[k] > ejes[k]) {
            cords_servo[k]--;
          }
          else if (cords_servo[k] < ejes[k]) {
            cords_servo[k]++;
          }
          actualizarServos();
          tiempos[k] = millis() + espera_por_grado[k];
        }

        if (cords_servo[0] == x && cords_servo[1] == y) {
          terminado = true;
        }
        delay(10);
      }
    }
  }
  else {
    int dif = abs(cords_servo[2] - y);
    int espera = tiempo / dif;

    while (cords_servo[2] != y) {
      if (cords_servo[2] > y) {
        cords_servo[2]--;
      }
      else {
        cords_servo[2]++;
      }
      obj_servo[4].write(cords_servo[2]);
      delay(espera);
    }
  }
}

void setup() {
  Serial.begin(9600);
  for (int x = 0; x < 5; x++) {
    obj_servo[x].attach(pines_servo[x]); // Asigna los pines a cada servo.
  }
  actualizarServos(); // Asigna las cords_servo a los respectivos servos

  pinMode(pir, INPUT);
  randomSeed(analogRead(A0));
}


void loop() {
  estado_pir = digitalRead(pir);
  if (estado_pir == HIGH) { //Sensor activado y limite tiempo alcanzado para volver a moverse
    Serial.println("Movimiento Detectado");
    
    // Abrir parapados
    Serial.println("Abriendo ojos");
    vel_random = random(500, 2501);
    mover_servo("parpado", 0, 20, vel_random); // abrir los ojos en un periodo random entre 0,5 secs y 2,5 secs

    // Mover los ojos
    int movimientos = random(3, 8); // Elegir aleatoriamente cuantos movimientos de ojos se harán (Entre 3 y 7)
    for (int u = 0; u < movimientos; u++) {
      vel_random = random(400, 801); //400, 801
      int x_rand = random(50, 121); // limites de coordenadas para mover los ojos. Van de 0 a 180. Hay que ajustarlo segun los
      int y_rand = random(50, 121); // grados maximos a los que se pueden mover los servos de los ejes de los ojos.
      mover_servo("ojos", x_rand, y_rand, vel_random);
    }

    // Devolver los ojos al centro lentamente.
    vel_random = random(800, 1501);
    mover_servo("ojos", 90, 90, vel_random);

    //cerrar los parpados
    Serial.println("Cerrando Ojos");
    vel_random = random(500, 2501);
    mover_servo("parpado", 0, 100, vel_random);
    delay(5000); // Esperar 10 segundos hasta que se vuelva a poder activar de nuevo.
  }

  delay(20); // 20 ms de espera entre cada medicion del sensor.
}

