from machine import Pin
import time

# Définition des pins
in1 = Pin(23, Pin.OUT)
in2 = Pin(19, Pin.OUT)
in3 = Pin(18, Pin.OUT)
in4 = Pin(26, Pin.OUT)

# Séquence demi-pas (8 étapes) - plus doux et précis
sequence = [
    [1, 0, 0, 0],
    [1, 1, 0, 0],
    [0, 1, 0, 0],
    [0, 1, 1, 0],
    [0, 0, 1, 0],
    [0, 0, 1, 1],
    [0, 0, 0, 1],
    [1, 0, 0, 1],
]

def set_step(w1, w2, w3, w4):
    in1.value(w1)
    in2.value(w2)
    in3.value(w3)
    in4.value(w4)

def stopper():
    set_step(0, 0, 0, 0)  # coupe le courant dans les bobines

def tourner(steps, direction=1, delai=2):
    """
    steps     : nombre de pas
    direction : 1 = sens horaire, -1 = sens anti-horaire
    delai     : vitesse en ms (min ~2ms, plus grand = plus lent)
    """
    seq = sequence if direction == 1 else list(reversed(sequence))
    
    for _ in range(steps):
        for step in seq:
            set_step(*step)
            time.sleep_ms(delai)
    
    stopper()

def tour_complet(nb_tours=1, direction=1, delai=2):
    """
    1 tour complet = 4096 demi-pas pour le 28BYJ-48
    """
    steps = 4096 * nb_tours
    tourner(steps, direction, delai)

# ─── Programme principal ───

print("Moteur prêt !")

# 1 tour dans le sens horaire
print("1 tour sens horaire...")
tour_complet(1, direction=1)
time.sleep(1)

# 1 tour dans le sens anti-horaire
print("1 tour sens anti-horaire...")
tour_complet(1, direction=-1)
time.sleep(1)

# Tourne en continu (retire le commentaire si tu veux)
# while True:
#     tour_complet(1, direction=1)
#     time.sleep(0.5)
#     tour_complet(1, direction=-1)
#     time.sleep(0.5)