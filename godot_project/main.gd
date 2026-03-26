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
	while not (serial.list_ports().has(0) and serial.open_buffered(serial.list_ports()[0].port_name, 115200, 100, GdSerialManager.MODE_LINE_BUFFERED)):
		await get_tree().create_timer(1.0).timeout
	
	connected_port = serial.list_ports()[0].port_name
	%SerialOutput.text += "Connected on port " + connected_port + "\n"

func _on_disconnected(port : String) -> void:
	connected_port = ""
	
	%SerialOutput.text += "Device disconnected from port " + connected_port + "\n"
	
	await get_tree().create_timer(2.5).timeout #wait to allow the firmware flasher to connect
	
	attempt_connection()

#TODO: replace this logic with a command parser that can more intelligently interpret incoming data
func _on_data(port: String, data: PackedByteArray) -> void:
	print(data)
	
	%SerialOutput.text += data.get_string_from_utf8().c_unescape()

func write(data : PackedByteArray) -> void:
	serial.write(connected_port, data)
