import pygame
import nxt.locator
from nxt.motor import *

# Inicializar pygame
pygame.init()

# Configuración de la pantalla
WIDTH, HEIGHT = 800, 600
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Control de Mira con NXT")

# Posición de la mira
x, y = WIDTH // 2, HEIGHT // 2
SPEED = 10  # Velocidad del movimiento de la mira

running = True

with nxt.locator.find() as brick:
    motor = brick.get_motor(nxt.motor.Port.A)
    
    while running:
        screen.fill((0, 0, 0))  # Fondo negro
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
        
        keys = pygame.key.get_pressed()
        
        # Control con flechas
        if keys[pygame.K_LEFT]:
            x -= SPEED
            motor.run(-100)  # Mueve motor en dirección izquierda
        elif keys[pygame.K_RIGHT]:
            x += SPEED
            motor.run(100)  # Mueve motor en dirección derecha
        else:
            motor.brake()
        
        # Limitar la mira dentro de la pantalla
        x = max(0, min(WIDTH, x))
        
        # Dibujar la mira
        pygame.draw.circle(screen, (255, 0, 0), (x, y), 10)
        pygame.display.flip()
        
        pygame.time.delay(50)

pygame.quit()
