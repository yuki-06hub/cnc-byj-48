/*
 * CONVERSION ARDUINO : Procédure de Homing pour Moteur Pas-à-Pas
 */

// ==========================================
// RÉGLAGES DE LA MACHINE
// ==========================================
const int DIRECTION_HOMING = -1;    // 1 ou -1
const int ETAT_PRESSE = LOW;        // LOW = Tirage interne (PULLUP) activé
const int VITESSE = 2;              // Délai en ms entre chaque pas
const int MAX_STEPS_SECURITE = 5000; 

// ==========================================
// CONFIGURATION DES BROCHES
// ==========================================
const int IN1 = 23;
const int IN2 = 19;
const int IN3 = 18;
const int IN4 = 26;
const int SWITCH_PIN = 2;
const int BUZZER_PIN = 0;

// Séquence demi-pas (8 étapes)
const int sequence[8][4] = {
  {1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0},
  {0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 1}, {1, 0, 0, 1}
};

// ==========================================
// FONCTIONS DE BASE
// ==========================================

void faireBip(int frequence = 2000, int duree_ms = 100) {
  tone(BUZZER_PIN, frequence, duree_ms);
  delay(duree_ms);
  noTone(BUZZER_PIN);
}

void setStep(int w1, int w2, int w3, int w4) {
  digitalWrite(IN1, w1);
  digitalWrite(IN2, w2);
  digitalWrite(IN3, w3);
  digitalWrite(IN4, w4);
}

void stopperMoteur() {
  setStep(0, 0, 0, 0);
}

void tourner(int steps, int direction, int delai) {
  for (int i = 0; i < steps; i++) {
    for (int s = 0; s < 8; s++) {
      // Si direction = -1, on parcourt la séquence à l'envers
      int stepIdx = (direction == 1) ? s : (7 - s);
      setStep(sequence[stepIdx][0], sequence[stepIdx][1], 
              sequence[stepIdx][2], sequence[stepIdx][3]);
      delay(delai);
    }
  }
  stopperMoteur();
}

// ==========================================
// FONCTION CHERCHER SWITCH
// ==========================================

bool chercherSwitch(int direction, int delai) {
  int pas_effectues = 0;
  
  // On boucle tant que le switch n'est pas pressé
  while (digitalRead(SWITCH_PIN) != ETAT_PRESSE) {
    for (int s = 0; s < 8; s++) {
      int stepIdx = (direction == 1) ? s : (7 - s);
      setStep(sequence[stepIdx][0], sequence[stepIdx][1], 
              sequence[stepIdx][2], sequence[stepIdx][3]);
      delay(delai);

      // Vérification immédiate après chaque micro-pas
      if (digitalRead(SWITCH_PIN) == ETAT_PRESSE) {
        stopperMoteur();
        return true;
      }
    }
    
    pas_effectues++;
    if (pas_effectues >= MAX_STEPS_SECURITE) {
      stopperMoteur();
      Serial.println("ERREUR CRITIQUE : Switch introuvable.");
      faireBip(500, 1000);
      return false;
    }
  }
  stopperMoteur();
  return true;
}

// ==========================================
// AUTO HOME
// ==========================================

void autoHome() {
  Serial.println("Début de la procédure de Homing...");
  
  for (int passe = 1; passe <= 3; passe++) {
    Serial.print("-> Passe "); Serial.print(passe); Serial.println("/3");
    
    bool succes = chercherSwitch(DIRECTION_HOMING, VITESSE);
    
    if (!succes) {
      while (true); // Bloque le programme si erreur
    }
    
    faireBip(2500, 100);
    
    // Reculer de 150 pas
    tourner(150, -DIRECTION_HOMING, VITESSE);
    delay(200); 
  }
  
  Serial.println("Homing terminé avec succès !");
  faireBip(3000, 100); delay(50);
  faireBip(3500, 200); delay(50);
  faireBip(3900, 120);
}

// ==========================================
// SETUP & LOOP
// ==========================================

void setup() {
  Serial.begin(115200);
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // INPUT_PULLUP active la résistance interne (pour éviter les parasites)
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  // Initialisation de l'axe
  autoHome();
  
  delay(1000);
  Serial.println("Machine prête à travailler.");
}

void loop() {
  // Ton code d'usinage ici
  // Exemple : tourner(1000, -DIRECTION_HOMING, VITESSE);
  // delay(5000);
}
