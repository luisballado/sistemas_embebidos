import pygame
import nxt.locator
from nxt.motor import *

# Inicializar pygame
pygame.init()

# Configuración de la pantalla
WIDTH, HEIGHT = 800, 600
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Control de Mira con NXT")

# Factor para convertir el cambio de tacho a píxeles (ajusta según necesites)
SCALE = 1

running = True

with nxt.locator.find() as brick:
    # Obtenemos los motores para cada eje
    motor_x = brick.get_motor(nxt.motor.Port.A)
    motor_y = brick.get_motor(nxt.motor.Port.B)
    
    # Guardar los valores iniciales de tacho para referencia
    initial_tacho_x = motor_x.get_tacho().tacho_count
    initial_tacho_y = motor_y.get_tacho().tacho_count
    
    while running:
        screen.fill((0, 0, 0))  # Fondo negro
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
        
        keys = pygame.key.get_pressed()
        
        # Procesar combinaciones de teclas (movimientos diagonales)
        if keys[pygame.K_LEFT] and keys[pygame.K_UP]:
            motor_x.run(80)
            motor_y.run(80)
        elif keys[pygame.K_LEFT] and keys[pygame.K_DOWN]:
            motor_x.run(-80)
            motor_y.run(-80)
        elif keys[pygame.K_RIGHT] and keys[pygame.K_UP]:
            motor_x.run(80)
            motor_y.run(80)
        elif keys[pygame.K_RIGHT] and keys[pygame.K_DOWN]:
            motor_x.run(80)
            motor_y.run(-80)
        elif keys[pygame.K_LEFT]:
            motor_x.run(-80)
        elif keys[pygame.K_RIGHT]:
            motor_x.run(80)
        else:
            motor_x.brake()
            
        # Movimientos individuales para el eje Y
        if keys[pygame.K_UP]:
            motor_y.run(80)
        elif keys[pygame.K_DOWN]:
            motor_y.run(-80)
        else:
            motor_y.brake()
        
        # Leer el valor actual de tacho y calcular el desplazamiento respecto a la posición inicial
        tacho_x = motor_x.get_tacho().tacho_count - initial_tacho_x
        tacho_y = motor_y.get_tacho().tacho_count - initial_tacho_y
        
        # Calcular la posición del círculo en la pantalla
        x = WIDTH // 2 + tacho_x * SCALE
        # Invertimos el eje Y para que el movimiento vertical sea coherente:
        y = HEIGHT // 2 - tacho_y * SCALE
        
        # Limitar la posición del círculo dentro de la pantalla
        x = max(0, min(WIDTH, x))
        y = max(0, min(HEIGHT, y))
        
        # Dibujar la mira
        pygame.draw.circle(screen, (255, 0, 0), (x, y), 10)
        pygame.display.flip()
        pygame.time.delay(50)

pygame.quit()
