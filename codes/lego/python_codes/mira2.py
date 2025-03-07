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
SPEED = 5  # Velocidad del movimiento de la mira

running = True

with nxt.locator.find() as brick:
    motor_x = brick.get_motor(nxt.motor.Port.A)
    motor_y = brick.get_motor(nxt.motor.Port.B)
    
    while running:
        screen.fill((0, 0, 0))  # Fondo negro
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
        
        keys = pygame.key.get_pressed()
        
        # Control con flechas y combinaciones
        if keys[pygame.K_LEFT] and keys[pygame.K_UP]:
            x -= SPEED
            y -= SPEED
            motor_x.run(80)
            motor_y.run(80)
        elif keys[pygame.K_LEFT] and keys[pygame.K_DOWN]:
            x -= SPEED
            y += SPEED
            motor_x.run(-80)
            motor_y.run(-80)
        elif keys[pygame.K_RIGHT] and keys[pygame.K_UP]:
            x += SPEED
            y -= SPEED
            motor_x.run(80)
            motor_y.run(80)
        elif keys[pygame.K_RIGHT] and keys[pygame.K_DOWN]:
            x += SPEED
            y += SPEED
            motor_x.run(80)
            motor_y.run(-80)
        elif keys[pygame.K_LEFT]:
            x -= SPEED
            motor_x.run(-80)
        elif keys[pygame.K_RIGHT]:
            x += SPEED
            motor_x.run(80)
        else:
            motor_x.brake()
        
        if keys[pygame.K_UP]:
            y -= SPEED
            motor_y.run(80)
        elif keys[pygame.K_DOWN]:
            y += SPEED
            motor_y.run(-80)
        else:
            motor_y.brake()
        
        # Limitar la mira dentro de la pantalla
        x = max(0, min(WIDTH, x))
        y = max(0, min(HEIGHT, y))
        
        # Dibujar la mira
        pygame.draw.circle(screen, (255, 0, 0), (x, y), 10)
        pygame.display.flip()
        
        pygame.time.delay(50)

pygame.quit()
