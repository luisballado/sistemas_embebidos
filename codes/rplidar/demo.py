from rplidar import RPLidar

# Configura el puerto correspondiente al RPLidar (por ejemplo, /dev/ttyUSB0)
PORT_NAME = '/dev/ttyUSB0'

# Inicializa el RPLidar
lidar = RPLidar(PORT_NAME)

try:
    print("Iniciando escaneo continuo... (Ctrl+C para detener)")
    # Itera sobre los escaneos de forma indefinida
    for scan in lidar.iter_scans():
        print(scan)
except KeyboardInterrupt:
    print("Escaneo detenido por el usuario.")
except Exception as e:
    print("Error durante el escaneo:", e)
finally:
    lidar.stop()
    lidar.stop_motor()
    lidar.disconnect()
