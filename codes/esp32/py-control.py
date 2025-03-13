import pygame
import requests
import time

# ----- Configuration -----
ESP32_IP = "192.168.1.72"  # Change this to your ESP32's IP address
BASE_URL = f"http://{ESP32_IP}"
# Endpoints on the ESP32 (for example, updateMotor, updateServo, updateSpeed)
# They are expected to handle queries like: ?dir=forward, ?dir=left, etc.

# ----- Pygame Setup -----
pygame.init()
screen = pygame.display.set_mode((400, 400))
pygame.display.set_caption("ESP32 Controller")
clock = pygame.time.Clock()

# ----- Slider Setup -----
slider_x = 50
slider_y = 300
slider_width = 300
slider_height = 20
slider_value = 128  # Initial motor speed (0-255)
dragging = False

# ----- Motor Command Variables -----
motor_dir = None  # Can be "forward" or "backward"
last_motor_time = 0
motor_interval = 0.1  # Seconds between repeated motor commands

def send_request(endpoint, params=""):
    """Send an HTTP GET request to the ESP32."""
    try:
        url = f"{BASE_URL}/{endpoint}?{params}"
        requests.get(url, timeout=0.5)
    except Exception as e:
        print("Error sending request:", e)

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

        # ----- Keyboard Controls -----
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_UP:
                motor_dir = "forward"
                send_request("updateMotor", "dir=forward")
                last_motor_time = time.time()
            elif event.key == pygame.K_DOWN:
                motor_dir = "backward"
                send_request("updateMotor", "dir=backward")
                last_motor_time = time.time()
            elif event.key == pygame.K_LEFT:
                send_request("updateServo", "dir=left")
            elif event.key == pygame.K_RIGHT:
                send_request("updateServo", "dir=right")
        if event.type == pygame.KEYUP:
            if event.key in (pygame.K_UP, pygame.K_DOWN):
                send_request("updateMotor", "dir=stop")
                motor_dir = None

        # ----- Mouse Events for the Slider -----
        if event.type == pygame.MOUSEBUTTONDOWN:
            mouse_x, mouse_y = event.pos
            if (slider_x <= mouse_x <= slider_x + slider_width and
                slider_y <= mouse_y <= slider_y + slider_height):
                dragging = True
        if event.type == pygame.MOUSEBUTTONUP:
            dragging = False
        if event.type == pygame.MOUSEMOTION and dragging:
            mouse_x, _ = event.pos
            rel_x = mouse_x - slider_x
            slider_value = int((rel_x / slider_width) * 255)
            slider_value = max(0, min(255, slider_value))
            send_request("updateSpeed", f"speed={slider_value}")

    # Repeatedly send motor command if key is held down
    if motor_dir and (time.time() - last_motor_time > motor_interval):
        send_request("updateMotor", f"dir={motor_dir}")
        last_motor_time = time.time()

    # ----- Drawing the Interface -----
    screen.fill((255, 255, 255))
    
    # Draw slider background
    pygame.draw.rect(screen, (200, 200, 200), (slider_x, slider_y, slider_width, slider_height))
    # Draw slider knob
    knob_x = slider_x + (slider_value / 255) * slider_width
    knob_rect = pygame.Rect(knob_x - 10, slider_y - 5, 20, slider_height + 10)
    pygame.draw.rect(screen, (100, 100, 100), knob_rect)
    
    # Display slider value
    font = pygame.font.SysFont(None, 24)
    text = font.render(f"Speed: {slider_value}", True, (0, 0, 0))
    screen.blit(text, (slider_x, slider_y - 30))
    
    pygame.display.flip()
    clock.tick(30)

pygame.quit()
