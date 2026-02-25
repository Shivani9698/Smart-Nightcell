# #!/usr/bin/env python3

# import serial
# import time
# import random
# import struct
# import logging
# from dataclasses import dataclass, field
# from typing import Optional, Dict, Any, Callable
# from enum import Enum
# import threading
# from queue import Queue, Empty

# # Configure logging
# logging.basicConfig(
#     level=logging.INFO,
#     format='%(asctime)s - %(levelname)s - %(message)s'
# )
# logger = logging.getLogger(__name__)


# class CommandCode(Enum):
#     SET_CONFIG = 0xB1
#     GET_CONFIG = 0xB2
#     CONFIGURE_APN = 0xB3
#     REBOOT = 0xB4
#     RESET_TO_FACTORY_DEFAULTS = 0xB5
#     TURN_OFF_EDGE_DEVICE = 0xB6
#     SET_LORAWAN_CONFIG = 0xB7

#     SET_CONFIG_ACK = 0xA3
#     GET_CONFIG_ACK = 0xA4
#     CONFIGURE_APN_ACK = 0xA5
#     REBOOT_ACK = 0xA6
#     RESET_TO_FACTORY_DEFAULTS_ACK = 0xA7
#     TURN_OFF_EDGE_DEVICE_ACK = 0xA8
#     SET_LORAWAN_CONFIG_ACK = 0xAA


# # Mapping from downlink command to expected ACK
# DOWNLINK_TO_ACK = {
#     CommandCode.SET_CONFIG: CommandCode.SET_CONFIG_ACK,
#     CommandCode.GET_CONFIG: CommandCode.GET_CONFIG_ACK,
#     CommandCode.CONFIGURE_APN: CommandCode.CONFIGURE_APN_ACK,
#     CommandCode.REBOOT: CommandCode.REBOOT_ACK,
#     CommandCode.RESET_TO_FACTORY_DEFAULTS: CommandCode.RESET_TO_FACTORY_DEFAULTS_ACK,
#     CommandCode.TURN_OFF_EDGE_DEVICE: CommandCode.TURN_OFF_EDGE_DEVICE_ACK,
#     CommandCode.SET_LORAWAN_CONFIG: CommandCode.SET_LORAWAN_CONFIG_ACK,
# }

# START_BYTE = 0xF9
# DOWNLINK_DIR = 0x02
# UPLINK_DIR = 0x01
# TERMINATOR = 0x0A

# MIN_RETRY_DELAY = 30  # seconds
# MAX_RETRY_DELAY = 120  # seconds


# @dataclass
# class DownlinkPacket:
#     """Represents a downlink packet to send"""
#     command: CommandCode
#     device_id: bytes  # 8 bytes
#     params: Dict[str, Any] = field(default_factory=dict)
#     raw_payload: Optional[bytes] = None

#     def build(self) -> bytes:
#         """Build the complete packet with header"""
#         payload = self._build_payload()
#         header = bytes([START_BYTE, DOWNLINK_DIR, len(payload)])
#         return header + payload

#     def _build_payload(self) -> bytes:
#         """Build the payload portion (command + device_id + params + terminator)"""
#         if self.raw_payload:
#             return self.raw_payload

#         payload = bytes([self.command.value]) + self.device_id

#         if self.command == CommandCode.SET_CONFIG:
#             payload += self._build_set_config_params()
#         elif self.command == CommandCode.GET_CONFIG:
#             pass  # No additional params
#         elif self.command == CommandCode.CONFIGURE_APN:
#             payload += self._build_configure_apn_params()
#         elif self.command == CommandCode.REBOOT:
#             pass  # No additional params
#         elif self.command == CommandCode.RESET_TO_FACTORY_DEFAULTS:
#             pass  # No additional params
#         elif self.command == CommandCode.TURN_OFF_EDGE_DEVICE:
#             pass  # No additional params
#         elif self.command == CommandCode.SET_LORAWAN_CONFIG:
#             payload += self._build_set_lorawan_config_params()

#         payload += bytes([TERMINATOR])
#         return payload

#     def _build_set_config_params(self) -> bytes:
#         """Build SET_CONFIG parameters"""
#         params = self.params
#         data = b''
#         data += struct.pack('<I', params.get('epoch_time', int(time.time())))
#         data += struct.pack('<H', params.get('sampling_frequency', 60))
#         data += struct.pack('<I', params.get('max_tank_capacity', 300000))
#         data += bytes([params.get('tank_length', 10)])
#         data += bytes([params.get('tank_width', 10)])
#         data += bytes([params.get('tank_height', 6)])
#         data += bytes([params.get('wifi_enabled', 0)])
#         data += bytes([params.get('cellular_enabled', 0)])
#         data += bytes([params.get('lorawan_enabled', 1)])
#         data += bytes([params.get('gps_enabled', 0)])
#         data += bytes([params.get('lbs_enabled', 0)])
#         return data

#     def _build_configure_apn_params(self) -> bytes:
#         """Build CONFIGURE_APN parameters"""
#         params = self.params
#         data = b''
#         data += struct.pack('<I', params.get('epoch_time', int(time.time())))
#         apn = params.get('apn', 'jiociot2')
#         apn_bytes = apn.encode('ascii')[:16].ljust(16, b'\x00')
#         data += apn_bytes
#         return data

#     def _build_set_lorawan_config_params(self) -> bytes:
#         """Build SET_LORAWAN_CONFIG parameters"""
#         params = self.params
#         data = b''
#         data += struct.pack('<I', params.get('epoch_time', int(time.time())))
#         data += params.get('lorawan_app_key', bytes(16))
#         data += params.get('lorawan_nwk_key', bytes(16))
#         data += params.get('lorawan_app_session_key', bytes(16))
#         data += params.get('lorawan_nwk_session_key', bytes(16))
#         data += params.get('new_lorawan_device_eui', bytes(8))
#         data += params.get('lorawan_join_eui', bytes(8))
#         data += params.get('lorawan_device_address', bytes(4))
#         return data


# @dataclass
# class UplinkPacket:
#     """Represents a received uplink packet (ACK)"""
#     command: CommandCode
#     device_id: bytes
#     params: Dict[str, Any]
#     raw_data: bytes

#     @classmethod
#     def parse(cls, data: bytes) -> Optional['UplinkPacket']:
#         """Parse raw bytes into an UplinkPacket"""
#         if len(data) < 4:
#             logger.error(f"Packet too short: {len(data)} bytes")
#             return None

#         if data[0] != START_BYTE:
#             logger.error(f"Invalid start byte: 0x{data[0]:02X}")
#             return None

#         direction = data[1]
#         if direction != UPLINK_DIR:
#             logger.warning(f"Not an uplink packet, direction: 0x{direction:02X}")
#             return None

#         payload_len = data[2]
#         if len(data) < 3 + payload_len:
#             logger.error(f"Incomplete packet: expected {3 + payload_len}, got {len(data)}")
#             return None

#         payload = data[3:3 + payload_len]

#         if len(payload) < 9:
#             logger.error(f"Payload too short: {len(payload)} bytes")
#             return None

#         try:
#             cmd_code = CommandCode(payload[0])
#         except ValueError:
#             logger.error(f"Unknown command code: 0x{payload[0]:02X}")
#             return None

#         device_id = payload[1:9]
#         params = cls._parse_params(cmd_code, payload[9:])

#         return cls(
#             command=cmd_code,
#             device_id=device_id,
#             params=params,
#             raw_data=data
#         )

#     @classmethod
#     def _parse_params(cls, cmd: CommandCode, data: bytes) -> Dict[str, Any]:
#         """Parse parameters based on command type"""
#         params = {}

#         # Remove terminator if present
#         if data and data[-1] == TERMINATOR:
#             data = data[:-1]

#         if cmd == CommandCode.SET_CONFIG_ACK:
#             if len(data) >= 17:
#                 params['epoch_time'] = struct.unpack('<I', data[0:4])[0]
#                 params['sampling_frequency'] = struct.unpack('<H', data[4:6])[0]
#                 params['max_tank_capacity'] = struct.unpack('<I', data[6:10])[0]
#                 params['tank_length'] = data[10]
#                 params['tank_width'] = data[11]
#                 params['tank_height'] = data[12]
#                 params['wifi_enabled'] = data[13]
#                 params['cellular_enabled'] = data[14]
#                 params['lorawan_enabled'] = data[15]
#                 params['gps_enabled'] = data[16]
#                 if len(data) > 17:
#                     params['lbs_enabled'] = data[17]

#         elif cmd == CommandCode.GET_CONFIG_ACK:
#             if len(data) >= 17:
#                 params['epoch_time'] = struct.unpack('<I', data[0:4])[0]
#                 params['sampling_frequency'] = struct.unpack('<H', data[4:6])[0]
#                 params['max_tank_capacity'] = struct.unpack('<I', data[6:10])[0]
#                 params['tank_length'] = data[10]
#                 params['tank_width'] = data[11]
#                 params['tank_height'] = data[12]
#                 params['wifi_enabled'] = data[13]
#                 params['cellular_enabled'] = data[14]
#                 params['lorawan_enabled'] = data[15]
#                 params['gps_enabled'] = data[16]
#                 if len(data) > 17:
#                     params['lbs_enabled'] = data[17]

#         elif cmd == CommandCode.CONFIGURE_APN_ACK:
#             if len(data) >= 20:
#                 params['epoch_time'] = struct.unpack('<I', data[0:4])[0]
#                 params['apn'] = data[4:20].rstrip(b'\x00').decode('ascii', errors='ignore')

#         elif cmd == CommandCode.REBOOT_ACK:
#             pass  # No params to parse

#         elif cmd == CommandCode.RESET_TO_FACTORY_DEFAULTS_ACK:
#             pass  # No params to parse

#         elif cmd == CommandCode.TURN_OFF_EDGE_DEVICE_ACK:
#             pass  # No params to parse

#         elif cmd == CommandCode.SET_LORAWAN_CONFIG_ACK:
#             if len(data) >= 84:
#                 params['epoch_time'] = struct.unpack('<I', data[0:4])[0]
#                 params['lorawan_app_key'] = data[4:20]
#                 params['lorawan_nwk_key'] = data[20:36]
#                 params['lorawan_app_session_key'] = data[36:52]
#                 params['lorawan_nwk_session_key'] = data[52:68]
#                 params['lorawan_device_eui'] = data[68:76]
#                 params['lorawan_join_eui'] = data[76:84]
#                 if len(data) >= 88:
#                     params['lorawan_device_address'] = data[84:88]

#         return params


# class ACKValidator:
#     """Validates ACK packets against sent downlink packets"""

#     @staticmethod
#     def validate(downlink: DownlinkPacket, ack: UplinkPacket) -> tuple[bool, str]:
#         """
#         Validate an ACK packet against the original downlink packet.
#         Returns (is_valid, message)
#         """
#         # Check if ACK command matches expected ACK for downlink command
#         expected_ack = DOWNLINK_TO_ACK.get(downlink.command)
#         if expected_ack != ack.command:
#             return False, f"Wrong ACK type: expected {expected_ack}, got {ack.command}"

#         # Validate device ID
#         if downlink.device_id != ack.device_id:
#             return False, f"Device ID mismatch: sent {downlink.device_id.hex()}, received {ack.device_id.hex()}"

#         # Validate command-specific parameters
#         if downlink.command == CommandCode.SET_CONFIG:
#             return ACKValidator._validate_set_config(downlink, ack)
#         elif downlink.command == CommandCode.GET_CONFIG:
#             # GET_CONFIG has no params in downlink, so ACK is valid if device_id matches
#             return True, "GET_CONFIG ACK validated (no params to match)"
#         elif downlink.command == CommandCode.CONFIGURE_APN:
#             return ACKValidator._validate_configure_apn(downlink, ack)
#         elif downlink.command == CommandCode.REBOOT:
#             return True, "REBOOT ACK validated (no params to match)"
#         elif downlink.command == CommandCode.RESET_TO_FACTORY_DEFAULTS:
#             return True, "RESET_TO_FACTORY_DEFAULTS ACK validated (no params to match)"
#         elif downlink.command == CommandCode.TURN_OFF_EDGE_DEVICE:
#             return True, "TURN_OFF_EDGE_DEVICE ACK validated (no params to match)"
#         elif downlink.command == CommandCode.SET_LORAWAN_CONFIG:
#             return ACKValidator._validate_set_lorawan_config(downlink, ack)

#         return True, "ACK validated"

#     @staticmethod
#     def _validate_set_config(downlink: DownlinkPacket, ack: UplinkPacket) -> tuple[bool, str]:
#         """Validate SET_CONFIG ACK parameters"""
#         dl_params = downlink.params
#         ack_params = ack.params

#         mismatches = []

#         for key in ['sampling_frequency', 'max_tank_capacity', 'tank_length',
#                     'tank_width', 'tank_height', 'wifi_enabled', 'cellular_enabled',
#                     'lorawan_enabled', 'gps_enabled', 'lbs_enabled']:
#             if key in dl_params and key in ack_params:
#                 if dl_params[key] != ack_params[key]:
#                     mismatches.append(f"{key}: sent {dl_params[key]}, received {ack_params[key]}")

#         if mismatches:
#             return False, f"Parameter mismatches: {'; '.join(mismatches)}"

#         return True, "SET_CONFIG ACK validated - all params match"

#     @staticmethod
#     def _validate_configure_apn(downlink: DownlinkPacket, ack: UplinkPacket) -> tuple[bool, str]:
#         """Validate CONFIGURE_APN ACK parameters"""
#         dl_apn = downlink.params.get('apn', '')
#         ack_apn = ack.params.get('apn', '')

#         if dl_apn != ack_apn:
#             return False, f"APN mismatch: sent '{dl_apn}', received '{ack_apn}'"

#         return True, "CONFIGURE_APN ACK validated - APN matches"

#     @staticmethod
#     def _validate_set_lorawan_config(downlink: DownlinkPacket, ack: UplinkPacket) -> tuple[bool, str]:
#         """Validate SET_LORAWAN_CONFIG ACK parameters"""
#         dl_params = downlink.params
#         ack_params = ack.params

#         mismatches = []

#         for key in ['lorawan_app_key', 'lorawan_nwk_key', 'lorawan_app_session_key',
#                     'lorawan_nwk_session_key', 'lorawan_join_eui', 'lorawan_device_address']:
#             if key in dl_params and key in ack_params:
#                 if dl_params[key] != ack_params[key]:
#                     mismatches.append(f"{key}: mismatch")

#         # Check new device EUI
#         new_eui = dl_params.get('new_lorawan_device_eui')
#         ack_eui = ack_params.get('lorawan_device_eui')
#         if new_eui and ack_eui and new_eui != ack_eui:
#             mismatches.append(f"new_device_eui: sent {new_eui.hex()}, received {ack_eui.hex()}")

#         if mismatches:
#             return False, f"Parameter mismatches: {'; '.join(mismatches)}"

#         return True, "SET_LORAWAN_CONFIG ACK validated - all params match"


# class LoRaWANTester:
#     """Main tester class for LoRaWAN downlink/uplink testing"""

#     def __init__(self, serial_port: str, baudrate: int = 115200, timeout: float = 60.0):
#         self.serial_port = serial_port
#         self.baudrate = baudrate
#         self.timeout = timeout
#         self.serial: Optional[serial.Serial] = None
#         self.running = False
#         self.rx_queue: Queue[bytes] = Queue()
#         self.rx_thread: Optional[threading.Thread] = None
#         self.packet_index = 0
#         self.test_packets: list[DownlinkPacket] = []

#     def connect(self) -> bool:
#         """Connect to serial port"""
#         try:
#             self.serial = serial.Serial(
#                 port=self.serial_port,
#                 baudrate=self.baudrate,
#                 timeout=0.1,
#                 bytesize=serial.EIGHTBITS,
#                 parity=serial.PARITY_NONE,
#                 stopbits=serial.STOPBITS_ONE
#             )
#             logger.info(f"Connected to {self.serial_port} at {self.baudrate} baud")
#             return True
#         except serial.SerialException as e:
#             logger.error(f"Failed to connect to {self.serial_port}: {e}")
#             return False

#     def disconnect(self):
#         """Disconnect from serial port"""
#         self.running = False
#         if self.rx_thread and self.rx_thread.is_alive():
#             self.rx_thread.join(timeout=2.0)
#         if self.serial and self.serial.is_open:
#             self.serial.close()
#             logger.info("Serial port closed")

#     def _rx_thread_func(self):
#         """Background thread for receiving data"""
#         buffer = bytearray()

#         while self.running:
#             try:
#                 if self.serial and self.serial.in_waiting:
#                     data = self.serial.read(self.serial.in_waiting)
#                     buffer.extend(data)

#                     # Try to parse complete packets from buffer
#                     while len(buffer) >= 4:
#                         # Look for start byte
#                         if buffer[0] != START_BYTE:
#                             buffer.pop(0)
#                             continue

#                         # Check if we have enough data for header
#                         if len(buffer) < 3:
#                             break

#                         payload_len = buffer[2]
#                         packet_len = 3 + payload_len

#                         if len(buffer) < packet_len:
#                             break

#                         # Extract complete packet
#                         packet = bytes(buffer[:packet_len])
#                         buffer = buffer[packet_len:]

#                         logger.debug(f"Received packet: {packet.hex()}")
#                         self.rx_queue.put(packet)
#                 else:
#                     time.sleep(0.01)
#             except Exception as e:
#                 logger.error(f"RX thread error: {e}")
#                 time.sleep(0.1)

#     def send_packet(self, packet: DownlinkPacket) -> bool:
#         """Send a downlink packet"""
#         if not self.serial or not self.serial.is_open:
#             logger.error("Serial port not open")
#             return False

#         data = packet.build()
#         logger.info(f"Sending downlink packet: {data.hex()}")
#         logger.info(f"  Command: {packet.command.name}")
#         logger.info(f"  Device ID: {packet.device_id.hex()}")
#         logger.info(f"  Params: {packet.params}")

#         try:
#             self.serial.write(data)
#             self.serial.flush()
#             return True
#         except serial.SerialException as e:
#             logger.error(f"Failed to send packet: {e}")
#             return False

#     def wait_for_ack(self, expected_cmd: CommandCode, timeout: float = None) -> Optional[UplinkPacket]:
#         """Wait for an ACK packet with the expected command"""
#         if timeout is None:
#             timeout = self.timeout

#         expected_ack = DOWNLINK_TO_ACK.get(expected_cmd)
#         start_time = time.time()

#         while time.time() - start_time < timeout:
#             try:
#                 raw_data = self.rx_queue.get(timeout=0.5)
#                 packet = UplinkPacket.parse(raw_data)

#                 if packet:
#                     logger.info(f"Received uplink packet: {raw_data.hex()}")
#                     logger.info(f"  Command: {packet.command.name}")
#                     logger.info(f"  Device ID: {packet.device_id.hex()}")
#                     logger.info(f"  Params: {packet.params}")

#                     if packet.command == expected_ack:
#                         return packet
#                     else:
#                         logger.warning(f"Unexpected ACK type: {packet.command.name}, expected {expected_ack.name}")
#             except Empty:
#                 continue

#         return None

#     def get_random_delay(self) -> float:
#         """Get random delay between MIN_RETRY_DELAY and MAX_RETRY_DELAY seconds"""
#         return random.uniform(MIN_RETRY_DELAY, MAX_RETRY_DELAY)

#     def create_test_packets(self, device_id: bytes) -> list[DownlinkPacket]:
#         """Create a list of test packets to cycle through"""
#         packets = []
#         current_time = int(time.time())

#         # 1. SET_CONFIG
#         packets.append(DownlinkPacket(
#             command=CommandCode.SET_CONFIG,
#             device_id=device_id,
#             params={
#                 'epoch_time': current_time,
#                 'sampling_frequency': 60,
#                 'max_tank_capacity': 300000,
#                 'tank_length': 10,
#                 'tank_width': 10,
#                 'tank_height': 6,
#                 'wifi_enabled': 0,
#                 'cellular_enabled': 0,
#                 'lorawan_enabled': 1,
#                 'gps_enabled': 0,
#                 'lbs_enabled': 0
#             }
#         ))

#         # 2. GET_CONFIG
#         packets.append(DownlinkPacket(
#             command=CommandCode.GET_CONFIG,
#             device_id=device_id,
#             params={}
#         ))

#         # 3. CONFIGURE_APN
#         packets.append(DownlinkPacket(
#             command=CommandCode.CONFIGURE_APN,
#             device_id=device_id,
#             params={
#                 'epoch_time': current_time,
#                 'apn': 'jiociot2'
#             }
#         ))

#         # 4. REBOOT
#         packets.append(DownlinkPacket(
#             command=CommandCode.REBOOT,
#             device_id=device_id,
#             params={}
#         ))

#         # 5. RESET_TO_FACTORY_DEFAULTS
#         packets.append(DownlinkPacket(
#             command=CommandCode.RESET_TO_FACTORY_DEFAULTS,
#             device_id=device_id,
#             params={}
#         ))

#         # 6. TURN_OFF_EDGE_DEVICE
#         packets.append(DownlinkPacket(
#             command=CommandCode.TURN_OFF_EDGE_DEVICE,
#             device_id=device_id,
#             params={}
#         ))

#         # 7. SET_LORAWAN_CONFIG
#         packets.append(DownlinkPacket(
#             command=CommandCode.SET_LORAWAN_CONFIG,
#             device_id=device_id,
#             params={
#                 'epoch_time': current_time,
#                 'lorawan_app_key': bytes.fromhex('1D72EDF9392A91FC4A4F0C01AA133E19'),
#                 'lorawan_nwk_key': bytes.fromhex('1D72EDF9392A91FC4A4F0C01AA133E19'),
#                 'lorawan_app_session_key': bytes.fromhex('1D72EDF9392A91FC4A4F0C01AA133E19'),
#                 'lorawan_nwk_session_key': bytes.fromhex('1D72EDF9392A91FC4A4F0C01AA133E19'),
#                 'new_lorawan_device_eui': device_id,
#                 'lorawan_join_eui': bytes.fromhex('0101010101010101'),
#                 'lorawan_device_address': bytes.fromhex('33221100')
#             }
#         ))

#         return packets

#     def run_test(self, device_id_hex: str, serial_port: str = None):
#         """
#         Run the test loop indefinitely.
#         - Send packets with random delay (30s-2m)
#         - Wait for ACK
#         - If ACK received and valid, move to next packet
#         - If no ACK or invalid, resend same packet after random delay
#         """
#         if serial_port:
#             self.serial_port = serial_port

#         # Parse device ID
#         device_id = bytes.fromhex(device_id_hex.replace('-', '').replace(':', ''))
#         if len(device_id) != 8:
#             logger.error(f"Device ID must be 8 bytes, got {len(device_id)}")
#             return

#         # Connect to serial port
#         if not self.connect():
#             return

#         # Start RX thread
#         self.running = True
#         self.rx_thread = threading.Thread(target=self._rx_thread_func, daemon=True)
#         self.rx_thread.start()

#         # Create test packets
#         self.test_packets = self.create_test_packets(device_id)
#         self.packet_index = 0

#         logger.info(f"Starting test with {len(self.test_packets)} packet types")
#         logger.info(f"Device ID: {device_id.hex()}")

#         try:
#             while self.running:
#                 current_packet = self.test_packets[self.packet_index]

#                 # Update epoch_time for time-sensitive packets
#                 if 'epoch_time' in current_packet.params:
#                     current_packet.params['epoch_time'] = int(time.time())

#                 logger.info("=" * 60)
#                 logger.info(f"Packet {self.packet_index + 1}/{len(self.test_packets)}: {current_packet.command.name}")

#                 # Send the packet
#                 if not self.send_packet(current_packet):
#                     delay = self.get_random_delay()
#                     logger.error(f"Failed to send packet, retrying in {delay:.1f}s")
#                     time.sleep(delay)
#                     continue

#                 # Wait for ACK
#                 ack = self.wait_for_ack(current_packet.command, timeout=self.timeout)

#                 if ack is None:
#                     # No ACK received, resend after random delay
#                     delay = self.get_random_delay()
#                     logger.warning(f"No ACK received, resending same packet in {delay:.1f}s")
#                     time.sleep(delay)
#                     continue

#                 # Validate ACK
#                 is_valid, message = ACKValidator.validate(current_packet, ack)

#                 if is_valid:
#                     logger.info(f"ACK VALID: {message}")

#                     # Move to next packet
#                     self.packet_index = (self.packet_index + 1) % len(self.test_packets)

#                     delay = self.get_random_delay()
#                     logger.info(f"Moving to next packet in {delay:.1f}s")
#                     time.sleep(delay)
#                 else:
#                     # Invalid ACK, resend after random delay
#                     delay = self.get_random_delay()
#                     logger.warning(f"ACK INVALID: {message}")
#                     logger.warning(f"Resending same packet in {delay:.1f}s")
#                     time.sleep(delay)

#         except KeyboardInterrupt:
#             logger.info("Test stopped by user")
#         finally:
#             self.disconnect()

#     def run_single_command(self, device_id_hex: str, command: str, params: Dict[str, Any] = None):
#         """Run a single command repeatedly until ACK is received"""
#         device_id = bytes.fromhex(device_id_hex.replace('-', '').replace(':', ''))
#         if len(device_id) != 8:
#             logger.error(f"Device ID must be 8 bytes, got {len(device_id)}")
#             return

#         # Map command string to CommandCode
#         cmd_map = {
#             'SET_CONFIG': CommandCode.SET_CONFIG,
#             'GET_CONFIG': CommandCode.GET_CONFIG,
#             'CONFIGURE_APN': CommandCode.CONFIGURE_APN,
#             'REBOOT': CommandCode.REBOOT,
#             'RESET_TO_FACTORY_DEFAULTS': CommandCode.RESET_TO_FACTORY_DEFAULTS,
#             'TURN_OFF_EDGE_DEVICE': CommandCode.TURN_OFF_EDGE_DEVICE,
#             'SET_LORAWAN_CONFIG': CommandCode.SET_LORAWAN_CONFIG,
#         }

#         cmd = cmd_map.get(command.upper())
#         if not cmd:
#             logger.error(f"Unknown command: {command}")
#             logger.info(f"Available commands: {list(cmd_map.keys())}")
#             return

#         if not self.connect():
#             return

#         self.running = True
#         self.rx_thread = threading.Thread(target=self._rx_thread_func, daemon=True)
#         self.rx_thread.start()

#         packet = DownlinkPacket(
#             command=cmd,
#             device_id=device_id,
#             params=params or {}
#         )

#         try:
#             while self.running:
#                 if 'epoch_time' in packet.params:
#                     packet.params['epoch_time'] = int(time.time())

#                 logger.info("=" * 60)

#                 if not self.send_packet(packet):
#                     delay = self.get_random_delay()
#                     logger.error(f"Failed to send packet, retrying in {delay:.1f}s")
#                     time.sleep(delay)
#                     continue

#                 ack = self.wait_for_ack(packet.command, timeout=self.timeout)

#                 if ack is None:
#                     delay = self.get_random_delay()
#                     logger.warning(f"No ACK received, resending in {delay:.1f}s")
#                     time.sleep(delay)
#                     continue

#                 is_valid, message = ACKValidator.validate(packet, ack)

#                 if is_valid:
#                     logger.info(f"ACK VALID: {message}")
#                     delay = self.get_random_delay()
#                     logger.info(f"Sending again in {delay:.1f}s")
#                     time.sleep(delay)
#                 else:
#                     delay = self.get_random_delay()
#                     logger.warning(f"ACK INVALID: {message}")
#                     logger.warning(f"Resending in {delay:.1f}s")
#                     time.sleep(delay)

#         except KeyboardInterrupt:
#             logger.info("Test stopped by user")
#         finally:
#             self.disconnect()


# def main():
#     import argparse

#     parser = argparse.ArgumentParser(
#         description='LoRaWAN Downlink Test Script for Water Level Monitoring',
#         formatter_class=argparse.RawDescriptionHelpFormatter,
#         epilog="""
# Examples:
#   # Run full test cycle on all commands
#   python lorawan_downlink_test.py --port /dev/ttyUSB0 --device-id 0901202600000001

#   # Run single command test
#   python lorawan_downlink_test.py --port /dev/ttyUSB0 --device-id 0901202600000001 --command GET_CONFIG

#   # With custom timeout and debug logging
#   python lorawan_downlink_test.py --port /dev/ttyUSB0 --device-id 0901202600000001 --timeout 120 --debug

# Available commands:
#   SET_CONFIG, GET_CONFIG, CONFIGURE_APN, REBOOT,
#   RESET_TO_FACTORY_DEFAULTS, TURN_OFF_EDGE_DEVICE, SET_LORAWAN_CONFIG
#         """
#     )

#     parser.add_argument('--port', '-p', required=True,
#                         help='Serial port (e.g., /dev/ttyUSB0, COM3)')
#     parser.add_argument('--device-id', '-d', required=True,
#                         help='Device EUI/ID as hex string (8 bytes, e.g., 0901202600000001)')
#     parser.add_argument('--baudrate', '-b', type=int, default=115200,
#                         help='Serial baudrate (default: 115200)')
#     parser.add_argument('--timeout', '-t', type=float, default=60.0,
#                         help='ACK timeout in seconds (default: 60)')
#     parser.add_argument('--command', '-c', type=str, default=None,
#                         help='Run single command repeatedly (optional)')
#     parser.add_argument('--debug', action='store_true',
#                         help='Enable debug logging')

#     args = parser.parse_args()

#     if args.debug:
#         logging.getLogger().setLevel(logging.DEBUG)

#     tester = LoRaWANTester(
#         serial_port=args.port,
#         baudrate=args.baudrate,
#         timeout=args.timeout
#     )

#     if args.command:
#         tester.run_single_command(args.device_id, args.command)
#     else:
#         tester.run_test(args.device_id)


# if __name__ == '__main__':
#     main()







#!/usr/bin/env python3

import serial
import time
import random
import struct
import logging
import threading
import os
from dataclasses import dataclass, field
from typing import Optional, Dict, Any
from enum import Enum
from queue import Queue, Empty

# =================================================================
# CONFIGURATION MACROS
# =================================================================
UART_PORT = "COM5"                  
DEVICE_ID_HEX = "2001202600000001"  
BAUDRATE = 115200
ACK_TIMEOUT = 60.0
LOG_FILE_NAME = r"E:/ANTZ_Systems/100_Acre_VIC_Water_Level_Monitoring_Control_Card_V1.0.1/UART_Logs/26-01-2026/regression_test_logs.txt"
# =================================================================

# --- AUTOMATIC DIRECTORY CREATION ---
log_dir = os.path.dirname(os.path.normpath(LOG_FILE_NAME))
if not os.path.exists(log_dir):
    try:
        os.makedirs(log_dir, exist_ok=True)
        print(f"Created directory: {log_dir}")
    except Exception as e:
        print(f"Error creating directory: {e}")

# --- LOGGING CONFIGURATION ---
logger = logging.getLogger("FirmwareTester")
logger.setLevel(logging.DEBUG)
log_formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')

file_handler = logging.FileHandler(LOG_FILE_NAME, mode='a', encoding='utf-8', delay=False)
file_handler.setFormatter(log_formatter)
logger.addHandler(file_handler)

console_handler = logging.StreamHandler()
console_handler.setFormatter(log_formatter)
logger.addHandler(console_handler)

class CommandCode(Enum):
    SET_CONFIG = 0xB1
    GET_CONFIG = 0xB2
    CONFIGURE_APN = 0xB3
    REBOOT = 0xB4
    RESET_TO_FACTORY_DEFAULTS = 0xB5
    TURN_OFF_EDGE_DEVICE = 0xB6
    SET_CONFIG_ACK = 0xA3
    GET_CONFIG_ACK = 0xA4
    CONFIGURE_APN_ACK = 0xA5
    REBOOT_ACK = 0xA6
    RESET_TO_FACTORY_DEFAULTS_ACK = 0xA7
    TURN_OFF_EDGE_DEVICE_ACK = 0xA8
    SET_LORAWAN_CONFIG_ACK = 0xAA

DOWNLINK_TO_ACK = {
    CommandCode.SET_CONFIG: CommandCode.SET_CONFIG_ACK,
    CommandCode.GET_CONFIG: CommandCode.GET_CONFIG_ACK,
    CommandCode.CONFIGURE_APN: CommandCode.CONFIGURE_APN_ACK,
    CommandCode.REBOOT: CommandCode.REBOOT_ACK,
    CommandCode.RESET_TO_FACTORY_DEFAULTS: CommandCode.RESET_TO_FACTORY_DEFAULTS_ACK,
    CommandCode.TURN_OFF_EDGE_DEVICE: CommandCode.TURN_OFF_EDGE_DEVICE_ACK,
}

START_BYTE = 0xF9
TERMINATOR = 0x0A
DOWNLINK_DIR = 0x02

@dataclass
class DownlinkPacket:
    command: CommandCode
    device_id: bytes
    params: Dict[str, Any] = field(default_factory=dict)

    def build(self) -> bytes:
        payload = bytes([self.command.value]) + self.device_id
        if self.command == CommandCode.SET_CONFIG:
            payload += struct.pack('<I', self.params.get('epoch_time', int(time.time())))
            payload += struct.pack('<H', self.params.get('sampling_frequency', 60))
            payload += struct.pack('<I', self.params.get('max_tank_capacity', 300000))
            payload += bytes([self.params.get('tank_length', 10), self.params.get('tank_width', 10), self.params.get('tank_height', 6)])
            payload += bytes([0, 0, 1, 0, 0]) 
        elif self.command == CommandCode.CONFIGURE_APN:
            payload += struct.pack('<I', int(time.time()))
            payload += self.params.get('apn', 'jiociot2').encode().ljust(16, b'\x00')

        payload += bytes([TERMINATOR])
        header = bytes([START_BYTE, DOWNLINK_DIR, len(payload) - 1])
        return header + payload

class LoRaWANTester:
    def __init__(self):
        self.serial = None
        self.running = False
        self.rx_queue = Queue()

    def connect(self):
        try:
            self.serial = serial.Serial(UART_PORT, BAUDRATE, timeout=0.1)
            logger.info(f"Connected to {UART_PORT} at {BAUDRATE}")
            return True
        except Exception as e:
            logger.error(f"Failed to connect: {e}")
            return False

    def _rx_thread(self):
        """Accumulates data until a full F9...0A packet is found."""
        buffer = bytearray()
        while self.running:
            try:
                if self.serial.in_waiting:
                    # Read available data and append to buffer
                    buffer.extend(self.serial.read(self.serial.in_waiting))
                    
                    while True:
                        # 1. Look for the start byte 0xF9
                        start_idx = buffer.find(START_BYTE)
                        if start_idx == -1:
                            buffer.clear() # No start byte, discard junk
                            break
                        
                        # Remove anything before the start byte
                        if start_idx > 0:
                            del buffer[:start_idx]
                        
                        # 2. Look for the terminator byte 0x0A
                        # We only look for 0x0A AFTER the start byte
                        end_idx = buffer.find(TERMINATOR)
                        
                        if end_idx != -1:
                            # Complete packet found
                            packet = bytes(buffer[:end_idx + 1])
                            
                            # Log the complete packet as one line
                            logger.info(f"[RECV RAW]: {packet.hex().upper()}")
                            self.rx_queue.put(packet)
                            
                            # Remove the processed packet from the buffer
                            del buffer[:end_idx + 1]
                        else:
                            # Packet is incomplete, wait for more data
                            break
            except Exception as e:
                logger.error(f"RX Thread Error: {e}")
            time.sleep(0.01)

    def send_packet(self, packet: DownlinkPacket):
        data = packet.build()
        logger.info(f"[SENDING]: {data.hex().upper()} | Cmd: {packet.command.name}")
        self.serial.write(data)
        self.serial.flush()

    def run(self):
        if not self.connect(): return
        dev_id = bytes.fromhex(DEVICE_ID_HEX)
        self.running = True
        threading.Thread(target=self._rx_thread, daemon=True).start()

        test_cmds = [CommandCode.GET_CONFIG, CommandCode.REBOOT]
        
        try:
            while True:
                for cmd in test_cmds:
                    pkt = DownlinkPacket(cmd, dev_id)
                    self.send_packet(pkt)
                    
                    start = time.time()
                    while time.time() - start < ACK_TIMEOUT:
                        try:
                            raw = self.rx_queue.get(timeout=1)
                            # Logic: START(0), DIR(1), LEN(2), CMD(3)
                            if len(raw) > 3 and raw[3] == DOWNLINK_TO_ACK[cmd].value:
                                logger.info(f"[ACK VALIDATED]: {raw.hex().upper()}")
                                break
                        except Empty: continue
                    
                    delay = random.uniform(30, 60)
                    logger.info(f"Cycle complete. Sleeping {delay:.1f}s...")
                    time.sleep(delay)
        except KeyboardInterrupt:
            logger.info("Test stopped by user.")
            self.running = False
            if self.serial: self.serial.close()

if __name__ == "__main__":
    tester = LoRaWANTester()
    tester.run()