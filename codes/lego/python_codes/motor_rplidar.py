from rplidar import RPLidar, RPLidarException
import nxt.locator
from nxt.motor import *
import time

# Función para iniciar y reiniciar el LIDAR
def iniciar_lidar(port):
    lidar = RPLidar(port)
    lidar.start_motor()
    return lidar

# Configuración
LIDAR_PORT = '/dev/ttyUSB0'   # Reemplaza con el puerto correcto para tu RPLidar
THRESHOLD_DISTANCE = 500      # Distancia en milímetros para activar el motor

# Se busca el brick NXT usando un context manager
with nxt.locator.find() as brick:
    print(f"Conectado a NXT: {brick.get_device_info()[0]}")
    
    # Inicializa el motor en el puerto A
    motor_a = brick.get_motor(nxt.motor.Port.A)
    
    # Inicializa el RPLidar
    lidar = RPLidar(LIDAR_PORT)
    print("Iniciando LIDAR...")
    lidar.start_motor()  # Enciende el motor del LIDAR

    try:
        print("Escaneo en curso. Presiona Ctrl+C para detener.")
        while True:
            try:
                # Bucle infinito de lecturas
                for scan in lidar.iter_scans():
                    # Procesar el escaneo para obtener la distancia mínima
                    min_distance = float('inf')
                    for _, angle, distance in scan:
                        if 0 < distance < min_distance:
                            min_distance = distance
            
                    # Mostrar la distancia mínima en metros
                    print(f"Distancia mínima: {min_distance / 1000:.2f} m")
            
                    # Si se detecta un obstáculo a menos de THRESHOLD_DISTANCE, se mueve el motor
                    if min_distance < THRESHOLD_DISTANCE:
                        print("Obstáculo detectado. Moviendo motor...")
                        motor_a.run(power=80)
                        time.sleep(2)   # Mueve el motor durante 2 segundos
                        motor_a.brake() # Detiene el motor
                    else:
                        print("Área despejada. Motor detenido.")
                        motor_a.brake()
                    
                    time.sleep(0.5)  # Pausa entre lecturas
            except (RPLidarException, ValueError) as e:
                print("Error en LIDAR:", e)
                # Detener y desconectar el LIDAR
                lidar.stop_motor()
                lidar.disconnect()
                print("Reiniciando LIDAR en 2 segundos...")
                time.sleep(2)
                # Reiniciar el LIDAR
                lidar = iniciar_lidar(LIDAR_PORT)
                
    except KeyboardInterrupt:
        print("Detenido por el usuario.")
    finally:
        print("Cerrando LIDAR y conexión con NXT...")
        lidar.stop()
        lidar.stop_motor()
        lidar.disconnect()
        motor_a.brake()
        brick.close()
