import nxt.bluesock

bluetooth_address = "00:16:53:03:60:CF"  # Replace with your NXT's MAC address

brick = nxt.bluesock.BlueSock(bluetooth_address).connect()
print(f"Connected to {brick.get_device_info()[0]}")

brick.close()
