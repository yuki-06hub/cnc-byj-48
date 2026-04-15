from machine import Pin, PWM
import time

# ==========================================
# RÉGLAGES DE LA MACHINE (À ajuster ici)
# ==========================================
DIRECTION_HOMING = -1  # 1 ou -1 selon le côté où est placé le switch
ETAT_PRESSE = 0        # 0 = Normalement Ouvert (NO), 1 = Normalement Fermé (NC)
VITESSE = 2            # Délai en ms entre chaque pas (2 = rapide, 5 = lent/précis)
MAX_STEPS_SECURITE = 5000 # Arrêt d'urgence si le switch n'est pas trouvé après X pas

# ==========================================
# CONFIGURATION DES BROCHES
# ==========================================
# Moteur
in1 = Pin(23, Pin.OUT)
in2 = Pin(19, Pin.OUT)
in3 = Pin(18, Pin.OUT)
in4 = Pin(26, Pin.OUT)

# Switch de fin de course avec résistance de tirage (Pull-up) interne
switch = Pin(2, Pin.IN, Pin.PULL_UP)

# Buzzer passif sur IO1
buzzer = PWM(Pin(0))
buzzer.duty(0) # Assure que le buzzer est silencieux au démarrage

# Séquence demi-pas
sequence = [
    [1, 0, 0, 0], [1, 1, 0, 0], [0, 1, 0, 0], [0, 1, 1, 0],
    [0, 0, 1, 0], [0, 0, 1, 1], [0, 0, 0, 1], [1, 0, 0, 1],
]

# ==========================================
# FONCTIONS DE BASE
# ==========================================
def faire_bip(frequence=2000, duree_ms=100):
    buzzer.freq(frequence)
    buzzer.duty(600) # Volume à 50%
    time.sleep_ms(duree_ms)
    buzzer.duty(0)   # Silence

def set_step(w1, w2, w3, w4):
    in1.value(w1)
    in2.value(w2)
    in3.value(w3)
    in4.value(w4)

def stopper_moteur():
    set_step(0, 0, 0, 0)

def tourner(steps, direction=1, delai=2):
    seq = sequence if direction == 1 else list(reversed(sequence))
    for _ in range(steps):
        for step in seq:
            set_step(*step)
            time.sleep_ms(delai)
    stopper_moteur()

# ==========================================
# FONCTIONS CNC (HOMING)
# ==========================================
def chercher_switch(direction, delai):
    """Avance jusqu'à toucher le switch, avec sécurité anti-forçage."""
    seq = sequence if direction == 1 else list(reversed(sequence))
    pas_effectues = 0
    
    # Tant que l'état du switch n'est PAS celui attendu (ETAT_PRESSE)
    while switch.value() != ETAT_PRESSE:
        for step in seq:
            set_step(*step)
            time.sleep_ms(delai)
            
            # Vérification très rapide (interruption de boucle si touché)
            if switch.value() == ETAT_PRESSE:
                stopper_moteur()
                return True # Succès
                
        # Sécurité pour ne pas casser le moteur
        pas_effectues += 1
        if pas_effectues >= MAX_STEPS_SECURITE:
            stopper_moteur()
            print("ERREUR CRITIQUE : Switch introuvable. Moteur arrêté.")
            faire_bip(500, 1000) # Long bip d'erreur grave
            return False # Échec

    stopper_moteur()
    return True

def auto_home():
    """Procédure de Homing professionnel (3 passes)"""
    print("Début de la procédure de Homing...")
    
    for passe in range(1, 4):
        print(f"-> Passe {passe}/3")
        
        # 1. On cherche le switch (si ça échoue, on bloque tout)
        succes = chercher_switch(DIRECTION_HOMING, delai=VITESSE)
        if not succes:
            while True: pass # Bloque le programme par sécurité
            
        # 2. On bip pour confirmer le contact
        faire_bip(2500, 100)
        
        # 3. On recule pour dégager le switch (150 pas = environ 13 degrés)
        tourner(150, direction=-DIRECTION_HOMING, delai=VITESSE)
        time.sleep_ms(200) # Pause de stabilisation mécanique
        
    print("Homing terminé avec succès !")
    faire_bip(3000, 100)
    time.sleep_ms(50)
    faire_bip(3500, 200)
    time.sleep_ms(50)
    faire_bip(3900, 120)

# ==========================================
# PROGRAMME PRINCIPAL
# ==========================================

# 1. Initialisation de l'axe
auto_home()

time.sleep(1)

# 2. La machine est à son point Zéro. Ton code d'usinage / mouvement commence ici.
print("Machine prête à travailler.")
# Exemple : Avancer de 1000 pas
# tourner(1000, direction=-DIRECTION_HOMING)
