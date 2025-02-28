import nxt.locator
from nxt.motor import *
import time

with nxt.locator.find() as brick:

    motor_a = brick.get_motor(nxt.motor.Port.A)
    initial_tacho_a = motor_a.get_tacho().tacho_count

    try:

        while True:
            print("Moviendo motor")
            motor_a.run(80)
            for _ in range(20):
                
                tacho = motor_a.get_tacho().tacho_count
                print(f"Tacho count: {tacho}")
                time.sleep(0.1)

            print("Deteniendo motor por 10 segundos..")
            motor_a.brake()
            time.sleep(30)
    except:
        print("\nStopping the motor and clossing connection..")
        motor_a.brake()
        brick.close()
