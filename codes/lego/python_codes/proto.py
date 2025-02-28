#!/usr/bin/env python
import nxt.locator
import nxt.sensor
import time

class HiTechnicProtoBoard:
    """
    Clase para controlar el HiTechnic Prototype Board mediante comunicación I2C.
    Esta implementación es un ejemplo que puede requerir ajustes de acuerdo a la documentación
    específica de la placa.
    """
    def __init__(self, brick, port, i2c_address=0x02):
        self.brick = brick
        self.port = port
        self.i2c_address = i2c_address
        # Inicializa el sensor en modo I2C.
        self.sensor = nxt.sensor.Sensor(brick, port, nxt.sensor.Type.I2C)
        self.output_state = 0

    def set_output(self, channel, state):
        """
        Activa o desactiva la salida del canal indicado.
        
        :param channel: Número del canal a modificar (ej. 0 para el LED).
        :param state: Booleano; True para encender, False para apagar.
        """
        # Actualiza el estado del canal correspondiente
        if state:
            self.output_state |= (1 << channel)
        else:
            self.output_state &= ~(1 << channel)
        
        # Se asume que el registro de salidas es 0x41 (verificar en la documentación)
        register = 0x41
        data = [self.output_state]
        # Envía el comando I2C para actualizar la salida
        self.sensor.write(self.i2c_address, register, data)
        # Es posible que se requiera un breve retardo para que la placa procese el comando
        time.sleep(0.05)

# Ejemplo de uso
if __name__ == "__main__":
    # Conectar al brick NXT
    brick = nxt.locator.find()
    
    # Inicializar la placa en el puerto 1
    proto_board = HiTechnicProtoBoard(brick, port=1)
    
    # Encender el LED (por ejemplo, en el canal 0)
    proto_board.set_output(0, True)
    print("LED encendido")
    time.sleep(2)
    
    # Apagar el LED
    proto_board.set_output(0, False)
    print("LED apagado")
