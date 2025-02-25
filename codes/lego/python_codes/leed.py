#!/usr/bin/env python
import nxt.locator
from nxt.brick import Brick
from nxt.sensor.hitechnic_proto_board import HiTechnicProtoBoard
import time

# Conectar al brick NXT (se busca uno disponible)
brick = nxt.locator.find_one_brick()

# Inicializar el HiTechnic Prototype Board (en este ejemplo, conectado al puerto 1)
proto_board = HiTechnicProtoBoard(brick, port=1)

# Encender el LED en el canal 0
proto_board.set_output(0, True)
print("LED encendido")
time.sleep(2)

# Apagar el LED
proto_board.set_output(0, False)
print("LED apagado")
