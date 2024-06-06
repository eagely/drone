import os
import asyncio
from bleak import BleakClient

address = "FC:B4:67:C5:37:0E"  # Replace with the address of your Bluetooth device

async def connect_ble(address):
    client = BleakClient(address)
    await client.connect()
    print("Connected to BLE device.")
    return client

async def write_data(client, uuid, data):
    try:
        await client.write_gatt_char(uuid, data)
        print(f"Successfully wrote data '{data}' to characteristic '{uuid}'.")
        return True
    except Exception as e:
        print(f"Error occurred while writing data to characteristic '{uuid}': {e}")
        return False

async def read_data_from_pipe_and_write_to_ble(client):
    fifo_path = "data_pipe"
    with open(fifo_path, "r") as fifo:
        while True:
            input_data = fifo.readline().strip()
            if input_data:
                uuid, data = input_data.split(":")
                await write_data(client, uuid, data.encode())

async def main():
    client = await connect_ble(address)
    await read_data_from_pipe_and_write_to_ble(client)

if __name__ == "__main__":
    asyncio.run(main())
