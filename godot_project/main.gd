extends MarginContainer

var serial = GdSerialManager.new()

var connected_port : String

func _ready() -> void:
	serial.data_received.connect(_on_data)
	serial.port_disconnected.connect(_on_disconnected)
	
	%SerialOutput.text = ""
	attempt_connection()

func _process(delta: float) -> void:
	serial.poll_events()

func serial_disconnect() -> void:
	serial.close(connected_port)
	serial.port_disconnected.emit(connected_port)

func attempt_connection() -> void:
	%SerialOutput.text += "Waiting to connect\n"
	
	#waits until connection is successful.
	#TODO: buffer mode LINE causes issues if the byte value of 10 is ever sent, splitting commands/data in half.
	while not (serial.list_ports().has(0) and serial.open_buffered(serial.list_ports()[0].port_name, 115200, 5, GdSerialManager.MODE_RAW)):
		await get_tree().create_timer(1.0).timeout
	
	connected_port = serial.list_ports()[0].port_name
	%SerialOutput.text += "Connected on port " + connected_port + "\n"

func _on_disconnected(port : String) -> void:
	connected_port = ""
	
	%SerialOutput.text += "Device disconnected from port " + connected_port + "\n"
	
	await get_tree().create_timer(2.5).timeout #wait to allow the firmware flasher to connect
	
	attempt_connection()


func write(data : PackedByteArray) -> void:
	serial.write(connected_port, data)

var read_buffer : Array = []


#TODO: replace this logic with a command parser that can more intelligently interpret incoming data
func _on_data(port: String, data: PackedByteArray) -> void:
	read_buffer += Array(data)
	#print(data)
	
	%SerialOutput.text += data.get_string_from_utf8().c_unescape()
	
	parse_command()

#TODO
#TODO
#TODO
#TODO
#TODO
#TODO
#TODO
#TODO     Create SetConfig function to send entire formatted block of configs to device
#TODO       Resource -> Bytes -> Device config
#TODO
#TODO
#TODO
#TODO
#TODO
#TODO
#TODO
#TODO


var read_buffer_index : int = 0
func parse_command() -> void:
	read_buffer_index = 0
	print(read_buffer)
	
	if read_buffer.size() < Enums.MINIMUM_MESSAGE_LENGTH:
		return
	
	var read_next_byte : Callable = func read_next_byte() -> int:
		var value : int = read_buffer[read_buffer_index]
		read_buffer_index += 1
		return value
	
	#//TODO: this basic command parsing logic should be placed into a dedicated function
	
	#//first two byes are the start code
	var start_byte_1 : int = read_next_byte.call()
	var start_byte_2 : int = read_next_byte.call()
	
	if not (start_byte_1 == 0xFF and start_byte_2 == 0x00):
		#there may be a more elegant solution here.
		read_buffer = []
		return
	
	var payload_length_byte_1 : int = read_next_byte.call() << 8
	var payload_length_byte_2 : int = read_next_byte.call()
	
	var payload_size : int = payload_length_byte_1 + payload_length_byte_2
	print("payload size=", payload_size)
	
	#if (payload_size > MAX_PAYLOAD_BUFFER_SIZE) {
		#//If sent message is too big, abort and clear buffer
		#while(Serial.available() > 0) Serial.read();
		#return;
	#}
	
	var expected_message_length : int = Enums.MINIMUM_MESSAGE_LENGTH + payload_size
	if read_buffer.size() < expected_message_length:
		print("less than expected length, expected:" , expected_message_length, " actual length:", read_buffer.size())
		return # stop parsing, but do not clear buffer. When new data is received by the serial manager, the parsing function will run again.
	
	var checksum_value : int = read_buffer[expected_message_length] #TODO double check that this is actually getting the correct checksum value
	read_buffer.remove_at(read_buffer_index + payload_size + 1)
	
	var payload := read_buffer.slice(read_buffer_index, read_buffer_index + payload_size + 1) #??? +1?
	var sum : int = 0;
	for byte in payload:
		sum += byte
	sum = sum & 0xFF
	
	if not checksum_value == sum:
		print("Checksum did not match: Expected checksum=", checksum_value, " calculated checksum=", sum)
		#discard all data that is associated with this parsed command, but retain any data afterwards for future parsing
		read_buffer = read_buffer.slice(expected_message_length)
		parse_command() #immediately attempt to parse remaining data
		return
	
	
	var command : int = read_next_byte.call();
	match command:
		Enums.Commands.GetConfigs: #device is returning all configs
			print("get configs")
			#TODO: these data_type values aren't actually used by the parser, but they should be.
			#right now it only works because both codebases agree on the order
			
			#PinDeclaration
			var data_type : int = read_next_byte.call()
			print("data type=", data_type, "=", Enums.Commands.find_key(data_type))
			var section_length : int = (read_next_byte.call() << 8) + read_next_byte.call()
			print("section length=", section_length)
			print(read_buffer.slice(read_buffer_index, read_buffer_index + section_length))
			var pins : Array[PinDeclaration] = PinDeclaration.from_byte_array(read_buffer.slice(read_buffer_index, read_buffer_index + section_length))
			print("created pin objects: ", pins)
			read_buffer_index += section_length
			
			#InputMapping
			data_type = read_next_byte.call()
			print("data type=", data_type, "=", Enums.Commands.find_key(data_type))
			section_length = (read_next_byte.call() << 8) + read_next_byte.call()
			print("section length=", section_length)
			print(read_buffer.slice(read_buffer_index, read_buffer_index + section_length))
			var mappings : Array[InputMapping] = InputMapping.from_byte_array(read_buffer.slice(read_buffer_index, read_buffer_index + section_length))
			print("created mapping objects: ", mappings)
			read_buffer_index += section_length
			
			#Augmentation
			data_type = read_next_byte.call()
			print("data type=", data_type, "=", Enums.Commands.find_key(data_type))
			section_length = (read_next_byte.call() << 8) + read_next_byte.call()
			print("section length=", section_length)
			print(read_buffer.slice(read_buffer_index, read_buffer_index + section_length))
			var augments : Array[Augmentation] = Augmentation.from_byte_array(read_buffer.slice(read_buffer_index, read_buffer_index + section_length))
			print("created augment objects: ", augments)
			read_buffer_index += section_length
			
			print("remaining bytes in buffer=", read_buffer.slice(read_buffer_index))

		Enums.Commands.GetPins:
			pass
			#u_int8_t index = *message_index++;
#
			#u_int16_t data_length = get_pin_data_size(index);
			#u_int8_t data_buffer[data_length];
			#
			#u_int8_t* data_buffer_index = data_buffer;
			#get_pin_data(data_buffer_index, index);
#
			#u_int16_t message_length = data_length + MINIMUM_MESSAGE_LENGTH;
			#u_int8_t message_buffer[message_length];
			#create_command(message_buffer, data_buffer, data_length);
#
			#//Serial.write(message_buffer, message_length);
			#Serial.write(data_buffer, data_length);
			#
			#break;
		#}
		Enums.Commands.GetMappings:
			pass
			#int index = *message_index++;
#
			#u_int16_t data_length = get_mapping_data_size(index);
			#u_int8_t data_buffer[data_length];
#
			#u_int8_t* data_buffer_index = data_buffer;
			#get_mapping_data(data_buffer_index, index);
#
			#u_int16_t message_length = data_length + MINIMUM_MESSAGE_LENGTH;
			#u_int8_t message_buffer[message_length];
			#create_command(message_buffer, data_buffer, data_length);
#
			#Serial.write(message_buffer, message_length);
			#break;
		#}
		Enums.Commands.GetAugments:
			pass
			#int index = *message_index++;
			#
			#u_int16_t data_length = get_augment_data_size(index);
			#u_int8_t data_buffer[data_length];
#
			#u_int8_t* data_buffer_index = data_buffer;
			#get_augment_data(data_buffer_index, index);
#
			#u_int16_t message_length = data_length + MINIMUM_MESSAGE_LENGTH;
			#u_int8_t message_buffer[message_length];
			#create_command(message_buffer, data_buffer, data_length);
#
			#Serial.write(message_buffer, message_length);
			#break;
			#break;
		#}

		#case PinDeclaration: {
			#//syntax: int pin_number, bool analog
			#PinName pin = (PinName) *message_index++;
			#bool analog = *message_index++;
			#pin_map[pin] = Pin(pin, analog);
			#Serial.println("Added new pin: pin number=" + (String) pin + " analog=" + (String) analog);
			#break;
		#}
		#case InputMapping: {
			#PinName pin = (PinName) *message_index++;
			#int input_device = *message_index++;
			#int input_id = *message_index++;
			#bool invert = *message_index++;
#
			#
			#pin_bindings.push_back(PinMapping(pin, input_device, input_id, invert));
			#Serial.println("Added new mapping: pin number=" + (String) pin + " input device=" + (String) input_device + " input id=" + (String) input_id + " inverted=" + (String) invert);
			#break;
		#}
		#case Augmentation: {
#
			#break;
	
	read_buffer = read_buffer.slice(read_buffer_index) #important to remove the used data from the read buffer
